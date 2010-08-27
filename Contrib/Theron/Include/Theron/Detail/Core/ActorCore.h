// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CORE_ACTORCORE_H
#define THERON_DETAIL_CORE_ACTORCORE_H

#pragma once


#include <new>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Containers/List.h>
#include <Theron/Detail/Containers/Queue.h>
#include <Theron/Detail/Containers/LockedQueue.h>

#include <Theron/Detail/Core/GlobalFreeList.h>
#include <Theron/Detail/Core/MessageCache.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Directory/IEntity.h>

#include <Theron/Detail/Handlers/DefaultHandler.h>
#include <Theron/Detail/Handlers/IDefaultHandler.h>
#include <Theron/Detail/Handlers/MessageHandlerBase.h>
#include <Theron/Detail/Handlers/MessageHandler.h>

#include <Theron/Detail/Messages/IMessage.h>
#include <Theron/Detail/Messages/Message.h>

#include <Theron/Detail/Threading/Lock.h>

#include <Theron/Detail/ThreadPool/ExecutionState.h>

#include <Theron/Detail/ThreadsafeObjects/Count.h>

#include <Theron/Address.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


class Actor;
class Framework;


/// Core functionality of an actor, not exposed to actor implementations.
class ActorCore : public IEntity
{

public:

    friend class Actor;
    friend class Framework;

    /// Destructor.
    ~ActorCore();

    /// Pushes a message into the actor.
    /// \note Implementation of IEntity::Push.
    inline virtual void Push(IMessage *const message, const bool wake);

    /// Sets the address of the actor.
    bool SetAddress(const Theron::Address address);

    /// Sets a pointer to the actor that contains this core.
    THERON_FORCEINLINE void SetParent(Actor *const parent)
    {
        mParent = parent;
    }

    /// Returns a pointer to the actor that contains this core.
    THERON_FORCEINLINE Actor *GetParent() const
    {
        return mParent;
    }

    /// Sets the work queue in which this actor queues itself for processing.
    THERON_FORCEINLINE void SetWorkQueue(LockedQueue<ActorCore *> *const workQueue)
    {
        mWorkQueue = workQueue;
    }

    /// Sets the framework to which this actor belongs.
    THERON_FORCEINLINE void SetFramework(Framework *const framework)
    {
        mFramework = framework;
    }

    /// Gets the framework to which this actor belongs. Derived actor class
    /// implementations can use this method to access the owning framework.
    /// \return A pointer to the framework to which the actor belongs.
    THERON_FORCEINLINE Framework *GetFramework() const
    {
        return mFramework;
    }

    /// Returns a reference to the reference count of the actor.
    THERON_FORCEINLINE Count &ReferenceCount()
    {
        return mReferenceCount;
    }

    /// Returns a const-reference to the reference count of the actor.
    THERON_FORCEINLINE const Count &ReferenceCount() const
    {
        return mReferenceCount;
    }

    /// Returns a reference to the execution state of the actor.
    THERON_FORCEINLINE ExecutionState &State()
    {
        return mState;
    }

    /// Returns a const-reference to the execution state of the actor.
    THERON_FORCEINLINE const ExecutionState &State() const
    {
        return mState;
    }

    /// Registers a handler for a specific message type.
    /// The given handler will be called when messages of the given type are received.
    template <class ActorType, class ValueType>
    inline bool RegisterHandler(
        ActorType *const actor,
        void (ActorType::*handler)(const ValueType &message, const Address from));

    /// Deregisters a previously registered handler.
    template <class ActorType, class ValueType>
    inline bool DeregisterHandler(
        ActorType *const actor,
        void (ActorType::*handler)(const ValueType &message, const Address from));

    /// Set the default handler run for unhandled messages.
    /// This handler, if set, is run when a message arrives of a type for which
    /// no regular message handlers are registered.
    /// Passing 0 to this method clears any previously set default handler.
    template <class ActorType>
    inline bool SetDefaultHandler(
        ActorType *const actor,
        void (ActorType::*handler)(const Address from));

    /// Removes any handlers marked for removal and adds any scheduled for adding.
    inline void UpdateHandlers();

