#include "key_value_data.hpp"


[[nodiscard]] std::string fme::key_value_data::replace_tags(
  std::string                                   keyed_string,
  const std::shared_ptr<const fme::Selections> &selections,
  const std::string                            &selected_path) const
{
     using namespace std::string_view_literals;
     constexpr static auto pattern          = CTRE_REGEX_INPUT_TYPE{ R"(\{([^\{\}]+)\}|\{([^\{]*)\{([^\}]+)\}([^\}]*)\})" };
     std::string           copy_for_matches = keyed_string;// copy so data won't get lost when we change it.
     auto                  matches          = ctre::search_all<pattern>(copy_for_matches);
     int                   layers_deep      = 10;
     const static size_t   replace_str_id   = 0;
     const static size_t   key_id_alt       = 3;
     const static size_t   key_id           = 1;
     const static size_t   prefix_id        = 2;
     const static size_t   suffix_id        = 4;
     do
     {
          for ([[maybe_unused]] const auto &match : matches)
          {
               const auto replace_str = std::string_view{ match.get<replace_str_id>() };
               const auto key         = std::string_view{ match.get<key_id>() }.empty() ? std::string_view{ match.get<key_id_alt>() }
                                                                                        : std::string_view{ match.get<key_id>() };
               const auto value       = [&]() {
                    if (key == keys::selected_path)
                    {
                         return selected_path;
                    }
                    return replace_tag(key, selections);
               }();
               const auto prefix = value.empty() ? std::string_view{} : std::string_view{ match.get<prefix_id>() };
               const auto suffix = value.empty() ? std::string_view{} : std::string_view{ match.get<suffix_id>() };
               keyed_string      = keyed_string | std::views::split(replace_str)
                              | std::views::join_with(fmt::format("{}{}{}", prefix, value, suffix)) | std::ranges::to<std::string>();
          }
          // check for nested keys.
          copy_for_matches = keyed_string;
          matches          = ctre::search_all<pattern>(copy_for_matches);
     } while (!std::ranges::empty(matches) && ((--layers_deep) != 0));
     tl::string::replace_slashes(keyed_string);// fixes slashes to be windows or linux based.
     return keyed_string;
}


[[nodiscard]] std::string
  fme::key_value_data::static_replace_tags(std::string keyed_string, const std::shared_ptr<const fme::Selections> &selections)
{
     using namespace std::string_view_literals;
     constexpr static auto pattern          = CTRE_REGEX_INPUT_TYPE{ R"(\{([^\{\}]+)\}|\{([^\{]*)\{([^\}]+)\}([^\}]*)\})" };
     std::string           copy_for_matches = keyed_string;// copy so data won't get lost when we change it.
     auto                  matches          = ctre::search_all<pattern>(copy_for_matches);
     int                   layers_deep      = 10;
     const static size_t   replace_str_id   = 0;
     const static size_t   key_id_alt       = 3;
     const static size_t   key_id           = 1;
     const static size_t   prefix_id        = 2;
     const static size_t   suffix_id        = 4;
     do
     {
          for ([[maybe_unused]] const auto &match : matches)
          {
               const auto replace_str = std::string_view{ match.get<replace_str_id>() };
               const auto key         = std::string_view{ match.get<key_id>() }.empty() ? std::string_view{ match.get<key_id_alt>() }
                                                                                        : std::string_view{ match.get<key_id>() };
               const auto value       = static_replace_tag(key, selections);
               const auto prefix      = value.empty() ? std::string_view{} : std::string_view{ match.get<prefix_id>() };
               const auto suffix      = value.empty() ? std::string_view{} : std::string_view{ match.get<suffix_id>() };
               keyed_string           = keyed_string | std::views::split(replace_str)
                              | std::views::join_with(fmt::format("{}{}{}", prefix, value, suffix)) | std::ranges::to<std::string>();
          }
          // check for nested keys.
          copy_for_matches = keyed_string;
          matches          = ctre::search_all<pattern>(copy_for_matches);
     } while (!std::ranges::empty(matches) && ((--layers_deep) != 0));
     tl::string::replace_slashes(keyed_string);// fixes slashes to be windows or linux based.
     return keyed_string;
}


[[nodiscard]] std::string
  fme::key_value_data::static_replace_tag(std::string_view key, const std::shared_ptr<const fme::Selections> &selections)
{
     if (selections)
     {
          if (keys::ff8_path == key)
               return selections->path;

          if (keys::ffnx_mod_path == key)
               return selections->ffnx_mod_path;

          if (keys::ffnx_direct_mode_path == key)
               return selections->ffnx_direct_mode_path;

          if (keys::ffnx_override_path == key)
               return selections->ffnx_override_path;

          if (key == keys::batch_input_path)
               return selections->batch_input_path;

          if (key == keys::batch_output_path)
               return selections->batch_output_path;
     }
     return static_replace_tag(key);
}


