#ifndef D21F089E_0643_4959_BDC9_40CD98C12D87
#define D21F089E_0643_4959_BDC9_40CD98C12D87
#include <string>
#include <fmt/format.h>
struct AsString
{
     template<typename T>
     std::string operator()(const T &value) const
     {
          return fmt::format("{}", value);
     }
};

#endif /* D21F089E_0643_4959_BDC9_40CD98C12D87 */
