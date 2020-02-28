#ifndef HHUOS_THREADSCHEDULER_H
#define HHUOS_THREADSCHEDULER_H

#include <kernel/thread/Thread.h>
#include <lib/util/ArrayBlockingQueue.h>
#include <lib/async/Spinlock.h>
#include "lib/system/priority/PriorityPattern.h"

namespace Kernel {

class ThreadScheduler {

    friend class Process;
    friend class ProcessScheduler;
    
public:

    explicit ThreadScheduler(PriorityPattern &priority);

    ThreadScheduler(const ThreadScheduler &copy) = delete;

    ThreadScheduler &operator=(const ThreadScheduler &copy) = delete;

    ~ThreadScheduler() = default;

    /**
     * Registers a new Thread.
     *
     * @param that A Thread.
     */
    void ready(Thread &that);

    /**
     * Terminates the current Thread.
     */
    void exit();

    /**
     * Kills a specific Thread.
     *
     * @param that A Thread
     */
    void kill(Thread &that);

    /**
     * Indicates if a Thread is waiting for execution.
     *
     * @return true, if a Thread is waiting, false else
     */
    bool isThreadWaiting();

    /**
     * Unblocks a specific Thread.
     *
     * @param that A Thread
     */
    void deblock(Thread &that);

    /**
     * Returns the active Thread.
     *
     * @return The active Thread
     */
    Thread &getCurrentThread();

    /**
     * Returns the number of active Threads.
     *
     * @return The number of active Threads
     */
    uint32_t getThreadCount();

    uint8_t changePriority(Thread &thread, uint8_t priority);

    Thread *getNextThread();

    uint8_t getMaxPriority();

private:

    /**
     * Blocks the current Thread.
     */
    void block();

    /**
     * Switches to the next Thread.
     */
    void yield();

    void yield(Thread& oldThread);

    /**
     * Switches to the given Thread.
     *
     * @param next A Thread.
     */
    void dispatch(Thread &current, Thread &next);

private:

    Thread *currentThread;

    PriorityPattern &priority;

    Spinlock lock;

    Util::Array<Util::ArrayBlockingQueue<Thread *>> readyQueues;
};

}

#endif
