// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_HANDLERS_DEFAULTHANDLER_H
#define THERON_DETAIL_HANDLERS_DEFAULTHANDLER_H


#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Messages/IMessage.h>

#include <Theron/Detail/Handlers/IDefaultHandler.h>

#include <Theron/Address.h>


namespace Theron
{


/// Instantiable class template that remembers a default handler function.
template <class ActorType>
class DefaultHandler : public IDefaultHandler
{
public:

    /// Pointer to a member function of the actor type.
    typedef void (ActorType::*HandlerFunction)(const Address from);

    /// Constructor.
    inline DefaultHandler(ActorType *const actor, HandlerFunction function) :
      mActor(actor),
      mHandlerFunction(function)
    {
    }

    /// Virtual destructor.
    inline virtual ~DefaultHandler()
    {
    }

    /// Handles the given message.
    /// \note The message is not consumed by the handler; just acted on or ignored.
    /// The message will be automatically destroyed when all handlers have seen it.
    inline virtual void Handle(const IMessage *const message) const;

private:

    /// Disabled copy constructor.
    DefaultHandler(const DefaultHandler &other);
    
    /// Disabled assignment operator.
    DefaultHandler &operator=(const DefaultHandler &other);

    ActorType *mActor;                          ///< Pointer to an actor owning a handler function.
    const HandlerFunction mHandlerFunction;     ///< Pointer to a handler member function on the actor.
};


template <class ActorType>
inline void DefaultHandler<ActorType>::Handle(const IMessage *const message) const
{
    THERON_ASSERT(mActor);
    THERON_ASSERT(mHandlerFunction);
    THERON_ASSERT(message);

    // Call the handler, passing it from address.
    // We can't pass the value because we don't even know the type.
    (mActor->*mHandlerFunction)(message->From());
}


} // namespace Theron


#endif // THERON_DETAIL_HANDLERS_DEFAULTHANDLER_H

