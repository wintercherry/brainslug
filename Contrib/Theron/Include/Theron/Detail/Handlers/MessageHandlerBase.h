// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_HANDLERS_MESSAGEHANDLERBASE_H
#define THERON_DETAIL_HANDLERS_MESSAGEHANDLERBASE_H


#include <Theron/Detail/Messages/IMessage.h>

#include <Theron/Defines.h>


namespace Theron
{


class Actor;


/// Baseclass that allows message handlers of various types to be stored in lists.
class MessageHandlerBase
{
public:

    /// Default constructor.
    inline MessageHandlerBase() : mMarked(false)
    {
    }

    /// Virtual destructor.
    inline virtual ~MessageHandlerBase()
    {
    }

    /// Marks the handler (eg. for deletion).    
    inline void Mark();

    /// Returns true if the handler is marked (eg. for deletion).    
    inline bool IsMarked() const;

    /// Returns true if the given handler represents the same handler method.
    virtual bool Equals(const MessageHandlerBase *const other) const = 0;

    /// Handles the given message, if it's of the type accepted by the handler.
    /// \return True, if the handler handled the message.
    virtual bool Handle(const IMessage *const message) const = 0;

private:

    /// Disabled copy constructor.
    MessageHandlerBase(const MessageHandlerBase &other);

    /// Disabled assignment operator.
    MessageHandlerBase &operator=(const MessageHandlerBase &other);

    bool mMarked;           ///< Flag used to mark the handler for deletion.
};


THERON_FORCEINLINE void MessageHandlerBase::Mark()
{
    mMarked = true;
}


THERON_FORCEINLINE bool MessageHandlerBase::IsMarked() const
{
    return mMarked;
}


} // namespace Theron


#endif // THERON_DETAIL_HANDLERS_MESSAGEHANDLERBASE_H

