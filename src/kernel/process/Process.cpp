#include <lib/file/elf/Elf.h>
#include <lib/system/priority/SimplePriorityPattern.h>
#include <kernel/core/Management.h>
#include <lib/async/SimpleThread.h>
#include <lib/math/Random.h>
#include <kernel/multiboot/Structure.h>
#include <device/graphic/lfb/LinearFrameBuffer.h>
#include <kernel/thread/ApplicationMainThread.h>
#include "Process.h"
#include "ProcessScheduler.h"

int testColor = 0;
Spinlock testLock;

namespace Kernel {

IdGenerator Process::idGenerator;

Process::Process(Kernel::VirtualAddressSpace &addressSpace) : id(idGenerator.getId()), addressSpace(addressSpace) {
    scheduler = new ThreadScheduler(*new SimplePriorityPattern(5));

    /*for(int i = 0; i < 4; i++) {
        scheduler->ready(*new SimpleThread([]() {

            Kernel::Multiboot::FrameBufferInfo fbInfo = Kernel::Multiboot::Structure::getFrameBufferInfo();
            auto *genericLfb = new LinearFrameBuffer(fbInfo.address, static_cast<uint16_t>(fbInfo.width),
                                                     static_cast<uint16_t>(fbInfo.height), fbInfo.bpp,
                                                     static_cast<uint16_t>(fbInfo.pitch));

            testLock.acquire();
            Color color = testColor % 2 == 0 ? Colors::MAGENTA : Colors::CYAN;
            int startLine = testColor * fbInfo.height / 8;
            testColor++;
            testLock.release();

            int counter = 0;

            while (counter < (fbInfo.width * fbInfo.height) / 8) {
                genericLfb->drawPixel(counter % fbInfo.width, startLine + counter / fbInfo.width, color);
                counter++;
            }

            while (true);
        }));
    }*/
}

void Process::loadExecutable(const String &path) {
    auto elf = Elf::load(path);

    if (elf == nullptr) {
        Cpu::throwException(Cpu::Exception::INVALID_ARGUMENT, "Unable to load elf file!");
    }

    auto addressSpace = Kernel::Management::getInstance().createAddressSpace(elf->getSizeInMemory(),"FreeListMemoryManager");

    auto process = new Process(*addressSpace);

    process->getScheduler().ready(*new ApplicationMainThread(elf->getEntryPoint(), 0, nullptr));

    Kernel::ProcessScheduler::getInstance().ready(*process);
}

uint8_t Process::getPriority() const {
    return priority;
}

void Process::setPriority(uint8_t priority) {
    this->priority = ProcessScheduler::getInstance().changePriority(*this, priority);
}

bool Process::operator==(const Process &other) {
    return id == other.id;
}

Kernel::VirtualAddressSpace &Process::getAddressSpace() const {
    return addressSpace;
}

Kernel::Thread &Process::getCurrentThread() const {
    return scheduler->getCurrentThread();
}

void Process::yieldThread() {
    scheduler->yield();
}

void Process::ready(Thread &thread) {
    scheduler->ready(thread);
}

ThreadScheduler &Process::getScheduler() const {
    return *scheduler;
}

void Process::start() {
    ProcessScheduler::getInstance().ready(*this);
}

}