#ifndef FIELD_MAP_EDITOR_BATCH_EMBED_HPP
#define FIELD_MAP_EDITOR_BATCH_EMBED_HPP
#include "format_imgui_text.hpp"
#include <chrono>
#include <filesystem>
#include <fmt/chrono.h>
#include <functional>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
struct batch_embed
{
  explicit batch_embed(
    std::string                                             in_operation_name,
    std::function<void(int&, const std::filesystem::path &)> in_process_function,
    std::function<bool(void)>                               in_ask_function)
    : m_operation_name{ std::move(in_operation_name) }
    , m_ask_function{ std::move(in_ask_function) }
    , m_process_function{ std::move(in_process_function) }
  {
  }

  void enable(
    std::filesystem::path                                       in_outgoing,
    std::chrono::time_point<std::chrono::high_resolution_clock> start =
      std::chrono::high_resolution_clock::now());
  void disable();
  std::chrono::time_point<std::chrono::high_resolution_clock>
       start_time() const noexcept;

  bool enabled() const noexcept;

  template<std::ranges::range T>
  bool operator()(const T &fields)
  {
    if (!m_enabled)
    {
      return false;
    }
    const char *title = "Batch embedding .map files into archives...";
    ImGui::SetNextWindowPos(
      ImGui::GetMainViewport()->GetCenter(),
      ImGuiCond_Always,
      ImVec2(0.5F, 0.5F));
    ImGui::OpenPopup(title);
    if (ImGui::BeginPopupModal(
          title,
          nullptr,
          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
      const auto g = scope_guard([]() { ImGui::EndPopup(); });
      if (fields.size() <= m_pos)
      {
        format_imgui_text("Processing please wait...");
        auto current = std::chrono::high_resolution_clock::now();
        spdlog::info(
          "{:%H:%M:%S} - Finished the batch embed operation... {}",
          current - m_start,
          m_operation_name);
        disable();
        return m_pos > 0U;
      }
      if (!m_asked)
      {
        format_imgui_text("Displaying Ask?");
        m_asked = m_ask_function();
      }
      else
      {
        format_imgui_text("Processing please wait...");
        auto current = std::chrono::high_resolution_clock::now();
        format_imgui_text(
          "{:%H:%M:%S} - {:>3.2f}% - {} / {} - {}...",
          current - m_start,
          static_cast<float>(m_pos) * 100.F
            / static_cast<float>(std::size(fields)),
          m_pos,
          std::ranges::size(fields),
          fields[m_pos]);
        ImGui::Separator();
        int tmp = static_cast<int>(m_pos);
        if (m_process_function)
        {
          m_process_function(tmp, m_outgoing);
        }
        ++tmp;
        m_pos = static_cast<std::size_t>(tmp);
      }
    }
    return false;
  }

private:
  std::string           m_operation_name = {};
  bool                  m_enabled        = { false };
  std::size_t           m_pos            = {};
  std::filesystem::path m_outgoing       = {};
  bool                  m_asked          = { false };
  std::chrono::time_point<std::chrono::high_resolution_clock> m_start = {};
  std::function<void(int&, const std::filesystem::path &)>
                            m_process_function = {};
  std::function<bool(void)> m_ask_function     = {};
};
#endif// FIELD_MAP_EDITOR_BATCH_EMBED_HPP
