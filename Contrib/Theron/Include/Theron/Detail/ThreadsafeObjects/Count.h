// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_THREADSAFEOBJECTS_COUNT_H
#define THERON_DETAIL_THREADSAFEOBJECTS_COUNT_H


#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Threading/Mutex.h>

#include <Theron/Defines.h>


namespace Theron
{


/// Thread-safe class representing a non-zero integer count.
/// Used as a reference count, typically.
class Count
{
public:

    /// Constructor.
    THERON_FORCEINLINE Count(const int count) :
      mCount(count),
      mMutex()
    {
    }

    /// Locks the reference count, providing thread-safe access.
    THERON_FORCEINLINE void Lock() const
    {
        mMutex.Lock();
    }

    /// Unlocks the reference count after thread-safe access.
    THERON_FORCEINLINE void Unlock() const
    {
        mMutex.Unlock();
    }

    THERON_FORCEINLINE void Increment()
    {
        ++mCount;
    }

    THERON_FORCEINLINE void Decrement()
    {
        THERON_ASSERT(mCount > 0);
        --mCount;
    }

    THERON_FORCEINLINE int Value() const
    {
        return mCount;
    }

private:

    /// Disallowed copy-constructor.
    Count(const Count &other);

    /// Disallowed assignment operator.
    Count &operator=(const Count &other);

    int mCount;                         ///< Current count value.
    mutable Mutex mMutex;               ///< Used to protect access to the count.
};


} // namespace Theron


#endif // THERON_DETAIL_THREADSAFEOBJECTS_COUNT_H

