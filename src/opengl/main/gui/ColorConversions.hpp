#ifndef B8F75462_5799_4CFA_80D6_4E3FE743A9D3
#define B8F75462_5799_4CFA_80D6_4E3FE743A9D3
#include <ff_8/Colors.hpp>
#include <glm/glm.hpp>
#include <imgui.h>
#include <ranges>

namespace ff_8::Colors
{
[[nodiscard]] constexpr ImVec4 to_imvec4(const Color &c) noexcept
{
     constexpr float max_f
       = static_cast<float>(std::numeric_limits<std::uint8_t>::max());
     return { static_cast<float>(c.r) / max_f, static_cast<float>(c.g) / max_f,
              static_cast<float>(c.b) / max_f,
              static_cast<float>(c.a) / max_f };
}

[[nodiscard]] constexpr ImColor to_imcolor(const Color &c) noexcept
{
     return ImColor(
       static_cast<int>(c.r),
       static_cast<int>(c.g),
       static_cast<int>(c.b),
       static_cast<int>(c.a));
}

[[nodiscard]] constexpr glm::vec4 to_vec4(const Color &c) noexcept
{
     constexpr float max_f
       = static_cast<float>(std::numeric_limits<std::uint8_t>::max());
     return { static_cast<float>(c.r) / max_f, static_cast<float>(c.g) / max_f,
              static_cast<float>(c.b) / max_f,
              static_cast<float>(c.a) / max_f };
}

//  Pipeable range adaptors
inline constexpr auto as_vec4
  = std::views::transform([](const Color &c) { return to_vec4(c); });

inline constexpr auto as_imvec4
  = std::views::transform([](const Color &c) { return to_imvec4(c); });

inline constexpr auto as_imcolor
  = std::views::transform([](const Color &c) { return to_imcolor(c); });

// Constructors from ImGui / GLM types
[[nodiscard]] constexpr Color from_imvec4(const ImVec4 &v) noexcept
{
     constexpr float max_f
       = static_cast<float>(std::numeric_limits<std::uint8_t>::max());
     return { static_cast<std::uint8_t>(std::clamp(v.x, 0.0f, 1.0f) * max_f),
              static_cast<std::uint8_t>(std::clamp(v.y, 0.0f, 1.0f) * max_f),
              static_cast<std::uint8_t>(std::clamp(v.z, 0.0f, 1.0f) * max_f),
              static_cast<std::uint8_t>(std::clamp(v.w, 0.0f, 1.0f) * max_f) };
}

[[nodiscard]] constexpr Color from_vec4(const glm::vec4 &v) noexcept
{
     constexpr float max_f
       = static_cast<float>(std::numeric_limits<std::uint8_t>::max());
     return { static_cast<std::uint8_t>(std::clamp(v.r, 0.0f, 1.0f) * max_f),
              static_cast<std::uint8_t>(std::clamp(v.g, 0.0f, 1.0f) * max_f),
              static_cast<std::uint8_t>(std::clamp(v.b, 0.0f, 1.0f) * max_f),
              static_cast<std::uint8_t>(std::clamp(v.a, 0.0f, 1.0f) * max_f) };
}

[[nodiscard]] constexpr Color from_imcolor(const ImColor &c) noexcept
{
     // ImColor stores normalized floats in c.Value
     const ImVec4 &v = c.Value;
     return from_imvec4(v);
}

}// namespace ff_8::Colors

#endif /* B8F75462_5799_4CFA_80D6_4E3FE743A9D3 */
