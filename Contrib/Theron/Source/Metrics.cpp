// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#include <new>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/AllocatorManager.h>
#include <Theron/Metrics.h>


namespace Theron
{


Metrics Metrics::smInstance;


#if THERON_ENABLE_METRICS
const char *Metrics::smEventNames[MAX_EVENTS] =
{
    "Threads created",
    "Threads destroyed",
    "Threads woken",
    "Actors created",
    "Actors destroyed",
    "Actors processed",
    "Messages sent",
    "SimpleAllocator allocations",
    "SimpleAllocator frees",
    "MessageCache hits",
    "MessageCache misses",
    "GlobalFreeList hits",
    "GlobalFreeList misses",
    "Receiver waits",
    "Receiver locks",
};
#endif // THERON_ENABLE_METRICS


Metrics::Metrics()
{
    Reset();
}


void Metrics::Reset()
{

#if THERON_ENABLE_METRICS
    // Reset the event counts.
    for (uint32_t index = 0; index < MAX_EVENTS; ++index)
    {
        mCounts[index] = 0;
    }
#endif // THERON_ENABLE_METRICS

}


void Metrics::Print() const
{

#if THERON_ENABLE_METRICS
    // Print the event counts.
    for (uint32_t index = 0; index < MAX_EVENTS; ++index)
    {
        printf("%s: %d\n", smEventNames[index], mCounts[index]);
    }
#else
    printf("Metrics not enabled\n");
#endif // THERON_ENABLE_METRICS

}


} // namespace Theron


