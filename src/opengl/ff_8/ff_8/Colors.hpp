#ifndef B8D7F206_870F_4BEB_9A0E_17FA763286F1
#define B8D7F206_870F_4BEB_9A0E_17FA763286F1
#include <algorithm>// std::clamp
#include <array>
#include <bit>
#include <cmath>
#include <compare>
#include <concepts>
#include <cstdint>
#include <limits>
#include <numeric>
#include <string_view>
#include <version>

namespace ff_8
{
// ----------------------------------------------------------------------
//  Color – the only type that lives in ff_8
// ----------------------------------------------------------------------
struct [[nodiscard]] Color
{
     std::uint8_t r             = {};
     std::uint8_t g             = {};
     std::uint8_t b             = {};
     std::uint8_t a             = (std::numeric_limits<std::uint8_t>::max)();

     // ----- ctors / assignment -----------------------------------------
     constexpr Color() noexcept = default;
     constexpr Color(const Color &) noexcept            = default;
     constexpr Color(Color &&) noexcept                 = default;
     constexpr Color &operator=(const Color &) noexcept = default;
     constexpr Color &operator=(Color &&) noexcept      = default;

     // ----- max / min helpers -----------------------------------------
     template<std::floating_point F>
     static constexpr auto max_f
       = static_cast<F>((std::numeric_limits<std::uint8_t>::max)());
     template<std::floating_point F>
     static constexpr auto min_f
       = static_cast<F>((std::numeric_limits<std::uint8_t>::min)());
     template<std::integral I>
     static constexpr auto max_i
       = static_cast<I>((std::numeric_limits<std::uint8_t>::max)());

