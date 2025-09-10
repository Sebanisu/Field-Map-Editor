#include "utilities.hpp"
#include <spdlog/spdlog.h>
#include <string>
#include <vector>
namespace fme
{
// Explicit instantiation for specific types
template bool sort_and_remove_duplicates(
  std::vector<std::string> &,
  std::vector<bool> &);
template bool sort_and_remove_duplicates(std::vector<std::string> &);

bool          has_balanced_braces([[maybe_unused]] const std::string_view s)
{
     int balance = 0;
     for (const char c : s)
     {
          if (c == '{')
          {
               ++balance;
          }
          else if (c == '}')
          {
               --balance;
               if (balance < 0)
               {
                    spdlog::error("Unmatched closing brace in input: \"{}\" (note: literal braces shown as {{ and }})", s);
                    return false;
               }
          }
     }

     if (balance != 0)
     {
          spdlog::error("Mismatched brace count in input: \"{}\" ({} unmatched opening brace{{}})", s, balance);
          return false;
     }

     return true;
}

bool has_balanced_braces([[maybe_unused]] const std::u8string_view s)
{
     int balance = 0;
     for (const char8_t c : s)
     {
          if (c == u8'{')
          {
               ++balance;
          }
          else if (c == u8'}')
          {
               --balance;
               if (balance < 0)
               {
                    spdlog::error(
                      "Unmatched closing brace in input: \"{}\" (note: literal braces shown as {{ and }})",
                      reinterpret_cast<const char *>(s.data()));
                    return false;
               }
          }
     }

     if (balance != 0)
     {
          spdlog::error(
            "Mismatched brace count in input: \"{}\" ({} unmatched opening brace{{}})", reinterpret_cast<const char *>(s.data()), balance);
          return false;
     }

     return true;
}
}// namespace fme