[[nodiscard]] std::string fme::key_value_data::static_replace_tag(std::string_view key)
{

     if (keys::current_path == key)
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

     if (key == keys::demaster_mod_path)
          return "DEMASTER_EXP";

     if (key == keys::ffnx_multi_texture)
          return "{ffnx_mod_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}_{texture_page}{_{pupu_id}}{ext}";

     if (key == keys::ffnx_single_texture)
          return "{ffnx_mod_path}/field/mapdata/{field_name}/{field_name}{_{2_letter_lang}}{_{pupu_id}}{ext}";

     if (key == keys::ffnx_map)
          return "{ffnx_direct_mode_path}/field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}";

     if (key == keys::demaster)
          return "{demaster_mod_path}/textures/field_bg/{field_prefix}/{field_name}/"
                 "{field_name}{_{2_letter_lang}}{_{texture_page}}{_{palette}}{_{pupu_id}}{ext}";

     if (key == keys::field_main)
          return "field/mapdata/{field_prefix}/{field_name}/{field_name}{ext}";

     if (key == keys::field_lang)
          return "field/mapdata/{field_prefix}/{field_name}/{field_name}{_{2_letter_lang}}{ext}";

     if (key == keys::chara_main)
          return "field/mapdata/{field_prefix}/{field_name}/chara.one";

     if (key == keys::chara_lang)
          return "field/mapdata/{field_prefix}/{field_name}/chara{_{2_letter_lang}}.one";

     if (key == keys::field_3lang_main)
          return "{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}/{field_name}{ext}";

     if (key == keys::chara_3lang_main)
          return "{3_letter_lang}/FIELD/mapdata/{field_prefix}/{field_name}/chara.one";
     return {};
}


[[nodiscard]] std::string
  fme::key_value_data::replace_tag(std::string_view key, const std::shared_ptr<const fme::Selections> &selections) const
{
     auto value = static_replace_tag(key, selections);
     if (value.empty())
     {
          value = replace_tag(key);
     }
     return value;
}


[[nodiscard]] std::string fme::key_value_data::replace_tag(std::string_view key) const
{
     // First try static replacements
     if (auto result = static_replace_tag(key); !result.empty())
          return result;

     if (key == keys::field_name)
     {
          // assert(!field_name.empty() && field_name.length() >= 3);
          return fmt::format("{}", field_name);
     }
     if (key == keys::ext)
     {
          if (ext.size() < 2 || ext[0] != '.')
          {
               spdlog::error("{}:{} ext \"{}\"must be ext.size(), {} >= 2 && ext[0] == '.' ", __FILE__, __LINE__, ext, ext.size());
          }
          // assert(ext.size() >= 2 && ext[0] == '.');
          return fmt::format("{}", ext);
     }
     if (key == keys::field_prefix)
          return field_prefix();

     if (key == keys::letter_2_lang)
          return fmt::format("{}", letter_2_lang());

     if (key == keys::letter_3_lang)
          return fmt::format("{}", letter_3_lang());

     if (key == keys::palette && palette.has_value())
          return fmt::format("{}", palette.value());

     if (key == keys::texture_page && texture_page.has_value())
          return fmt::format("{}", texture_page.value());

     if (key == keys::pupu_id && pupu_id.has_value())
          return fmt::format("{:08X}", pupu_id.value());

     if (key == keys::eng)
          return { open_viii::LangCommon::ENG.begin(), open_viii::LangCommon::ENG.end() };

     if (key == keys::fre)
          return { open_viii::LangCommon::FRE.begin(), open_viii::LangCommon::FRE.end() };

     if (key == keys::ger)
          return { open_viii::LangCommon::GER.begin(), open_viii::LangCommon::GER.end() };

     if (key == keys::ita)
          return { open_viii::LangCommon::ITA.begin(), open_viii::LangCommon::ITA.end() };

     if (key == keys::spa)
          return { open_viii::LangCommon::SPA.begin(), open_viii::LangCommon::SPA.end() };

     if (key == keys::jp)
          return { open_viii::LangCommon::JP.begin(), open_viii::LangCommon::JP.end() };

     if (key == keys::x)
          return { open_viii::LangCommon::MISC.begin(), open_viii::LangCommon::MISC.end() };

     return {};
}


[[nodiscard]] std::string
  fme::key_value_data::operator()(std::string_view key, const std::shared_ptr<const fme::Selections> &selections) const
{
     return replace_tag(key, selections);
}

[[nodiscard]] std::string fme::key_value_data::operator()(std::string_view key) const
{
     return replace_tag(key);
}