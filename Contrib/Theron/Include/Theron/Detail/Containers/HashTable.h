// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CONTAINERS_HASHTABLE_H
#define THERON_DETAIL_CONTAINERS_HASHTABLE_H

#pragma once


#include <new>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Containers/List.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>


namespace Theron
{


/// A simple hash table implementation.
template <class ItemType>
class HashTable
{
public:

    /// List of pointers to items.
    typedef List<ItemType> ItemList;

    /// Constructor.
    inline explicit HashTable(const uint32_t hashSize);

    /// Destructor.
    inline ~HashTable();

    /// Inserts an entry into the table.
    inline void Insert(const uint32_t key, const ItemType &item);

    /// Removes an entry from the table.
    inline bool Remove(const uint32_t key, const ItemType &item);

    /// Returns a list of all items associated with the given key.
    /// \note Due to the use of hashing, there can in general be
    /// many items associated with the same key. It's the caller's
    /// responsibility to interpret the returned list.
    inline const ItemList &Items(const uint32_t key) const;

    /// Returns the number of items in the container.
    /// \note This performs an actual count, so is expensive.
    inline uint32_t Count() const;

    /// Removes all items from the table.
    inline void Clear();

private:

    /// Disallowed copy constructor.
    HashTable(const HashTable &other);
    
    /// Disallowed assignment operator.
    HashTable &operator=(const HashTable &other);

    /// Computes an integer hash of an integer key.
    inline uint32_t Hash(const uint32_t key) const;

    uint32_t mHashSize;                 ///< Width of the hash table.
    ItemList **mItemLists;              ///< Array of pointers to item lists.
};


template <class ItemType>
inline HashTable<ItemType>::HashTable(const uint32_t hashSize) :
  mHashSize(hashSize)
{
    // We allocate an extra four bytes for the count that many compilers prepend to the array.
    void *memory = AllocatorManager::Instance().GetAllocator()->Allocate(mHashSize * sizeof(ItemList *) + 4);
    THERON_ASSERT(memory);

    mItemLists = new (memory) ItemList *[mHashSize];
    THERON_ASSERT(mItemLists);

    // Allocate and construct the lists.
    for (uint32_t index = 0; index < mHashSize; ++index)
    {
        memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(ItemList));
        THERON_ASSERT(memory);

        mItemLists[index] = new (memory) ItemList;
        THERON_ASSERT(mItemLists[index]);
    }
}


template <class ItemType>
inline HashTable<ItemType>::~HashTable()
{
    // Destruct and free the lists.
    for (uint32_t index = 0; index < mHashSize; ++index)
    {
        // Call the list destructor explicitly
        mItemLists[index]->~ItemList();
        AllocatorManager::Instance().GetAllocator()->Free(mItemLists[index]);
    }

    // Finally free the array of list pointers.
    AllocatorManager::Instance().GetAllocator()->Free(mItemLists);
}


template <class ItemType>
THERON_FORCEINLINE void HashTable<ItemType>::Insert(const uint32_t key, const ItemType &item)
{
    const uint32_t hash(Hash(key));
    THERON_ASSERT(hash < mHashSize);
    ItemList &itemList(*mItemLists[hash]);
    itemList.Insert(item);
}


template <class ItemType>
THERON_FORCEINLINE bool HashTable<ItemType>::Remove(const uint32_t key, const ItemType &item)
{
    const uint32_t hash(Hash(key));
    THERON_ASSERT(hash < mHashSize);
    ItemList &itemList(*mItemLists[hash]);
    return itemList.Remove(item);
}


template <class ItemType>
THERON_FORCEINLINE const typename HashTable<ItemType>::ItemList &HashTable<ItemType>::Items(const uint32_t key) const
{
    const uint32_t hash(Hash(key));
    THERON_ASSERT(hash < mHashSize);
    return *mItemLists[hash];
}


template <class ItemType>
THERON_FORCEINLINE uint32_t HashTable<ItemType>::Hash(const uint32_t key) const
{
    // We use a trivial mod as the hash function.
    return (key % mHashSize);
}


template <class ItemType>
THERON_FORCEINLINE uint32_t HashTable<ItemType>::Count() const
{
    uint32_t count(0);
    for (uint32_t index = 0; index < mHashSize; ++index)
    {
        count += mItemLists[index]->Count();
    }

    return count;
}


template <class ItemType>
THERON_FORCEINLINE void HashTable<ItemType>::Clear()
{
    for (uint32_t index = 0; index < mHashSize; ++index)
    {
        mItemLists[index]->Clear();
    }
}


} // namespace Theron


#endif // THERON_DETAIL_CONTAINERS_HASHTABLE_H

