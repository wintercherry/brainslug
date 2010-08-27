// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_FRAMEWORK_H
#define THERON_FRAMEWORK_H

#pragma once


/**
\file Framework.h
Framework object that hosts and executes actors.
*/


#include <new>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Containers/LockedQueue.h>

#include <Theron/Detail/Core/ActorCore.h>
#include <Theron/Detail/Core/ActorProcessor.h>
#include <Theron/Detail/Core/GlobalFreeList.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Directory/Directory.h>

#include <Theron/Detail/Messages/Message.h>

#include <Theron/Detail/ThreadPool/ThreadPool.h>

#include <Theron/ActorRef.h>
#include <Theron/Address.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// \brief The manager class that hosts, manages, and run actors.
/// Users should construct an instance of the Framework in non-actor application
/// code before creating any actors. Actors can then be created by calling
/// the CreateActor method of the instantiated Framework.
/// It's possible to construct multiple Framework objects within an
/// application. The actors created within each Framework are processed
/// by separate pools of worker threads, but can send messages to each other
/// across frameworks.
class Framework
{
public:

    friend class ActorCore;

    /// \brief Default constructor
    /// Constructs a framework with two worker threads by default.
    Framework();

    /// \brief Constructor
    /// Constructs a framework with the given number of worker threads.
    explicit Framework(const uint32_t numThreads);
    
    /// \brief Destructor
    ~Framework();

    /// \brief Creates an actor object within the framework.
    /// \return An ActorRef referencing the new actor, returned by value.
    template <class ActorType>
    ActorRef CreateActor();

    /// \brief Creates an actor object within the framework.
    /// Accepts an instance of the Parameters type exposed by the ActorType class,
    /// which is in turn passed to the ActorType constructor. This provides a way for
    /// user-defined actor classes to be provided with user-defined parameters on construction.
    /// \param params An instance of the Parameters type exposed by the ActorType class.
    /// \return An ActorRef referencing the new actor, returned by value.
    template <class ActorType>
    ActorRef CreateActor(const typename ActorType::Parameters &params);

    /// \brief Creates an actor object within the framework at the specified address.
    /// \param address The address for the actor, which must be unique and non-null.
    /// \return An ActorRef referencing the new actor, or a null reference on failure.
    template <class ActorType>
    ActorRef CreateActorAtAddress(const Theron::Address &address);

    /// \brief Creates an actor object within the framework.
    /// Accepts an instance of the Parameters type exposed by the ActorType class,
    /// which is in turn passed to the ActorType constructor. This provides a way for
    /// user-defined actor classes to be provided with user-defined parameters on construction.
    /// \param address The address for the actor, which must be unique and non-null.
    /// \param params An instance of the Parameters type exposed by the ActorType class.
    /// \return An ActorRef referencing the new actor, or a null reference on failure.
    template <class ActorType>
    ActorRef CreateActorAtAddress(const Theron::Address &address, const typename ActorType::Parameters &params);

    /// \brief Sends a message to the entity (typically an actor) at the given address.
    /// \note If no actor or receiver exists with the given address, the
    /// message will not be delivered, and this method will return false.
    /// Alternatively if the destination address exists, but the receiving
    /// entity has no handler registered for messages of the given
    /// type, so declines to read the message, then the message will be ignored,
    /// and this method will return false.
    /// \return True, if the message was read by the target entity, otherwise false.
    template <class ValueType>
    inline bool Send(const ValueType &value, const Address &from, const Address &to) const;

private:

    typedef LockedQueue<ActorCore *> WorkQueue;
    typedef ThreadPool<ActorCore, ActorProcessor> Processor;

    /// Disallowed copy constructor. Frameworks can't be copied or assigned.
    Framework(const Framework &other);
    
    /// Disallowed assignment operator. Frameworks can't be copied or assigned.
    Framework &operator=(const Framework &other);

    WorkQueue mWorkQueue;   ///< Threadsafe queue of actors waiting to be processed.
    Processor mProcessor;   ///< Pool of worker threads used to run actor message handlers.
};


template <class ActorType>
inline ActorRef Framework::CreateActor()
{
    // Auto-generate a unique address.
    return CreateActorAtAddress<ActorType>(Address());
}


template <class ActorType>
inline ActorRef Framework::CreateActor(const typename ActorType::Parameters &params)
{
    // Auto-generate a unique address.
    return CreateActorAtAddress<ActorType>(Address(), params);
}


template <class ActorType>
inline ActorRef Framework::CreateActorAtAddress(const Theron::Address &address)
{
    if (address != Address::Null())
    {
        IAllocator *const allocator(AllocatorManager::Instance().GetAllocator());

        void *const memory = allocator->Allocate(sizeof(ActorType));
        if (memory)
        {
            ActorType *const actor = new (memory) ActorType();
            THERON_ASSERT(actor);

            // Reject the request if the address is already taken.
            if (actor->Core().SetAddress(address))
            {
                actor->Core().SetFramework(this);
                actor->Core().SetWorkQueue(&mWorkQueue);

                return ActorRef(actor);
            }

            // Failed, destroy
            allocator->Free(memory);
        }
    }

    return ActorRef();
}


template <class ActorType>
inline ActorRef Framework::CreateActorAtAddress(const Theron::Address &address, const typename ActorType::Parameters &params)
{
    if (address != Address::Null())
    {
        IAllocator *const allocator(AllocatorManager::Instance().GetAllocator());

        void *const memory = allocator->Allocate(sizeof(ActorType));
        if (memory)
        {
            ActorType *const actor = new (memory) ActorType(params);
            THERON_ASSERT(actor);

            // Reject the request if the address is already taken.
            if (actor->Core().SetAddress(address))
            {
                actor->Core().SetFramework(this);
                actor->Core().SetWorkQueue(&mWorkQueue);

                return ActorRef(actor);
            }

            // Failed, destroy
            allocator->Free(memory);
        }
    }

    return ActorRef();
}


template <class ValueType>
inline bool Framework::Send(const ValueType &value, const Address &from, const Address &to) const
{
    typedef Message<ValueType> MessageType;

    Metrics::Instance().Count(Metrics::EVENT_MESSAGE_SENT);

    // Allocate a message. It'll be deleted by the recipient.
    void *const memory = GlobalFreeList::Instance().Allocate(sizeof(MessageType));
    if (memory == 0)
    {
        return false;
    }

    MessageType *const message = new (memory) MessageType(value, from);
    IEntity *entity(0);

    // Lock the directory to make sure the actor can't be destroyed.
    Directory::Lock();
    {
        // Look up the target entity in the directory using its unique address.
        entity = Directory::Instance().Lookup(to);
        if (entity)
        {
            // Push the message and wake a thread.
            entity->Push(message, true);
        }
    }
    Directory::Unlock();

    // If the message wasn't delivered we need to delete it ourselves.
    if (entity == 0)
    {
        GlobalFreeList::Instance().Free(message, message->Size());
        return false;
    }

    return true;
}


} // namespace Theron


#endif // THERON_FRAMEWORK_H

