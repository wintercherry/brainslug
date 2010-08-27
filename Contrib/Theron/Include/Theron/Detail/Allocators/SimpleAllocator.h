// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_ALLOCATORS_SIMPLEALLOCATOR_H
#define THERON_DETAIL_ALLOCATORS_SIMPLEALLOCATOR_H


#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Threading/Mutex.h>

#include <Theron/IAllocator.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// A trivial general purpose memory allocator.
/// This is the allocator implementation used by default within Theron.
/// It is a simple wrapper around global new and delete.
/// Allocation counting is performed
/// in debug builds (where _DEBUG is defined) in order to detect memory leaks. This tracking
/// is made thread-safe by means of an internal Mutex that unavoidably adds to the
/// overhead of the tracking, in debug builds only.
/// \note Users can replace this default allocator by calling AllocatorManager::SetAllocator
/// and AllocatorManager::SetListAllocator.
/// \see AllocatorManager
/// \see PoolAllocator
class SimpleAllocator : public IAllocator
{
public:

    /// Default constructor
    inline SimpleAllocator();

    /// Virtual destructor.
    /// In debug builds the destructor checks that the number of de-allocations matches the
    /// number of allocations, and reports a failure with FAIL if it does not.
    inline virtual ~SimpleAllocator();

    /// Allocates a piece of contiguous memory.
    /// \param size The size of the memory to allocate, in bytes.
    /// \return A pointer to the allocated memory.
    inline virtual void *Allocate(const SizeType size);

    /// Frees a previously allocated piece of contiguous memory.
    /// \param memory Pointer to the memory to be deallocated.
    inline virtual void Free(void *const memory);

private:

    /// Disallowed copy constructor.
    SimpleAllocator(const SimpleAllocator &other);
    /// Disallowed assignment operator.
    SimpleAllocator &operator=(const SimpleAllocator &other);

#ifdef _DEBUG
    SizeType mAllocations;  ///< Tracks the number of allocations not yet freed, to detect memory leaks.
    Mutex mMutex;           ///< Critical section object used to protect access to the allocation count.
#endif // _DEBUG

};


inline SimpleAllocator::SimpleAllocator()
{

#ifdef _DEBUG
    mAllocations = 0;
#endif // _DEBUG

}


inline SimpleAllocator::~SimpleAllocator()
{

#ifdef _DEBUG
    // Memory leak detection. If this value isn't zero then the number of allocations
    // wasn't matched by an identical number of frees. If this occurs then it's probably
    // a bug in Theron rather than a bug in user code.
    if (mAllocations > 0)
    {
        THERON_FAIL_MSG("SimpleAllocator detected memory leaks");
    }
#endif // _DEBUG

}


inline void *SimpleAllocator::Allocate(const SizeType size)
{

    Metrics::Instance().Count(Metrics::EVENT_SIMPLEALLOCATOR_ALLOCATE);

#ifdef _DEBUG
    mMutex.Lock();
    ++mAllocations;
    mMutex.Unlock();
#endif // _DEBUG

    return new unsigned char[size];
}


inline void SimpleAllocator::Free(void *const memory)
{

    Metrics::Instance().Count(Metrics::EVENT_SIMPLEALLOCATOR_FREE);

#ifdef _DEBUG
    // Duplicate free detection. If this value is already zero then a piece of allocated
    // memory is freed twice (not necessarily this one!). If this occurs then it's probably
    // a bug in Theron rather than a bug in user code.
    mMutex.Lock();
    THERON_ASSERT(mAllocations > 0);
    --mAllocations;
    mMutex.Unlock();
#endif // _DEBUG

    delete [] static_cast<unsigned char *>(memory);
}


} // namespace Theron


#endif // THERON_DETAIL_ALLOCATORS_SIMPLEALLOCATOR_H

