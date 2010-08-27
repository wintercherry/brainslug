// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CORE_GLOBALFREELIST_H
#define THERON_DETAIL_CORE_GLOBALFREELIST_H


#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Core/Pool.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Threading/Lock.h>
#include <Theron/Detail/Threading/Mutex.h>

#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// Manages a global list of free memory blocks of different sizes.
class GlobalFreeList
{
public:

    /// Number of memory block pools maintained.
    /// Each pool holds memory blocks of a specific size.
    static const uint32_t MAX_POOLS = THERON_GLOBALFREELIST_POOLS;

    /// Maximum number of memory blocks in each pool.
    static const uint32_t MAX_BLOCKS_PER_POOL = THERON_GLOBALFREELIST_MAXPOOLSIZE;

    /// References the singleton instance (and creates it on first reference).
    inline static void Reference();

    /// Dereferences the singleton instance (and destroys it on last reference).
    inline static void Dereference();

    /// Gets a reference to the single directory object.
    inline static GlobalFreeList &Instance();

    /// Allocates a memory block of the given size.
    inline void *Allocate(const uint32_t size);

    /// Allocates n memory blocks of the same size.
    inline void AllocateMany(void *blocks[], const uint32_t count, const uint32_t size);

    /// Frees a previously allocated object.
    /// \param block Pointer to the memory block containing the object.
    /// \param size The size of the memory block containing the object.
    inline void Free(void *const block, const uint32_t size);

private:

    /// A thread-safe pool of memory blocks of identical size.
    struct LockedPool
    {
        Mutex mMutex;                       ///< Thread synchronization for internal state.
        Pool<MAX_BLOCKS_PER_POOL> mPool;    ///< Pool of memory blocks, all of the same fixed size.
    };

    /// Creates the singleton instance.
    /// \note This method is intentionally non-inlined.
    static void CreateInstance();

    /// Destroys the singleton instance.
    /// \note This method is intentionally non-inlined.
    static void DestroyInstance();

    /// Private constructor.
    inline GlobalFreeList();

    /// Disallowed copy constructor.
    GlobalFreeList(const GlobalFreeList &other);

    /// Disallowed assignment operator.
    GlobalFreeList &operator=(const GlobalFreeList &other);

    /// Maps an allocation block size to a specific pool.
    inline uint32_t MapSizeToPool(const uint32_t size);

    static Mutex smMutex;               ///< Synchronizes access to the singleton instance.
    static uint32_t smReferenceCount;   ///< Counts how many times the instance is referenced.
    static GlobalFreeList *smInstance;  ///< Pointer to the single, static instance.

    LockedPool mPools[MAX_POOLS];       ///< Pools of memory blocks of different sizes.
};


THERON_FORCEINLINE void GlobalFreeList::Reference()
{
    Lock lock(smMutex);

    // Create the singleton instance if it doesn't already exist.
    if (smReferenceCount++ == 0)
    {
        // Non-inlined call, uncommon case.
        CreateInstance();
    }
}


THERON_FORCEINLINE void GlobalFreeList::Dereference()
{
    Lock lock(smMutex);

    // Destroy the singleton instance if it has become unreferenced.
    // This ensures the memory is deallocated correctly prior to program termination.
    if (--smReferenceCount == 0)
    {
        // Non-inlined call, uncommon case.
        DestroyInstance();
    }
}


THERON_FORCEINLINE GlobalFreeList &GlobalFreeList::Instance()
{
    THERON_ASSERT(smInstance);
    return *smInstance;
}


THERON_FORCEINLINE GlobalFreeList::GlobalFreeList()
{
}


inline void *GlobalFreeList::Allocate(const uint32_t size)
{
    void *block(0);

    // Map the block size to a specific pool.
    const uint32_t poolIndex(MapSizeToPool(size));

    // We only bother with pools up to a certain maximum block size.
    if (poolIndex < MAX_POOLS)
    {
        LockedPool &pool(mPools[poolIndex]);

        // Early-reject empty pools without locking the mutex.
        if (!pool.mPool.Empty())
        {
            // We have a separate mutex per pool to minimize contention.
            Lock lock(pool.mMutex);

            // Note the pool may turn out to be empty now, and return null.
            block = pool.mPool.Fetch();
        }
    }

    if (block)
    {
        Metrics::Instance().Count(Metrics::EVENT_GLOBALFREELIST_HIT);
    }
    else
    {
        // Allocate a new memory block.
        Metrics::Instance().Count(Metrics::EVENT_GLOBALFREELIST_MISS);
        block = AllocatorManager::Instance().GetAllocator()->Allocate(size);
    }

    THERON_ASSERT(block);
    return block;
}


THERON_FORCEINLINE void GlobalFreeList::AllocateMany(void *blocks[], const uint32_t count, const uint32_t size)
{
    uint32_t blockIndex(0);

    // Map the block size to a specific pool.
    const uint32_t poolIndex(MapSizeToPool(size));

    // We only bother with pools up to a certain maximum block size.
    if (poolIndex < MAX_POOLS)
    {
        LockedPool &pool(mPools[poolIndex]);

        // Early-reject empty pools without locking the mutex.
        if (!pool.mPool.Empty())
        {
            // We have a separate mutex per pool to minimize contention.
            Lock lock(pool.mMutex);

            while (blockIndex < count && !pool.mPool.Empty())
            {
                // TODO: We could replace the if in Fetch with an assert?
                blocks[blockIndex++] = pool.mPool.Fetch();
                Metrics::Instance().Count(Metrics::EVENT_GLOBALFREELIST_HIT);
            }
        }
    }

    if (blockIndex == count)
    {
        return;
    }

    // Perform real allocations for any unfilled requests.
    IAllocator *const allocator(AllocatorManager::Instance().GetAllocator());
    while (blockIndex < count)
    {
        Metrics::Instance().Count(Metrics::EVENT_GLOBALFREELIST_MISS);
        blocks[blockIndex++] = allocator->Allocate(size);
    }
}


inline void GlobalFreeList::Free(void *const block, const uint32_t size)
{
    bool added(false);

    // Map the block size to a specific pool.
    const uint32_t poolIndex(MapSizeToPool(size));

    // We only bother with pools up to a certain maximum block size.
    if (poolIndex < MAX_POOLS)
    {
        LockedPool &pool(mPools[poolIndex]);
        Lock lock(pool.mMutex);

        // Try to add the block to the pool.
        added = pool.mPool.Add(block);
    }

    if (!added)
    {
        // If we failed to add the block to any pool then we finally free the block.
        AllocatorManager::Instance().GetAllocator()->Free(block);
    }
}


THERON_FORCEINLINE uint32_t GlobalFreeList::MapSizeToPool(const uint32_t size)
{
    // We assume all allocations are multiples of four, due to default integer alignment.
    // We also assume all allocations are for Message classes, so at least 8 bytes in size.
    // However adding three to the size before rounding down 
    // probably saves the day anyway, effectively rounding up.
    THERON_ASSERT(size >= 8);
    THERON_ASSERT(size % 4 == 0);
    return (((size + 3) >> 2) - 2);
}


} // namespace Theron


#endif // THERON_DETAIL_CORE_GLOBALFREELIST_H

