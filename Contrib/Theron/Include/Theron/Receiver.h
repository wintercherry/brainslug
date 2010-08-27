// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_RECEIVER_H
#define THERON_RECEIVER_H

#pragma once


/**
\file Receiver.h
Utility that can receive messages from actors.
*/


#include <new>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Containers/Queue.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Directory/IEntity.h>

#include <Theron/Detail/Handlers/MessageHandlerBase.h>
#include <Theron/Detail/Handlers/MessageHandler.h>

#include <Theron/Detail/Messages/Message.h>

#include <Theron/Detail/Threading/Lock.h>
#include <Theron/Detail/Threading/Monitor.h>

#include <Theron/Address.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>


namespace Theron
{


/// \brief A standalone entity that can accept messages sent by actors.
/// A receiver can be instantiated in non-actor user code. Each receiver has
/// a unique address and can receive messages sent to it by actors (usually
/// in response to messages sent to those actors by the non-actor code).
/// User code can register handler functions with receivers they construct,
/// allowing non-actor code to collect and handle messages sent by the actors
/// with which they are in communication.
class Receiver : public IEntity
{
public:

    /// \brief Default constructor.
    /// Constructs a receiver object with an automatically-assigned unique address.
    Receiver();

    /// \brief Destructor.
    virtual ~Receiver();

    /// \brief Pushes a message into the receiver.
    /// \param wake Indicates whether a worker thread should be woken.
    /// \return True, if the receiver accepted the message.
    /// \note Implementation of IEntity::Push.
    virtual void Push(IMessage *const message, const bool wake);

    /// \brief Registers a handler for a specific message type.
    /// The handler is a member function of an object of type ClassType.
    /// The handler takes as input a message of type ValueType and a from address
    /// indicating the address of the entity that sent the message.
    /// The given handler will be called when messages are received of type ValueType.
    template <class ClassType, class ValueType>
    inline bool RegisterHandler(
        ClassType *const owner,
        void (ClassType::*handler)(const ValueType &message, const Address from));

    /// \brief Deregisters a previously registered handler.
    template <class ClassType, class ValueType>
    inline bool DeregisterHandler(
        ClassType *const owner,
        void (ClassType::*handler)(const ValueType &message, const Address from));

    /// \brief Resets the count of received messages to zero.
    inline void Reset();

    /// \brief Returns the number of messages received but not yet waited for.
    inline uint32_t Count() const;

    /// \brief Waits until a message arrives at the receiver.
    ///
    /// The calling thread is expected to be in non-actor user code.
    /// It will block, going to sleep until woken by the arrival of
    /// an incoming message. Note that any message handlers registered
    /// for the message type of the received message will still be called,
    /// strictly prior to the waiting thread being woken up.
    ///
    /// Conversely, the waiting thread will block and then be woken up
    /// even if no message handlers are registered for the arriving message
    /// type. In that sense the waiting functionality is independent from
    /// the message handler functionality. However they can be combined by
    /// registered a handler function and then waiting to be woken up when
    /// the handler has been executed. In such cases the waiting thread is
    /// guaranteed to be woken only after execution of all appropriate
    /// handlers.
    ///
    /// An important subtlety is how previously received messages are treated.
    /// The Receiver maintains a thread-safe internal count of how many messages
    /// it has received (ever). Every time a message is received the count is
    /// incremented, and every time a caller calls Wait, the count is decremented.
    /// The behaviour of Wait is that it waits until the count is greater than
    /// zero - in other words, a until a message has arrived which hasn't been
    /// waited on previously. The benefit of this is that if you expect n
    /// messages to arrive at a receiver, you can call Wait n times, and every
    /// message will be correctly accounted for, even if they arrive very close
    /// together.
    ///
    /// \note This is a blocking call and will cause the calling thread to
    /// block indefinately until a message is received by the receiver.
    void Wait();

private:

    /// Disallowed copy constructor.
    Receiver(const Receiver &other);
    
    /// Disallowed assignment operator.
    Receiver &operator=(const Receiver &other);

    List<MessageHandlerBase *> mMessageHandlers;    ///< List of registered message handlers.
    mutable Monitor mMonitor;                       ///< Synchronizes access to the message handlers.
    uint32_t mMessagesReceived;                     ///< Indicates that a message was received.
};


template <class ClassType, class ValueType>
inline bool Receiver::RegisterHandler(
    ClassType *const owner,
    void (ClassType::*handler)(const ValueType &message, const Address from))
{
    typedef MessageHandler<ClassType, ValueType> MessageHandlerType;

    // Allocate memory for a message handler object.
    void *const memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(MessageHandlerType));
    if (memory == 0)
    {
        return false;
    }

    // Construct a handler object to remember the function pointer and message value type.
    MessageHandlerType *const messageHandler = new (memory) MessageHandlerType(owner, handler);
    if (messageHandler == 0)
    {
        return false;
    }

    {
        Lock lock(mMonitor.GetMutex());
        mMessageHandlers.Insert(messageHandler);
    }
    
    return true;
}


template <class ClassType, class ValueType>
inline bool Receiver::DeregisterHandler(
    ClassType *const owner,
    void (ClassType::*handler)(const ValueType &message, const Address from))
{
    // Create a temporary handler object on the stack for comparison
    typedef MessageHandler<ClassType, ValueType> MessageHandlerType;
    MessageHandlerType targetMessageHandler(owner, handler);

    {
        Lock lock(mMonitor.GetMutex());

        // Find the handler in the registered handler list.
        List<MessageHandlerBase *>::const_iterator handlers(mMessageHandlers.Begin());
        const List<MessageHandlerBase *>::const_iterator handlersEnd(mMessageHandlers.End());

        while (handlers != handlersEnd)
        {
            MessageHandlerBase *const messageHandler(*handlers);
            if (targetMessageHandler.Equals(messageHandler))
            {
                // Remove the handler from the list.
                mMessageHandlers.Remove(messageHandler);

                // Free the handler object, which was allocated on registration.
                AllocatorManager::Instance().GetAllocator()->Free(messageHandler);
                
                return true;
            }
            
            ++handlers;
        }
    }

    return false;
}


THERON_FORCEINLINE void Receiver::Reset()
{
    Lock lock(mMonitor.GetMutex());
    mMessagesReceived = 0;
}


THERON_FORCEINLINE uint32_t Receiver::Count() const
{
    uint32_t count(0);

    {
        Lock lock(mMonitor.GetMutex());
        count = mMessagesReceived;
    }

    return count;
}


} // namespace Theron


#endif // THERON_RECEIVER_H

