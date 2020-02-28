/*
 * Copyright (C) 2018 Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
 * Heinrich-Heine University
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "lib/libc/printf.h"
#include "kernel/service/TimeService.h"
#include "kernel/core/System.h"
#include "device/misc/Pic.h"
#include "kernel/interrupt/InterruptDispatcher.h"
#include "kernel/core/Management.h"
#include "device/misc/Cmos.h"
#include "FloppyController.h"
#include "FloppyDevice.h"
#include "FloppyMotorControlThread.h"

bool FloppyController::isAvailable() {
    if(Cmos::isAvailable()) {
        return Cmos::readRegister(0x10) != 0;
    }

    return false;
}

FloppyController::FloppyController() :
        statusRegisterA(IO_BASE_ADDRESS + 0), statusRegisterB(IO_BASE_ADDRESS + 1), digitalOutputRegister(IO_BASE_ADDRESS + 2),
        tapeDriveRegister(IO_BASE_ADDRESS + 3), mainStatusRegister(IO_BASE_ADDRESS + 4), datarateSelectRegister(IO_BASE_ADDRESS + 4),
        fifoRegister(IO_BASE_ADDRESS + 5), digitalInputRegister(IO_BASE_ADDRESS + 7), configControlRegister(IO_BASE_ADDRESS + 7) {
    log = &Kernel::Logger::get("FLOPPY");
    
    dmaMemory = Isa::allocDmaBuffer();

    timeService = Kernel::System::getService<Kernel::TimeService>();
    storageService = Kernel::System::getService<Kernel::StorageService>();
}

FloppyController::~FloppyController() {
    Isa::freeDmaBuffer(dmaMemory);
}

bool FloppyController::isBusy() {
    return (mainStatusRegister.inb() & 0x10u) == 0x10;
}

bool FloppyController::isFifoBufferReady() {
    return (mainStatusRegister.inb() & 0x80u) == 0x80;
}

void FloppyController::setup() {
    if(!Cmos::isAvailable()) {
        log->error("No CMOS available! Aborting RTC setup...");

        return;
    }

    if(!isAvailable()) {
        log->info("No floppy drives available");
    }

    uint8_t driveInfo = Cmos::readRegister(0x10);

    auto primaryDriveType = static_cast<DriveType>(driveInfo >> 4u);
    auto secondaryDriveType = static_cast<DriveType>(driveInfo & static_cast<uint8_t>(0xfu));

    if(primaryDriveType != DriveType::DRIVE_TYPE_NONE && primaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_1 &&
       primaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_2) {

        log->info("Found primary floppy drive");

        FloppyDevice *device = new FloppyDevice(*this, 0, primaryDriveType, "fdd0");

        bool ret = resetDrive(*device);

        if(ret) {
            storageService->registerDevice(device);
        } else {
            delete device;
        }
    }

    if(secondaryDriveType != DriveType::DRIVE_TYPE_NONE && secondaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_1 &&
       secondaryDriveType != DriveType::DRIVE_TYPE_UNKNOWN_2) {

        log->info("Found secondary floppy drive");

        FloppyDevice *device = new FloppyDevice(*this, 1, secondaryDriveType, "fdd1");

        bool ret = resetDrive(*device);

        if(ret) {
            storageService->registerDevice(device);
        } else {
            delete device;
        }
    }
}

void FloppyController::writeFifoByte(uint8_t command) {
    uint32_t timeout = 0;

    while(timeout < FLOPPY_TIMEOUT) {
        if(isFifoBufferReady()) {
            fifoRegister.outb(command);

            return;
        }

        timeService->msleep(10);
        timeout += 10;
    }

    log->error("Timeout while issuing write command");
}

uint8_t FloppyController::readFifoByte() {
    uint32_t timeout = 0;

    while(timeout < FLOPPY_TIMEOUT) {
        if(isFifoBufferReady()) {
            return fifoRegister.inb();
        }

        timeService->msleep(10);
        timeout += 10;
    }

    log->error("Timeout while reading data from FIFO-buffer");

    return 0;
}

FloppyController::SenseInterruptState FloppyController::senseInterrupt() {
    SenseInterruptState ret{};

    writeFifoByte(COMMAND_SENSE_INTERRUPT);

    ret.statusRegister0 = readFifoByte();
    ret.currentCylinder = readFifoByte();

    return ret;
}

FloppyController::CommandStatus FloppyController::readCommandStatus() {
    CommandStatus ret{};

    ret.statusRegister0 = readFifoByte();
    ret.statusRegister1 = readFifoByte();
    ret.statusRegister2 = readFifoByte();
    ret.currentCylinder = readFifoByte();
    ret.currentHead = readFifoByte();
    ret.currentSector = readFifoByte();
    ret.bytesPerSector = readFifoByte();

    return ret;
}

void FloppyController::setMotorState(FloppyDevice &device, FloppyController::FloppyMotorState desiredState) {
    if(desiredState == FLOPPY_MOTOR_WAIT) {
        return;
    } else if(desiredState == FLOPPY_MOTOR_ON) {
        if(device.motorState == FLOPPY_MOTOR_ON) {
            return;
        } else if(device.motorState == FLOPPY_MOTOR_WAIT) {
            device.motorState = FLOPPY_MOTOR_ON;
            device.motorControlThread->timeout = FLOPPY_TIMEOUT;

            return;
        }

        digitalOutputRegister.outb(device.driveNumber | static_cast<uint8_t>(0x1c)); // Turn on the floppy motor
        timeService->msleep(1000); // Wait a second; This should be enough time, even for older drives

        device.motorState = FLOPPY_MOTOR_ON;
    } else if(desiredState == FLOPPY_MOTOR_OFF) {
        if(device.motorState == FLOPPY_MOTOR_OFF || device.motorState == FLOPPY_MOTOR_WAIT) {
            return;
        }

        device.motorState = FLOPPY_MOTOR_WAIT;
        device.motorControlThread->timeout = FLOPPY_TIMEOUT;
    }
}

void FloppyController::killMotor(FloppyDevice &device) {
    digitalOutputRegister.outb(device.driveNumber | static_cast<uint8_t>(0x0c));
    device.motorState = FLOPPY_MOTOR_OFF;
}

bool FloppyController::checkMedia(FloppyDevice &device) {
    setMotorState(device, FLOPPY_MOTOR_ON);

    // Check disk change indicator flag
    if(digitalInputRegister.inb() & 0x80) {
        // If the bit is set, a disk change has occurred and we need to check, if a disk is present
        // First we let the drive seek a cylinder, other than 0 and afterwards let it seek back to zero
        seek(device, 37, 0);
        seek(device, 0, 0);

        // If the bit is still set, there is no disk present in the drive
        // Otherwise, a disk swap has occurred
        return (digitalInputRegister.inb() & 0x80) == 0;
    } else {
        // If bit is not set, a disk is present
        return true;
    }
}

bool FloppyController::resetDrive(FloppyDevice &device) {
    log->trace("Resetting drive %u", device.driveNumber);

    receivedInterrupt = false;

    digitalOutputRegister.outb(0x00); // Disable controller;
    digitalOutputRegister.outb(device.driveNumber | static_cast<uint8_t>(0x0c)); // Enable controller;

    uint32_t timeout = 0;

    while(!receivedInterrupt) {
        timeService->msleep(10);
        timeout += 10;

        if(timeout > FLOPPY_TIMEOUT) {
            log->error("Timeout while resetting drive");

            return false;
        }
    }

    senseInterrupt();

    switch(device.driveType) {
        case DRIVE_TYPE_360KB_5_25 :
            configControlRegister.outb(0x02);
            break;
        case DRIVE_TYPE_1200KB_5_25 :
            configControlRegister.outb(0x00);
            break;
        case DRIVE_TYPE_720KB_3_5 :
            configControlRegister.outb(0x01);
            break;
        case DRIVE_TYPE_1440KB_3_5 :
            configControlRegister.outb(0x00);
            break;
        case DRIVE_TYPE_2880KB_3_5 :
            configControlRegister.outb(0x00);
            break;
        default :
            configControlRegister.outb(0x00);
            break;
    }

    writeFifoByte(COMMAND_SPECIFY);
    writeFifoByte(0xdf);
    writeFifoByte(0x02);

    bool ret = calibrateDrive(device);

    if(ret) {
        log->trace("Successfully reset drive %u", device.driveNumber);
    } else {
        log->error("Failed to reset  drive %u", device.driveNumber);
    }

    return ret;
}

bool FloppyController::calibrateDrive(FloppyDevice &device) {
    log->trace("Calibrating drive %u", device.driveNumber);

    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        writeFifoByte(COMMAND_RECALIBRATE);
        writeFifoByte(device.driveNumber);

        uint32_t timeout = 0;

        while(!receivedInterrupt && timeout < FLOPPY_TIMEOUT) {
            timeService->msleep(10);
            timeout += 10;
        }

        if(!receivedInterrupt) {
            continue;
        }

        SenseInterruptState interruptState = senseInterrupt();

        if((interruptState.statusRegister0 & 0xc0u) == 0xc0) {
            continue;
        }

        if(interruptState.currentCylinder == 0) {
            setMotorState(device, FLOPPY_MOTOR_OFF);

            log->trace("Successfully calibrated drive %u", device.driveNumber);

            return true;
        }
    }

    log->error("Failed to calibrate drive %u", device.driveNumber);

    setMotorState(device, FLOPPY_MOTOR_OFF);

    return false;
}

bool FloppyController::seek(FloppyDevice &device, uint8_t cylinder, uint8_t head) {
    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        writeFifoByte(COMMAND_SEEK);
        writeFifoByte(device.driveNumber | static_cast<uint8_t>(head << 2u));
        writeFifoByte(cylinder);
        uint32_t timeout = 0;

        while(!receivedInterrupt && timeout < FLOPPY_TIMEOUT) {
            timeService->msleep(10);
            timeout += 10;
        }

        if(!receivedInterrupt) {
            continue;
        }

        SenseInterruptState interruptState = senseInterrupt();

        if((interruptState.statusRegister0 & 0xc0u) == 0xc0) {
            continue;
        }

        if(interruptState.currentCylinder == cylinder) {
            setMotorState(device, FLOPPY_MOTOR_OFF);

            return true;
        }
    }

    log->error("Failed to seek on drive %u: Did not find cylinder %u", device.driveNumber, cylinder);

    setMotorState(device, FLOPPY_MOTOR_OFF);

    return false;
}

void FloppyController::plugin() {
    Kernel::InterruptDispatcher::getInstance().assign(Kernel::InterruptDispatcher::FLOPPY, *this);
    Pic::getInstance().allow(Pic::Interrupt::FLOPPY);
}

void FloppyController::trigger(Kernel::InterruptFrame &frame) {
    receivedInterrupt = true;
}

void FloppyController::prepareDma(FloppyDevice &device, Isa::TransferMode transferMode) {
    Isa::selectChannel(2);

    Isa::setAddress(2, (uint32_t) Kernel::Management::getInstance().getPhysicalAddress(dmaMemory));
    Isa::setCount(2, static_cast<uint16_t>(device.getSectorSize() - 1));
    Isa::setMode(2, transferMode, false, false, Isa::DMA_MODE_SINGLE_TRANSFER);

    Isa::deselectChannel(2);
}

bool FloppyController::readSector(FloppyDevice &device, uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector) {
    auto lastSector = static_cast<uint8_t>(sector + 1 > device.sectorsPerTrack ? device.sectorsPerTrack : sector + 1);

    if(!seek(device, cylinder, head)) {
        return false;
    }

    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        prepareDma(device, Isa::TRANSFER_MODE_WRITE);

        writeFifoByte(COMMAND_READ_DATA | FLAG_MULTITRACK | FLAG_MFM);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(cylinder);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(sector);
        writeFifoByte(2);
        writeFifoByte(lastSector);
        writeFifoByte(device.gapLength);
        writeFifoByte(0xff);

        uint32_t timeout = 0;

        while(!receivedInterrupt && timeout < FLOPPY_TIMEOUT) {
            timeService->msleep(10);
            timeout += 10;
        }

        if(!receivedInterrupt) {
            if(!handleReadWriteError(device, cylinder, head)) {
                return false;
            }

            continue;
        }

        CommandStatus status = readCommandStatus();

        if((status.statusRegister0 & 0xc0u) != 0) {
            if(!handleReadWriteError(device, cylinder, head)) {
                log->error("Failed to ready a sector on drive %u", device.driveNumber);

                return false;
            }

            continue;
        }

        memcpy(buff, dmaMemory, device.getSectorSize());

        setMotorState(device, FLOPPY_MOTOR_OFF);
        return true;
    }

    setMotorState(device, FLOPPY_MOTOR_OFF);
	
	log->error("Failed to ready a sector on drive %u", device.driveNumber);

    return false;
}

bool FloppyController::writeSector(FloppyDevice &device, const uint8_t *buff, uint8_t cylinder, uint8_t head, uint8_t sector) {
    auto lastSector = static_cast<uint8_t>(sector + 1 > device.sectorsPerTrack ? device.sectorsPerTrack : sector + 1);

    memcpy(dmaMemory, buff, device.getSectorSize());

    if(!seek(device, cylinder, head)) {
        return false;
    }

    setMotorState(device, FLOPPY_MOTOR_ON);

    for(uint8_t i = 0; i < FLOPPY_RETRY_COUNT; i++) {
        receivedInterrupt = false;

        prepareDma(device, Isa::TRANSFER_MODE_READ);

        writeFifoByte(COMMAND_WRITE_DATA | FLAG_MULTITRACK | FLAG_MFM);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(cylinder);
        writeFifoByte(device.driveNumber | (head << 2u));
        writeFifoByte(sector);
        writeFifoByte(2);
        writeFifoByte(lastSector);
        writeFifoByte(device.gapLength);
        writeFifoByte(0xff);

        uint32_t timeout = 0;

        while(!receivedInterrupt && timeout < FLOPPY_TIMEOUT) {
            timeService->msleep(10);
            timeout += 10;
        }

        if(!receivedInterrupt) {
            if(!handleReadWriteError(device, cylinder, head)) {
                return false;
            }

            continue;
        }

        CommandStatus status = readCommandStatus();

        if((status.statusRegister0 & 0xc0u) != 0) {
            if(!handleReadWriteError(device, cylinder, head)) {
                log->error("Failed to write a sector on drive %u", device.driveNumber);

                return false;
            }

            continue;
        }

        setMotorState(device, FLOPPY_MOTOR_OFF);

        return true;
    }

    setMotorState(device, FLOPPY_MOTOR_OFF);

    log->error("Failed to write a sector on drive %u", device.driveNumber);

    return false;
}

bool FloppyController::handleReadWriteError(FloppyDevice &device, uint8_t cylinder, uint8_t head) {
    if(checkMedia(device)) {
        calibrateDrive(device);

        if(!seek(device, 0, 0)) {
            return false;
        }

        return seek(device, cylinder, head);
    }

    return false;
}
