/*
 * Copyright (C) 2018  Filip Krakowski
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __EventBus_include__
#define __EventBus_include__


#include <lib/util/ArrayList.h>
#include <lib/util/HashMap.h>
#include <lib/util/RingBuffer.h>
#include "kernel/Spinlock.h"
#include "kernel/threads/Thread.h"
#include "kernel/KernelService.h"
#include "kernel/events/Receiver.h"
#include <kernel/events/EventPublisher.h>

class LinearFrameBuffer;

/**
 * Publishes Events to previously registered Receivers.
 *
 * @author Filip Krakowski
 */
class EventBus : public Thread, public KernelService {

public:

    EventBus();

    EventBus(const EventBus &other) = delete;

    EventBus &operator=(const EventBus &other) = delete;

    ~EventBus() = default;

    /**
     * Subscribes to a certain type of Event.
     *
     * @param receiver The Receiver
     * @param type The Event type
     */
    void subscribe(Receiver &receiver, uint32_t type);

    /**
     * Unsubscribes from a certain type of Event.
     *
     * @param receiver The Receiver
     * @param type The Event type
     */
    void unsubscribe(const Receiver &receiver, uint32_t type);

    /**
     * Publishes an Event.
     *
     * @param event The Event
     */
    void publish(const Event &event);

    void run () override;

    static constexpr char* SERVICE_NAME = "EventBus";

private:

    Util::ArrayList<EventPublisher*> publishers[1024];

    Util::HashMap<const Receiver*, EventPublisher*> receiverMap;

    Util::RingBuffer<const Event*> eventBuffer;

    LinearFrameBuffer *g2d;

    Spinlock lock;

    bool isRunning = true;

    void notify();

    bool isInitialized = false;
};


#endif
