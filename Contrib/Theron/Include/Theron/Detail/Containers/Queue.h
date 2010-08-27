// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CONTAINERS_QUEUE_H
#define THERON_DETAIL_CONTAINERS_QUEUE_H


#include <new>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Core/GlobalFreeList.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Defines.h>


namespace Theron
{


/// Class template describing a generic queue implemented on top of an allocator.
template <class ItemType>
class Queue
{
public:

    /// Maximum number of unused nodes cached in the internal free list.
    static const uint32_t MAX_FREE_LIST_SIZE = 5;

    /// Defines the node type used within the queue.
    /// Class template defining a generic node in a generic queue.
    /// Wraps the queue item type in a wrapper class with next and previous pointers.
    class Node
    {
    public:

        /// Default constructor.
        THERON_FORCEINLINE Node() :
          mPrev(0),
          mNext(0)
        {
        }

        /// Constructor.
        /// \param item A const reference to the item that the node will contain.
        /// \note The item is copied by value.
        THERON_FORCEINLINE explicit Node(const ItemType &item) :
          mItem(item),
          mPrev(0),
          mNext(0)
        {
        }

        ItemType mItem;     ///> The item stored in the node.
        Node *mPrev;        ///> Pointer to the previous node in the queue.
        Node *mNext;        ///> Pointer to the next node in the queue.

    private:

        /// Disallowed copy constructor.
        Node(const Node &node);
        
        /// Disallowed assignment operator.
        Node &operator=(const Node &node);
    };

    /// Constructor
    inline Queue();
    
    /// Destructor
    inline ~Queue();

    /// Returns true if the queue contains at least one item.
    inline bool Empty() const;

    /// Empties the queue, removing all previously inserted items.
    inline void Clear();

    /// Pushes an item into the queue. The new item is copied by reference and added at the back.
    /// No checking for duplicates is performed.
    /// \param item A const reference to the item to be added to the queue.
    inline void Push(const ItemType &item);

    /// Peeks at the item at the front of the queue without removing it.
    /// \return A reference to the item at the front of the queue.
    /// \see Count
    /// \see Pop
    inline const ItemType &Peek() const;

    /// Removes the item at the front of the queue.
    /// \see Count
    /// \see Peek
    inline void Pop();

private:

    /// Disallowed copy constructor.
    Queue(const Queue &other);

    /// Disallowed assignment operator.
    Queue &operator=(const Queue &other);

    /// Allocates a new node.
    inline Node *Allocate();
    
    /// Frees a previously allocated node.
    inline void Free(Node *const node);

    Node *mFront;               ///< Pointer to the node at the front of the queue.
    Node *mBack;                ///< Pointer to the node at the back of the queue.
    Node *mFreeList;            ///< First in a list of free nodes maintained for speedy allocations.
    uint32_t mFreeCount;        ///< Number of nodes currently in the free list.
};


template <class ItemType>
THERON_FORCEINLINE Queue<ItemType>::Queue() :
  mFront(0),
  mBack(0),
  mFreeList(0),
  mFreeCount(0)
{
    // Create the global free list, if it doesn't exist yet.
    GlobalFreeList::Reference();
}


template <class ItemType>
THERON_FORCEINLINE Queue<ItemType>::~Queue()
{
    // Free all the nodes currently allocated for the queue.
    Clear();
    
    // Properly deallocate the nodes on the free list.
    Node *node(mFreeList);
    while (node)
    {
        Node *const temp(node);
        node = node->mNext;
        GlobalFreeList::Instance().Free(temp, sizeof(Node));
    }

    // Destroy the global free list, if we were the last to reference it.
    GlobalFreeList::Dereference();
}


template <class ItemType>
THERON_FORCEINLINE bool Queue<ItemType>::Empty() const
{
    return (mFront == 0);
}


template <class ItemType>
THERON_FORCEINLINE void Queue<ItemType>::Clear()
{
    // Free all the nodes currently allocated for the queue.
    Node *node = mBack;
    while (node)
    {
        Node *const temp(node);
        node = node->mNext;
        Free(temp);
    }

    mBack = 0;
    mFront = 0;
}


template <class ItemType>
THERON_FORCEINLINE void Queue<ItemType>::Push(const ItemType &item)
{
    Node *const newNode = Allocate();

    newNode->mItem = item;
    newNode->mNext = mBack;
    newNode->mPrev = 0;

    if (mBack)
    {
        mBack->mPrev = newNode;
    }

    mBack = newNode;
    if (mFront == 0)
    {
        mFront = newNode;
    }
}


template <class ItemType>
THERON_FORCEINLINE const ItemType &Queue<ItemType>::Peek() const
{
    THERON_ASSERT(mBack);
    THERON_ASSERT(mFront);
    return mFront->mItem;
}


template <class ItemType>
THERON_FORCEINLINE void Queue<ItemType>::Pop()
{
    THERON_ASSERT(mBack);
    THERON_ASSERT(mFront);

    Node *const previous = mFront->mPrev;
    if (previous)
    {
        previous->mNext = 0;
    }
    else
    {
        THERON_ASSERT(mFront == mBack);
        mBack = 0;
    }

    Free(mFront);
    mFront = previous;
}


template <class ItemType>
THERON_FORCEINLINE typename Queue<ItemType>::Node *Queue<ItemType>::Allocate()
{
    // Use an existing memory block from the freelist if we have one.
    if (mFreeList)
    {
        Node *const temp(mFreeList);
        mFreeList = mFreeList->mNext;
        --mFreeCount;
        return temp;
    }

    // Allocate a new memory block for the node.
    void *const memory = GlobalFreeList::Instance().Allocate(sizeof(Node));
    THERON_ASSERT(memory);

    Node *const newNode = new (memory) Node;
    THERON_ASSERT(newNode);

    return newNode;
}


template <class ItemType>
THERON_FORCEINLINE void Queue<ItemType>::Free(Node *const node)
{
    // Limit the growth of the free list.
    if (mFreeCount < MAX_FREE_LIST_SIZE)
    {
        // Add the the returned memory block to the free list.
        node->mNext = mFreeList;
        mFreeList = node;
        ++mFreeCount;
        return;
    }

    // Deallocate the node.
    GlobalFreeList::Instance().Free(node, sizeof(Node));
}


} // namespace Theron


#endif // THERON_DETAIL_CONTAINERS_QUEUE_H

