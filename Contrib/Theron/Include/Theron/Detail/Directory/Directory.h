// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_DIRECTORY_DIRECTORY_H
#define THERON_DETAIL_DIRECTORY_DIRECTORY_H

#pragma once


#include <new>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Containers/HashTable.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Directory/IEntity.h>

#include <Theron/Detail/Threading/Mutex.h>

#include <Theron/Address.h>
#include <Theron/Defines.h>


namespace Theron
{


/// A directory mapping unique addresses to addressable entities.
/// The addressable entities include actors and receivers.
class Directory
{
public:

    // TODO: Use a better dictionary table implementation.
    // TODO: Make dictionary hash table user-sizeable.
    /// Size of the internal hash table.
    static const uint32_t HASH_SIZE = 100;

    /// Locks the directory singleton for exclusive thread access.
    inline static void Lock();

    /// Unlocks exclusive thread access to the directory singleton.
    inline static void Unlock();

    /// Gets a reference to the single directory object.
    inline static Directory &Instance();

    /// Registers the address of an entity.
    inline void Register(IEntity *const entity);

    /// Deregisters the address of an entity.
    inline bool Deregister(IEntity *const entity);

    /// Gets a pointer to the entity with the given address.
    /// \return A pointer to the entity, or null if none exists at that address.
    inline IEntity *Lookup(const Address address) const;

    /// Counts the number of entities in the directory.
    /// The count includes all registered actors and receivers.
    /// \note This performs an exhaustive count, so is expensive.
    inline uint32_t Count() const;

    /// Removes all registered entries from the directory.
    inline void Clear();

private:

    /// Creates the singleton instance.
    /// \note This method is intentionally non-inlined.
    static void CreateInstance();

    /// Destroys the singleton instance.
    /// \note This method is intentionally non-inlined.
    static void DestroyInstance();

    /// Private default constructor.
    /// \note The directory is a singleton and can't be constructed directly.    
    inline Directory();

    /// Disallowed copy constructor.
    Directory(const Directory &other);

    /// Disallowed assignment operator.
    Directory &operator=(const Directory &other);

    static Directory *smInstance;           ///< Pointer to the single, static instance.
    static Mutex smMutex;                   ///< Synchronizes access to the singleton instance.

    HashTable<IEntity *> mHashTable;        ///< Maps addresses to entities.
    uint32_t mNumEntities;                  ///< Number of entities in the dictionary.
};


THERON_FORCEINLINE void Directory::Lock()
{
    smMutex.Lock();

    // Create the singleton instance if it doesn't already exist.
    if (smInstance == 0)
    {
        // Non-inlined call, uncommon case.
        CreateInstance();
    }
}


THERON_FORCEINLINE void Directory::Unlock()
{
    // Destroy the singleton instance if it has become unreferenced.
    // This ensures the memory is deallocated correctly prior to program termination.
    if (smInstance->Count() == 0)
    {
        // Non-inlined call, uncommon case.
        DestroyInstance();
    }

    smMutex.Unlock();
}


THERON_FORCEINLINE Directory &Directory::Instance()
{
    THERON_ASSERT(smInstance);
    return *smInstance;
}


THERON_FORCEINLINE Directory::Directory() :
  mHashTable(HASH_SIZE),
  mNumEntities(0)
{
}


THERON_FORCEINLINE void Directory::Register(IEntity *const entity)
{
    const Address &address(entity->GetAddress());
    const uint32_t addressValue(address.AsInteger());

    mHashTable.Insert(addressValue, entity);
    ++mNumEntities;
}


THERON_FORCEINLINE bool Directory::Deregister(IEntity *const entity)
{
    const Address &address(entity->GetAddress());
    const uint32_t addressValue(address.AsInteger());

    if (mHashTable.Remove(addressValue, entity))
    {
        --mNumEntities;
        return true;
    }

    return false;
}


inline IEntity *Directory::Lookup(const Address address) const
{
    typedef HashTable<IEntity *>::ItemList ItemList;

    IEntity *result(0);
    const uint32_t addressValue(address.AsInteger());
    const ItemList &itemList(mHashTable.Items(addressValue));

    // Search the list of items for one with the given address
    ItemList::const_iterator items(itemList.Begin());
    const ItemList::const_iterator itemsEnd(itemList.End());

    while (items != itemsEnd)
    {
        IEntity *const entity(*items);
        if (entity->GetAddress() == address)
        {
            result = entity;
            break;
        }
        
        ++items;
    }

    return result;
}


THERON_FORCEINLINE uint32_t Directory::Count() const
{
    return mNumEntities;
}


THERON_FORCEINLINE void Directory::Clear()
{
    mHashTable.Clear();
    mNumEntities = 0;
}


} // namespace Theron


#endif // THERON_DETAIL_DIRECTORY_DIRECTORY_H

