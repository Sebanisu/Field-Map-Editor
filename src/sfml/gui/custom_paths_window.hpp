#ifndef A68AC487_FAA2_4B0F_9C15_BB16CD240B56
#define A68AC487_FAA2_4B0F_9C15_BB16CD240B56

#include "format_imgui_text.hpp"
#include "gui_labels.hpp"
#include "push_pop_id.hpp"
#include "scope_guard.hpp"
#include "Selections.hpp"
#include "tool_tip.hpp"
#include <array>
#include <cstdint>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <imgui.h>
#include <open_viii/strings/LangCommon.hpp>
#include <optional>
#include <ranges>
#include <string_view>
#include <tl/string.hpp>
namespace fme
{
struct custom_paths_map
{
     std::string_view                field_name    = {};
     std::string_view                ext           = { ".png" };
     std::optional<open_viii::LangT> language_code = {};
     std::optional<std::uint8_t>     palette       = {};
     std::optional<std::uint8_t>     texture_page  = {};
     std::optional<std::uint32_t>    pupu_id       = {};

     struct keys
     {
          static constexpr std::string_view ff8_path              = { "ff8_path" };
          static constexpr std::string_view ffnx_mod_path         = { "ffnx_mod_path" };
          static constexpr std::string_view ffnx_direct_mode_path = { "ffnx_direct_mode_path" };
          static constexpr std::string_view ffnx_override_path    = { "ffnx_override_path" };
          static constexpr std::string_view input_path            = { "input_path" };
          static constexpr std::string_view batch_input_path      = { "batch_input_path" };
          static constexpr std::string_view batch_output_path     = { "batch_output_path" };
          static constexpr std::string_view field_name            = { "field_name" };
          static constexpr std::string_view ext                   = { "ext" };
          static constexpr std::string_view field_prefix          = { "field_prefix" };
          static constexpr std::string_view letter_2_lang         = { "2_letter_lang" };
          static constexpr std::string_view letter_3_lang         = { "3_letter_lang" };
          static constexpr std::string_view palette               = { "palette" };
          static constexpr std::string_view texture_page          = { "texture_page" };
          static constexpr std::string_view pupu_id               = { "pupu_id" };
          static constexpr std::string_view demaster_mod_path     = { "demaster_mod_path" };
          static constexpr std::string_view ffnx_multi_texture    = { "ffnx_multi_texture" };
          static constexpr std::string_view ffnx_single_texture   = { "ffnx_single_texture" };
          static constexpr std::string_view ffnx_map              = { "ffnx_map" };
          static constexpr std::string_view demaster              = { "demaster" };

          static constexpr std::string_view field_main            = { "field_main" };
          static constexpr std::string_view field_lang            = { "field_lang" };
          static constexpr std::string_view chara_main            = { "chara_main" };
          static constexpr std::string_view chara_lang            = { "chara_lang" };
          static constexpr std::string_view field_3lang_main      = { "field_3lang_main" };
          static constexpr std::string_view chara_3lang_main      = { "chara_3lang_main" };
          static constexpr std::string_view chara_3lang_lang      = { "chara_3lang_lang" };


          static constexpr auto             all_keys              = std::to_array<std::string_view>({ ff8_path,
                                                                                                      ffnx_mod_path,
                                                                                                      ffnx_direct_mode_path,
                                                                                                      ffnx_override_path,
                                                                                                      input_path,
                                                                                                      batch_input_path,
                                                                                                      batch_output_path,
                                                                                                      field_name,
                                                                                                      ext,
                                                                                                      field_prefix,
                                                                                                      letter_2_lang,
                                                                                                      letter_3_lang,
                                                                                                      palette,
                                                                                                      texture_page,
                                                                                                      pupu_id,
                                                                                                      demaster_mod_path,
                                                                                                      ffnx_multi_texture,
                                                                                                      ffnx_single_texture,
                                                                                                      ffnx_map,
                                                                                                      demaster,
                                                                                                      field_main,
                                                                                                      field_lang,
                                                                                                      chara_main,
                                                                                                      chara_lang,
                                                                                                      field_3lang_main,
                                                                                                      chara_3lang_main,
                                                                                                      chara_3lang_lang });
     };

