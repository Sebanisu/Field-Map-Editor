//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_BATCH_RESWIZZLE_HPP
#define FIELD_MAP_EDITOR_BATCH_RESWIZZLE_HPP
#include "compact_type.hpp"
#include "filter.hpp"
#include "format_imgui_text.hpp"
#include <chrono>
#include <filesystem>
#include <fmt/chrono.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
struct batch_reswizzle
{
     void enable(std::filesystem::path in_incoming, std::filesystem::path in_outgoing);
     void disable();
     template<typename lambdaT, typename askT>
     bool operator()(const std::vector<std::string> &fields, lambdaT &&lambda, [[maybe_unused]] askT &&ask_lambda)

     {
          if (!enabled)
          {
               return false;
          }
          const char *title = "Batch saving swizzle textures...";
          ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5F, 0.5F));
          ImGui::OpenPopup(title);
          if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
          {
               const auto g = scope_guard([]() { ImGui::EndPopup(); });
               if (fields.size() <= pos)
               {
                    const auto current    = std::chrono::high_resolution_clock::now();
                    const auto difference = current - start;
                    spdlog::info("{:%H:%M:%S} - Finished the batch swizzle...", difference);
                    disable();
                    return pos > 0U;
               }
               if (!asked)
               {
                    asked = ask_lambda(compact_filter, bpp, palette);
               }
               else
               {
                    const auto current    = std::chrono::high_resolution_clock::now();
                    const auto difference = current - start;
                    format_imgui_text(
                      "{:%H:%M:%S} - {:>3.2f}% - Processing {}...",
                      difference,
                      static_cast<float>(pos) * 100.F / static_cast<float>(std::size(fields)),
                      fields[pos]);
                    ImGui::Separator();
                    lambda(static_cast<int>(pos), outgoing, filters, compact_filter, bpp, palette);
                    ++pos;
               }
          }
          return false;
     }

   private:
     bool                                                        enabled        = { false };
     bool                                                        asked          = { false };
     bool                                                        bpp            = { false };
     bool                                                        palette        = { false };
     std::size_t                                                 pos            = {};
     std::filesystem::path                                       outgoing       = {};
     ff_8::filters                                               filters        = {};
     ff_8::filter_old<compact_type>                              compact_filter = {};
     std::chrono::time_point<std::chrono::high_resolution_clock> start          = {};
};
#endif// FIELD_MAP_EDITOR_BATCH_RESWIZZLE_HPP
