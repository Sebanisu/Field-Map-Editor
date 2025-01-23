#ifndef E9FF3A6F_3CCA_4C0B_86CF_665262267C20
#define E9FF3A6F_3CCA_4C0B_86CF_665262267C20
#include <algorithm>
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
/**
 * @brief Represents a color with RGBA components.
 *
 * This structure holds the red, green, blue, and alpha components of a color.
 * The components are stored as 8-bit unsigned integers (0-255).
 */
struct [[nodiscard]] color
{
     std::uint8_t r                          = {};//!< Red component (0-255)
     std::uint8_t b                          = {};//!< Green component (0-255)
     std::uint8_t g                          = {};//!< Blue component (0-255)
     std::uint8_t a                          = { (std::numeric_limits<std::uint8_t>::max)() };//!< Alpha (opacity) component (0-255)

     /**
      * @brief Default constructor.
      *
      * Initializes the color to fully opaque black.
      */
     constexpr color() noexcept              = default;

     /**
      * @brief Copy constructor.
      */
     constexpr color(const color &) noexcept = default;

     /**
      * @brief Move constructor.
      */
     constexpr color(color &&) noexcept      = default;

     /**
      * @brief Copy assignment operator.
      */
     constexpr color &operator=(const color &) noexcept = default;

     /**
      * @brief Move assignment operator.
      */
     constexpr color &operator=(color &&) noexcept      = default;

     /**
      * @brief Maximum value for floating-point conversions.
      *
      * @tparam float_t A floating-point type.
      */
     template<std::floating_point float_t>
     static constexpr auto max_f = static_cast<float_t>((std::numeric_limits<std::uint8_t>::max)());


     /**
      * @brief Minimum value for floating-point conversions.
      *
      * @tparam float_t A floating-point type.
      */
     template<std::floating_point float_t>
     static constexpr auto min_f = static_cast<float_t>((std::numeric_limits<std::uint8_t>::min)());

     /**
      * @brief Maximum value for integral conversions.
      *
      * @tparam int_t An integral type.
      */
     template<std::integral int_t>
     static constexpr auto max_i = static_cast<int_t>((std::numeric_limits<std::uint8_t>::max)());

     /**
      * @brief Constructs a color from an ImVec4 instance.
      *
      * @param c The ImVec4 object containing RGBA components.
      */
     explicit constexpr color(const ImVec4 c) noexcept
       : color(c.x, c.y, c.z, c.w)
     {
     }

     /**
      * @brief Constructs a color from an ImColor instance.
      *
      * @param c The ImColor object containing RGBA components.
      */
     explicit constexpr color(const ImColor &c) noexcept
       : color(c.Value)
     {
     }

     /**
      * @brief Constructs a color from an SFML color instance.
      *
      * @param c The sf::Color object containing RGBA components.
      */
     explicit constexpr color(const sf::Color &c) noexcept
       : color(std::bit_cast<color>(c))
     {
     }

     /**
      * @brief Constructs a color from integral RGBA components.
      *
      * @tparam int_t An integral type.
      * @param in_r Red component.
      * @param in_g Green component.
      * @param in_b Blue component.
      * @param in_a Alpha (opacity) component.
      */
     template<std::integral int_t>
     constexpr color(int_t in_r, int_t in_g, int_t in_b, int_t in_a) noexcept
     {
#if defined(__cpp_lib_saturation_arithmetic) && __cpp_lib_saturation_arithmetic >= 202311L
          r = std::saturate_cast<std::uint8_t>(in_r);
          g = std::saturate_cast<std::uint8_t>(in_g);
          b = std::saturate_cast<std::uint8_t>(in_b);
          a = std::saturate_cast<std::uint8_t>(in_a);
#else
          r = static_cast<std::uint8_t>(in_r);
          g = static_cast<std::uint8_t>(in_g);
          b = static_cast<std::uint8_t>(in_b);
          a = static_cast<std::uint8_t>(in_a);
#endif
     }

     /**
      * @brief Constructs a color from integral RGB components with default alpha.
      *
      * @tparam int_t An integral type.
      * @param in_r Red component.
      * @param in_g Green component.
      * @param in_b Blue component.
      */
     template<std::integral int_t>
     constexpr color(int_t in_r, int_t in_g, int_t in_b) noexcept
       : color(in_r, in_g, in_b, max_i<int_t>)
     {
     }