    /// Gets the first message from the message queue.
    /// \note The caller must hold the work queue lock.
    inline void GetQueuedMessage(IMessage *&message, bool &more);

    /// Presents the actor with one of its queued messages, if any,
    /// and calls the associated handler.
    /// \return True, if a message was found on the queue, so processed.
    inline void ProcessMessage(IMessage *const message);

    /// Allocates and constructs a message of the given value type.
    template <class ValueType>
    inline Message<ValueType> *AllocateMessage(const ValueType &value, const Address &from) const;

    /// Frees and destructs a message of unknown type.
    inline void FreeMessage(IMessage *const message) const;

private:

    /// Private default constructor.
    /// Actor cores can't be constructed directly in user code.
    ActorCore();

    /// Disallowed copy constructor.
    ActorCore(const ActorCore &other);
    
    /// Disallowed assignment operator.
    ActorCore &operator=(const ActorCore &other);

    /// Internal non-inlined method.
    void DoUpdateHandlers();

    Actor *mParent;                                     ///< Address of the actor instance containing this core.
    Framework *mFramework;                              ///< The framework instance that owns this actor.
    Count mReferenceCount;                              ///< Counts how many ActorRef instances reference this actor.
    Queue<IMessage *> mMessageQueue;                    ///< Queue of messages awaiting processing.
    List<MessageHandlerBase *> mMessageHandlers;        ///< List of registered message handlers.
    IDefaultHandler *mDefaultMessageHandler;            ///< Handler executed for unhandled messages.
    ExecutionState mState;                              ///< Execution state (idle, busy, dirty).
    Queue<MessageHandlerBase *> mNewHandlerQueue;       ///< Holds new handlers until they're added.
    bool mHandlersDirty;                                ///< True if the handlers need updating.
    LockedQueue<ActorCore *> *mWorkQueue;               ///< Pointer to the work queue of the owning framework.
    mutable MessageCache mMessageCache;                 ///< Caches message memory blocks.
};


template <class ActorType, class ValueType>
inline bool ActorCore::RegisterHandler(
    ActorType *const actor,
    void (ActorType::*handler)(const ValueType &message, const Address from))
{
    typedef MessageHandler<ActorType, ValueType> MessageHandlerType;

    // Allocate memory for a message handler object.
    void *const memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(MessageHandlerType));
    if (memory == 0)
    {
        return false;
    }

    // Construct a handler object to remember the function pointer and message value type.
    MessageHandlerType *const messageHandler = new (memory) MessageHandlerType(actor, handler);
    if (messageHandler == 0)
    {
        return false;
    }

    // Add the handler to a queue for adding later when we're sure no handlers are running.
    // We don't need to lock this because only one thread can access it at a time.
    mNewHandlerQueue.Push(messageHandler);
    mHandlersDirty = true;

    return true;
}


template <class ActorType, class ValueType>
inline bool ActorCore::DeregisterHandler(
    ActorType *const actor,
    void (ActorType::*handler)(const ValueType &message, const Address from))
{
    bool result(false);
    
    // Create a temporary handler object on the stack for comparison
    typedef MessageHandler<ActorType, ValueType> MessageHandlerType;
    MessageHandlerType targetMessageHandler(actor, handler);

    // We don't need to lock this because only one thread can access it at a time.
    // Find the handler in the registered handler list.
    List<MessageHandlerBase *>::const_iterator handlers(mMessageHandlers.Begin());
    const List<MessageHandlerBase *>::const_iterator handlersEnd(mMessageHandlers.End());

    while (handlers != handlersEnd)
    {
        MessageHandlerBase *const messageHandler(*handlers);
        if (targetMessageHandler.Equals(messageHandler))
        {
            // Mark the handler for deregistration.
            // We defer the actual deregistration and deletion until
            // after all active message handlers have been executed, because
            // message handlers themselves can deregister handlers.
            messageHandler->Mark();
            mHandlersDirty = true;

            result = true;
            break;
        }
        
        ++handlers;
    }

    return result;
}


