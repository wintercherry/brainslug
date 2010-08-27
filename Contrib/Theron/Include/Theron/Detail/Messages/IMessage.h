// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_MESSAGES_IMESSAGE_H
#define THERON_DETAIL_MESSAGES_IMESSAGE_H


#include <Theron/Detail/BasicTypes.h>

#include <Theron/Address.h>
#include <Theron/Defines.h>


namespace Theron
{


/// Interface describing the generic API of the message class template.
class IMessage
{
public:

    /// Constructs an IMessage with the given size.
    THERON_FORCEINLINE explicit IMessage(const Address &from, const uint32_t size) :
      mFrom(from),
      mSize(size)
    {
    }

    /// Virtual destructor.
    THERON_FORCEINLINE virtual ~IMessage()
    {
    }

    /// Returns a reference to the size of the message in bytes.
    THERON_FORCEINLINE uint32_t &Size()
    {
        return mSize;
    }
    
    /// Returns a const-reference to the size of the message in bytes.
    THERON_FORCEINLINE const uint32_t &Size() const
    {
        return mSize;
    }
    
    /// Gets the address from which the message was sent.
    THERON_FORCEINLINE Address From() const
    {
        return mFrom;
    }

private:
    
    Address mFrom;          ///< The address from which the message was sent.
    uint32_t mSize;         ///< Size of the message in bytes.
};


} // namespace Theron


#endif // THERON_DETAIL_MESSAGES_IMESSAGE_H