     /**
      * @brief Constructs a color from floating-point RGBA components.
      *
      * @tparam float_t A floating-point type.
      * @param in_r Red component (0.0-1.0).
      * @param in_g Green component (0.0-1.0).
      * @param in_b Blue component (0.0-1.0).
      * @param in_a Alpha (opacity) component (0.0-1.0).
      */
     template<std::floating_point float_t>
     constexpr color(float_t in_r, float_t in_g, float_t in_b, float_t in_a) noexcept
     {
#if defined(__cpp_lib_saturation_arithmetic) && __cpp_lib_saturation_arithmetic >= 202311L
          r = std::saturate_cast<std::uint8_t>(in_r * max_f<float_t>);
          g = std::saturate_cast<std::uint8_t>(in_g * max_f<float_t>);
          b = std::saturate_cast<std::uint8_t>(in_b * max_f<float_t>);
          a = std::saturate_cast<std::uint8_t>(in_a * max_f<float_t>);
#else
          r = static_cast<std::uint8_t>(in_r * max_f<float_t>);
          g = static_cast<std::uint8_t>(in_g * max_f<float_t>);
          b = static_cast<std::uint8_t>(in_b * max_f<float_t>);
          a = static_cast<std::uint8_t>(in_a * max_f<float_t>);
#endif
     }

     /**
      * @brief Constructs a color from floating-point RGB components with default alpha.
      *
      * @tparam float_t A floating-point type.
      * @param in_r Red component (0.0-1.0).
      * @param in_g Green component (0.0-1.0).
      * @param in_b Blue component (0.0-1.0).
      */
     template<std::floating_point float_t>
     constexpr color(float_t in_r, float_t in_g, float_t in_b) noexcept
       : color(in_r, in_g, in_b, float_t{ 1 })
     {
     }

     /**
      * @brief Converts the color to an SFML color.
      *
      * @return An sf::Color object representing the color.
      */
     [[nodiscard]] operator sf::Color() const noexcept
     {
          return std::bit_cast<sf::Color>(*this);
     }

     /**
      * @brief Converts the color to a std::uint32_t.
      *
      * @return An std::uint32_t object representing the color.
      */
     [[nodiscard]] explicit constexpr operator std::uint32_t() const noexcept
     {
          return IM_COL32(r, g, b, a);// std::bit_cast<std::uint32_t>(*this);
     }

     /**
      * @brief Converts the color to an ImColor object.
      *
      * @return An ImColor object representing the color.
      */
     [[nodiscard]] constexpr operator ImColor() const noexcept
     {
          return ImColor(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b), static_cast<int>(a));
     }

     /**
      * @brief Converts the color to an ImVec4 object.
      *
      * @return An ImVec4 object representing the color.
      */
     [[nodiscard]] constexpr operator ImVec4() const noexcept
     {
          return { static_cast<float>(r) / max_f<float>,
                   static_cast<float>(g) / max_f<float>,
                   static_cast<float>(b) / max_f<float>,
                   static_cast<float>(a) / max_f<float> };
     }

     /**
      * @brief Compares two colors for equality or ordering.
      *
      * @param other Another color to compare with.
      * @return A strong ordering result.
      */
     constexpr auto                operator<=>(const color &) const = default;

     /**
      * @brief Fades the color's RGB by a given value.
      *
      * If the value is positive, the color will become brighter. If negative, the color will become darker.
      *
      * @param fadeValue A float that determines the amount of fade. Positive values brighten the color, negative values darken it.
      */
     [[nodiscard]] constexpr color fade(float fadeValue) const noexcept
     {
          // Clamp the fadeValue to avoid overflowing the color channels
          const float factor = 1.0f + fadeValue;// The factor by which the color channels will be adjusted
          return { static_cast<std::uint8_t>(std::clamp(r * factor, min_f<float>, max_f<float>)),
                   static_cast<std::uint8_t>(std::clamp(g * factor, min_f<float>, max_f<float>)),
                   static_cast<std::uint8_t>(std::clamp(b * factor, min_f<float>, max_f<float>)),
                   a };
     }

     /**
      * @brief Fades the color's alpha by a given value.
      *
      * If the value is positive, the color will become less transparent. If negative, the color will become more transparent.
      *
      * @param fadeValue A float that determines the amount of fade. Positive values brighten the color, negative values darken it.
      */
     [[nodiscard]] constexpr color fade_alpha(float fadeValue) const noexcept
     {
          // Clamp the fadeValue to avoid overflowing the color channels
          const float factor = 1.0f + fadeValue;// The factor by which the color channels will be adjusted
          return { r, g, b, static_cast<std::uint8_t>(std::clamp(a * factor, min_f<float>, max_f<float>)) };
     }

     [[nodiscard]] constexpr color opaque() const noexcept
     {
          return { r, g, b };
     }
};

