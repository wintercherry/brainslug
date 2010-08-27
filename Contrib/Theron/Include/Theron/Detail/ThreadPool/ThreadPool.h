// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_THREADPOOL_THREADPOOL_H
#define THERON_DETAIL_THREADPOOL_THREADPOOL_H

#pragma once


#include <new>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Containers/List.h>
#include <Theron/Detail/Containers/LockedQueue.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Threading/Lock.h>
#include <Theron/Detail/Threading/Monitor.h>
#include <Theron/Detail/Threading/Thread.h>

#include <Theron/Detail/ThreadPool/ExecutionState.h>

#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// A pool of worker threads.
template <class WorkItem, class Processor>
class ThreadPool
{
public:

    /// Constructor
    inline explicit ThreadPool(LockedQueue<WorkItem *> &workQueue);

    /// Initializes the worker threads
    inline void Start(const uint32_t numThreads);

    /// Terminates the worker threads
    inline void Stop();

private:

    /// Worker thread entry point function.
    /// Only global (static) functions can be used as thread entry points. Therefore this static method
    /// exists to wrap the non-static class method that is the real entry point.
    /// \param context Pointer to a context class that provides the context in which the thread is run.
    static void StaticWorkerThreadEntryPoint(void *const context);

    /// Disallowed copy constructor. ThreadPools can't be copied or assigned.
    ThreadPool(const ThreadPool &other);
    
    /// Disallowed assignment operator. ThreadPools can't be copied or assigned.
    ThreadPool &operator=(const ThreadPool &other);

    /// Worker thread function
    void WorkerThreadProc();
    
    bool mRunning;                          ///< Mode flag indicating whether the threads should be running.
    uint32_t mStartedCount;                 ///< Counts the number of threads properly started
    List<Thread *> mWorkerThreads;          ///< References the worker threads owned by the pool.
    LockedQueue<WorkItem *> &mWorkQueue;    ///< Reference to a thread-safe queue of waiting items.
};


template <class WorkItem, class Processor>
inline ThreadPool<WorkItem, Processor>::ThreadPool(LockedQueue<WorkItem *> &workQueue) :
  mRunning(false),
  mStartedCount(0),
  mWorkerThreads(),
  mWorkQueue(workQueue)
{
}


template <class WorkItem, class Processor>
inline void ThreadPool<WorkItem, Processor>::Start(const uint32_t numThreads)
{
    THERON_ASSERT(numThreads > 0);

    THERON_ASSERT(!mRunning);
    mRunning = true;
    mStartedCount = 0;

    // Create the worker threads. Each worker runs the same entry point function.
    for (uint32_t index = 0; index < numThreads; ++index)
    {
        // Allocate the memory using the provided allocator.
        void *const memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(Thread));
        THERON_ASSERT(memory);
        Thread *const thread = new (memory) Thread();
        THERON_ASSERT(thread);

        // Pass a pointer to this ThreadPool instance as context.
        thread->Start(StaticWorkerThreadEntryPoint, this);
        mWorkerThreads.Insert(thread);

        Metrics::Instance().Count(Metrics::EVENT_THREAD_CREATED);
    }

    // Wait until all workers have started completely before returning.
    // This guards against cases where the threads have already been told to
    // stop before they even get as far as checking the started flag, so they
    // terminate without ever checking the message queue.

    uint32_t numThreadsStarted(0);
    while (numThreadsStarted < numThreads)
    {
        Lock lock(mWorkQueue.GetMutex());
        numThreadsStarted = mStartedCount;
    }
}


template <class WorkItem, class Processor>
inline void ThreadPool<WorkItem, Processor>::Stop()
{
    THERON_ASSERT(mRunning);

    // Wake the worker threads and signal them to terminate.
    {
        Lock lock(mWorkQueue.GetMutex());

        mRunning = false;
        mWorkQueue.PulseAll();
    }

    // Wait for the worker threads to stop.
    for (List<Thread *>::iterator it = mWorkerThreads.Begin(); it != mWorkerThreads.End(); ++it)
    {
        Thread *const thread = (*it);
        thread->Join();

        // Explicitly call the destructor, since we allocated using placement new.
        thread->~Thread();
        AllocatorManager::Instance().GetAllocator()->Free(thread);

        Metrics::Instance().Count(Metrics::EVENT_THREAD_DESTROYED);
    }

    mWorkQueue.Clear();
    mWorkerThreads.Clear();
}


template <class WorkItem, class Processor>
inline void ThreadPool<WorkItem, Processor>::StaticWorkerThreadEntryPoint(void *const context)
{
    // The thread entry point has to be a static function,
    // so in this static wrapper function we call the non-static method
    // on the instance, which is provided as context.
    ThreadPool *const instance = static_cast<ThreadPool *const>(context);
    instance->WorkerThreadProc();
}


template <class WorkItem, class Processor>
inline void ThreadPool<WorkItem, Processor>::WorkerThreadProc()
{
    // This whole function is inside a lock-unlock pair. But the workers actually spend
    // most of their time outside the lock - either doing the processing of an item or
    // waiting for more work.
    Lock lock(mWorkQueue.GetMutex());

    // Increment the count of the number of threads started.
    // This count is checked, when starting the threads, to be sure
    // they've all started before continuing.
    ++mStartedCount;

    bool done(false);
    while (!done)
    {
        // Check the dirty list for work.
        if (!mWorkQueue.Empty())
        {
            // Get an item from the work queue.
            WorkItem *item = mWorkQueue.Peek();
            mWorkQueue.Pop();

            // Read the message from the actor's queue while we hold the lock.
            // We reuse the work queue lock to protect the actor message queues.
            IMessage *message(0);
            bool moreMessages(false);
            item->GetQueuedMessage(message, moreMessages);

            lock.Unlock();

            // Process the item, passing the pointer by reference.
            // If the item is deleted then the pointer is nulled.
            Processor::Process(item, message);

            lock.Relock();

            if (item)
            {
                // Re-add the item to the work queue, if it still needs more processing.
                if (moreMessages || item->State() == STATE_DIRTY)
                {
                    item->State() = STATE_BUSY;
                    mWorkQueue.Push(item);
                }
                else
                {
                    item->State() = STATE_IDLE;
                }
            }
        }
        else
        {
            if (mRunning)
            {
                // Wait for work to arrive or to be told to exit.
                // This releases the lock on the monitor and then re-aquires it when woken.
                mWorkQueue.Wait(lock);
                Metrics::Instance().Count(Metrics::EVENT_THREAD_WOKEN);
            }
            else
            {
                done = true;
            }
        }
    }
}


} // namespace Theron


#endif // THERON_DETAIL_THREADPOOL_THREADPOOL_H

