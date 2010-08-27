// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_THREADING_BOOST_THREAD_H
#define THERON_DETAIL_THREADING_BOOST_THREAD_H


#ifdef _MSC_VER
#pragma warning(push,0)
#endif //_MSC_VER

#include <new>
#include <boost/thread/thread.hpp>

#ifdef _MSC_VER
#pragma warning(pop)
#endif //_MSC_VER

#include <Theron/Detail/Debug/Assert.h>

#include <Theron/AllocatorManager.h>

#include <Theron/Defines.h>


namespace Theron
{


/// Thread class implementing a simple system thread.
/// \note This implementation uses Boost threads so depends on the Boost library
class Thread
{
public:

    /// Defines a function that can serve as a thread entry point.
    /// \note Entry point functions must be static -- implying that they can't be class
    /// member functions.
    typedef void (*EntryPoint)(void *const context);

    /// Default constructor
    THERON_FORCEINLINE Thread() : mThread(0)
    {
    }

    /// Destructor
    THERON_FORCEINLINE ~Thread()
    {
        // Explicitly destruct because allocated with placement new.
        mThread->~thread();
        AllocatorManager::Instance().GetAllocator()->Free(mThread); 
    }

    /// Starts the thread, executing the given entry point function.
    /// \param entryPoint The entry point function that the thread should execute.
    /// \param context Pointer to a context object providing the environment in which the thread runs.
    /// \return True, if the thread was started successfully.
    inline bool Start(EntryPoint entryPoint, void *const context);

    /// Waits for the thread to finish and return.
    THERON_FORCEINLINE void Join()
    {
        THERON_ASSERT(mThread);
        mThread->join();
    }

private:

    class ThreadStarter
    {
    public:
    
        THERON_FORCEINLINE ThreadStarter(EntryPoint entryPoint, void *const context) :
          mEntryPoint(entryPoint),
          mContext(context)
        {
        }

        THERON_FORCEINLINE void operator()()
        {
            mEntryPoint(mContext);
        }

    private:
    
        EntryPoint mEntryPoint;
        void *mContext;
    };

    /// Disallowed copy constructor. Threads can't be copied.
    Thread(const Thread &other);
    
    /// Disallowed assignment operator. Threads can't be assigned.
    Thread &operator=(const Thread &other);

    boost::thread *mThread;     ///< Pointer to the owned boost::thread.
};


THERON_FORCEINLINE bool Thread::Start(EntryPoint entryPoint, void *const context)
{
    THERON_ASSERT(mThread == 0);

    // Allocate memory for a boost::thread object. They're not copyable.
    void *const memory = AllocatorManager::Instance().GetAllocator()->Allocate(sizeof(boost::thread));
    if (memory == 0)
    {
        return false;
    }

    // Construct a boost::mutex in the allocated memory
    // Pass it a callable object that in turn calls the entry point, passing it some context.
    ThreadStarter starter(entryPoint, context);
    mThread = new (memory) boost::thread(starter);

    if (mThread == 0)
    {
        return false;
    }

    return true;
}


} // namespace Theron


#endif // THERON_DETAIL_THREADING_BOOST_THREAD_H

