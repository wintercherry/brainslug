// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_MESSAGES_MESSAGE_H
#define THERON_DETAIL_MESSAGES_MESSAGE_H


#include <Theron/Detail/Messages/IMessage.h>

#include <Theron/Defines.h>


namespace Theron
{


/// Message class, used for sending data between actors.
template <class ValueType>
class Message : public IMessage
{
public:

    typedef Message<ValueType> ThisType;

    /// Constructor.
    THERON_FORCEINLINE Message(
        const ValueType &value,
        const Address &from) :
      IMessage(from, sizeof(ThisType)),
      mValue(value)
    {
    }

    /// Virtual destructor.
    THERON_FORCEINLINE virtual ~Message()
    {
    }

    /// Gets the value carried by the message.
    THERON_FORCEINLINE const ValueType &Value() const
    {
        return mValue;
    }

private:

    ValueType mValue;           ///< The value of the data contained in the message.
};


} // namespace Theron


#endif // THERON_DETAIL_MESSAGES_MESSAGE_H

