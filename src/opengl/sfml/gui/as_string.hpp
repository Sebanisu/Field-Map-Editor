#ifndef D21F089E_0643_4959_BDC9_40CD98C12D87
#define D21F089E_0643_4959_BDC9_40CD98C12D87
#include <fmt/format.h>
#include <string>
namespace fme
{
struct AsString
{
     template<typename T>
     std::string operator()(const T &value) const
     {
          return fmt::format("{}", value);
     }
};
}// namespace fme
#endif /* D21F089E_0643_4959_BDC9_40CD98C12D87 */
