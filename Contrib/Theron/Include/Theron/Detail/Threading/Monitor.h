// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_THREADING_BOOST_MONITOR_H
#define THERON_DETAIL_THREADING_BOOST_MONITOR_H


#ifdef _MSC_VER
#pragma warning(push,0)
#endif //_MSC_VER

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#include <Theron/Detail/Debug/Assert.h>

#include "Lock.h"
#include "Mutex.h"

#include <Theron/Defines.h>


namespace Theron
{


/// Monitor class implementing a C#/Java-style Monitor primitive.
/// \note This implementation uses Boost threads so depends on the Boost library
class Monitor
{
public:

    /// Default constructor
    THERON_FORCEINLINE Monitor() :
      mMutex(),
      mCond()
    {
    }

    /// Returns a reference to the Mutex owned by the condition.
    THERON_FORCEINLINE Mutex &GetMutex()
    {
        return mMutex;
    }

    /// Waits for the monitor to be pulsed via Pulse or PulseAll.
    /// The calling thread should hold a lock on the mutex.
    /// The calling thread is blocked until another thread wakes it.
    /// The lock owned by the caller is released, and regained when the thread is woken.
    THERON_FORCEINLINE void Wait(Lock &lock)
    {
        // The lock must be .. er .. locked, before calling Wait().
        THERON_ASSERT(lock.GetLock().owns_lock());
        mCond.wait(lock.GetLock());
    }

    /// Pulses the monitor, waking a single waiting thread.
    /// \note
    /// The calling thread should own a lock on the mutex. When the calling thread
    /// releases the lock, a woken thread aquires the lock and proceeds.
    THERON_FORCEINLINE void Pulse()
    {
        mCond.notify_one();
    }

    /// Pulses the monitor, waking all waiting threads.
    /// \note
    /// The calling thread should own the lock on the mutex. When the calling thread
    /// releases the lock, a woken thread aquires the lock and proceeds.
    THERON_FORCEINLINE void PulseAll()
    {
        mCond.notify_all();
    }

private:

    /// Disallowed copy constructor. Monitor objects can't be copied.
    Monitor(const Monitor &other);
    
    /// Disallowed assignment operator. Monitor objects can't be assigned.
    Monitor &operator=(const Monitor &other);

    Mutex mMutex;                                   ///< Owned mutex
    boost::condition_variable mCond;                ///< Owned condition variable.
};


} // namespace Theron


#endif // THERON_DETAIL_THREADING_BOOST_MONITOR_H