template <class ActorType>
inline bool ActorCore::SetDefaultHandler(
    ActorType *const actor,
    void (ActorType::*handler)(const Address from))
{
    typedef DefaultHandler<ActorType> DefaultHandlerType;
    DefaultHandlerType *defaultHandler(0);

    if (handler)
    {
        // Allocate memory for a default handler object.
        void *const memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(DefaultHandlerType));
        if (memory == 0)
        {
            return false;
        }

        // Construct a handler object to remember the function pointer.
        defaultHandler = new (memory) DefaultHandlerType(actor, handler);
        if (defaultHandler == 0)
        {
            return false;
        }
    }

    // We don't need to lock this because only one thread can access it at a time.
    // Destroy any previously set handler.
    if (mDefaultMessageHandler)
    {
        AllocatorManager::Instance().GetAllocator()->Free(mDefaultMessageHandler);
    }
    
    // Set the new handler.
    mDefaultMessageHandler = defaultHandler;

    return true;
}


THERON_FORCEINLINE void ActorCore::Push(IMessage *const message, const bool wake)
{
    Lock lock(mWorkQueue->GetMutex());

    if (message)
    {
        // Push the message onto the internal queue to await delivery.
        // The point of this is to maintain correct message delivery order within the actor.
        // The queue stores pointers to the IMessage interfaces for polymorphism.
        // We reuse the work queue lock to protect the per-actor message queues.
        mMessageQueue.Push(message);
    }

    // Don't schedule ourselves if we're already scheduled or running.
    // This relies on the assumption that actors which need further
    // processing at the end of being processed will always be rescheduled.
    ExecutionState &state(State());
    if (state == STATE_IDLE)
    {
        state = STATE_BUSY;
        mWorkQueue->Push(this);

        if (wake)
        {
            mWorkQueue->Pulse();
        }
    }
    else if (state == STATE_BUSY)
    {
        // Marking ourselves as dirty causes us to be re-scheduled when
        // our current processing is complete.
        state = STATE_DIRTY;
    }
    else
    {
        // We're already dirty, now even dirtier!
        THERON_ASSERT_MSG(state == STATE_DIRTY, "Unexpected work item state");
    }
}


THERON_FORCEINLINE void ActorCore::GetQueuedMessage(IMessage *&message, bool &more)
{
    if (!mMessageQueue.Empty())
    {
        message = mMessageQueue.Peek();
        mMessageQueue.Pop();

        if (!mMessageQueue.Empty())
        {
            more = true;
        }
    }
}


THERON_FORCEINLINE void ActorCore::UpdateHandlers()
{
    if (mHandlersDirty)
    {
        mHandlersDirty = false;
        DoUpdateHandlers();
    }
}


THERON_FORCEINLINE void ActorCore::ProcessMessage(IMessage *const message)
{
    THERON_ASSERT(message);

    Metrics::Instance().Count(Metrics::EVENT_ACTOR_PROCESSED);

    //
    // Give each registered handler a chance to handle this message.
    //

    bool handled(false);

    List<MessageHandlerBase *>::const_iterator handlersIt(mMessageHandlers.Begin());
    const List<MessageHandlerBase *>::const_iterator handlersEnd(mMessageHandlers.End());

    while (handlersIt != handlersEnd)
    {
        const MessageHandlerBase *const handler(*handlersIt);
        if (handler->Handle(message))
        {
            handled = true;
        }

        ++handlersIt;
    }

    //
    // If no registered handler handled the message, execute the default handler instead.
    //
    
    if (!handled && mDefaultMessageHandler)
    {
        mDefaultMessageHandler->Handle(message);
    }
}


template <class ValueType>
THERON_FORCEINLINE Message<ValueType> *ActorCore::AllocateMessage(const ValueType &value, const Address &from) const
{
    typedef Message<ValueType> MessageType;

    // Get a block of memory from the message cache.
    void *const memory = mMessageCache.Allocate<MessageType>();
    THERON_ASSERT(memory);

    // Construct the message.
    return new (memory) MessageType(value, from);
}


THERON_FORCEINLINE void ActorCore::FreeMessage(IMessage *const message) const
{
    // Free to the global free list rather than the local message cache, to
    // avoid having to make the local message cache thread-safe.
    // Allocations and frees happen in different threads.
    GlobalFreeList::Instance().Free(message, message->Size());
}


} // namespace Theron


#endif // THERON_DETAIL_CORE_ACTORCORE_H

