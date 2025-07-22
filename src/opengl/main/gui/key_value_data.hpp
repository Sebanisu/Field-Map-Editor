#ifndef C49C61B3_3B3C_45D4_BBF2_6E18296CA239
#define C49C61B3_3B3C_45D4_BBF2_6E18296CA239
#include "Selections.hpp"
#include <array>
#include <cstdint>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <memory>
#include <open_viii/strings/LangCommon.hpp>
#include <optional>
#include <ranges>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <tl/string.hpp>

namespace fme
{
struct key_value_data
{
     std::string                     field_name    = {};
     std::string                     ext           = { ".png" };
     std::optional<open_viii::LangT> language_code = {};
     std::optional<std::uint8_t>     palette       = {};
     std::optional<std::uint8_t>     texture_page  = {};
     std::optional<std::uint32_t>    pupu_id       = {};

     struct keys
     {
          static constexpr std::string_view selected_path                 = { "selected_path" };
          static constexpr std::string_view ff8_path                      = { "ff8_path" };
          static constexpr std::string_view current_path                  = { "current_path" };
          static constexpr std::string_view ffnx_mod_path                 = { "ffnx_mod_path" };
          static constexpr std::string_view ffnx_direct_mode_path         = { "ffnx_direct_mode_path" };
          static constexpr std::string_view ffnx_override_path            = { "ffnx_override_path" };
          static constexpr std::string_view batch_input_path              = { "batch_input_path" };
          static constexpr std::string_view batch_output_path             = { "batch_output_path" };
          static constexpr std::string_view field_name                    = { "field_name" };
          static constexpr std::string_view ext                           = { "ext" };
          static constexpr std::string_view field_prefix                  = { "field_prefix" };
          static constexpr std::string_view letter_2_lang                 = { "2_letter_lang" };
          static constexpr std::string_view letter_3_lang                 = { "3_letter_lang" };
          static constexpr std::string_view palette                       = { "palette" };
          static constexpr std::string_view texture_page                  = { "texture_page" };
          static constexpr std::string_view pupu_id                       = { "pupu_id" };
          static constexpr std::string_view demaster_mod_path             = { "demaster_mod_path" };
          static constexpr std::string_view ffnx_multi_texture            = { "ffnx_multi_texture" };
          static constexpr std::string_view ffnx_single_texture           = { "ffnx_single_texture" };
          static constexpr std::string_view ffnx_map                      = { "ffnx_map" };
          static constexpr std::string_view demaster                      = { "demaster" };
          static constexpr std::string_view field_main                    = { "field_main" };
          static constexpr std::string_view field_lang                    = { "field_lang" };
          static constexpr std::string_view chara_main                    = { "chara_main" };
          static constexpr std::string_view chara_lang                    = { "chara_lang" };
          static constexpr std::string_view field_3lang_main              = { "field_3lang_main" };
          static constexpr std::string_view chara_3lang_main              = { "chara_3lang_main" };
          static constexpr std::string_view eng                           = { "eng" };// open_viii::LangCommon::ENG
          static constexpr std::string_view fre                           = { "fre" };
          static constexpr std::string_view ger                           = { "ger" };
          static constexpr std::string_view ita                           = { "ita" };
          static constexpr std::string_view spa                           = { "spa" };
          static constexpr std::string_view jp                            = { "jp" };
          static constexpr std::string_view x                             = { "x" };

