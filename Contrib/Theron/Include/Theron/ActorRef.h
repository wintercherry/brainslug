// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_ACTORREF_H
#define THERON_ACTORREF_H

#pragma once


/**
\file ActorRef.h
Actor reference, references an actor in user code.
*/


#include <new>

#include <Theron/Detail/Core/ActorCore.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Messages/Message.h>

#include <Theron/Actor.h>
#include <Theron/Address.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// \brief Used in user code to reference an actor.
/// Actors can't be created or referenced directly in user code.
/// Instead, when an actor is created with \ref Framework::CreateActor,
/// an ActorRef class is returned referencing the created actor.
/// ActorRef objects are like reference-counted smart pointers to actors,
/// and expose only appropriate actor methods to the code that owns the
/// reference. ActorRef objects can be copied, assigned, returned by value
/// from functions, and passed by value to functions. An actor is
/// automatically destructed (garbage collected) when all ActorRef objects
/// referencing it have been destructed.
class ActorRef
{

public:

    friend class Framework;

    /// \brief Default constructor.
    /// Constructs an uninitialized actor reference referencing no actor.
    inline ActorRef();

    /// \brief Copy constructor.
    /// Constructs another actor reference referencing the same actor.
    inline ActorRef(const ActorRef &other);

    /// \brief Assignment operator.
    /// Sets this actor reference to reference the same actor as another.
    /// The actor previously referenced (if any) is dereferenced.
    inline ActorRef &operator=(const ActorRef &other);

    /// \brief Destructor.
    /// Destroys a reference to an actor.
    inline ~ActorRef();

    /// \brief Returns the unique address of the referenced actor.
    /// \return The unique address of the actor.
    inline const Address &GetAddress() const;

    /// \brief Pushes a message into the referenced actor.
    /// This method is an alternative to \ref Framework::Send. It can be
    /// called from non-actor code in situations where the caller has an
    /// ActorRef referencing the actor they want to message. To send messages
    /// to actors using their unique addresses, see \ref Framework::Send.
    /// \note The return value of this method should be understood to mean
    /// just that the message was *accepted* by the actor. This doesn't mean
    /// necessarily that the actor took any action in response to the message.
    /// If the actor has no handlers registered for messages of that type, then
    /// the message will simply be consumed without any effect. In such cases
    /// this method will still return true. This surprising behaviour is a result
    /// of the asynchronous nature of message sending: the sender doesn't wait
    /// for the recipient to process the message. It is the sender's
    /// responsibility to ensure that messages are appropriate for the actors to
    /// which they are sent. Actor implementations can also register a default
    /// message handler (see \ref Actor::SetDefaultHandler).
    /// \return True, if the actor accepted the message.
    template <class ValueType>
    inline bool Push(const ValueType &value, const Address &from);

private:

    /// Constructor. Constructs a reference to the given actor.
    /// \param actor A pointer to the actor to be referenced.
    /// \note This method is private and is accessed only by the Framework class.
    inline explicit ActorRef(Actor *const actor);

    /// References the actor referenced by the actor reference.
    inline void Reference();

    /// Dereferences the actor previously referenced by the actor reference.
    inline void Dereference();

    Actor *mActor;      ///< Pointer to the referenced actor.
};


THERON_FORCEINLINE ActorRef::ActorRef() : mActor(0)
{
}


THERON_FORCEINLINE ActorRef::ActorRef(Actor *const actor) : mActor(actor)
{
    Reference();
}


THERON_FORCEINLINE ActorRef::ActorRef(const ActorRef &other) : mActor(other.mActor)
{
    Reference();
}


THERON_FORCEINLINE ActorRef &ActorRef::operator=(const ActorRef &other)
{
    Dereference();
    mActor = other.mActor;
    Reference();
    
    return *this;
}


THERON_FORCEINLINE ActorRef::~ActorRef()
{
    Dereference();
}


THERON_FORCEINLINE const Address &ActorRef::GetAddress() const
{
    return mActor->Core().GetAddress();
}


THERON_FORCEINLINE void ActorRef::Reference()
{
    if (mActor)
    {
        ActorCore &core(mActor->Core());
    
        core.ReferenceCount().Lock();
        core.ReferenceCount().Increment();
        core.ReferenceCount().Unlock();
    }
}


inline void ActorRef::Dereference()
{
    if (mActor)
    {
        ActorCore &core(mActor->Core());
        bool killActor(false);
    
        core.ReferenceCount().Lock();

        // The framework eventually destroys actors that become unreferenced.
        // However we have to tell the framework that the actor is dead.
        core.ReferenceCount().Decrement();
        if (core.ReferenceCount().Value() == 0)
        {
            killActor = true;
        }

        core.ReferenceCount().Unlock();

        if (killActor)
        {
            // TODO: Don't like that we have to pass a fake message here.
            // Schedule the actor for processing by the worker threads.
            // On finding that it's unreferenced, the worker threads
            // will destroy it.
            core.Push(0, true);
        }
    }
}


template <class ValueType>
THERON_FORCEINLINE bool ActorRef::Push(const ValueType &value, const Address &from)
{
    typedef Message<ValueType> MessageType;

    Metrics::Instance().Count(Metrics::EVENT_MESSAGE_SENT);

    // Allocate a message. It'll be deleted by the actor after it's been handled.
    // NOTE: We use the GlobalFreeList here to avoid requiring ActorCore::AllocateMessage
    // to be threadsafe due to accessing it from multiple threads.
    void *const memory = GlobalFreeList::Instance().Allocate(sizeof(MessageType));
    if (memory == 0)
    {
        return false;
    }

    MessageType *const message = new (memory) MessageType(value, from);
    THERON_ASSERT(message);

    // Push the message and wake a thread.
    mActor->Core().Push(message, true);
    return true;
}


} // namespace Theron


#endif // THERON_ACTORREF_H

