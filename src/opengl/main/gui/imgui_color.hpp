#ifndef CC65C6B1_E0C1_485A_BC29_965EB1AE26A7
#define CC65C6B1_E0C1_485A_BC29_965EB1AE26A7
#include <concepts>
#include <imgui.h>
#include <open_viii/Concepts.hpp>
#include <type_traits>

template<typename T>
concept is_enum = std::is_enum_v<T>;
template<typename T>
concept is_enum_or_integral = is_enum<T> || std::integral<T>;
template<
  is_enum_or_integral number_type,
  is_enum_or_integral... rest_number_type>
inline static constexpr number_type bitwise_or(
  number_type first,
  rest_number_type... rest)
{
     return static_cast<number_type>(
       static_cast<std::uint32_t>(first)
       | (static_cast<std::uint32_t>(rest) | ...));
}
template<
  is_enum_or_integral number_type,
  is_enum_or_integral... rest_number_type>
inline static constexpr number_type bitwise_and(
  number_type start,
  rest_number_type... rest)
{
     return static_cast<number_type>(
       static_cast<std::uint32_t>(start)
       & (static_cast<std::uint32_t>(rest) & ...));
}
template<is_enum_or_integral number_type>
inline static constexpr number_type bitwise_not(number_type value)
{
     return static_cast<number_type>(~static_cast<std::uint32_t>(value));
}
inline static constexpr ImU32 imgui_color32(
  std::uint8_t red,
  std::uint8_t green,
  std::uint8_t blue,
  std::uint8_t alpha)
{
     constexpr auto R_SHIFT = 0U;
     constexpr auto G_SHIFT = 8U;
     constexpr auto B_SHIFT = 16U;
     constexpr auto A_SHIFT = 24U;
     return bitwise_or(
       ImU32{},
       alpha << A_SHIFT,
       blue << B_SHIFT,
       green << G_SHIFT,
       red << R_SHIFT);
}
inline static constexpr ImU32
  imgui_color32(open_viii::struct_of_color32_byte auto color)
{
     return imgui_color32(color.r, color.g, color.b, color.a);
}
inline static constexpr ImU32 imgui_color32(open_viii::Color auto color)
{
     return imgui_color32(color.r(), color.b(), color.b(), color.a());
}

#endif /* CC65C6B1_E0C1_485A_BC29_965EB1AE26A7 */
