// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#include <Theron/Detail/Core/GlobalFreeList.h>

#include <Theron/Detail/Directory/Directory.h>

#include <Theron/Detail/Threading/Lock.h>

#include <Theron/Address.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Metrics.h>
#include <Theron/Receiver.h>


namespace Theron
{


Receiver::Receiver() :
  mMessageHandlers(),
  mMonitor(),
  mMessagesReceived(0)
{
    // Create the global free list, if it doesn't exist yet.
    GlobalFreeList::Reference();

    // Assign a proper unique address.
    SetAddress(Address());

    Directory::Lock();
    {
        Directory::Instance().Register(this);
    }
    Directory::Unlock();
}


Receiver::~Receiver()
{
    Directory::Lock();
    {
        Directory::Instance().Deregister(this);
    }
    Directory::Unlock();

    {
        Lock lock(mMonitor.GetMutex());

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
    }

    // Destroy the global free list, if we were the last to reference it.
    GlobalFreeList::Dereference();
}


void Receiver::Push(IMessage *const message, const bool /*wake*/)
{
    THERON_ASSERT(message);

    {
        Lock lock(mMonitor.GetMutex());
    
        List<MessageHandlerBase *>::const_iterator handlers(mMessageHandlers.Begin());
        const List<MessageHandlerBase *>::const_iterator handlersEnd(mMessageHandlers.End());

        while (handlers != handlersEnd)
        {
            // Execute the handler.
            // It does nothing if it can't handle the message type.
            MessageHandlerBase *const handler(*handlers);
            handler->Handle(message);

            ++handlers;
        }

        // Wake up anyone who's waiting for a message to arrive.
        ++mMessagesReceived;

        mMonitor.Pulse();
    }

    // Free the message, whether it was handled or not.
    GlobalFreeList::Instance().Free(message, message->Size());
}


void Receiver::Wait()
{
    Lock lock(mMonitor.GetMutex());

    Metrics::Instance().Count(Metrics::EVENT_RECEIVER_WAIT);

    // If messages were received since the last wait (or creation),
    // then we regard those messages as qualifying and early-exit.
    while (mMessagesReceived == 0)
    {
        Metrics::Instance().Count(Metrics::EVENT_RECEIVER_LOCK);

        // Wait to be woken by an arriving message.
        // This blocks until a message arrives!
        mMonitor.Wait(lock);
    }

    THERON_ASSERT(mMessagesReceived > 0);
    --mMessagesReceived;
}


} // namespace Theron