     std::string operator()(std::string_view key, const std::shared_ptr<Selections> &selections) const
     {
          if (selections)
          {
               if (keys::ff8_path == key)
               {
                    return selections->path;
               }
               else if (keys::ffnx_mod_path == key)
               {
                    return selections->ffnx_mod_path;
               }
               else if (keys::ffnx_direct_mode_path == key)
               {
                    return selections->ffnx_direct_mode_path;
               }
               else if (keys::ffnx_override_path == key)
               {
                    return selections->ffnx_override_path;
               }
               else if (keys::input_path == key)
               {
                    return selections->path;
               }
          }
          return operator()(key);
     }

     std::string operator()(std::string_view key) const
     {
          if (keys::field_name == key)
          {
               assert(!field_name.empty() && field_name.length() >= 3);
               return fmt::format("{}", field_name);
          }
          else if (keys::ext == key)
          {
               assert(ext.size() >= 2 && ext[0] == '.');
               return fmt::format("{}", ext);
          }
          else if (keys::field_prefix == key)
          {
               return fmt::format("{}", field_prefix());
          }
          else if (keys::letter_2_lang == key)
          {
               return fmt::format("{}", letter_2_lang());
          }
          else if (keys::letter_3_lang == key)
          {
               return fmt::format("{}", letter_3_lang());
          }
          else if (keys::palette == key)
          {
               if (palette.has_value())
               {
                    return fmt::format("{:02}", palette.value());
               }
          }
          else if (keys::texture_page == key)
          {
               if (texture_page.has_value())
               {
                    return fmt::format("{:02}", texture_page.value());
               }
          }
          else if (keys::pupu_id == key)
          {
               if (pupu_id.has_value())
               {
                    return fmt::format("{:08x}", pupu_id.value());
               }
          }
          else if (keys::demaster_mod_path == key)
          {
               const static auto path = std::string("DEMASTER_EXP");
               return path;
          }
          else if (keys::batch_input_path == key)
          {
               const Configuration config{};
               return config["batch_input_path"].value_or(std::string{});
          }
          else if (keys::batch_output_path == key)
          {
               const Configuration config{};
               return config["batch_output_path"].value_or(std::string{});
          }
          else if (keys::ffnx_multi_texture == key)
          {
               const static auto path = std::string(
                 "{ffnx_mod_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}");
               return path;
          }
          else if (keys::ffnx_single_texture == key)
          {
               const static auto path = std::string("{ffnx_mod_path}/field/mapdata/{field_name}/{field_name}{_{2_letter_lang}}{ext}");
               return path;
          }
          else if (keys::ffnx_map == key)
          {
               const static auto path =
                 std::string("{ffnx_direct_mode_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}");
               return path;
          }
          else if (keys::demaster == key)
          {
               const static auto path = std::string(
                 "{demaster_mod_path}/textures/field_bg/{field_prefix}/{field_name}/"
                 "{field_name}{_{2_letter_lang}}{_{texture_page}}{_{palette}}{ext}");
               return path;
          }
          else if (keys::field_main == key)
          {
               const static auto path = std::string("field/mapdata/{field_prefix}/{field_name}/{field_name}{ext}");
               return path;
          }
          else if (keys::field_lang == key)
          {
               const static auto path = std::string("field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}");
               return path;
          }
          else if (keys::chara_main == key)
          {
               const static auto path = std::string("field/mapdata/{field_prefix}/{field_name}/chara.one");
               return path;
          }
          else if (keys::chara_lang == key)
          {
               const static auto path = std::string("field/mapdata/{field_prefix}/{field_name}/chara{_{2_letter_lang}}.one");
               return path;
          }
          else if (keys::field_3lang_main == key)
          {
               const static auto path = std::string("{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}/{field_name}{ext}");
               return path;
          }
          else if (keys::chara_3lang_main == key)
          {
               const static auto path = std::string("{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}/chara.one");
               return path;
          }
          else if (keys::chara_3lang_lang == key)
          {
               const static auto path =
                 std::string("{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}/chara{_{2_letter_lang}}.one");
               return path;
          }
          return {};
     }