struct colors
{
     // Static asserts to verify invariants and assumptions
     static_assert(std::is_trivially_copyable_v<color>, "color must be trivially copyable.");
     static_assert(std::is_standard_layout_v<color>, "color must have a standard layout.");
     static_assert(sizeof(color) == 4, "color must occupy exactly 4 bytes.");
     static_assert(std::is_nothrow_default_constructible_v<color>, "color must be nothrow default constructible.");
     static_assert(std::is_nothrow_copy_constructible_v<color>, "color must be nothrow copy constructible.");
     static_assert(std::is_nothrow_move_constructible_v<color>, "color must be nothrow move constructible.");
     static_assert(std::is_nothrow_copy_assignable_v<color>, "color must be nothrow copy assignable.");
     static_assert(std::is_nothrow_move_assignable_v<color>, "color must be nothrow move assignable.");

     // Static checks for default RGBA values
     static_assert(color{}.r == 0, "Default red component of color must be 0.");
     static_assert(color{}.g == 0, "Default green component of color must be 0.");
     static_assert(color{}.b == 0, "Default blue component of color must be 0.");
     static_assert(color{}.a == std::numeric_limits<std::uint8_t>::max(), "Default alpha component of color must be fully opaque.");

     static_assert(color{}.r == 0 && color{}.g == 0 && color{}.b == 0 && color{}.a == 255, "Default constructor failed!");

     static_assert(
       color(255, 128, 64, 32).r == 255 && color(255, 128, 64, 32).g == 128 && color(255, 128, 64, 32).b == 64
         && color(255, 128, 64, 32).a == 32,
       "Integral constructor failed!");

     static_assert(
       color(255, 128, 64).r == 255 && color(255, 128, 64).g == 128 && color(255, 128, 64).b == 64 && color(255, 128, 64).a == 255,
       "Integral constructor with default alpha failed!");

     static_assert(
       color(1.0f, 0.5f, 0.25f, 0.125f).r == 255 && color(1.0f, 0.5f, 0.25f, 0.125f).g == 127 && color(1.0f, 0.5f, 0.25f, 0.125f).b == 63
         && color(1.0f, 0.5f, 0.25f, 0.125f).a == 31,
       "Floating-point constructor failed!");

     static_assert(
       color(1.0f, 0.5f, 0.25f).r == 255 && color(1.0f, 0.5f, 0.25f).g == 127 && color(1.0f, 0.5f, 0.25f).b == 63
         && color(1.0f, 0.5f, 0.25f).a == 255,
       "Floating-point constructor with default alpha failed!");

     static inline const ImGuiStyle DarkStyle = []() {
          auto s = ImGuiStyle{};
          ImGui::StyleColorsDark(&s);
          return s;
     }();

     static constexpr color    Red                   = { 255, 0, 0 };//!< Fully red.
     static constexpr color    Green                 = { 0, 255, 0 };//!< Fully green.
     static constexpr color    Blue                  = { 0, 0, 255 };//!< Fully blue.
     static constexpr color    White                 = { 255, 255, 255 };//!< Fully white.
     static constexpr color    Black                 = { 0, 0, 0 };//!< Fully black.
     static constexpr color    Transparent           = { 0, 0, 0, 0 };//!< Fully transparent.
     static constexpr color    Yellow                = { 255, 255, 0 };//!< Fully yellow.
     static constexpr color    Cyan                  = { 0, 255, 255 };//!< Fully cyan.
     static constexpr color    Magenta               = { 255, 0, 255 };//!< Fully magenta.
     static constexpr color    Gray                  = { 128, 128, 128 };//!< Neutral gray.
     static constexpr color    LightGray             = { 192, 192, 192 };//!< Light gray.
     static constexpr color    DarkGray              = { 64, 64, 64 };//!< Dark gray.
     static constexpr color    Orange                = { 255, 165, 0 };//!< Orange color.
     static constexpr color    Purple                = { 128, 0, 128 };//!< Purple color.
     static constexpr color    Pink                  = { 255, 192, 203 };//!< Pink color.
     static constexpr color    Brown                 = { 165, 42, 42 };//!< Brown color.

