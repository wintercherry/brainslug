// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_IALLOCATOR_H
#define THERON_IALLOCATOR_H

#pragma once


/**
\file IAllocator.h
Allocator interface.
*/


#include <Theron/Detail/BasicTypes.h>


namespace Theron
{


/// \brief Interface describing a general-purpose memory allocator.
class IAllocator
{
public:

    /// \brief Defines an integer type used for specifying sizes of memory allocations.
    typedef uint32_t SizeType;

    /// \brief Default constructor
    inline IAllocator()
    {
    }
    
    /// \brief Virtual destructor
    inline virtual ~IAllocator()
    {
    }

    /// \brief Allocates a piece of contiguous memory.
    /// \param size The size of the memory to allocate, in bytes.
    /// \return A pointer to the allocated memory.
    virtual void *Allocate(const SizeType size) = 0;

    /// \brief Frees a previously allocated piece of contiguous memory.
    /// \param memory A pointer to the memory to be deallocated.
    virtual void Free(void *const memory) = 0;

private:

    /// Disallowed copy constructor.
    IAllocator(const IAllocator &other);
    
    /// Disallowed assignment operator.
    IAllocator &operator=(const IAllocator &other);
};


} // namespace Theron


#endif // THERON_IALLOCATOR_H

