//
// Created by pcvii on 9/29/2021.
//

#ifndef FIELD_MAP_EDITOR_UPSCALES_HPP
#define FIELD_MAP_EDITOR_UPSCALES_HPP
#include "gui/key_value_data.hpp"
#include "gui/selections.hpp"
#include "open_viii/strings/LangCommon.hpp"
#include "open_viii/strings/LangT.hpp"
#include "safedir.hpp"
#include "tl/string.hpp"
#include <array>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <string_view>
#include <utility>
struct upscales
{
   private:
     std::optional<open_viii::LangT> m_coo{};
     std::string                     m_root       = "{current_path}";
     //  /**
     //   * for lang code remaster uses x, other versions have 3 char coo
     //   */
     std::weak_ptr<fme::Selections>  m_selections = {};

   public:
     // upscales() = default;
     upscales(std::weak_ptr<fme::Selections> selections)
       : m_selections(selections)
     {
     }
     upscales(std::string root, std::optional<open_viii::LangT> coo, std::weak_ptr<fme::Selections> selections)
       : m_coo(coo)
       , m_root(std::move(root))
       , m_selections(selections)
     {
     }

     [[nodiscard]] std::vector<std::filesystem::path>
       generate_upscale_paths(const std::filesystem::path &field_root, fme::key_value_data copy_data) const
     {

          namespace v           = std::ranges::views;
          const auto selections = m_selections.lock();
          if (!selections)
          {
               spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
               return {};
          }
          const auto operation = [&](const std::string &pattern) -> std::filesystem::path {
               return copy_data.replace_tags(pattern, selections, field_root.string());
          };
          // include if copy_data.texture_page.has_value() and          copy_data.palette.has_value();
          static const auto paths_with_palette_and_texture_page = []() {
               const auto ret = std::to_array<std::string>(
                 { "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}",
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}_0{palette}{ext}",

                   "{selected_path}/{field_name}_0{texture_page}_0{palette}{ext}",
                   "{selected_path}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}_0{palette}{ext}",

                   "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}",
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}_{palette}{ext}",

                   "{selected_path}/{field_name}_{texture_page}_{palette}{ext}",
                   "{selected_path}/{field_name}/{field_name}_{texture_page}_{palette}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}_{palette}{ext}" });
               assert(fme::key_value_data::has_balanced_braces(ret));
               return ret;
          }();
          // include if copy_data.palette.has_value();
          static const auto paths_with_texture_page = []() {
               const auto ret = std::to_array<std::string>(
                 { "{selected_path}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}",
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_0{texture_page}{ext}",

                   "{selected_path}/{field_name}_0{texture_page}{ext}",
                   "{selected_path}/{field_name}/{field_name}_0{texture_page}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_0{texture_page}{ext}",

                   "{selected_path}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}",
                   "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}",

                   "{selected_path}/{field_name}_{texture_page}{ext}",
                   "{selected_path}/{field_name}/{field_name}_{texture_page}{ext}",
                   "{selected_path}/{field_prefix}/{field_name}/{field_name}_{texture_page}{ext}" });
               assert(fme::key_value_data::has_balanced_braces(ret));
               return ret;
          }();

          static const auto paths_no_palette_and_texture_page = []() {
               const auto ret =
                 std::to_array<std::string>({ "{selected_path}/{field_name}{_{2_letter_lang}}{ext}",
                                              "{selected_path}/{field_name}/{field_name}{_{2_letter_lang}}{ext}",
                                              "{selected_path}/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}",

                                              "{selected_path}/{field_name}{ext}",
                                              "{selected_path}/{field_name}/{field_name}{ext}",
                                              "{selected_path}/{field_prefix}/{field_name}/{field_name}{ext}" });
               assert(fme::key_value_data::has_balanced_braces(ret));
               return ret;
          }();
          const auto transform_and_find_a_match = [&](const auto &...arr) {
               std::vector<std::filesystem::path> result = {};
               (
                 [&]() {
                      if (result.empty())
                      {
                           for (const auto &path_str : arr | std::ranges::views::transform(operation))
                           {
                                try
                                {
                                     if (std::filesystem::exists(path_str))
                                     {
                                          result.push_back(path_str);
                                          return;
                                     }
                                }
                                catch (...)
                                {
                                }
                           }
                      }
                 }(),
                 ...);

               return result;
          };
          if (copy_data.texture_page.has_value() && copy_data.palette.has_value())
          {
               return transform_and_find_a_match(paths_with_palette_and_texture_page, paths_with_palette_and_texture_page);
          }
          if (copy_data.texture_page.has_value())
          {
               return transform_and_find_a_match(paths_with_texture_page, paths_with_palette_and_texture_page);
          }
          return transform_and_find_a_match(paths_no_palette_and_texture_page);
     }

     [[nodiscard]] std::vector<std::filesystem::path> get_paths() const
     {

          const auto selections = m_selections.lock();
          if (!selections)
          {
               spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
               return {};
          }
          const fme::key_value_data data      = { .language_code = m_coo };
          const auto                operation = [&](const std::string &pattern) -> std::filesystem::path {
               return data.replace_tags(pattern, selections, m_root);
          };
          const auto        filter_dir = [](safedir path) { return path.is_exists() && path.is_dir(); };

          static const auto paths =
            std::to_array<std::string>({ // "{selected_path}/mods/Textures/{field_prefix}/{field_name}",
                                         //  "{selected_path}/{demaster_mod_path}/textures/field_bg/{field_prefix}/{field_name}",
                                         //  "{selected_path}/field_bg/{field_prefix}/{field_name}",
                                         //  "{selected_path}/textures/fields/{field_prefix}/{field_name}",
                                         //  "{selected_path}/textures/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{3_letter_lang}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{eng}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{eng}/FIELD/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{fre}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{fre}/FIELD/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ger}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ger}/FIELD/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ita}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ita}/FIELD/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{spa}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{spa}/FIELD/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{jp}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{jp}/FIELD/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{x}/field/mapdata/{field_prefix}/{field_name}",
                                         //  "{selected_path}/ff8/Data/{x}/FIELD/mapdata/{field_prefix}/{field_name}",

                                         //  "{selected_path}/mods/Textures/{field_name}",
                                         //  "{selected_path}/{demaster_mod_path}/textures/field_bg/{field_name}",
                                         //  "{selected_path}/field_bg/{field_name}",
                                         //  "{selected_path}/textures/fields/{field_name}",
                                         //  "{selected_path}/textures/{field_name}",
                                         //  "{selected_path}/ff8/Data/{3_letter_lang}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{3_letter_lang}/FIELD/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{eng}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{eng}/FIELD/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{fre}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{fre}/FIELD/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ger}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ger}/FIELD/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ita}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{ita}/FIELD/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{spa}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{spa}/FIELD/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{jp}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{jp}/FIELD/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{x}/field/mapdata/{field_name}",
                                         //  "{selected_path}/ff8/Data/{x}/FIELD/mapdata/{field_name}",

                                         "{selected_path}/mods/Textures",
                                         "{selected_path}/{demaster_mod_path}/textures/field_bg",
                                         "{selected_path}/field_bg",
                                         "{selected_path}/textures/fields",
                                         "{selected_path}/textures",
                                         "{selected_path}/ff8/Data/{3_letter_lang}/field/mapdata",
                                         "{selected_path}/ff8/Data/{3_letter_lang}/FIELD/mapdata",
                                         "{selected_path}/ff8/Data/{eng}/field/mapdata",
                                         "{selected_path}/ff8/Data/{eng}/FIELD/mapdata",
                                         "{selected_path}/ff8/Data/{fre}/field/mapdata",
                                         "{selected_path}/ff8/Data/{fre}/FIELD/mapdata",
                                         "{selected_path}/ff8/Data/{ger}/field/mapdata",
                                         "{selected_path}/ff8/Data/{ger}/FIELD/mapdata",
                                         "{selected_path}/ff8/Data/{ita}/field/mapdata",
                                         "{selected_path}/ff8/Data/{ita}/FIELD/mapdata",
                                         "{selected_path}/ff8/Data/{spa}/field/mapdata",
                                         "{selected_path}/ff8/Data/{spa}/FIELD/mapdata",
                                         "{selected_path}/ff8/Data/{jp}/field/mapdata",
                                         "{selected_path}/ff8/Data/{jp}/FIELD/mapdata",
                                         "{selected_path}/ff8/Data/{x}/field/mapdata",
                                         "{selected_path}/ff8/Data/{x}/FIELD/mapdata" });

          return paths | std::ranges::views::transform(operation) | std::ranges::views::filter(filter_dir) | std::ranges::to<std::vector>();
     }

     /*
      * Scan the directories looking for textures.
      *
      * Look for textures
      *
      *
      */
};
#endif// FIELD_MAP_EDITOR_UPSCALES_HPP
