// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_DIRECTORY_IENTITY_H
#define THERON_DETAIL_DIRECTORY_IENTITY_H

#pragma once


#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Messages/IMessage.h>

#include <Theron/Address.h>


namespace Theron
{


/// Interface describing an addressable entity.
/// An addressable entity has a unique address and can accept messages.
/// \note Examples of addressable entities are actors and receivers.
class IEntity
{
public:

    /// Default constructor.
    inline IEntity() : mAddress(Address::Null())
    {
    }

    /// Virtual destructor.
    inline virtual ~IEntity()
    {
    }

    /// Sets the unique address of the entity.
    void SetAddress(const Address &address)
    {
        THERON_ASSERT(address != Address::Null());
        mAddress = address;
    }

    /// Gets the unique address of the entity.
    const Address &GetAddress() const
    {
        return mAddress;
    }

    /// Pushes a message into the entity.
    /// \param wake Indicates whether a worker thread should be woken.
    /// \return True, if the entity accepted the message.
    virtual void Push(IMessage *const message, const bool wake) = 0;

private:

    /// Disallowed copy constructor.
    IEntity(const IEntity &other);
    
    /// Disallowed assignment operator.
    IEntity &operator=(const IEntity &other);

    Address mAddress;           ///< The unique address (name) of this entity.
};


} // namespace Theron


#endif // THERON_DETAIL_DIRECTORY_IENTITY_H