          static constexpr auto             all_keys                      = std::to_array<std::string_view>({ selected_path,
                                                                                                              // ff8_path,
                                                                                                              // current_path,
                                                                                                              ffnx_mod_path,
                                                                                                              ffnx_direct_mode_path,
                                                                                                              ffnx_override_path,
                                                                                                              // batch_input_path,
                                                                                                              // batch_output_path,
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

          static constexpr std::string_view selected_path_tooltip         = { "The user chooses a path to output or read data from." };
          static constexpr std::string_view ff8_path_tooltip              = { "The root directory where Final Fantasy VIII is installed." };
          static constexpr std::string_view current_path_tooltip          = { "The current working directory." };
          static constexpr std::string_view ffnx_mod_path_tooltip         = { "The base directory for FFNx mod files." };
          static constexpr std::string_view ffnx_direct_mode_path_tooltip = {
               "The directory for FFNx direct mode files, allowing file overrides. `.map` files would go here."
          };
          static constexpr std::string_view ffnx_override_path_tooltip = { "The FFNx override path for replacing game assets." };
          static constexpr std::string_view batch_input_path_tooltip   = { "The folder containing files for batch processing." };
          static constexpr std::string_view batch_output_path_tooltip  = { "The destination folder for processed batch output." };
          static constexpr std::string_view field_name_tooltip         = { "The name of the current field or scene." };
          static constexpr std::string_view ext_tooltip                = { "The file extension of the current resource." };
          static constexpr std::string_view field_prefix_tooltip       = { "The prefix used for field-related assets." };
          static constexpr std::string_view letter_2_lang_tooltip      = { "The two-letter language code (e.g., 'en' for English)." };
          static constexpr std::string_view letter_3_lang_tooltip      = { "The three-letter language code (e.g., 'eng' for English)." };
          static constexpr std::string_view palette_tooltip            = { "The color palette ID for textures." };
          static constexpr std::string_view texture_page_tooltip       = { "The texture page ID used for rendering." };
          static constexpr std::string_view pupu_id_tooltip = { "The 32-bit hex number used for Pupu ID. This is for deswizzles." };
          static constexpr std::string_view demaster_mod_path_tooltip  = { "The mod directory for Demaster, an FF8 modding tool." };

          static constexpr std::string_view ffnx_multi_texture_tooltip = {
               "Path layout for FFNx multi-texture field assets:\n"
               "{ffnx_mod_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{ext}"
          };
          static constexpr std::string_view ffnx_single_texture_tooltip = {
               "Path layout for FFNx single-texture field assets:\n"
               "{ffnx_mod_path}/field/mapdata/{field_name}/{field_name}{_{2_letter_lang}}{ext}"
          };
          static constexpr std::string_view ffnx_map_tooltip = {
               "Path layout for FFNx map data in direct mode:\n"
               "{ffnx_direct_mode_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}"
          };
          static constexpr std::string_view demaster_tooltip = {
               "Path layout for field background textures in Demaster:\n"
               "{demaster_mod_path}/textures/field_bg/{field_prefix}/{field_name}/"
               "{field_name}{_{2_letter_lang}}{_{texture_page}}{_{palette}}{_{pupu_id}}{ext}"
          };

          static constexpr std::string_view field_main_tooltip = {
               "Standard path for field assets:\n"
               "field/mapdata/{field_prefix}/{field_name}/{field_name}{ext}"
          };
          static constexpr std::string_view field_lang_tooltip = {
               "Path for language-specific field assets:\n"
               "field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}"
          };
          static constexpr std::string_view chara_main_tooltip = {
               "Path for main character assets:\n"
               "field/mapdata/{field_prefix}/{field_name}/chara.one"
          };
          static constexpr std::string_view chara_lang_tooltip = {
               "Path for language-specific character assets:\n"
               "field/mapdata/{field_prefix}/{field_name}/chara{_{2_letter_lang}}.one"
          };
          static constexpr std::string_view field_3lang_main_tooltip = {
               "Path for three-letter language-specific field assets:\n"
               "{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}/{field_name}{ext}"
          };
          static constexpr std::string_view chara_3lang_main_tooltip = {
               "Path for three-letter language-specific character assets:\n"
               "{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}/chara.one"
          };


          static constexpr auto all_tooltips = std::to_array<std::string_view>({ selected_path_tooltip,
                                                                                 // ff8_path_tooltip,
                                                                                 // current_path_tooltip,
                                                                                 ffnx_mod_path_tooltip,
                                                                                 ffnx_direct_mode_path_tooltip,
                                                                                 ffnx_override_path_tooltip,
                                                                                 // batch_input_path_tooltip,
                                                                                 // batch_output_path_tooltip,
                                                                                 field_name_tooltip,
                                                                                 ext_tooltip,
                                                                                 field_prefix_tooltip,
                                                                                 letter_2_lang_tooltip,
                                                                                 letter_3_lang_tooltip,
                                                                                 palette_tooltip,
                                                                                 texture_page_tooltip,
                                                                                 pupu_id_tooltip,
                                                                                 demaster_mod_path_tooltip,
                                                                                 ffnx_multi_texture_tooltip,
                                                                                 ffnx_single_texture_tooltip,
                                                                                 ffnx_map_tooltip,
                                                                                 demaster_tooltip,
                                                                                 field_main_tooltip,
                                                                                 field_lang_tooltip,
                                                                                 chara_main_tooltip,
                                                                                 chara_lang_tooltip,
                                                                                 field_3lang_main_tooltip,
                                                                                 chara_3lang_main_tooltip });
     };

     /**
      * @brief Replaces placeholder tags in a string with values from test data or a selected path.
      *
      * This function scans the input string for tags in the following formats:
      * - `{key}`: Replaced directly with the corresponding value from `test_data` or `selected_path`.
      * - `{prefix{key}suffix}`: If the key resolves to a non-empty value, it's inserted between the prefix and suffix.
      *
      * The function uses a regular expression to identify these patterns:
      * @code
      *   R"(\{([^\{\}]+)\}|\{([^\{]*)\{([^\}]+)\}([^\}]*)\})"
      * @endcode
      * This matches either a simple `{key}` or a nested format with prefix and suffix.
      *
      * Nested tags are supported and resolved recursively up to a maximum depth of 10 layers to prevent infinite loops.
      * After replacements, all slashes in the resulting string are normalized for the platform (Windows or Linux).
      *
      * @param keyed_string  The input string possibly containing tag placeholders.
      * @param test_data     A key-value data source to resolve the values associated with tags.
      * @param selections    A shared pointer to additional selection context used by `test_data`.
      * @param selected_path The replacement value for the `{selected_path}` tag. Defaults to "{ff8_path}".
      *
      * @return A new string where all recognized tags have been replaced with their corresponding values.
      * @todo This function might move as this is an odd place to put it. Maybe it should be apart of key_value_data.
      */
     [[nodiscard]] std::string replace_tags(
       std::string                                   keyed_string,
       const std::shared_ptr<const fme::Selections> &selections,
       const std::filesystem::path                  &selected_path = "{ff8_path}") const;

     /**
      * @brief Static variant of replace_tags. Doesn't access instance-specific data.
      *
      * @param keyed_string  The input string with tags.
      * @param selections    Context for resolving dynamic tags like `ff8_path`, etc.
      * @return A string with all known tags replaced and slashes normalized.
      */
     [[nodiscard]] static std::string
       static_replace_tags(std::string keyed_string, const std::shared_ptr<const fme::Selections> &selections);

     /**
      * @brief Resolves a key using the Selections object.
      *
      * @param key         The tag name (e.g., "ff8_path").
      * @param selections  Context to resolve selection-dependent keys.
      * @return Resolved value or empty string if unknown.
      */
     [[nodiscard]] static std::string static_replace_tag(std::string_view key, const std::shared_ptr<const fme::Selections> &selections);

     /**
      * @brief Resolves static keys that do not require a selections context.
      *
      * @param key The tag name to resolve.
      * @return The corresponding path/template or empty string.
      */
     [[nodiscard]] static std::string static_replace_tag(std::string_view key);

     /**
      * @brief Attempts to resolve a key using selections first, then static fallbacks.
      *
      * @param key         Tag key to resolve.
      * @param selections  Optional selections object to provide context.
      * @return Resolved string or empty.
      */
     [[nodiscard]] std::string        replace_tag(std::string_view key, const std::shared_ptr<const fme::Selections> &selections) const;

     /**
      * @brief Attempts to resolve a tag from instance data or static fallbacks.
      *
      * @param key The key/tag to resolve.
      * @return The resolved value or empty string.
      */
     [[nodiscard]] std::string        replace_tag(std::string_view key) const;

     /**
      * @brief Replaces a tag in the given key using the provided selections.
      *
      * This function replaces any tags found in the input key string
      * using the given selections context. It's typically used to resolve
      * placeholders or tags to their corresponding values based on external data.
      *
      * @param key The input string containing a tag to replace.
      * @param selections A shared pointer to a Selections object used for tag resolution.
      * @return A new string with the tag(s) replaced using the given selections.
      */
     [[nodiscard]] std::string        operator()(std::string_view key, const std::shared_ptr<const fme::Selections> &selections) const;

     /**
      * @brief Replaces a tag in the given key without using external selections.
      *
      * This overload resolves tags found in the key using internal or default mechanisms,
      * without any additional context from a Selections object.
      *
      * @param key The input string containing a tag to replace.
      * @return A new string with the tag(s) replaced using default or internal rules.
      */
     [[nodiscard]] std::string        operator()(std::string_view key) const;

   private:
     /**
      * @brief Retrieves the prefix of the field name.
      *
      * This function returns the first two characters of the `field_name`,
      * typically used as a shorthand or identifier prefix.
      *
      * @return A string view representing the first two characters of the field name.
      */
     constexpr std::string field_prefix() const noexcept
     {
          if (std::ranges::size(field_name) >= 2U)
               return field_name.substr(0, 2);
          else
               return {};
     }

     /**
      * @brief Gets the 2-letter language code.
      *
      * If a language code is present, this function converts it to its
      * 2-letter string representation (e.g., "en" for English).
      *
      * @return A string view of the 2-letter language code, or an empty view if not set.
      */
     constexpr std::string_view letter_2_lang() const noexcept
     {
          if (language_code.has_value())
               return open_viii::LangCommon::to_string(language_code.value());
          else
               return {};
     }


     /**
      * @brief Gets the 3-letter language code.
      *
      * Converts the stored language code to a 3-letter representation (e.g., "eng" for English).
      * If no language code is set, the default "eng" (English) is returned.
      *
      * @return A string view of the 3-letter language code.
      */
     constexpr std::string_view letter_3_lang() const noexcept
     {
          if (language_code.has_value())
               return open_viii::LangCommon::to_string_3_char(language_code.value());
          else
               return open_viii::LangCommon::to_string_3_char(open_viii::LangT::en);
     }
};
}// namespace fme

#endif /* C49C61B3_3B3C_45D4_BBF2_6E18296CA239 */
