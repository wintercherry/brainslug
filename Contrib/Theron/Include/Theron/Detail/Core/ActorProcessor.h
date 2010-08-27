// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_CORE_ACTORPROCESSOR_H
#define THERON_DETAIL_CORE_ACTORPROCESSOR_H


#include <Theron/Detail/Core/ActorCore.h>

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/Detail/Messages/IMessage.h>

#include <Theron/Actor.h>
#include <Theron/AllocatorManager.h>
#include <Theron/Defines.h>
#include <Theron/Metrics.h>


namespace Theron
{


/// Processor class for the message handler work queue.
class ActorProcessor
{
public:

    /// Processes an actor.
    /// If the actor is referenced then message handlers are executed
    /// for one queued message. Otherwise the actor is destroyed.
    /// \return True, if the actor still needs more processing.
    THERON_FORCEINLINE static void Process(ActorCore *&actorCore, IMessage *const message)
    {
        THERON_ASSERT(actorCore);

        // TODO: We could use the message pointer as the test here!
        // Process the actor, if it's still referenced.
        // NOTE: We chance our arm here and read the value without locking,
        // since we only care whether it's reached zero, and once it does
        // it can't magically become non-zero again.
        if (actorCore->ReferenceCount().Value() > 0)
        {
            // We expect referenced actors to be processed only for arrived messages.
            THERON_ASSERT(message);

            // Update the actor's message handler registrations.
            actorCore->UpdateHandlers();

            // Process the message.
            actorCore->ProcessMessage(message);
            actorCore->FreeMessage(message);
        }
        else
        {
            // TODO: Handle more gracefully.
            if (message)
            {
                actorCore->FreeMessage(message);
            }

            // Otherwise destroy it.
            Destroy(actorCore);
        }
    }
    
private:

    // This method is intended to not be inlined.
    inline static void Destroy(ActorCore *&actorCore)
    {
        Actor *const actor(actorCore->GetParent());
        THERON_ASSERT(actor);

        actor->~Actor();
        AllocatorManager::Instance().GetAllocator()->Free(actor);

        // Null the passed pointer to indicate it's now invalid.
        actorCore = 0;
    }
};


} // namespace Theron


#endif // THERON_DETAIL_CORE_ACTORPROCESSOR_H

