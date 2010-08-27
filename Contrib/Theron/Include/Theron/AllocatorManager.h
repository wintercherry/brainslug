// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_ALLOCATORMANAGER_H
#define THERON_ALLOCATORMANAGER_H


/**
\file AllocatorManager.h
Manager for allocators used within Theron.
*/


#include <Theron/Detail/Allocators/SimpleAllocator.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Defines.h>
#include <Theron/IAllocator.h>


namespace Theron
{


/// \brief Singleton class that manages allocators for use by Theron.
class AllocatorManager
{
public:

    /// \brief Returns a reference to the AllocatorManager singleton instance.
    static AllocatorManager &Instance();

    /// \brief Sets the allocator used for internal allocations, replacing the default allocator.
    /// Calling this method allows applications to provide custom allocators and hence to control
    /// how and where memory is allocated by Theron. This is useful, for example, in embedded systems
    /// where memory is scarce or of different available types. The allocator provided to this method
    /// must implement IAllocator and can be a wrapper around another existing allocator implementation.
    /// The provided allocator is used for all internal heap allocations, including the allocation of
    /// instantiated actors. If this method is not called by user code then a default SimpleAllocator
    /// is used, which is a trivial wrapper around global new and delete.
    /// \see GetAllocator
    inline void SetAllocator(IAllocator *const allocator)
    {
        // Can't call SetAllocator after allocations have been performed!
        THERON_ASSERT(mAllocator == 0);
        THERON_ASSERT(allocator != 0);

        mAllocator = allocator;
    }

    /// \brief Gets a pointer to the general allocator used by Theron.
    /// \see SetAllocator
    THERON_FORCEINLINE IAllocator *GetAllocator()
    {
        if (mAllocator == 0)
        {
            return &mDefaultAllocator;
        }

        return mAllocator;
    }

private:

    /// Default constructor. Private, since the AllocatorManager is a singleton class.
    inline AllocatorManager() :
       mAllocator(0),
       mDefaultAllocator()
    {
    }

    /// Disallowed copy constructor.
    AllocatorManager(const AllocatorManager &other);
    
    /// Disallowed assignment operator.
    AllocatorManager &operator=(const AllocatorManager &other);

    static AllocatorManager sInstance;  ///< The single, static instance.

    IAllocator *mAllocator;             ///< Pointer to a general allocator for use in internal allocations.
    SimpleAllocator mDefaultAllocator;  ///< Default allocator used if none is explicitly set.
};


} // namespace Theron


#endif // THERON_ALLOCATORMANAGER_H