     // ----- integral constructors --------------------------------------
     template<std::integral I>
     constexpr Color(
       I in_r,
       I in_g,
       I in_b,
       I in_a) noexcept
     {
#if defined(__cpp_lib_saturation_arithmetic) \
  && __cpp_lib_saturation_arithmetic >= 202311L
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

     template<std::integral I>
     constexpr Color(
       I in_r,
       I in_g,
       I in_b) noexcept
       : Color(
           in_r,
           in_g,
           in_b,
           max_i<I>)
     {
     }

     // ----- floating-point constructors --------------------------------
     template<std::floating_point F>
     constexpr Color(
       F in_r,
       F in_g,
       F in_b,
       F in_a) noexcept
     {
#if defined(__cpp_lib_saturation_arithmetic) \
  && __cpp_lib_saturation_arithmetic >= 202311L
          r = std::saturate_cast<std::uint8_t>(in_r * max_f<F>);
          g = std::saturate_cast<std::uint8_t>(in_g * max_f<F>);
          b = std::saturate_cast<std::uint8_t>(in_b * max_f<F>);
          a = std::saturate_cast<std::uint8_t>(in_a * max_f<F>);
#else
          r = static_cast<std::uint8_t>(in_r * max_f<F>);
          g = static_cast<std::uint8_t>(in_g * max_f<F>);
          b = static_cast<std::uint8_t>(in_b * max_f<F>);
          a = static_cast<std::uint8_t>(in_a * max_f<F>);
#endif
     }

     template<std::floating_point F>
     constexpr Color(
       F in_r,
       F in_g,
       F in_b) noexcept
       : Color(
           in_r,
           in_g,
           in_b,
           F{ 1 })
     {
     }


     // ----- generic constructor ---------------------------------------

     template<typename T>
          requires(
            requires(T v) {
                 v.x;
                 v.y;
                 v.z;
            } ||// has x,y,z
            requires(T v) {
                 v.r;
                 v.g;
                 v.b;
            }// or has r,g,b
            )
     explicit constexpr Color(const T &v) noexcept
     {
          if constexpr (requires {
                             v.x;
                             v.y;
                             v.z;
                        })
          {
               // Try to detect float vs integer range
               using value_type = std::remove_cvref_t<decltype(v.x)>;
               if constexpr (std::floating_point<value_type>)
               {
#if defined(__cpp_lib_saturation_arithmetic) \
  && __cpp_lib_saturation_arithmetic >= 202311L
                    r = std::saturate_cast<std::uint8_t>(
                      v.x * max_f<value_type>);
                    g = std::saturate_cast<std::uint8_t>(
                      v.y * max_f<value_type>);
                    b = std::saturate_cast<std::uint8_t>(
                      v.z * max_f<value_type>);

                    if constexpr (requires { v.w; })
                    {
                         a = std::saturate_cast<std::uint8_t>(
                           v.w * max_f<value_type>);
                    }
                    else
                    {
                         a = 255;
                    }
#else
                    r = static_cast<std::uint8_t>(v.x * max_f<value_type>);
                    g = static_cast<std::uint8_t>(v.y * max_f<value_type>);
                    b = static_cast<std::uint8_t>(v.z * max_f<value_type>);

                    if constexpr (requires { v.w; })
                    {
                         a = static_cast<std::uint8_t>(v.w * max_f<value_type>);
                    }
                    else
                    {
                         a = 255;
                    }
#endif
               }
               else
               {
#if defined(__cpp_lib_saturation_arithmetic) \
  && __cpp_lib_saturation_arithmetic >= 202311L
                    r = std::saturate_cast<std::uint8_t>(v.x);
                    g = std::saturate_cast<std::uint8_t>(v.y);
                    b = std::saturate_cast<std::uint8_t>(v.z);
                    if constexpr (requires { v.w; })
                    {
                         a = std::saturate_cast<std::uint8_t>(v.w);
                    }
                    else
                    {
                         a = 255;
                    }
#else
                    r = static_cast<std::uint8_t>(v.x);
                    g = static_cast<std::uint8_t>(v.y);
                    b = static_cast<std::uint8_t>(v.z);

                    if constexpr (requires { v.w; })
                    {
                         a = static_cast<std::uint8_t>(v.w);
                    }

                    else
                    {
                         a = 255;
                    }

#endif
               }
          }
          else if constexpr (requires {
                                  v.r;
                                  v.g;
                                  v.b;
                             })
          {
               // Try to detect float vs integer range
               using value_type = std::remove_cvref_t<decltype(v.r)>;
               if constexpr (std::floating_point<value_type>)

#if defined(__cpp_lib_saturation_arithmetic) \
  && __cpp_lib_saturation_arithmetic >= 202311L
                    r = std::saturate_cast<std::uint8_t>(
                      v.r * max_f<value_type>);
               g = std::saturate_cast<std::uint8_t>(v.g * max_f<value_type>);
               b = std::saturate_cast<std::uint8_t>(v.b * max_f<value_type>);

               if constexpr (requires { v.a; })
               {
                    a = std::saturate_cast<std::uint8_t>(
                      v.a * max_f<value_type>);
               }
               else
               {
                    a = 255;
               }
#else
                    r = static_cast<std::uint8_t>(v.r * max_f<value_type>);
               g = static_cast<std::uint8_t>(v.g * max_f<value_type>);
               b = static_cast<std::uint8_t>(v.b * max_f<value_type>);

               if constexpr (requires { v.a; })
               {
                    a = static_cast<std::uint8_t>(v.a * max_f<value_type>);
               }
               else
               {
                    a = 255;
               }
#endif
          }
          else
          {
#if defined(__cpp_lib_saturation_arithmetic) \
  && __cpp_lib_saturation_arithmetic >= 202311L
               r = std::saturate_cast<std::uint8_t>(v.r);
               g = std::saturate_cast<std::uint8_t>(v.g);
               b = std::saturate_cast<std::uint8_t>(v.b);
               if constexpr (requires { v.a; })
               {
                    a = std::saturate_cast<std::uint8_t>(v.a);
               }
               else
               {
                    a = 255;
               }
#else
               r = static_cast<std::uint8_t>(v.r);
               g = static_cast<std::uint8_t>(v.g);
               b = static_cast<std::uint8_t>(v.b);

               if constexpr (requires { v.a; })
               {
                    a = static_cast<std::uint8_t>(v.a);
               }

               else
               {
                    a = 255;
               }

#endif
          }
     }


     // ----- utility ----------------------------------------------------
     [[nodiscard]] double difference(const Color &o) const
     {
          int dr = static_cast<int>(r) - static_cast<int>(o.r);
          int dg = static_cast<int>(g) - static_cast<int>(o.g);
          int db = static_cast<int>(b) - static_cast<int>(o.b);
          int da = static_cast<int>(a) - static_cast<int>(o.a);
          return std::sqrt(dr * dr + dg * dg + db * db + da * da);
     }

     constexpr auto                operator<=>(const Color &) const = default;

     [[nodiscard]] constexpr Color fade(float f) const noexcept
     {
          const float factor = 1.0f + f;
          return { static_cast<std::uint8_t>(
                     std::clamp(static_cast<float>(r) * factor, 0.f, 255.f)),
                   static_cast<std::uint8_t>(
                     std::clamp(static_cast<float>(g) * factor, 0.f, 255.f)),
                   static_cast<std::uint8_t>(
                     std::clamp(static_cast<float>(b) * factor, 0.f, 255.f)),
                   a };
     }

     [[nodiscard]] constexpr Color fade_alpha(float f) const noexcept
     {
          const float factor = 1.0f + f;
          return { r, g, b,
                   static_cast<std::uint8_t>(
                     std::clamp(static_cast<float>(a) * factor, 0.f, 255.f)) };
     }

     [[nodiscard]] constexpr Color opaque() const noexcept
     {
          return { r, g, b };
     }

     // ----- conversion to uint32_t (IM_COL32) – always available ----------
     [[nodiscard]] explicit constexpr operator std::uint32_t() const noexcept
     {
          return (static_cast<std::uint32_t>(a) << 24)
                 | (static_cast<std::uint32_t>(b) << 16)
                 | (static_cast<std::uint32_t>(g) << 8)
                 | (static_cast<std::uint32_t>(r));
     }
};


// ----------------------------------------------------------------------
//  Color constants
// ----------------------------------------------------------------------
namespace Colors
{
     static constexpr Color Red         = { 255, 0, 0 };
     static constexpr Color Green       = { 0, 255, 0 };
     static constexpr Color Blue        = { 0, 0, 255 };
     static constexpr Color White       = { 255, 255, 255 };
     static constexpr Color Black       = { 0, 0, 0 };
     static constexpr Color Transparent = { 0, 0, 0, 0 };
     static constexpr Color Yellow      = { 255, 255, 0 };
     static constexpr Color Cyan        = { 0, 255, 255 };
     static constexpr Color Magenta     = { 255, 0, 255 };
     static constexpr Color Gray        = { 128, 128, 128 };
     static constexpr Color LightGray   = { 192, 192, 192 };
     static constexpr Color DarkGray    = { 64, 64, 64 };
     static constexpr Color Orange      = { 255, 165, 0 };
     static constexpr Color Peach       = { 255, 153, 102 };
     static constexpr Color Purple      = { 128, 0, 128 };
     static constexpr Color Pink        = { 255, 192, 203 };
     static constexpr Color Brown       = { 78, 39, 5 };

