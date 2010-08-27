// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#include <Theron/Detail/BasicTypes.h>

#include <Theron/Detail/Core/ActorCore.h>
#include <Theron/Detail/Core/GlobalFreeList.h>

#include <Theron/Detail/Directory/Directory.h>

#include <Theron/Detail/Threading/Lock.h>

#include <Theron/Detail/ThreadPool/ExecutionState.h>

#include <Theron/Actor.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Framework.h>


namespace Theron
{


ActorCore::ActorCore() :
  mParent(0),
  mFramework(0),
  mReferenceCount(0),
  mMessageQueue(),
  mMessageHandlers(),
  mDefaultMessageHandler(0),
  mState(STATE_IDLE),
  mNewHandlerQueue(),
  mHandlersDirty(false),
  mWorkQueue(0),
  mMessageCache()
{
    // Create the global free list, if it doesn't exist yet.
    GlobalFreeList::Reference();
}


ActorCore::~ActorCore()
{
    // Destroy the global free list, if we were the last to reference it.
    GlobalFreeList::Dereference();

    Directory::Lock();
    {
        Directory::Instance().Deregister(this);
    }
    Directory::Unlock();

    // Update the handlers to clear the new handler list.
    UpdateHandlers();

    // We don't need to lock this because only one thread can access it at a time.
    // Free all currently allocated handler objects.
    List<MessageHandlerBase *>::const_iterator handlers(mMessageHandlers.Begin());
    const List<MessageHandlerBase *>::const_iterator handlersEnd(mMessageHandlers.End());

    while (handlers != handlersEnd)
    {
        MessageHandlerBase *const messageHandler(*handlers);

        // Free the handler object, which was allocated on registration.
        AllocatorManager::Instance().GetAllocator()->Free(messageHandler);

        ++handlers;
    }

    mMessageHandlers.Clear();

    // Destroy the default handler, if one is set.
    if (mDefaultMessageHandler)
    {
        AllocatorManager::Instance().GetAllocator()->Free(mDefaultMessageHandler);
    }

    // Free any left-over messages that haven't been processed.
    // This is undesirable but can happen if the actor is killed while
    // still processing messages.
    {
        Lock lock(mWorkQueue->GetMutex());

        while (!mMessageQueue.Empty())
        {
            IMessage *const message(mMessageQueue.Peek());
            AllocatorManager::Instance().GetAllocator()->Free(message);

            mMessageQueue.Pop();
        }
    }
}


bool ActorCore::SetAddress(const Theron::Address address)
{
    bool success(true);

    THERON_ASSERT(GetAddress() == Address::Null());
    THERON_ASSERT(address != Address::Null());

    IEntity::SetAddress(address);

    Directory::Lock();
    {
        // Check for an existing entity at this address.
        if (Directory::Instance().Lookup(address) == 0)
        {
            Directory::Instance().Register(this);
        }
        else
        {
            success = false;
        }
    }
    Directory::Unlock();

    return success;
}


void ActorCore::DoUpdateHandlers()
{
    // Remove any handlers marked for deletion.
    bool done(false);
    while (!done)
    {
        done = true;

        // We don't need to lock this because only one thread can access it at a time.
        List<MessageHandlerBase *>::const_iterator handlersIt(mMessageHandlers.Begin());
        const List<MessageHandlerBase *>::const_iterator handlersEnd(mMessageHandlers.End());

        while (handlersIt != handlersEnd)
        {
            MessageHandlerBase *const handler(*handlersIt);
            if (handler->IsMarked())
            {
                mMessageHandlers.Remove(handler);

                // We don't need to lock this because only one thread can access it at a time.
                // Free the handler object, which was allocated on registration.
                AllocatorManager::Instance().GetAllocator()->Free(handler);

                // Restart the search since the iteration may be broken by deletion.
                done = false;
                break;
            }

            ++handlersIt;
        }
    }

    // Add any new handlers
    while (!mNewHandlerQueue.Empty())
    {
        MessageHandlerBase *const handler(mNewHandlerQueue.Peek());
        mMessageHandlers.Insert(handler);
        mNewHandlerQueue.Pop();
    }
}


} // namespace Theron


