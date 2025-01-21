#ifndef E9FF3A6F_3CCA_4C0B_86CF_665262267C20
#define E9FF3A6F_3CCA_4C0B_86CF_665262267C20
#include <array>
#include <bit>
#include <compare>
#include <concepts>
#include <cstdint>
#include <imgui.h>
#include <limits>
#include <numeric>
#include <SFML/Graphics/Color.hpp>
#include <version>
namespace fme
{
struct colors
{
     /**
      * @brief Represents a color with RGBA components.
      *
      * This structure holds the red, green, blue, and alpha components of a color.
      * The components are stored as 8-bit unsigned integers (0-255).
      */
     struct [[nodiscard]] color
     {
          std::uint8_t r;//!< Red component (0-255)
          std::uint8_t g;//!< Green component (0-255)
          std::uint8_t b;//!< Blue component (0-255)
          std::uint8_t a;//!< Alpha (opacity) component (0-255)

          /**
           * @brief Converts the color structure to an sf::Color object.
           *
           * @return sf::Color The converted color.
           */
          [[nodiscard]] operator sf::Color() const noexcept
          {
               return std::bit_cast<sf::Color>(*this);
          }

          /**
           * @brief Compares two color objects.
           *
           * @param other Another color object to compare with.
           * @return The result of the comparison.
           */
          constexpr auto operator<=>(const color &) const = default;
     };

     /**
      * @brief Converts an sf::Color to an ImColor.
      *
      * @param c The sf::Color to convert.
      * @return ImColor The converted color.
      */
     [[nodiscard]] static constexpr ImColor toImColor(const sf::Color &c) noexcept
     {
          return ImColor(static_cast<int>(c.r), static_cast<int>(c.g), static_cast<int>(c.b), static_cast<int>(c.a));
     }

     /**
      * @brief Converts an sf::Color to an ImVec4.
      *
      * @param c The sf::Color to convert.
      * @return ImVec4 The converted color in floating-point format [0, 1].
      */
     [[nodiscard]] static constexpr ImVec4 toImVec4(const sf::Color &c) noexcept
     {
          constexpr auto max = static_cast<float>((std::numeric_limits<std::uint8_t>::max)());
          return {
               static_cast<float>(c.r) / max, static_cast<float>(c.g) / max, static_cast<float>(c.b) / max, static_cast<float>(c.a) / max
          };
     }

     /**
      * @brief Converts an ImVec4 color to an sf::Color.
      *
      * @param c The input color in ImVec4 format, with components in the range [0, 1].
      * @return color The converted color with components scaled to the range [0, 255].
      */
     [[nodiscard]] static constexpr color toSFColor(const ImVec4 &c) noexcept
     {
          constexpr auto max = static_cast<float>((std::numeric_limits<std::uint8_t>::max)());

#if defined(__cpp_lib_saturation_arithmetic) && __cpp_lib_saturation_arithmetic >= 202311L
          return { std::saturate_cast<std::uint8_t>(c.x * max),
                   std::saturate_cast<std::uint8_t>(c.y * max),
                   std::saturate_cast<std::uint8_t>(c.z * max),
                   std::saturate_cast<std::uint8_t>(c.w * max) };
#else
          return { static_cast<std::uint8_t>(c.x * max),
                   static_cast<std::uint8_t>(c.y * max),
                   static_cast<std::uint8_t>(c.z * max),
                   static_cast<std::uint8_t>(c.w * max) };
#endif
     }

     /**
      * @brief Converts RGBA values (as 4 uint8_t components) to an sf::Color.
      *
      * @param r The red component (0-255).
      * @param g The green component (0-255).
      * @param b The blue component (0-255).
      * @param a The alpha component (0-255), defaults to 255 (fully opaque).
      * @return color The converted color.
      */
     template<std::integral int_t>
     [[nodiscard]] static constexpr color toSFColor(int_t r, int_t g, int_t b, int_t a) noexcept
     {
#if defined(__cpp_lib_saturation_arithmetic) && __cpp_lib_saturation_arithmetic >= 202311L
          return { std::saturate_cast<std::uint8_t>(r),
                   std::saturate_cast<std::uint8_t>(g),
                   std::saturate_cast<std::uint8_t>(b),
                   std::saturate_cast<std::uint8_t>(a) };
#else
          return { static_cast<std::uint8_t>(r), static_cast<std::uint8_t>(g), static_cast<std::uint8_t>(b), static_cast<std::uint8_t>(a) };
#endif
     }

     /**
      * @brief Converts RGBA values (as 3 uint8_t components) to an sf::Color with fully opaque alpha.
      *
      * @param r The red component (0-255).
      * @param g The green component (0-255).
      * @param b The blue component (0-255).
      * @return color The converted color, with alpha set to 255 (fully opaque).
      */
     template<std::integral int_t>
     [[nodiscard]] static constexpr color toSFColor(int_t r, int_t g, int_t b) noexcept
     {
#if defined(__cpp_lib_saturation_arithmetic) && __cpp_lib_saturation_arithmetic >= 202311L
          return { std::saturate_cast<std::uint8_t>(r),
                   std::saturate_cast<std::uint8_t>(g),
                   std::saturate_cast<std::uint8_t>(b),
                   (std::numeric_limits<std::uint8_t>::max)() };
#else
          return { static_cast<std::uint8_t>(r),
                   static_cast<std::uint8_t>(g),
                   static_cast<std::uint8_t>(b),
                   (std::numeric_limits<std::uint8_t>::max)() };
#endif
     }

