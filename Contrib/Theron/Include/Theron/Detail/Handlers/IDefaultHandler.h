// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_HANDLERS_IDEFAULTHANDLER_H
#define THERON_DETAIL_HANDLERS_IDEFAULTHANDLER_H


#include <Theron/Detail/Messages/IMessage.h>


namespace Theron
{


class Actor;


/// Interface that allows a default handler on an unknown actor to be referenced.
class IDefaultHandler
{
public:

    /// Default constructor.
    inline IDefaultHandler()
    {
    }

    /// Virtual destructor.
    inline virtual ~IDefaultHandler()
    {
    }

    /// Handles the given message.
    virtual void Handle(const IMessage *const message) const = 0;
};


} // namespace Theron


#endif // THERON_DETAIL_HANDLERS_IDEFAULTHANDLER_H