   private:
     constexpr std::string_view field_prefix() const noexcept
     {
          return field_name.substr(0, 2);
     }
     constexpr std::string_view letter_2_lang() const noexcept
     {
          if (language_code.has_value())
               return open_viii::LangCommon::to_string(language_code.value());
          else
               return {};
     }
     constexpr std::string_view letter_3_lang() const noexcept
     {
          if (language_code.has_value())
               return open_viii::LangCommon::to_string_3_char(language_code.value());
          else
               return open_viii::LangCommon::to_string_3_char(open_viii::LangT::en);
     }
};
struct custom_paths_window
{
   private:
     std::weak_ptr<Selections> m_selections = {};
     mutable bool              m_changed    = {};

   public:
     custom_paths_window(std::weak_ptr<Selections> input_selections)
       : m_selections(input_selections)
     {
     }

     custom_paths_window &refresh(std::weak_ptr<Selections> input_selections)
     {
          m_selections = input_selections;
          return refresh();
     }

     custom_paths_window &refresh()
     {
          m_changed = true;
          return *this;
     }

     void render() const
     {
          using namespace std::string_view_literals;
          bool override_changed = { false };
          auto selections       = m_selections.lock();
          if (!selections || !selections->display_custom_paths_window)
          {
               return;
          }
          const auto pop_end = scope_guard(&ImGui::End);
          if (!ImGui::Begin(gui_labels::custom_paths_window.data()))
          {
               return;
          }
          const auto                    pop_changed    = scope_guard([this, &override_changed]() { m_changed = override_changed; });

          // Test values based on provided directories
          static const auto             tests          = std::to_array<custom_paths_map>({
            { .field_name = "ecmall1"sv, .ext = ".ca"sv },// Basic field_name + ext match
            { .field_name = "ecmall1"sv, .ext = ".jsm"sv, .language_code = open_viii::LangT::en },// Field with language suffix
            { .field_name = "ecmall1"sv, .ext = ".msd"sv, .language_code = open_viii::LangT::jp },// Another language case
            { .field_name = "ecmall1"sv, .ext = ".map"sv, .language_code = open_viii::LangT::de },// Different language, different ext
            { .field_name = "ecmall1"sv, .ext = ".mim"sv, .language_code = open_viii::LangT::fr },// Another unique case
            { .field_name = "ecmall1"sv, .ext = ".inf"sv, .language_code = open_viii::LangT::it },// Italian match
            { .field_name = "ecmall1"sv, .ext = ".sfx"sv },// No language, unique ext
            { .field_name = "ecmall1"sv, .ext = ".tdw"sv, .language_code = open_viii::LangT::es },// Spanish case
            { .field_name = "cwwood2"sv, .ext = ".one"sv },// `chara.one` match
            { .field_name = "cwwood2"sv, .ext = ".one"sv, .language_code = open_viii::LangT::jp },// `chara_{2_letter_lang}.one` match
            { .field_name = "cdfield1"sv, .ext = ".pmd"sv },// Another general field match
            { .field_name = "cdfield2"sv, .ext = ".pvp"sv, .palette = std::uint8_t{ 2 } },// Field with palette
            { .field_name    = "bgkote1a"sv,
                                   .ext           = ".tiff"sv,
                                   .language_code = open_viii::LangT::es,
                                   .texture_page  = std::uint8_t{ 5 } },// With texture_page
            { .field_name = "bggate_1"sv, .ext = ".gif"sv, .language_code = open_viii::LangT::it, .pupu_id = 78901U },// With pupu_id
            { .field_name    = "bgeat1a"sv,
                                   .ext           = ".bmp"sv,
                                   .language_code = open_viii::LangT::de,
                                   .palette       = std::uint8_t{ 4 },
                                   .texture_page  = std::uint8_t{ 3 } }// Full case
          });


          static auto                   output_tests   = std::vector<std::string>{ tests.size() };
          static std::array<char, 256U> input_path_str = {};

          if (ImGui::InputText("test input:", input_path_str.data(), input_path_str.size()) || m_changed)
          {
               output_tests.clear();
               for (const auto test_data : tests)
               {
                    auto                 &output_test = output_tests.emplace_back(input_path_str.begin(), input_path_str.end());
                    constexpr static auto pattern     = CTRE_REGEX_INPUT_TYPE{ R"(\{([^\{\}]+)\}|\{([^\{]*)\{([^\}]+)\}([^\}]*)\})" };
                    auto                  input_test  = output_test;
                    auto                  matches     = ctre::search_all<pattern>(input_test);
                    do
                    {

                         for ([[maybe_unused]] const auto &match : matches)
                         {
                              const auto replace_str = std::string_view{ match.get<0>() };
                              const auto key         = std::string_view{ match.get<1>() }.empty() ? std::string_view{ match.get<3>() }
                                                                                                  : std::string_view{ match.get<1>() };
                              const auto value       = test_data(key, selections);
                              const auto prefix      = value.empty() ? std::string_view{} : std::string_view{ match.get<2>() };
                              const auto suffix      = value.empty() ? std::string_view{} : std::string_view{ match.get<4>() };
                              output_test            = output_test | std::views::split(replace_str)
                                            | std::views::join_with(fmt::format("{}{}{}", prefix, value, suffix))
                                            | std::ranges::to<std::string>();
                         }

                         input_test = output_test;
                         matches    = ctre::search_all<pattern>(input_test);
                    } while (!std::ranges::empty(matches));
                    tl::string::replace_slashes(output_test);
               }
          }
          if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
          {
               if (ImGui::Selectable("Copy Path"))
               {
                    const auto trim = [](const std::string &str) -> std::string {
                         auto start = str.find_first_not_of(" \t\n\r\f\v");
                         if (start == std::string::npos)
                              return "";// Empty or all spaces

                         auto end = str.find_last_not_of(" \t\n\r\f\v");
                         return str.substr(start, end - start + 1);
                    };
                    const auto test_str = trim(std::string{ input_path_str.data(), input_path_str.size() });
                    ImGui::SetClipboardText(test_str.data());
               }
               if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
               ImGui::EndPopup();
          }
          if (ImGui::Button("Remove Last {Key}"))
          {
               std::string_view path_view(input_path_str.data());

               // Find the last occurrence of '{' and '}'
               size_t           last_open  = path_view.rfind('{');
               size_t           last_close = path_view.rfind('}');

               // Ensure they are a valid pair at the end
               if (
                 last_open != std::string_view::npos && last_close != std::string_view::npos && last_open < last_close
                 && last_close == path_view.size() - 1)
               {
                    // Remove the last {key_value}
                    input_path_str[last_open] = '\0';
                    override_changed          = true;
               }
          }
          // Button to append the path separator
          {
               ImGui::SameLine();
               static constexpr auto seperator = std::array<char, 2>{ std::filesystem::path::preferred_separator, '\0' };
               if (ImGui::Button("Add Separator"))
               {

                    size_t current_length = std::strlen(input_path_str.data());
                    if (current_length + 1 < input_path_str.size())
                    {
                         std::strncat(input_path_str.data(), seperator.data(), 1);
                         override_changed = true;
                    }
               }
               else
               {
                    tool_tip(std::string_view{ seperator });
               }
          }

          ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.F);
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.F, 2.F));
          ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.F, 4.F));
          const auto color = ImVec4(1.F, 1.F, 1.F, 1.F);
          ImGui::PushStyleColor(ImGuiCol_TableBorderLight, color);
          ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, color);
          ImVec2 scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);

          if (ImGui::BeginChild("scrollingKeys", scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
          {
               static constexpr int cols = 3;
               if (ImGui::BeginTable("scrollingKeys", cols, ImGuiTableFlags_BordersInnerV))
               {
                    bool bg_color = true;
                    for (const auto &[index, key] : custom_paths_map::keys::all_keys | std::ranges::views::enumerate)
                    {

                         if (index % cols == 0)
                         {
                              ImGui::TableNextRow();
                              if (bg_color)
                              {
                                   ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkRed.fade(-0.4F) });// Dark red
                              }
                              else
                              {
                                   ImGui::TableSetBgColor(
                                     ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkRed.fade(-0.4F) });// Slightly lighter dark red
                              }
                              bg_color = !bg_color;
                         }
                         ImGui::TableNextColumn();
                         const auto  pop_id       = PushPopID();
                         std::string key_brackets = fmt::format("{{{}}}", key);
                         if (ImGui::Selectable(key_brackets.data()))
                         {
                              // Ensure null-termination and safe appending
                              static constexpr size_t max_size       = input_path_str.size() - 1;// Leave space for null terminator
                              size_t                  current_length = std::strlen(input_path_str.data());
                              size_t                  append_length  = key_brackets.size();

                              if (current_length + append_length < max_size)
                              {
                                   std::strncat(input_path_str.data(), key_brackets.data(), append_length);
                              }
                              else
                              {
                                   std::strncat(input_path_str.data(), key_brackets.data(), max_size - current_length);
                              }

                              override_changed = true;
                         }
                         if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
                         {
                              if (ImGui::Selectable("Copy Key"))
                              {
                                   ImGui::SetClipboardText(key_brackets.data());
                              }
                              if (ImGui::Selectable("Copy All Keys"))
                              {
                                   using namespace std::string_view_literals;
                                   auto combined_keys = fmt::format("{{{}}}", fmt::join(custom_paths_map::keys::all_keys, "}\n{"sv));
                                   ImGui::SetClipboardText(combined_keys.data());
                              }
                              if (ImGui::Button("Close"))
                                   ImGui::CloseCurrentPopup();
                              ImGui::EndPopup();
                         }
                         const auto tooltip = fmt::format("Click to add {}\nRight-click to open popup", key_brackets);
                         ImGui::SetItemTooltip(tooltip.data());
                    }
                    ImGui::EndTable();
               }
               ImGui::EndChild();
          }
          if (ImGui::BeginChild("scrollingTest", scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
          {
               static constexpr int cols = 2;
               if (ImGui::BeginTable("scrollingKeys", cols, ImGuiTableFlags_BordersInnerV))
               {
                    bool bg_color = true;
                    for (const auto &[index, test_str] : output_tests | std::ranges::views::enumerate)
                    {
                         if (index % cols == 0)
                         {
                              ImGui::TableNextRow();
                              if (bg_color)
                              {
                                   ImGui::TableSetBgColor(
                                     ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkGreen.fade(-0.4F) });// Dark red
                              }
                              else
                              {
                                   ImGui::TableSetBgColor(
                                     ImGuiTableBgTarget_RowBg0,
                                     ImU32{ colors::TableLightDarkGreen.fade(-0.4F) });// Slightly lighter dark red
                              }
                              bg_color = !bg_color;
                         }
                         ImGui::TableNextColumn();
                         const auto pop_id = PushPopID();
                         (void)ImGui::Selectable(test_str.data());
                         if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
                         {
                              if (ImGui::Selectable("Copy Path"))
                              {
                                   ImGui::SetClipboardText(test_str.data());
                              }
                              if (ImGui::Selectable("Copy All Paths"))
                              {
                                   using namespace std::string_literals;
                                   auto combined_paths =
                                     output_tests | std::ranges::views::join_with("\n"s) | std::ranges::to<std::string>();
                                   ImGui::SetClipboardText(combined_paths.data());
                              }
                              if (ImGui::Button("Close"))
                                   ImGui::CloseCurrentPopup();
                              ImGui::EndPopup();
                         }
                         ImGui::SetItemTooltip("Right-click to open popup");
                    }
                    ImGui::EndTable();
               }
               ImGui::EndChild();
          }
          ImGui::PopStyleColor(2);
          ImGui::PopStyleVar(3);
     }
};
}// namespace fme

#endif /* A68AC487_FAA2_4B0F_9C15_BB16CD240B56 */
