// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_HANDLERS_MESSAGEHANDLER_H
#define THERON_DETAIL_HANDLERS_MESSAGEHANDLER_H


#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Handlers/MessageHandlerBase.h>

#include <Theron/Detail/Messages/IMessage.h>
#include <Theron/Detail/Messages/Message.h>

#include <Theron/Address.h>


namespace Theron
{


/// Instantiable class template that remembers a message handler function and
/// the type of message it accepts.
template <class ActorType, class ValueType>
class MessageHandler : public MessageHandlerBase
{
public:

    /// Pointer to a member function of the actor type that can handle messages
    /// with the given value type.
    typedef void (ActorType::*HandlerFunction)(const ValueType &message, const Address from);

    /// Constructor.
    inline MessageHandler(ActorType *const actor, HandlerFunction function) :
      mActor(actor),
      mHandlerFunction(function)
    {
    }

    /// Virtual destructor.
    inline virtual ~MessageHandler()
    {
    }

    /// Returns true if the given handler represents the same handler method.
    inline virtual bool Equals(const MessageHandlerBase *const other) const;

    /// Handles the given message, if it's of the type accepted by the handler.
    /// \return True, if the handler handled the message.
    /// \note The message is not consumed by the handler; just acted on or ignored.
    /// The message will be automatically destroyed when all handlers have seen it.
    inline virtual bool Handle(const IMessage *const message) const;

private:

    /// Disabled copy constructor.
    MessageHandler(const MessageHandler &other);
    
    /// Disabled assignment operator.
    MessageHandler &operator=(const MessageHandler &other);

    ActorType *mActor;                          ///< Pointer to an actor owning a handler function.
    const HandlerFunction mHandlerFunction;     ///< Pointer to a handler member function on the actor.
};


template <class ActorType, class ValueType>
inline bool MessageHandler<ActorType, ValueType>::Equals(const MessageHandlerBase *const other) const
{
    typedef MessageHandler<ActorType, ValueType> ThisType;

    // Try to convert the given message handler of this type.
    const ThisType *const typedOther = dynamic_cast<const ThisType *>(other);
    if (typedOther == 0)
    {
        return false;
    }

    if (mHandlerFunction != typedOther->mHandlerFunction)
    {
        return false;
    }
    
    return true;
}


template <class ActorType, class ValueType>
inline bool MessageHandler<ActorType, ValueType>::Handle(const IMessage *const message) const
{
    THERON_ASSERT(mActor);
    THERON_ASSERT(mHandlerFunction);
    THERON_ASSERT(message);

    // Try to convert the given message to a message of the type we accept.
    const Message<ValueType> *const typedMessage = dynamic_cast<const Message<ValueType> *>(message);
    if (typedMessage == 0)
    {
        return false;
    }

    // Read the message value, taking a reference to avoid a copy.
    const ValueType &value(typedMessage->Value());
    const Address &from(typedMessage->From());

    // Call the handler, passing it the message value and from address.
    (mActor->*mHandlerFunction)(value, from);

    return true;
}


} // namespace Theron


#endif // THERON_DETAIL_HANDLERS_MESSAGEHANDLER_H

