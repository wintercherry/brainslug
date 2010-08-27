// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_THREADPOOL_EXECUTIONSTATE_H
#define THERON_DETAIL_THREADPOOL_EXECUTIONSTATE_H


namespace Theron
{


/// Enum representing the execution state of an actor.
enum ExecutionState
{
    STATE_INVALID = 0,                  ///< Invalid value.
    STATE_IDLE,                         ///< Not waiting or being processed.
    STATE_BUSY,                         ///< Waiting or being processed.
    STATE_DIRTY,                        ///< In need of more processing after current.
    STATE_FORCESIZEINT = 0xFFFFFFFF     ///< Ensures the enum is an integer.
};


} // namespace Theron


#endif // THERON_DETAIL_THREADPOOL_EXECUTIONSTATE_H

