// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CORE_MESSAGECACHE_H
#define THERON_DETAIL_CORE_MESSAGECACHE_H


#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Core/GlobalFreeList.h>
#include <Theron/Detail/Core/Pool.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// Caches message memory blocks to avoid frequent allocations.
class MessageCache
{
public:

    /// Maximum number of pools maintained at once.
    /// Each pool holds memory blocks of a specific size.
    /// \note This must always be three due to unrolled loops.
    static const uint32_t MAX_POOLS = 3;

    /// Number of memory blocks preloaded in each pool.
    static const uint32_t PRELOAD_LEVEL = THERON_ACTOR_MESSAGEPRELOAD;

    /// Constructor.
    inline MessageCache();

    /// Destructor.
    inline ~MessageCache();

    /// Allocates a memory block to hold a message of the given type.
    template <class MessageType>
    inline void *Allocate();

private:

    /// A pool of memory blocks of identical size, tagged with the size and time since last hit.
    struct TaggedPool
    {
        inline TaggedPool() :
          mBlockSize(0)
        {
        }

        uint32_t mBlockSize;            ///< Size in bytes of the memory blocks in this pool.
        Pool<PRELOAD_LEVEL> mPool;      ///< Pool of memory blocks, all of the same fixed size.
    };

    /// Allocates a memory block of the given size.
    inline void *AllocateBlock(const uint32_t size);

    /// Allocates a memory block from the given pool.
    inline void *AllocateFromPool(TaggedPool &pool);

    /// Refills the given pool and returns an allocated block from it.
    inline void *RefillPool(TaggedPool &pool);

    TaggedPool mPools[MAX_POOLS];       ///< Pools of memory blocks of different sizes.
    uint32_t mFirstFreePool;            ///< Index of first unassigned pool.
};


THERON_FORCEINLINE MessageCache::MessageCache() : mFirstFreePool(0)
{
    // Create the global free list, if it doesn't exist yet.
    GlobalFreeList::Reference();
}


THERON_FORCEINLINE MessageCache::~MessageCache()
{
    // Destroy the global free list, if we were the last to reference it.
    GlobalFreeList::Dereference();
}


template <class MessageType>
THERON_FORCEINLINE void *MessageCache::Allocate()
{
    return AllocateBlock(sizeof(MessageType));
}


THERON_FORCEINLINE void *MessageCache::AllocateBlock(const uint32_t size)
{
    // The idea is that this function doesn't need a mutex,
    // because Allocate() can only be called by one thread at a time.
    // So in the common case of a cache hit, no locks are performed.
    // And in the miss case, we read multiple blocks at once and
    // pay the locking overhead in the GlobalFreeList only once.

    // TODO: For now we just assign pools to message block sizes on a
    // first-come, first-served basis, until they're all assigned,
    // with no replacement.

    // Search for an existing pool with blocks of the right size.
    if (mPools[0].mBlockSize == size)
    {
        Metrics::Instance().Count(Metrics::EVENT_MESSAGECACHE_HIT);
        return AllocateFromPool(mPools[0]);
    }

    if (mPools[1].mBlockSize == size)
    {
        Metrics::Instance().Count(Metrics::EVENT_MESSAGECACHE_HIT);
        return AllocateFromPool(mPools[1]);
    }

    if (mPools[2].mBlockSize == size)
    {
        Metrics::Instance().Count(Metrics::EVENT_MESSAGECACHE_HIT);
        return AllocateFromPool(mPools[2]);
    }

    Metrics::Instance().Count(Metrics::EVENT_MESSAGECACHE_MISS);

    // Do we have any unassigned pools left?
    if (mFirstFreePool < MAX_POOLS)
    {
        // Assign the next pool, fill it, and allocate from it.
        TaggedPool &pool(mPools[mFirstFreePool++]);

        pool.mBlockSize = size;
        return AllocateFromPool(pool);
    }

    // No unassigned pools.
    // Pass the allocation through unhandled.
    return GlobalFreeList::Instance().Allocate(size);
}


THERON_FORCEINLINE void *MessageCache::AllocateFromPool(TaggedPool &pool)
{
    // Allocate from the pool if the pool is non-empty.
    // This is intended to be the fast common case.
    void *const block(pool.mPool.Fetch());
    if (block)
    {
        return block;
    }

    return RefillPool(pool);
}


// Intentionally not force-inlined
inline void *MessageCache::RefillPool(TaggedPool &pool)
{
    void *blocks[PRELOAD_LEVEL + 1];
    GlobalFreeList::Instance().AllocateMany(
        blocks,
        PRELOAD_LEVEL + 1,
        pool.mBlockSize);

    for (uint32_t index = 0; index < PRELOAD_LEVEL; ++index)
    {
        // TODO: Could do with a fast version of Pool with no internal checking.
        THERON_ASSERT(blocks[index]);
        pool.mPool.Add(blocks[index]);
    }

    // Return the extra block.
    THERON_ASSERT(blocks[PRELOAD_LEVEL]);
    return blocks[PRELOAD_LEVEL];
}


} // namespace Theron


#endif // THERON_DETAIL_CORE_MESSAGECACHE_H

