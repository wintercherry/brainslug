// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DEFINES_H
#define THERON_DEFINES_H

#pragma once


/**
\file Defines.h
Global project defines.
*/


#ifndef THERON_USE_BOOST_THREADS
/// \brief Controls whether Boost threads, locks, etc are used.
/// If this is set to 0 then Windows threads are used instead,
/// introducing a dependency on the windows.h header.
/// Default is disabled (use Windows threads).
#define THERON_USE_BOOST_THREADS 0
#endif // THERON_USE_BOOST_THREADS


#ifndef THERON_FORCEINLINE
#ifdef _DEBUG
/// \brief Controls whether core functions are force-inlined.
/// By default functions are never force-inlined in debug builds.
#define THERON_FORCEINLINE inline
#else // _DEBUG
#ifdef _MSC_VER
#define THERON_FORCEINLINE __forceinline
#elif defined(__GNUC__)
#define THERON_FORCEINLINE inline __attribute__((always_inline))
#else
#define THERON_FORCEINLINE inline
#endif
#endif // _DEBUG
#endif // THERON_FORCEINLINE


#ifndef THERON_ENABLE_METRICS
/// \brief Enables counting of thread and message processing events.
#define THERON_ENABLE_METRICS 0
#endif // THERON_ENABLE_METRICS


#ifndef THERON_GLOBALFREELIST_POOLS
/// \brief Number of pools in the global free list.
/// The global free list holds cached free memory blocks used to
/// allocate messages. Each pool holds free memory blocks of a
/// specific size. Available pools are assigned to hold blocks of
/// successively larger multiples of four, with the first pool
/// holding blocks of 8 bytes (the minimum size of a message).
/// More pools means we can cache larger message blocks, at the
/// expense of the global free list potentially using more memory.
#define THERON_GLOBALFREELIST_POOLS 32
#endif // THERON_GLOBALFREELIST_POOLS


#ifndef THERON_GLOBALFREELIST_MAXPOOLSIZE
/// \brief Maximum number of memory blocks in each pool of the global free list.
/// Each pool is allowed to hold only a limited maximum number of blocks,
/// to ensure that the free list doesn't grow arbitrarily large.
#define THERON_GLOBALFREELIST_MAXPOOLSIZE 2048
#endif // THERON_GLOBALFREELIST_MAXPOOLSIZE


#ifndef THERON_ACTOR_MESSAGEPRELOAD
/// \brief Number of memory blocks pre-loaded by actors, for each pre-loaded
/// message type. The numeric, integer value of this define is controls
/// the number of message memory blocks pre-allocated, for each automatically
/// pre-allocated message type, by actors. Each actor instance automatically
/// pre-loads message memory blocks for up to three different message sizes.
/// Pre-loading is automatically assigned to the first three message sizes
/// allocated (sent) by the actor, currently. Messages of subsequent new sizes
/// are not preloaded. If this define is set in user code integer value overrides
/// the default value.
#define THERON_ACTOR_MESSAGEPRELOAD 4
#endif // THERON_ACTOR_MESSAGEPRELOAD


#endif // THERON_DEFINES_H

