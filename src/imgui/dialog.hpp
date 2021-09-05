//
// Created by pcvii on 9/5/2021.
//

#ifndef MYPROJECT_DIALOG_HPP
#define MYPROJECT_DIALOG_HPP
#include <imgui.h>
template<typename T>
struct dialog
{
private:
  const char      *m_title;
  ImVec2           m_pos{};
  mutable T        m_f{};
  ImGuiWindowFlags m_flags{};

public:
  dialog(const char *const title,
    ImVec2                 pos,
    T                    &&f,
    ImGuiWindowFlags       flags = static_cast<ImGuiWindowFlags>(
      static_cast<unsigned int>(ImGuiWindowFlags_AlwaysAutoResize)
      | static_cast<unsigned int>(ImGuiWindowFlags_NoCollapse)))
    : m_title(title), m_pos(pos), m_f(std::forward<T>(f)), m_flags(flags)
  {}
  void draw(bool first = false) const
  {
    ImGui::Begin(m_title, nullptr, m_flags);
    // const auto current_size = ImGui::GetWindowSize();
    // auto size = ImVec2{ std::max(current_size.x, m_size.x),
    // std::max(std::round((current_size.y / current_size.x) * m_size.y),
    // m_size.y) }; ImGui::SetWindowSize(ImVec2{ m_size.x * scale, m_size.y *
    // scale });
    if (first) {
      ImGui::SetWindowPos(m_pos);
    }
    m_f();
    ImGui::End();
  }
};
#endif// MYPROJECT_DIALOG_HPP
