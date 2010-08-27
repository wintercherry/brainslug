// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_ACTOR_H
#define THERON_ACTOR_H

#pragma once


/**
\file Actor.h
Actor baseclass.
*/


#include <new>

#include <Theron/Detail/Core/ActorCore.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Directory/Directory.h>
#include <Theron/Detail/Directory/IEntity.h>

#include <Theron/Detail/Messages/Message.h>

#include <Theron/Address.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// \brief The actor baseclass.
/// All actors in Theron must derive from this class.
/// It provides the core functionality of an actor, such as the ability
/// to register message handlers and respond to messages, including sending
/// new messages to other actors. When implementing actors derived from this
/// baseclass users can define an override for the \ref Initialize method of
/// the baseclass (the baseclass version does nothing). They can also call
/// the various protected methods of the baseclass to perform actions like
/// registering message handlers and sending messages.
class Actor
{

public:

    friend class ActorRef;
    friend class Framework;

    /// \brief Baseclass virtual destructor.
    inline virtual ~Actor();

protected:

    /// \brief Protected default constructor.
    /// Only the framework can create actors directly.
    /// \note Constructors of derived actor classes should not register message handlers.
    /// Registration of handlers should be done in the \ref Initialize method override.
    inline Actor();

    /// \brief Returns the unique address of the actor.
    inline const Address &GetAddress() const;

    /// \brief Returns a reference to the framework that owns the actor.
    inline Framework &GetFramework() const;

    /// \brief Registers a handler for a specific message type.
    /// The given handler will be called when messages of the given type are received.
    /// That includes any message types derived from the given type.
    /// \note Derived actor classes should call this method to register message handlers.
    template <class ActorType, class ValueType>
    inline bool RegisterHandler(
        ActorType *const actor,
        void (ActorType::*handler)(const ValueType &message, const Address from));

    /// \brief Deregisters a previously registered handler.
    /// The message type given must match the type given on registration.
    /// \note Call this method to deregister message handlers.
    template <class ActorType, class ValueType>
    inline bool DeregisterHandler(
        ActorType *const actor,
        void (ActorType::*handler)(const ValueType &message, const Address from));

    /// \brief Sets the default handler for unhandled messages.
    /// This handler, if set, is run when a message arrives of a type for which
    /// no regular message handlers are registered.
    /// Passing 0 to this method clears any previously set default handler.
    template <class ActorType>
    inline bool SetDefaultHandler(
        ActorType *const actor,
        void (ActorType::*handler)(const Address from));

    /// \brief Sends a message to the entity (actor or receiver) at the given address.
    /// If no actor or receiver exists with the given address, the
    /// message will not be delivered, and this method will return false.
    /// \note If the destination address exists, but the receiving entity has no
    /// handlers registered for messages of the given type, so declines to read the
    /// message, then the message will be ignored, but this method will return true.
    /// \return True, if the message was delivered to the target entity, otherwise
    /// false.
    template <class ValueType>
    inline bool Send(const ValueType &value, const Address &address) const;

    /// \brief Sends a message to the entity at the given address, without waking a thread.
    /// Unlike \ref Send, a thread is not woken to process the message handlers of the
    /// receiving actor. Instead, the receiving actor is effectively processed after
    /// the current actor, by one of the worker threads already active. This method
    /// is mainly useful when called as the last operation in a message handler.
    /// In such cases the message handler is almost finished processing so the worker
    /// thread processing it will typically become available to process the receiving
    /// actor. This method can avoid the cost of waking a worker thread so can be more
    /// efficient than the more conventional Send. \note Calling this method from positions
    /// other than at the end ("tail") of a message handler is not generally recommended.
    /// \return True, if the message was delivered to the target entity, otherwise false.
    template <class ValueType>
    inline bool TailSend(const ValueType &value, const Address &address) const;

private:

    /// Disallowed copy constructor. Actors may not be copied or assigned.
    Actor(const Actor &other);
    
