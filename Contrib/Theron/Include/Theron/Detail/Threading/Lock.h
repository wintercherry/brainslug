// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_THREADING_BOOST_LOCK_H
#define THERON_DETAIL_THREADING_BOOST_LOCK_H


#ifdef _MSC_VER
#pragma warning(push,0)
#endif //_MSC_VER

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#include <Theron/Detail/Debug/Assert.h>
#include "Mutex.h"
#include <Theron/Defines.h>


namespace Theron
{


/// Lock object that wraps a Mutex for use with a Monitor.
class Lock
{
public:

    friend class Monitor;

    /// Constructor.
    /// Creates a locked lock around the given boost::mutex.
    THERON_FORCEINLINE explicit Lock(Mutex &mutex) : mLock(mutex.GetMutex())
    {
    }

    /// Relocks the lock
    THERON_FORCEINLINE void Relock()
    {
        THERON_ASSERT(mLock.owns_lock() == false);
        mLock.lock();
    }

    /// Unlocks the lock
    THERON_FORCEINLINE void Unlock()
    {
        THERON_ASSERT(mLock.owns_lock() == true);
        mLock.unlock();
    }

private:

    /// Disallowed copy constructor. Lock objects can't be copied.
    Lock(const Lock &other);
    
    /// Disallowed assignment operator. Lock objects can't be assigned.
    Lock &operator=(const Lock &other);

    /// Returns a reference to the wrapped boost::unique_lock.
    THERON_FORCEINLINE boost::unique_lock<boost::mutex> &GetLock()
    {
        return mLock;
    }

    boost::unique_lock<boost::mutex> mLock;     ///< Wrapped boost::unique_lock.
};


} // namespace Theron


#endif // THERON_DETAIL_THREADING_BOOST_LOCK_H

