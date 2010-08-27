// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#include <new>

#include <Theron/Detail/Core/GlobalFreeList.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/AllocatorManager.h>


namespace Theron
{


Mutex GlobalFreeList::smMutex;
uint32_t GlobalFreeList::smReferenceCount = 0;
GlobalFreeList *GlobalFreeList::smInstance = 0;


void GlobalFreeList::CreateInstance()
{
    THERON_ASSERT(smInstance == 0);

    void *const memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(GlobalFreeList));
    THERON_ASSERT(memory);

    smInstance = new (memory) GlobalFreeList;
    THERON_ASSERT(smInstance);
}


void GlobalFreeList::DestroyInstance()
{
    THERON_ASSERT(smInstance);

    // We have to explicitly destruct the instance because we in-place constructed it.
    smInstance->~GlobalFreeList();
    AllocatorManager::Instance().GetAllocator()->Free(smInstance);
    smInstance = 0;
}


} // namespace Theron