    /// Disallowed assignment operator. Actors may not be copied or assigned.
    Actor &operator=(const Actor &other);

    /// Sends the given message to the given address, and optionally wakes a worker thread.
    inline bool DoSend(IMessage *const message, const Address &address, const bool wake) const;

    /// Returns a reference to the core of the actor.
    inline ActorCore &Core();
    
    /// Returns a const-reference to the core of the actor.
    inline const ActorCore &Core() const;

    ActorCore mCore;    ///< The core implementation of the actor.
};


THERON_FORCEINLINE Actor::Actor() : mCore()
{
    mCore.SetParent(this);

    Metrics::Instance().Count(Metrics::EVENT_ACTOR_CREATED);
}


inline Actor::~Actor()
{
    Metrics::Instance().Count(Metrics::EVENT_ACTOR_DESTROYED);
}


THERON_FORCEINLINE const Address &Actor::GetAddress() const
{
    return mCore.GetAddress();
}


THERON_FORCEINLINE Framework &Actor::GetFramework() const
{
    return *Core().GetFramework();
}


template <class ActorType, class ValueType>
THERON_FORCEINLINE bool Actor::RegisterHandler(
    ActorType *const actor,
    void (ActorType::*handler)(const ValueType &message, const Address from))
{
    return Core().RegisterHandler<ActorType, ValueType>(actor, handler);
}


template <class ActorType, class ValueType>
THERON_FORCEINLINE bool Actor::DeregisterHandler(
    ActorType *const actor,
    void (ActorType::*handler)(const ValueType &message, const Address from))
{
    return Core().DeregisterHandler<ActorType, ValueType>(actor, handler);
}


template <class ActorType>
THERON_FORCEINLINE bool Actor::SetDefaultHandler(
    ActorType *const actor,
    void (ActorType::*handler)(const Address from))
{
    return Core().SetDefaultHandler<ActorType>(actor, handler);
}


template <class ValueType>
THERON_FORCEINLINE bool Actor::Send(const ValueType &value, const Address &address) const
{
    typedef Message<ValueType> MessageType;

    // Allocate a message. It'll be deleted by the target after it's been handled.
    MessageType *const message = Core().AllocateMessage(value, mCore.GetAddress());
    if (message == 0)
    {
        return false;
    }

    // Send the message and wake a worker thread.
    return DoSend(message, address, true);
}


template <class ValueType>
THERON_FORCEINLINE bool Actor::TailSend(const ValueType &value, const Address &address) const
{
    typedef Message<ValueType> MessageType;

    // Allocate a message. It'll be deleted by the target after it's been handled.
    MessageType *const message = Core().AllocateMessage(value, mCore.GetAddress());
    if (message == 0)
    {
        return false;
    }

    // Send the message without waking a worker thread.
    return DoSend(message, address, false);
}


inline bool Actor::DoSend(IMessage *const message, const Address &address, const bool wake) const
{
    Metrics::Instance().Count(Metrics::EVENT_MESSAGE_SENT);

    // Look up the target entity in the directory using its unique address.
    // Send the message while still holding the directory lock to make sure
    // the actor isn't deregistered and destroyed between checking the directory
    // and sending the message.
    IEntity *entity(0);
    Directory::Lock();
    {
        entity = Directory::Instance().Lookup(address);
        if (entity)
        {
            // Push the message and optionally wake a thread.
            entity->Push(message, wake);
        }
    }
    Directory::Unlock();

    // If the message wasn't delivered we need to delete it ourselves.
    if (entity == 0)
    {
        Core().FreeMessage(message);
        return false;
    }

    return true;
}


THERON_FORCEINLINE ActorCore &Actor::Core()
{
    return mCore;
}


THERON_FORCEINLINE const ActorCore &Actor::Core() const
{
    return mCore;
}


} // namespace Theron


#endif // THERON_ACTOR_H

