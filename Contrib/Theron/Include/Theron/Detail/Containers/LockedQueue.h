// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CORE_LOCKEDQUEUE_H
#define THERON_DETAIL_CORE_LOCKEDQUEUE_H


#include <Theron/Detail/Containers/Queue.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Threading/Monitor.h>

#include <Theron/Defines.h>


namespace Theron
{


/// A threadsafe queue.
template <class ItemType>
class LockedQueue
{
public:

    /// Constructor.
    inline LockedQueue()
    {
    }

    /// Returns a reference to the Mutex owned by the queue.
    THERON_FORCEINLINE Mutex &GetMutex() const
    {
        return mMonitor.GetMutex();
    }

    /// Waits for the queue to be pulsed via Pulse or PulseAll.
    /// The calling thread should hold the lock on the queue via Lock first.
    /// The calling thread is blocked until another thread wakes it.
    /// The lock owned by the caller is released, and regained when the thread is woken.
    THERON_FORCEINLINE void Wait(Lock &lock) const
    {
        mMonitor.Wait(lock);
    }

    /// Pulses the monitor, waking a single waiting thread.
    /// \note
    /// The calling thread should own a lock on the queue. When the calling thread
    /// releases the lock, a woken thread aquires the lock and proceeds.
    THERON_FORCEINLINE void Pulse() const
    {
        mMonitor.Pulse();
    }

    /// Pulses the monitor, waking all waiting threads.
    /// \note
    /// The calling thread should own a lock on the queue. When the calling thread
    /// releases the lock, a woken thread aquires the lock and proceeds.
    THERON_FORCEINLINE void PulseAll() const
    {
        mMonitor.PulseAll();
    }

    /// Returns true if the queue contains no items.
    /// \note The caller should lock the queue first.    
    THERON_FORCEINLINE bool Empty() const
    {
        return mQueue.Empty();
    }

    /// Empties the queue, removing all previously inserted items.
    THERON_FORCEINLINE void Clear()
    {
        mQueue.Clear();
    }

    /// Pushes an item into the queue. The new item is copied by reference and added at the back.
    /// \note The caller should lock the queue first.    
    THERON_FORCEINLINE void Push(const ItemType &item)
    {
        mQueue.Push(item);
    }

    /// Peeks at the value of the first item on the queue.
    /// \note The caller should lock the queue first.    
    THERON_FORCEINLINE const ItemType &Peek() const
    {
        THERON_ASSERT(!mQueue.Empty());
        return mQueue.Peek();
    }

    /// Removes the item at the front of the queue.
    /// \note The caller should lock the queue first.    
    THERON_FORCEINLINE void Pop()
    {
        THERON_ASSERT(!mQueue.Empty());
        mQueue.Pop();
    }

private:

    mutable Monitor mMonitor;       ///< Synchronizes access to the queue.
    Queue<ItemType> mQueue;         ///< Internal queue implementation.
};


} // namespace Theron


#endif // THERON_DETAIL_CORE_LOCKEDQUEUE_H

