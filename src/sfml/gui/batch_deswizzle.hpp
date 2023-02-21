//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_BATCH_DESWIZZLE_HPP
#define FIELD_MAP_EDITOR_BATCH_DESWIZZLE_HPP
#include "filter.hpp"
#include "format_imgui_text.hpp"
#include "gui_labels.hpp"
#include "scope_guard.hpp"
#include <chrono>
#include <filesystem>
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
struct batch_deswizzle
{
     void enable(std::filesystem::path in_outgoing);
     void disable();
     template<typename lambdaT, typename askT>
     bool operator()(const std::vector<std::string> &fields, lambdaT &&lambda, askT &&ask_lambda)
     {
          if (!enabled)
          {
               return false;
          }
          ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
          ImGui::OpenPopup(gui_labels::batch_deswizzle_title.data());
          if (ImGui::BeginPopupModal(
                gui_labels::batch_deswizzle_title.data(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
          {
               const auto g = scope_guard([]() { ImGui::EndPopup(); });
               if (fields.size() <= pos)
               {
                    const auto current    = std::chrono::high_resolution_clock::now();
                    const auto difference = current - start;
                    spdlog::info("{:%H:%M:%S} - {}", difference, gui_labels::batch_deswizzle_finish);
                    disable();
                    return pos > 0U;
               }
               if (!asked)
               {
                    asked = ask_lambda(filters.upscale);
               }
               else
               {
                    const auto current    = std::chrono::high_resolution_clock::now();
                    const auto difference = current - start;
                    format_imgui_text(
                      "{:%H:%M:%S} - {:>3.2f}% - {} {}...",
                      difference,
                      static_cast<float>(pos) * 100.F / static_cast<float>(std::size(fields)),
                      gui_labels::processing,
                      fields[pos]);
                    ImGui::Separator();
                    lambda(static_cast<int>(pos), outgoing, filters);
                    ++pos;
               }
          }
          return false;
     }

   private:
     bool                                                        enabled  = { false };
     std::size_t                                                 pos      = {};
     std::filesystem::path                                       outgoing = {};
     bool                                                        asked    = { false };
     ff_8::filters                                               filters  = {};
     std::chrono::time_point<std::chrono::high_resolution_clock> start    = {};
};
#endif// FIELD_MAP_EDITOR_BATCH_DESWIZZLE_HPP
