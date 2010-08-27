// Theron (http://theron.ashtonmason.net) by Ashton Mason (ash@ashtonmason.net).
// This work is licensed under the Creative Commons Attribution 3.0 License.
// To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/
// or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco,
// California, 94105, USA.


#ifndef THERON_DETAIL_DEBUG_ASSERT_H
#define THERON_DETAIL_DEBUG_ASSERT_H


#include <stdio.h>
#include <assert.h>


#ifdef _DEBUG

#define THERON_ASSERT(condition)               if (!(condition)) Theron::Fail(__FILE__, __LINE__); else { }
#define THERON_ASSERT_MSG(condition, msg)      if (!(condition)) Theron::Fail(__FILE__, __LINE__, msg); else { }
#define THERON_FAIL()                          Theron::Fail(__FILE__, __LINE__)
#define THERON_FAIL_MSG(msg)                   Theron::Fail(__FILE__, __LINE__, msg)

#else

#define THERON_ASSERT(condition)
#define THERON_ASSERT_MSG(condition, msg)
#define THERON_FAIL()
#define THERON_FAIL_MSG(msg)

#endif //_DEBUG



namespace Theron
{


#ifdef _DEBUG
/// Reports an internal application or system failure.
/// A message describing the failure is printed to stderr.
/// \param file The name of the file in which the failure occurred.
/// \param line The line number at which the failure occurred.
/// \param message A message describing the failure.
/// \note This method doesn't currently halt application execution.
inline void Fail(const char *const file, const unsigned int line, const char *const message = 0)
{
    fprintf(stderr, "FAIL in %s (%d)\n", file, line);
    if (message)
    {
        fprintf(stderr, "ERROR: %s\n", message);
    }

    assert(false);
}
#endif //_DEBUG


} // namespace Theron


#endif // THERON_DETAIL_DEBUG_ASSERT_H

