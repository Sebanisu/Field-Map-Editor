#ifndef B681257A_AA38_4E42_9BAB_958BA6EE00D5
#define B681257A_AA38_4E42_9BAB_958BA6EE00D5
#include <string_view>

struct LabelKey
{
   private:
     std::string_view key = {};

   public:
     constexpr LabelKey() = default;
     template<size_t N>
     constexpr LabelKey(const char (&lit)[N])
       : key(lit, N)
     {
     }
     constexpr bool has_value() const
     {
          return !key.empty();
     }
     constexpr bool empty() const
     {
          return key.empty();
     }
     constexpr std::string_view value() const
     {
          return key;
     }
     constexpr operator std::string_view() const
     {
          return key;
     }
     constexpr std::string_view operator*() const
     {
          return key;
     }
     constexpr size_t size() const
     {
          return key.size();
     }
     constexpr std::string_view value_or(std::string_view fallback) const
     {
          return key.empty() ? fallback : key;
     }
};
#endif /* B681257A_AA38_4E42_9BAB_958BA6EE00D5 */