     // darkstyle imgui color blue tint
     static inline const color Button                = color{ DarkStyle.Colors[ImGuiCol_Button] };
     static inline const color ButtonHovered         = color{ DarkStyle.Colors[ImGuiCol_ButtonHovered] };
     static inline const color ButtonActive          = color{ DarkStyle.Colors[ImGuiCol_ButtonActive] };

     // Green tint
     static constexpr color    ButtonGreen           = color{ ImVec4(0.26f, 0.98f, 0.26f, 0.40f) };
     static constexpr color    ButtonGreenHovered    = color{ ImVec4(0.26f, 0.98f, 0.26f, 1.00f) };
     static constexpr color    ButtonGreenActive     = color{ ImVec4(0.06f, 0.53f, 0.06f, 1.00f) };

     // Red tint
     static constexpr color    ButtonRed             = color{ ImVec4(0.98f, 0.26f, 0.26f, 0.40f) };
     static constexpr color    ButtonRedHovered      = color{ ImVec4(0.98f, 0.26f, 0.26f, 1.00f) };
     static constexpr color    ButtonRedActive       = color{ ImVec4(0.53f, 0.06f, 0.06f, 1.00f) };

     // Dark Blue tint
     static constexpr color    ButtonDarkBlue        = color{ ImVec4(0.26f, 0.26f, 0.59f, 0.40f) };
     static constexpr color    ButtonDarkBlueHovered = color{ ImVec4(0.26f, 0.26f, 0.59f, 1.00f) };
     static constexpr color    ButtonDarkBlueActive  = color{ ImVec4(0.06f, 0.06f, 0.53f, 1.00f) };

     // Yellow tint
     static constexpr color    ButtonYellow          = color{ ImVec4(0.98f, 0.98f, 0.26f, 0.40f) };
     static constexpr color    ButtonYellowHovered   = color{ ImVec4(0.98f, 0.98f, 0.26f, 1.00f) };
     static constexpr color    ButtonYellowActive    = color{ ImVec4(0.53f, 0.53f, 0.06f, 1.00f) };

     // Orange tint
     static constexpr color    ButtonOrange          = color{ ImVec4(0.98f, 0.59f, 0.26f, 0.40f) };
     static constexpr color    ButtonOrangeHovered   = color{ ImVec4(0.98f, 0.59f, 0.26f, 1.00f) };
     static constexpr color    ButtonOrangeActive    = color{ ImVec4(0.53f, 0.26f, 0.06f, 1.00f) };

     // Pink tint
     static constexpr color    ButtonPink            = color{ ImVec4(0.98f, 0.26f, 0.59f, 0.40f) };
     static constexpr color    ButtonPinkHovered     = color{ ImVec4(0.98f, 0.26f, 0.59f, 1.00f) };
     static constexpr color    ButtonPinkActive      = color{ ImVec4(0.53f, 0.06f, 0.29f, 1.00f) };

     // Tan tint
     static constexpr color    ButtonTan             = color{ ImVec4(0.98f, 0.84f, 0.57f, 0.40f) };
     static constexpr color    ButtonTanHovered      = color{ ImVec4(0.98f, 0.84f, 0.57f, 1.00f) };
     static constexpr color    ButtonTanActive       = color{ ImVec4(0.53f, 0.48f, 0.29f, 1.00f) };

     // tableColors
     static constexpr color    TableDarkRed          = color{ 120, 40, 40 };
     static constexpr color    TableLightDarkRed     = color{ 160, 60, 60 };
     static constexpr color    TableHoverDarkRed     = TableLightDarkRed.fade(0.4F);
     static constexpr color    TableActiveDarkRed    = color{ 100, 20, 20 };


     static constexpr color    TableDarkGray         = color{ 80, 80, 80 };
     static constexpr color    TableLightDarkGray    = color{ 100, 100, 100 };
     static constexpr color    TableHoverDarkGray    = TableLightDarkGray.fade(0.4F);
     static constexpr color    TableActiveDarkGray   = color{ 50, 50, 50 };
};
}// namespace fme

#endif /* E9FF3A6F_3CCA_4C0B_86CF_665262267C20 */
