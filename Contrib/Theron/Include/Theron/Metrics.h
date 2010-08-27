// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_METRICS_H
#define THERON_METRICS_H

#pragma once


/**
\file Metrics.h
Event counts for profiling.
*/


#include <stdio.h>

#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Threading/Lock.h>
#include <Theron/Detail/Threading/Mutex.h>

#include <Theron/Defines.h>


namespace Theron
{


/// \brief Singleton class that counts events for optimization and debugging.
class Metrics
{
public:

    /// \brief Events for which counts are recorded.
    enum Event
    {
        EVENT_THREAD_CREATED = 0,
        EVENT_THREAD_DESTROYED,
        EVENT_THREAD_WOKEN,
        EVENT_ACTOR_CREATED,
        EVENT_ACTOR_DESTROYED,
        EVENT_ACTOR_PROCESSED,
        EVENT_MESSAGE_SENT,
        EVENT_SIMPLEALLOCATOR_ALLOCATE,
        EVENT_SIMPLEALLOCATOR_FREE,
        EVENT_MESSAGECACHE_HIT,
        EVENT_MESSAGECACHE_MISS,
        EVENT_GLOBALFREELIST_HIT,
        EVENT_GLOBALFREELIST_MISS,
        EVENT_RECEIVER_WAIT,
        EVENT_RECEIVER_LOCK,
        MAX_EVENTS
    };

#if THERON_ENABLE_METRICS
    /// \brief String names describing the counted events.
    static const char *smEventNames[MAX_EVENTS];
#endif // THERON_ENABLE_METRICS

    /// \brief Returns true if metrics are enabled.
    inline static bool Enabled();

    /// \brief Gets a reference to the single instance of the singleton.
    inline static Metrics &Instance();

    /// \brief Resets the event counts.
    void Reset();

    /// \brief Counts an event.
    inline void Count(const Event event);

    /// \brief Returns the hit count for a specific event.
    inline uint32_t GetCount(const Event event) const;

    /// \brief Prints the event counts.
    void Print() const;

private:

    /// Singleton constructor.
    Metrics();

    static Metrics smInstance;          ///< Singleton instance.

    // These are present unconditionally to avoid alignment/size differences.
    uint32_t mCounts[MAX_EVENTS];       ///< Counts of various events.

#if THERON_ENABLE_METRICS
    mutable Mutex mMutex;               ///< Protected thread access.
#endif // THERON_ENABLE_METRICS
   
};


THERON_FORCEINLINE bool Metrics::Enabled()
{

#if THERON_ENABLE_METRICS
    return true;
#else
    return false;
#endif // THERON_ENABLE_METRICS

}


THERON_FORCEINLINE Metrics &Metrics::Instance()
{
    return smInstance;
}


THERON_FORCEINLINE void Metrics::Count(const Event eventId)
{
    THERON_ASSERT(eventId < MAX_EVENTS);

    (void) eventId;

    {
#if THERON_ENABLE_METRICS

        Lock lock(mMutex);
        ++mCounts[eventId];

#endif // THERON_ENABLE_METRICS
    }
}


THERON_FORCEINLINE uint32_t Metrics::GetCount(const Event eventId) const
{
    uint32_t count(0);
    THERON_ASSERT(eventId < MAX_EVENTS);

    (void) eventId;

    {
#if THERON_ENABLE_METRICS

        Lock lock(mMutex);
        count = mCounts[eventId];

#endif // THERON_ENABLE_METRICS
    }

    return count;
}


} // namespace Theron


#endif // THERON_METRICS_H