     /**
      * @brief Converts RGBA values (as 4 float components) to an sf::Color.
      *
      * @param r The red component (0.0f-1.0f).
      * @param g The green component (0.0f-1.0f).
      * @param b The blue component (0.0f-1.0f).
      * @param a The alpha component (0.0f-1.0f), defaults to 1.0f (fully opaque).
      * @return color The converted color, with each component scaled to the range [0, 255].
      */
     template<std::floating_point float_t>
     [[nodiscard]] static constexpr color toSFColor(float_t r, float_t g, float_t b, float_t a) noexcept
     {
          constexpr auto max = static_cast<float_t>((std::numeric_limits<std::uint8_t>::max)());
#if defined(__cpp_lib_saturation_arithmetic) && __cpp_lib_saturation_arithmetic >= 202311L
          return { std::saturate_cast<std::uint8_t>(r * max),
                   std::saturate_cast<std::uint8_t>(g * max),
                   std::saturate_cast<std::uint8_t>(b * max),
                   std::saturate_cast<std::uint8_t>(a * max) };
#else
          return { static_cast<std::uint8_t>(r * max),
                   static_cast<std::uint8_t>(g * max),
                   static_cast<std::uint8_t>(b * max),
                   static_cast<std::uint8_t>(a * max) };
#endif
     }

     /**
      * @brief Converts RGBA values (as 3 float components) to an sf::Color with fully opaque alpha.
      *
      * @param r The red component (0.0f-1.0f).
      * @param g The green component (0.0f-1.0f).
      * @param b The blue component (0.0f-1.0f).
      * @return color The converted color, with alpha set to 1.0f (fully opaque).
      */
     template<std::floating_point float_t>
     [[nodiscard]] static constexpr color toSFColor(float_t r, float_t g, float_t b) noexcept
     {
#if defined(__cpp_lib_saturation_arithmetic) && __cpp_lib_saturation_arithmetic >= 202311L
          return { std::saturate_cast<std::uint8_t>(r * max),
                   std::saturate_cast<std::uint8_t>(g * max),
                   std::saturate_cast<std::uint8_t>(b * max),
                   (std::numeric_limits<std::uint8_t>::max)() };
#else
          constexpr auto max = static_cast<float_t>((std::numeric_limits<std::uint8_t>::max)());
          return { static_cast<std::uint8_t>(r * max),
                   static_cast<std::uint8_t>(g * max),
                   static_cast<std::uint8_t>(b * max),
                   (std::numeric_limits<std::uint8_t>::max)() };
#endif
     }
};
// Static assert for ImVec4 overload
static_assert(colors::toSFColor(ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f }) == colors::color(255, 0, 0, 255), "ImVec4 to color conversion failed");
static_assert(colors::toSFColor(ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f }) == colors::color(0, 255, 0, 255), "ImVec4 to color conversion failed");
static_assert(colors::toSFColor(ImVec4{ 0.0f, 0.0f, 1.0f, 1.0f }) == colors::color(0, 0, 255, 255), "ImVec4 to color conversion failed");
static_assert(
  colors::toSFColor(ImVec4{ 0.5f, 0.5f, 0.5f, 0.5f }) == colors::color(127, 127, 127, 127),
  "ImVec4 to color conversion failed");

// Static assert for uint8_t overload
static_assert(colors::toSFColor(255, 0, 0) == colors::color(255, 0, 0, 255), "uint8_t to color conversion failed");
static_assert(colors::toSFColor(0, 255, 0) == colors::color(0, 255, 0, 255), "uint8_t to color conversion failed");
static_assert(colors::toSFColor(0, 0, 255, 128) == colors::color(0, 0, 255, 128), "uint8_t to color conversion failed");

// Static assert for float overload
static_assert(colors::toSFColor(1.0f, 0.0f, 0.0f) == colors::color(255, 0, 0, 255), "float to color conversion failed");
static_assert(colors::toSFColor(0.0f, 1.0f, 0.0f) == colors::color(0, 255, 0, 255), "float to color conversion failed");
static_assert(colors::toSFColor(0.0f, 0.0f, 1.0f, 0.5f) == colors::color(0, 0, 255, 127), "float to color conversion failed");

// Static assert for default alpha
static_assert(colors::toSFColor(255, 0, 0) == colors::color(255, 0, 0, 255), "Default alpha in uint8_t overload failed");
static_assert(colors::toSFColor(1.0f, 1.0f, 1.0f) == colors::color(255, 255, 255, 255), "Default alpha in float overload failed");
}// namespace fme

#endif /* E9FF3A6F_3CCA_4C0B_86CF_665262267C20 */
