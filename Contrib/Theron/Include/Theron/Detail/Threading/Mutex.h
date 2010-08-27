// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_THREADING_BOOST_MUTEX_H
#define THERON_DETAIL_THREADING_BOOST_MUTEX_H


#ifdef _MSC_VER
#pragma warning(push,0)
#endif //_MSC_VER

#include <boost/thread/mutex.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#include <Theron/Defines.h>


namespace Theron
{


/// A simple mutex class used internally for thread synchronization.
/// \note This implementation uses Boost threads so depends on the Boost library
class Mutex
{
public:

    friend class Lock;

    /// Default constructor.
    THERON_FORCEINLINE Mutex() : mMutex()
    {
    }

    /// Locks the mutex, guaranteeing exclusive access to a protected resource associated with it.
    /// \note This is a blocking call and should be used with care to avoid deadlocks.
    THERON_FORCEINLINE void Lock()
    {
        mMutex.lock();
    }

    /// Unlocks the mutex, releasing exclusive access to a protected resource associated with it.
    THERON_FORCEINLINE void Unlock()
    {
        mMutex.unlock();
    }

private:

    /// Returns a reference to the owned boost::mutex.
    THERON_FORCEINLINE boost::mutex &GetMutex()
    {
        return mMutex;
    }

    /// Disallowed copy constructor.
    Mutex(const Mutex &other);
    
    /// Disallowed assignment operator.
    Mutex &operator=(const Mutex &other);

    boost::mutex mMutex;                ///< Owned boost::mutex
};


} // namespace Theron


#endif // THERON_DETAIL_THREADING_BOOST_MUTEX_H
