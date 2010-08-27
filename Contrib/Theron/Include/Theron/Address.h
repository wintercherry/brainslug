// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_ADDRESS_H
#define THERON_ADDRESS_H

#pragma once


/**
\file Address.h
Address object, the unique name of an actor.
*/


#include <Theron/Detail/BasicTypes.h>

#include <Theron/Defines.h>


namespace Theron
{


/// \brief The unique address of an entity that can send or receive messages.
/// Default-constructed addresses are are assigned unique values using an internal counter.
/// This means ensures that addressable entities have unique addresses.
/// Addresses themselves can be copied and assigned, allowing the copying of the 'to' and
/// 'from' addresses of sent messages.
class Address
{
public:

    /// \brief Static method that returns a const reference to the unique null address.
    /// The null address is guaranteed not to be equal to any non-null address.
    /// \return A const reference to the unique null address.
    inline static const Address &Null()
    {
        static Address sNullAddress(0UL);
        return sNullAddress;
    }

    /// \brief Default constructor.
    /// When an address is constructed it is automatically assigned a unique non-null value.
    THERON_FORCEINLINE Address()
    {
        // Zero is an invalid address value.
        static uint32_t sNextValue = 0;
        mValue = ++sNextValue;
    }

    /// \brief Copy constructor.
    /// Addresses can be copied.
    /// \param other The existing address to be copied.
    THERON_FORCEINLINE Address(const Address &other) : mValue(other.mValue)
    {
    }

    /// \brief Assignment operator.
    /// Addresses can be assigned.
    /// \param other The existing address whose value is to be assigned.
    THERON_FORCEINLINE Address &operator=(const Address &other)
    {
        mValue = other.mValue;
        return *this;
    }

    /// \brief Gets the unique value of the address as an unsigned integer.
    THERON_FORCEINLINE uint32_t AsInteger() const
    {
        return mValue;
    }

    /// \brief Equality operator.
    /// Returns true if two addresses are the same, otherwise false.
    /// \param other The address to be compared.
    /// \return True if the given address is the same as this one.
    THERON_FORCEINLINE bool operator==(const Address &other) const
    {
        return (mValue == other.mValue);
    }

    /// \brief Inequality operator.
    /// Returns true if two addresses are different, otherwise false.
    /// \param other The address to be compared.
    /// \return True if the given address is different from this one.
    THERON_FORCEINLINE bool operator!=(const Address &other) const
    {
        return (mValue != other.mValue);
    }

    /// \brief Less-than operator.
    /// This allows addresses to be sorted, for example in containers.
    THERON_FORCEINLINE bool operator<(const Address &other) const
    {
        return (mValue < other.mValue);
    }

private:

    /// Constructor that accepts a specific value for the address.
    /// \param value The value for the newly constructed address.
    /// \note This method is private and not available for general use.
    THERON_FORCEINLINE explicit Address(uint32_t value) : mValue(value)
    {
    }

    uint32_t mValue;        ///< The integer value of the address.
};


} // namespace Theron


#endif // THERON_ADDRESS_H

