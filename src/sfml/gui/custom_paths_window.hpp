#ifndef A68AC487_FAA2_4B0F_9C15_BB16CD240B56
#define A68AC487_FAA2_4B0F_9C15_BB16CD240B56

#include "format_imgui_text.hpp"
#include "gui_labels.hpp"
#include "scope_guard.hpp"
#include "Selections.hpp"
#include <array>
#include <cstdint>
#include <ctre.hpp>
#include <fmt/format.h>
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
                                                                                                      demaster });
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
                 "{ff8_path}/{ffnx_mod_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}");
               return path;
          }
          else if (keys::ffnx_single_texture == key)
          {
               const static auto path =
                 std::string("{ff8_path}/{ffnx_mod_path}/field/mapdata/{field_name}/{field_name}{_{2_letter_lang}}{ext}");
               return path;
          }
          else if (keys::ffnx_map == key)
          {
               const static auto path = std::string(
                 "{ff8_path}/{ffnx_direct_mode_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}");
               return path;
          }
          else if (keys::demaster == key)
          {
               const static auto path = std::string(
                 "{ff8_path}/{demaster_mod_path}/textures/field_bg/{field_prefix}/{field_name}/"
                 "{field_name}{_{2_letter_lang}}_{texture_page}{_{palette}}{ext}");
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
               return {};
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
          auto selections = m_selections.lock();
          if (!selections || !selections->display_custom_paths_window)
          {
               return;
          }
          const auto pop_end = scope_guard(&ImGui::End);
          if (!ImGui::Begin(gui_labels::custom_paths_window.data()))
          {
               return;
          }
          const auto        pop_changed = scope_guard([this]() { m_changed = false; });

          // Test values based on provided directories
          static const auto tests       = std::to_array<custom_paths_map>(
            { { .field_name    = "bg2f_1"sv,
                      .ext           = ".png"sv,
                      .language_code = open_viii::LangT::en,
                      .palette       = std::uint8_t{ 3 },
                      .texture_page  = std::uint8_t{ 1 },
                      .pupu_id       = 12345U },

                    { .field_name = "bgbook1a"sv, .ext = ".jpg"sv },

                    { .field_name    = "bgeat1a"sv,
                      .ext           = ".bmp"sv,
                      .language_code = open_viii::LangT::de,
                      .palette       = std::uint8_t{ 4 },
                      .texture_page  = std::uint8_t{ 3 } },

                    { .field_name = "bggate_1"sv, .ext = ".gif"sv, .language_code = open_viii::LangT::it, .pupu_id = 78901U },

                    { .field_name = "bgkote1a"sv, .ext = ".tiff"sv, .language_code = open_viii::LangT::es, .texture_page = std::uint8_t{ 5 } } });

          static std::vector<std::string> output_tests   = {};
          static std::array<char, 256U>   input_path_str = {};

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
          for (const auto &test_str : output_tests)
          {
               format_imgui_text("{}", test_str);
          }
     }
};
}// namespace fme

#endif /* A68AC487_FAA2_4B0F_9C15_BB16CD240B56 */
