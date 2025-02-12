#ifndef A68AC487_FAA2_4B0F_9C15_BB16CD240B56
#define A68AC487_FAA2_4B0F_9C15_BB16CD240B56

#include "format_imgui_text.hpp"
#include "generic_combo.hpp"
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
          static constexpr std::string_view current_path          = { "current_path" };
          static constexpr std::string_view ffnx_mod_path         = { "ffnx_mod_path" };
          static constexpr std::string_view ffnx_direct_mode_path = { "ffnx_direct_mode_path" };
          static constexpr std::string_view ffnx_override_path    = { "ffnx_override_path" };
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


          static constexpr auto             all_keys              = std::to_array<std::string_view>({ ff8_path,
                                                                                                      current_path,
                                                                                                      ffnx_mod_path,
                                                                                                      ffnx_direct_mode_path,
                                                                                                      ffnx_override_path,
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
                                                                                                      chara_3lang_main });
     };

     std::string operator()(std::string_view key, const std::shared_ptr<Selections> &selections) const
     {
          if (selections)
          {
               if (keys::ff8_path == key)
               {
                    return selections->path;
               }
               else if (keys::current_path == key)
               {
                    std::error_code ec{};
                    auto            path = std::filesystem::current_path(ec);
                    if (ec)
                    {
                         spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, ec.value(), ec.message(), path);
                         ec.clear();
                    }

                    return path.string();
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
     std::weak_ptr<Selections>                                           m_selections              = {};
     mutable bool                                                        m_changed                 = {};

     static constexpr std::uint32_t                                      m_options_size_value      = { 4U };
     static constexpr std::size_t                                        m_input_string_size_value = { 256U };
     mutable std::array<char, m_input_string_size_value>                 m_input_pattern_string    = {};
     mutable std::array<std::string *, m_options_size_value>             m_selections_values       = get_selections_values();
     static constexpr std::array<std::string_view, m_options_size_value> m_config_strings          = { "Output swizzle pattern",
                                                                                                       "Output deswizzle pattern",
                                                                                                       "Output map pattern for swizzle",
                                                                                                       "Output map pattern for deswizzle" };
     static constexpr std::array<std::string_view, m_options_size_value> m_config_values           = {
          "selections_output_swizzle_pattern",
          "selections_output_deswizzle_pattern",
          "selections_output_map_pattern_for_swizzle",
          "selections_output_map_pattern_for_deswizzle"
     };
     static inline const auto                        m_tests        = std::to_array<custom_paths_map>({
       { .field_name = "ecmall1", .ext = ".ca" },// Basic field_name + ext match
       { .field_name = "ecmall1", .ext = ".jsm", .language_code = open_viii::LangT::en, .pupu_id = 987654U },// Field with language suffix
       { .field_name = "ecmall1", .ext = ".msd", .language_code = open_viii::LangT::jp, .pupu_id = 543210U },// Another language case
       { .field_name    = "ecmall1",
                                       .ext           = ".map",
                                       .language_code = open_viii::LangT::de,
                                       .pupu_id       = 234567U },// Different language, different ext
       { .field_name = "ecmall1", .ext = ".mim", .language_code = open_viii::LangT::fr, .pupu_id = 890123U },// Another unique case
       { .field_name = "ecmall1", .ext = ".inf", .language_code = open_viii::LangT::it, .pupu_id = 456789U },// Italian match
       { .field_name = "ecmall1", .ext = ".sfx", .pupu_id = 678901U },// No language, unique ext
       { .field_name = "ecmall1", .ext = ".tdw", .language_code = open_viii::LangT::es, .pupu_id = 321098U },// Spanish case
       { .field_name = "cwwood2", .ext = ".one" },// `chara.one` match
       { .field_name    = "cwwood2",
                                       .ext           = ".one",
                                       .language_code = open_viii::LangT::jp,
                                       .pupu_id       = 765432U },// `chara_{2_letter_lang}.one` match
       { .field_name = "cdfield1", .ext = ".pmd", .pupu_id = 210987U },// Another general field match
       { .field_name = "cdfield2", .ext = ".pvp", .palette = std::uint8_t{ 2 }, .pupu_id = 210987U },// Field with palette
       { .field_name    = "bgkote1a",
                                       .ext           = ".tiff",
                                       .language_code = open_viii::LangT::es,
                                       .texture_page  = std::uint8_t{ 5 } },// With texture_page
       { .field_name = "bggate_1", .ext = ".gif", .language_code = open_viii::LangT::it, .pupu_id = 78901U },// With pupu_id
       { .field_name    = "bgeat1a",
                                       .ext           = ".bmp",
                                       .language_code = open_viii::LangT::de,
                                       .palette       = std::uint8_t{ 4 },
                                       .texture_page  = std::uint8_t{ 3 },
                                       .pupu_id       = 123456U }// Full case
     });


     mutable std::vector<std::string>                m_output_tests = std::vector<std::string>{ m_tests.size() };

     static constexpr auto                           m_index_values = std::ranges::views::iota(std::uint32_t{}, m_options_size_value);
     mutable ImVec2                                  m_scrolling_child_size = {};

     std::array<std::string *, m_options_size_value> get_selections_values() const
     {
          auto selections = m_selections.lock();
          if (!selections)
          {
               return {};
          }
          return { &selections->output_swizzle_pattern,
                   &selections->output_desizzle_pattern,
                   &selections->output_map_pattern_for_swizzle,
                   &selections->output_map_pattern_for_deswizzle };
     }
     std::string &get_current_string_value_mutable() const
     {
          auto selections = m_selections.lock();
          if (!selections)
          {
               throw;
          }
          return *m_selections_values[selections->current_pattern];
     }
     const std::string &get_current_string_value() const
     {
          auto selections = m_selections.lock();
          if (!selections)
          {
               static std::string dummy{};
               return dummy;
          }
          return *m_selections_values[selections->current_pattern];
     }

     void populate_input_pattern() const
     {

          auto it = fmt::vformat_to_n(
            std::ranges::begin(m_input_pattern_string),
            std::ranges::size(m_input_pattern_string) - 1U,
            "{}",
            fmt::make_format_args(get_current_string_value()));
          *it.out = '\0';
     }

     void populate_test_output() const
     {
          auto selections = m_selections.lock();
          if (!selections)
          {
               return;
          }
          m_output_tests.clear();
          for (const auto test_data : m_tests)
          {
               auto &output_test                = m_output_tests.emplace_back(m_input_pattern_string.begin(), m_input_pattern_string.end());
               constexpr static auto pattern    = CTRE_REGEX_INPUT_TYPE{ R"(\{([^\{\}]+)\}|\{([^\{]*)\{([^\}]+)\}([^\}]*)\})" };
               auto                  input_test = output_test;
               auto                  matches    = ctre::search_all<pattern>(input_test);
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

     bool combo_selected_pattern() const
     {
          auto selections = m_selections.lock();
          if (!selections)
          {
               return false;
          }
          if (!ImGui::BeginTable("##test input", 2, ImGuiTableFlags_SizingStretchProp))
          {
               return false;
          }
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          format_imgui_text("{}", "Selected Pattern: ");
          ImGui::TableNextColumn();
          const auto pop_table = scope_guard{ &ImGui::EndTable };
          using namespace std::string_view_literals;
          const GenericComboClass gcc = {
               ""sv, []() { return m_index_values; }, []() { return m_config_strings; }, selections->current_pattern
          };
          if (gcc.render())
          {
               populate_input_pattern();
               save_pattern();
               return true;
          }
          return false;
     }

     void save_pattern() const
     {
          auto selections = m_selections.lock();
          if (!selections)
          {
               return;
          }
          get_current_string_value_mutable() = std::string{ m_input_pattern_string.data() };
          Configuration config{};
          config->insert_or_assign(m_config_values[selections->current_pattern], get_current_string_value());
          config->insert_or_assign("selections_current_pattern", static_cast<std::uint32_t>(selections->current_pattern));
          config.save();
     }

     bool textbox_pattern() const
     {

          if (!ImGui::BeginTable("##test input", 2, ImGuiTableFlags_SizingStretchProp))
          {
               return false;
          }
          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          format_imgui_text("{}", "Pattern: ");
          ImGui::TableNextColumn();
          const auto pop_table = scope_guard{ &ImGui::EndTable };
          if (ImGui::InputText("##test input", m_input_pattern_string.data(), m_input_pattern_string.size()))
          {
               save_pattern();
               return true;
          }
          if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
          {
               if (ImGui::Selectable("Copy Pattern"))
               {
                    const auto trim = [](const std::string &str) -> std::string {
                         auto start = str.find_first_not_of(" \t\n\r\f\v");
                         if (start == std::string::npos)
                              return "";// Empty or all spaces

                         auto end = str.find_last_not_of(" \t\n\r\f\v");
                         return str.substr(start, end - start + 1);
                    };
                    const auto test_str = trim(std::string{ m_input_pattern_string.data(), m_input_pattern_string.size() });
                    ImGui::SetClipboardText(test_str.data());
               }
               if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
               ImGui::EndPopup();
          }
          return false;
     }

     bool button_add_seperator() const
     {
          static constexpr auto seperator = std::array<char, 2>{ std::filesystem::path::preferred_separator, '\0' };
          if (ImGui::Button("Add Separator"))
          {
               size_t current_length = std::strlen(m_input_pattern_string.data());
               if (current_length + 1 < m_input_pattern_string.size())
               {
                    std::strncat(m_input_pattern_string.data(), seperator.data(), 1);
                    save_pattern();
                    return true;
               }
          }
          else
          {
               tool_tip(std::string_view{ seperator });
          }
          return false;
     }

     bool button_remove_last_key() const
     {
          if (ImGui::Button("Remove Last {Key}"))
          {
               std::string_view path_view(m_input_pattern_string.data());

               // Find the last occurrence of '{' and '}'
               size_t           last_open  = path_view.rfind('{');
               size_t           last_close = path_view.rfind('}');

               // Ensure they are a valid pair at the end
               if (
                 last_open != std::string_view::npos && last_close != std::string_view::npos && last_open < last_close
                 && last_close == path_view.size() - 1)
               {
                    // Remove the last {key_value}
                    m_input_pattern_string[last_open] = '\0';
                    save_pattern();
                    return true;
               }
          }
          return false;
     }

     bool child_keys() const
     {
          bool override_changed = false;
          if (!ImGui::CollapsingHeader("Keys", ImGuiTreeNodeFlags_DefaultOpen))
          {
               return false;
          }

          format_imgui_wrapped_text(
            "{}",
            "Click a {key} to add it to the pattern text box.\n"
            "You can use extra braces like {{key}} to create prefixes or suffixes, e.g., {prefix{key}suffix}.\n"
            "These will only appear if the {key} has a value.\n\n"
            "Right-click a {key} to access a context menu for copying key values.");

          {
               const auto pop_child = scope_guard{ &ImGui::EndChild };
               if (!ImGui::BeginChild(
                     "##scrollingKeys", m_scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
               {
                    return false;
               }
               static constexpr int cols = 3;
               if (!ImGui::BeginTable("##scrollingKeys", cols, ImGuiTableFlags_BordersInnerV))
               {
                    return false;
               }
               const auto pop_table = scope_guard{ &ImGui::EndTable };
               bool       bg_color  = true;
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
                         static constexpr size_t max_size       = m_input_string_size_value - 1;// Leave space for null terminator
                         size_t                  current_length = std::strlen(m_input_pattern_string.data());
                         size_t                  append_length  = key_brackets.size();

                         if (current_length + append_length < max_size)
                         {
                              std::strncat(m_input_pattern_string.data(), key_brackets.data(), append_length);
                         }
                         else
                         {
                              std::strncat(m_input_pattern_string.data(), key_brackets.data(), max_size - current_length);
                         }

                         save_pattern();
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
          }
          const auto same_line = []() -> bool {
               ImGui::SameLine();
               return {};
          };
          if (std::ranges::any_of(std::array{ button_remove_last_key(), same_line(), button_add_seperator() }, std::identity{}))
          {
               override_changed = true;
          }
          return override_changed;
     }

     bool child_test_output() const
     {
          bool override_changed = false;
          if (!ImGui::CollapsingHeader("Test Output", ImGuiTreeNodeFlags_DefaultOpen))
          {
               return false;
          }

          format_imgui_wrapped_text(
            "{}",
            "This second section displays what the pattern will look like with various test inputs.\n"
            "{ff8_path}/Textures could output something like c:\\ff8\\Textures.\n"
            "Right-click a key to access a context menu for copying test output.");

          const auto pop_child = scope_guard{ &ImGui::EndChild };
          if (!ImGui::BeginChild("##scrollingTest", m_scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
          {
               return false;
          }
          static constexpr int cols = 2;
          if (!ImGui::BeginTable("##scrollingTest", cols, ImGuiTableFlags_BordersInnerV))
          {
               return false;
          }
          const auto pop_table = scope_guard{ &ImGui::EndTable };
          bool       bg_color  = true;
          for (const auto &[index, test_str] : m_output_tests | std::ranges::views::enumerate)
          {
               if (index % cols == 0)
               {
                    ImGui::TableNextRow();
                    if (bg_color)
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkGreen.fade(-0.4F) });// Dark red
                    }
                    else
                    {
                         ImGui::TableSetBgColor(
                           ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkGreen.fade(-0.4F) });// Slightly lighter dark red
                    }
                    bg_color = !bg_color;
               }
               ImGui::TableNextColumn();
               const auto pop_id = PushPopID();
               (void)ImGui::Selectable(test_str.data());
               if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
               {
                    if (ImGui::Selectable("Copy Test Output"))
                    {
                         ImGui::SetClipboardText(test_str.data());
                    }
                    if (ImGui::Selectable("Copy All Test Output"))
                    {
                         using namespace std::string_literals;
                         auto combined_paths = m_output_tests | std::ranges::views::join_with("\n"s) | std::ranges::to<std::string>();
                         ImGui::SetClipboardText(combined_paths.data());
                    }
                    if (ImGui::Button("Close"))
                         ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
               }
               ImGui::SetItemTooltip("Right-click to open popup");
          }
          return override_changed;
     }

   public:
     custom_paths_window(std::weak_ptr<Selections> input_selections)
       : m_selections(input_selections)
     {
          populate_input_pattern();
          populate_test_output();
     }

     custom_paths_window &refresh(std::weak_ptr<Selections> input_selections)
     {
          m_selections        = input_selections;
          m_selections_values = get_selections_values();
          populate_input_pattern();

          return refresh();
     }

     custom_paths_window &refresh()
     {
          m_changed = true;
          return *this;
     }

     void render() const
     {
          m_scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);
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
          const auto pop_changed = scope_guard([this, &override_changed]() { m_changed = override_changed; });


          if (std::ranges::any_of(std::array{ combo_selected_pattern(), textbox_pattern() }, std::identity{}))
          {
               override_changed = true;
          }


          ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.F);
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.F, 2.F));
          ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.F, 4.F));
          const auto color = ImVec4(0.5F, 0.5F, 0.5F, 1.F);
          ImGui::PushStyleColor(ImGuiCol_TableBorderLight, color);
          ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, color);
          const auto pop_styles = scope_guard{ []() {
               ImGui::PopStyleColor(2);
               ImGui::PopStyleVar(3);
          } };
          if (child_keys())
          {
               override_changed = true;
          }
          if (m_changed)
          {
               populate_test_output();
          }
          if (child_test_output())
          {
               override_changed = true;
          }
     }
};
}// namespace fme

#endif /* A68AC487_FAA2_4B0F_9C15_BB16CD240B56 */
