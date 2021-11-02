//
// Created by pcvii on 9/5/2021.
//

#ifndef MYPROJECT_FORMAT_IMGUI_TEXT_HPP
#define MYPROJECT_FORMAT_IMGUI_TEXT_HPP
#include <array>
#include <fmt/core.h>
#include <imgui.h>
inline const auto format_imgui_text =
  []<std::size_t sizeT = 0xFFU, typename... T>(fmt::format_string<T...> fmt,
    T &&...items)
{
  std::array<char, sizeT> buffer{};
  const auto              it = fmt::vformat_to_n(std::begin(buffer),
                 sizeT - 1U,
                 fmt::string_view(fmt),
                 fmt::make_format_args(std::forward<decltype(items)>(items)...));
  *it.out                    = '\0';
  ImGui::Text("%s", std::data(buffer));
  // it seems ImGui::Text is coping the values into it's own buffer.
};
#endif// MYPROJECT_FORMAT_IMGUI_TEXT_HPP