     // (All the button / table Colors from the original file – unchanged)
     // …


     // HSV → RGB conversion (GLM-free)
     [[nodiscard]] static inline std::array<
       float,
       3>
       HSV2RGB(
         const std::array<
           float,
           3> &c)
     {
          constexpr float Kx    = 1.0f;
          constexpr float Ky    = 2.0f / 3.0f;
          constexpr float Kz    = 1.0f / 3.0f;
          constexpr float Kw    = 3.0f;

          auto            fract = [](float x) { return x - std::floor(x); };
          auto            clamp = [](float v, float lo, float hi)
          { return std::max(lo, std::min(v, hi)); };

          float px    = std::fabs(fract(c[0] + Kx) * 6.0f - Kw);
          float py    = std::fabs(fract(c[0] + Ky) * 6.0f - Kw);
          float pz    = std::fabs(fract(c[0] + Kz) * 6.0f - Kw);

          float mix_x = Kx + (clamp(px - Kx, 0.0f, 1.0f) - Kx) * c[1];
          float mix_y = Kx + (clamp(py - Kx, 0.0f, 1.0f) - Kx) * c[1];
          float mix_z = Kx + (clamp(pz - Kx, 0.0f, 1.0f) - Kx) * c[1];

          return { c[2] * mix_x, c[2] * mix_y, c[2] * mix_z };
     }

     // Final function returning ff_8::Color
     [[nodiscard]] static inline ff_8::Color EncodeUintToColor(unsigned int id)
     {
          constexpr float GOLDEN_RATIO = 0.61803398875f;
          constexpr float SATURATION   = 0.8f;
          constexpr float VALUE        = 0.9f;

          float           hue          = std::fmod(id * GOLDEN_RATIO, 1.0f);
          if (hue < 0.0f)
               hue += 1.0f;

          auto temp_color = HSV2RGB({ hue, SATURATION, VALUE });
          return ff_8::Color(temp_color[0], temp_color[1], temp_color[2], 1.0f);
     }
};// namespace Colors

// ----------------------------------------------------------------------
//  BackgroundSettings – unchanged
// ----------------------------------------------------------------------
enum class BackgroundSettings : std::uint8_t
{
     Default      = 0,
     OneColor     = Default,
     TwoColors    = 1 << 0,
     Checkerboard = Default,
     Solid        = 1 << 1,
};

// (All the bitwise helpers, ValidateBackgroundSettings, HasFlag, SetFlag …)
// – copy-paste from your original file, they only need the enum.
}// namespace ff_8
#endif /* B8D7F206_870F_4BEB_9A0E_17FA763286F1 */
