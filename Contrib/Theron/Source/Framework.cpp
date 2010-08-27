// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#include <Theron/Detail/Core/GlobalFreeList.h>

#include <Theron/Framework.h>


namespace Theron
{


Framework::Framework() :
  mWorkQueue(),
  mProcessor(mWorkQueue)
{
    // Create the global free list, if it doesn't exist yet.
    GlobalFreeList::Reference();

    mProcessor.Start(2);
}


Framework::Framework(const uint32_t numThreads) :
  mWorkQueue(),
  mProcessor(mWorkQueue)
{
    // Create the global free list, if it doesn't exist yet.
    GlobalFreeList::Reference();

    THERON_ASSERT_MSG(numThreads > 0, "numThreads must be greater than zero");
    mProcessor.Start(numThreads);
}


Framework::~Framework()
{
    mProcessor.Stop();

    // Destroy the global free list, if we were the last to reference it.
    GlobalFreeList::Dereference();
}


} // namespace Theron

