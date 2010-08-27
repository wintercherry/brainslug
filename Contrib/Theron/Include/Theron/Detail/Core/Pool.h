// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CORE_POOL_H
#define THERON_DETAIL_CORE_POOL_H


#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/IAllocator.h>


namespace Theron
{


/// A list of free memory blocks of identical sizes.
/// Maximum number of memory blocks the pool can hold is set by MAX_BLOCKS.
/// Once the pool is full, further blocks are discarded.
/// \note This is a template purely to avoid storing the size in every pool.
template <uint32_t MAX_BLOCKS>
class Pool
{
public:

    /// Constructor.
    inline Pool();

    /// Destructor.
    inline ~Pool();

    /// Adds a memory block to the pool.
    /// \return True, if the block was added to the pool.
    /// \note The pools has a limited maximum size, so may be full.
    inline bool Add(void *memory);

    /// Retreives a memory block from the pool.
    /// \return Zero if no blocks in pool.
    inline void *Fetch();

    /// Returns true if the pool contains no blocks.
    inline bool Empty() const;

    /// Empties the pool, freeing all stored memory blocks.
    inline void Clear();
    
private:

    /// A node representing a free memory block within the pool.
    struct Node
    {
        Node *mNext;                ///< Pointer to the next node in the pool.
    };

    uint32_t mSize;                 ///< Number of memory blocks in the pool.
    Node *mFirst;                   ///< Pointer to first in a list of free messages.
};


template <uint32_t MAX_BLOCKS>
THERON_FORCEINLINE Pool<MAX_BLOCKS>::Pool() :
  mSize(0),
  mFirst(0)
{
}


template <uint32_t MAX_BLOCKS>
inline Pool<MAX_BLOCKS>::~Pool()
{
    Clear();
}


template <uint32_t MAX_BLOCKS>
THERON_FORCEINLINE bool Pool<MAX_BLOCKS>::Add(void *const memory)
{
    THERON_ASSERT(memory);

    if (mSize < MAX_BLOCKS)
    {
        // Convert the object to a pool node and add it to the pool.
        Node *const node(reinterpret_cast<Node *>(memory));
        node->mNext = mFirst;
        mFirst = node;

        ++mSize;
        return true;
    }

    // Pool is full.
    return false;
}


template <uint32_t MAX_BLOCKS>
THERON_FORCEINLINE void *Pool<MAX_BLOCKS>::Fetch()
{
    if (mFirst)
    {
        // Remove the first memory block from the pool.
        Node *const node(mFirst);
        mFirst = mFirst->mNext;

        --mSize;
        return reinterpret_cast<void *>(node);
    }

    // Zero result indicates failure.
    return 0;
}


template <uint32_t MAX_BLOCKS>
THERON_FORCEINLINE bool Pool<MAX_BLOCKS>::Empty() const
{
    return (mFirst == 0);
}


template <uint32_t MAX_BLOCKS>
inline void Pool<MAX_BLOCKS>::Clear()
{
    IAllocator *const allocator(AllocatorManager::Instance().GetAllocator());

    // Free all allocated blocks.
    Node *node(mFirst);
    while (node)
    {
        Node *const temp(node);
        node = node->mNext;
        allocator->Free(temp);
    }
}


} // namespace Theron


#endif // THERON_DETAIL_CORE_POOL_H

