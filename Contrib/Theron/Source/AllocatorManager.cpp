// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#include <Theron/AllocatorManager.h>


namespace Theron
{


AllocatorManager AllocatorManager::sInstance;


AllocatorManager &AllocatorManager::Instance()
{
    return sInstance;
}


} // namespace Theron

