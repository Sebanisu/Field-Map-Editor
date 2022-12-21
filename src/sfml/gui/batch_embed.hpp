#ifndef FIELD_MAP_EDITOR_BATCH_EMBED_HPP
#define FIELD_MAP_EDITOR_BATCH_EMBED_HPP
#include <filesystem>
#include <chrono>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "format_imgui_text.hpp"
struct batch_embed
{
  void enable(
    std::filesystem::path                                       in_outgoing,
    std::chrono::time_point<std::chrono::high_resolution_clock> start =
      std::chrono::high_resolution_clock::now());
  void disable();
  std::chrono::time_point<std::chrono::high_resolution_clock>
       start_time() const noexcept;

  bool enabled() const noexcept;

    template<typename lambdaT, typename askT, std::ranges::range T>
    bool operator()(const T &fields, lambdaT &&lambda, askT &&ask_lambda)
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
            "{:%H:%M:%S} - Finished the batch embed operation...",
            current - m_start);
          disable();
          return m_pos > 0U;
        }
        if (!m_asked)
        {
          format_imgui_text("Displaying Ask?");
          m_asked = ask_lambda();
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
          lambda(tmp, m_outgoing);
          ++tmp;
          m_pos = static_cast<std::size_t>(tmp);
        }
      }
      return false;
    }
private:
  bool                  m_enabled  = { false };
  std::size_t           m_pos      = {};
  std::filesystem::path m_outgoing = {};
  bool                  m_asked    = { false };
  std::chrono::time_point<std::chrono::high_resolution_clock> m_start = {};
};
#endif// FIELD_MAP_EDITOR_BATCH_EMBED_HPP
