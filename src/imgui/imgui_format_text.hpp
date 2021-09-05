//
// Created by pcvii on 9/5/2021.
//

#ifndef MYPROJECT_IMGUI_FORMAT_TEXT_HPP
#define MYPROJECT_IMGUI_FORMAT_TEXT_HPP
#include <fmt/core.h>
#include <imgui.h>
const auto format_imgui_text =
  []<typename... T>(fmt::format_string<T...> fmt, T &&...items)
{
  const auto data = fmt::format(
    std::forward<decltype(fmt)>(fmt), std::forward<decltype(items)>(items)...);
  ImGui::Text(
    "%s", data.c_str());// I hate this doesn't just take a std::string.
};
#endif// MYPROJECT_IMGUI_FORMAT_TEXT_HPP
