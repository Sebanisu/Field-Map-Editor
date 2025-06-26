#include "path_search.hpp"
#include "safedir.hpp"
#include <fmt/core.h>
namespace ff_8
{

bool path_search::has_deswizzle_path(const ff_8::PupuID pupu, const std::string &ext) const
{
     return has_deswizzle_path(filters_deswizzle_value_string, pupu, ext);
}
bool path_search::has_deswizzle_path(const std::filesystem::path &filter_path, const std::string &ext) const
{
     return std::ranges::any_of(working_unique_pupu, [&](const ff_8::PupuID pupu) { return has_deswizzle_path(filter_path, pupu, ext); });
}
bool path_search::has_deswizzle_path(const std::filesystem::path &filter_path, const ff_8::PupuID pupu, const std::string &ext) const
{
     return path_search::has_upscale_path(
       filter_path.string(),
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt,
         .pupu_id       = pupu.raw() },
       selections);
     //   || safedir(cpm.replace_tags(output_map_pattern_for_deswizzle, selections, filter_path.string())).is_exists();
}

bool path_search::has_swizzle_path(const std::uint8_t texture_page, const std::string &ext) const
{
     return has_swizzle_path(filters_upscale_value_string, texture_page, ext);
}

bool path_search::has_swizzle_path(const std::uint8_t texture_page, std::uint8_t palette, const std::string &ext) const
{
     return has_swizzle_path(filters_upscale_value_string, texture_page, palette, ext);
}

bool path_search::has_swizzle_path(const std::filesystem::path &filter_path, const std::string &ext) const
{
     return [&]() {
          for (const auto &[bpp, palette_set] : bpp_palette)
          {
               if (bpp.bpp24())
               {
                    continue;
               }
               for (const auto &palette : palette_set.values())
               {
                    for (const auto &texture_page : texture_page_id.values())
                    {
                         if (has_swizzle_path(filter_path, texture_page, palette, ext))
                         {
                              return true;
                         }
                    }
               }
          }
          return false;
     }() || [&]() {
          for (const auto &texture_page : texture_page_id.values())
          {
               if (has_swizzle_path(filter_path, texture_page, ext))
               {
                    return true;
               }
          }
          return false;
     }();
}

bool path_search::has_swizzle_path(const std::filesystem::path &filter_path, const std::uint8_t texture_page, const std::string &ext) const
{

     return has_upscale_path(
       filter_path.string(),
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt,
         .texture_page  = texture_page },
       selections);
     //   || safedir(cpm.replace_tags(output_swizzle_pattern, selections, filter_path.string())).is_exists();
}

bool path_search::has_map_path(
  const std::filesystem::path &filter_path,
  const std::string           &ext,
  const std::string           &secondary_output_pattern) const
{

     const auto cpm =
       fme::key_value_data{ .field_name    = field_name,
                            .ext           = ext,
                            .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt };
     if (!filter_path.empty() && has_upscale_path(filter_path.string(), cpm, selections))
     {
          return true;
     }
     if (secondary_output_pattern.empty())
     {
          return false;
     }
     auto       temp = cpm.replace_tags(secondary_output_pattern, selections, filter_path.string());
     const auto test = safedir{ temp };
     return !test.is_dir() && test.is_exists();
}

bool path_search::has_swizzle_path(
  const std::filesystem::path &filter_path,
  const std::uint8_t           texture_page,
  std::uint8_t                 palette,
  const std::string           &ext) const
{


     return has_upscale_path(
       filter_path.string(),
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt,
         .palette       = palette,
         .texture_page  = texture_page },
       selections);
}

std::vector<std::filesystem::path> path_search::generate_swizzle_paths(const std::string &ext) const
{

     return generate_paths(
       filters_upscale_value_string,
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       // selections->output_map_pattern_for_swizzle,
       selections->output_swizzle_pattern);
}


std::vector<std::filesystem::path> path_search::generate_swizzle_paths(const std::filesystem::path &path, const std::string &ext) const
{

     return generate_paths(
       path.string(),
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       // selections->output_map_pattern_for_swizzle,
       selections->output_swizzle_pattern);
}

std::vector<std::filesystem::path> path_search::generate_swizzle_map_paths(const std::string &ext) const
{

     return generate_paths(
       filters_upscale_map_value_string,
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       selections->output_map_pattern_for_swizzle,
       selections->output_swizzle_pattern);
}

std::vector<std::filesystem::path> path_search::generate_swizzle_map_paths(const std::filesystem::path &path, const std::string &ext) const
{

     return generate_paths(
       path.string(),
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       selections->output_map_pattern_for_swizzle,
       selections->output_swizzle_pattern);
}

std::vector<std::filesystem::path>
  path_search::generate_deswizzle_map_paths(const std::filesystem::path &path, const std::string &ext) const
{

     return generate_paths(
       path.string(),
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       selections->output_map_pattern_for_deswizzle,
       selections->output_deswizzle_pattern);
}

std::vector<std::filesystem::path> path_search::generate_deswizzle_paths(const std::string &ext) const
{

     return generate_paths(
       filters_deswizzle_value_string,
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       // selections->output_map_pattern_for_deswizzle,
       selections->output_deswizzle_pattern);
}

std::vector<std::filesystem::path> path_search::generate_deswizzle_paths(const std::filesystem::path &path, const std::string &ext) const
{

     return generate_paths(
       path.string(),
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       // selections->output_map_pattern_for_deswizzle,
       selections->output_deswizzle_pattern);
}

std::vector<std::filesystem::path> path_search::generate_deswizzle_map_paths(const std::string &ext) const
{

     return generate_paths(
       filters_deswizzle_map_value_string,
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt },

       selections->output_map_pattern_for_deswizzle,
       selections->output_deswizzle_pattern);
}


std::vector<std::filesystem::path> path_search::generate_deswizzle_paths(const ff_8::PupuID pupu, const std::string &ext) const
{

     return generate_paths(
       filters_deswizzle_value_string,
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt,
         .pupu_id       = pupu.raw() },
       selections->output_deswizzle_pattern);
}

std::vector<std::filesystem::path>
  path_search::generate_swizzle_paths(const std::uint8_t texture_page, std::uint8_t palette, const std::string &ext) const
{

     return generate_paths(
       filters_upscale_value_string,
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt,
         .palette       = palette,
         .texture_page  = texture_page },
       selections->output_swizzle_pattern);
}

std::vector<std::filesystem::path> path_search::generate_swizzle_paths(const std::uint8_t texture_page, const std::string &ext) const
{

     return generate_paths(
       filters_upscale_value_string,
       { .field_name    = field_name,
         .ext           = ext,
         .language_code = opt_coo.has_value() && opt_coo.value() != open_viii::LangT::generic ? opt_coo : std::nullopt,
         .texture_page  = texture_page },
       selections->output_swizzle_pattern);
}

std::vector<std::filesystem::path> path_search::generate_paths(
  const std::string         &filter_path,
  const fme::key_value_data &cpm,
  const std::string         &output_pattern,
  const std::string         &secondary_output_pattern) const
{
     std::vector<std::filesystem::path> paths = {};
     if (!filter_path.empty())
     {
          paths = generate_upscale_paths(filter_path, cpm, selections);
     }
     if (!output_pattern.empty())
     {
          auto temp = cpm.replace_tags(output_pattern, selections, filter_path);
          if (const auto test = safedir{ temp }; !test.is_dir() && test.is_exists())
          {
               paths.push_back(std::move(temp));
          }
     }
     if (!secondary_output_pattern.empty())
     {
          auto temp = cpm.replace_tags(secondary_output_pattern, selections, filter_path);
          if (const auto test = safedir{ temp }; !test.is_dir() && test.is_exists())
          {
               paths.push_back(std::move(temp));
          }
     }
     return paths;
}

[[nodiscard]] std::vector<std::filesystem::path> path_search::generate_upscale_paths(
  const std::filesystem::path           &field_root,
  fme::key_value_data                    copy_data,
  std::shared_ptr<const fme::Selections> selections)
{

     namespace v = std::ranges::views;
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     const auto operation = [&](const std::string &pattern) -> std::filesystem::path {
          return copy_data.replace_tags(pattern, selections, field_root.string());
     };

     const auto transform_and_find_a_match = [&](const auto &...arr) {
          std::vector<std::filesystem::path> result = {};
          (
            [&]() {
                 if (result.empty())
                 {
                      for (const auto &path_str : arr | std::ranges::views::transform(operation))
                      {
                           std::error_code ec{};
                           const auto      exists = std::filesystem::exists(path_str, ec);
                           if (ec)
                           {
                                spdlog::info("Failed to check path '{}': error={}", path_str, ec.message());
                           }
                           ec.clear();
                           const auto is_regular_file = exists && std::filesystem::is_regular_file(path_str, ec);
                           if (ec)
                           {
                                spdlog::info("Failed to check if is file path '{}': error={}", path_str, ec.message());
                           }
                           if (exists && is_regular_file)
                           {
                                result.push_back(path_str);
                                spdlog::info("Found file and added path '{}'", path_str);
                                return;// we only want one match right now.
                           }
                      }
                 }
            }(),
            ...);

          return result;
     };
     if (copy_data.pupu_id.has_value())
     {
          return transform_and_find_a_match(selections->paths_with_pupu_id);
     }
     if (copy_data.texture_page.has_value() && copy_data.palette.has_value())
     {
          return transform_and_find_a_match(selections->paths_with_palette_and_texture_page, selections->paths_no_palette_and_texture_page);
     }
     if (copy_data.texture_page.has_value())
     {
          return transform_and_find_a_match(selections->paths_with_texture_page, selections->paths_no_palette_and_texture_page);
     }
     return transform_and_find_a_match(selections->paths_no_palette_and_texture_page);
}


[[nodiscard]] bool path_search::has_upscale_path(
  const std::filesystem::path           &field_root,
  fme::key_value_data                    copy_data,
  std::shared_ptr<const fme::Selections> selections)
{

     namespace v = std::ranges::views;
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return false;
     }
     const auto transform_and_find_a_match = [&](const auto &...arr) -> bool {
          return (
            std::ranges::any_of(
              arr,
              [](const auto &path_str) {
                   std::error_code ec{};
                   const auto      status = std::filesystem::status(path_str, ec);
                   if (ec)
                   {// Ignore "not found" errors
                        if (ec == std::errc::no_such_file_or_directory)
                        {
                             // Not found - common, safe to ignore
                             return false;
                        }
                        spdlog::info("Failed to check path '{}': error={}", path_str, ec.message());
                        return false;
                   }
                   ec.clear();
                   if (std::filesystem::exists(status) && std::filesystem::is_regular_file(status))
                   {
                        spdlog::info("Found file path '{}'", path_str);
                        return true;
                   }
                   return false;
              },
              [&](const std::string &pattern) -> std::filesystem::path {
                   return copy_data.replace_tags(pattern, selections, field_root.string());
              })
            || ...);
     };
     if (copy_data.pupu_id.has_value())
     {
          return transform_and_find_a_match(selections->paths_with_pupu_id);
     }
     if (copy_data.texture_page.has_value() && copy_data.palette.has_value())
     {
          return transform_and_find_a_match(selections->paths_with_palette_and_texture_page, selections->paths_no_palette_and_texture_page);
     }
     if (copy_data.texture_page.has_value())
     {
          return transform_and_find_a_match(selections->paths_with_texture_page, selections->paths_no_palette_and_texture_page);
     }
     return transform_and_find_a_match(selections->paths_no_palette_and_texture_page);
}


[[nodiscard]] std::vector<std::filesystem::path>
  path_search::get_paths(std::shared_ptr<const fme::Selections> selections, std::optional<open_viii::LangT> coo, std::string root)
{

     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     const fme::key_value_data data      = { .language_code = coo };
     const auto                operation = [&](const std::string &pattern) -> std::filesystem::path {
          return data.replace_tags(pattern, selections, root);
     };
     const auto filter_dir = [](safedir path) { return path.is_exists() && path.is_dir(); };


     return selections->paths_common_upscale | std::ranges::views::transform(operation) | std::ranges::views::filter(filter_dir)
            | std::ranges::to<std::vector>();
}

[[nodiscard]] std::vector<std::filesystem::path>
  path_search::get_map_paths(std::shared_ptr<const fme::Selections> selections, std::optional<open_viii::LangT> coo, std::string root)
{

     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     const fme::key_value_data data      = { .language_code = coo };
     const auto                operation = [&](const std::string &pattern) -> std::filesystem::path {
          return data.replace_tags(pattern, selections, root);
     };
     const auto filter_dir = [](safedir path) { return path.is_exists() && path.is_dir(); };


     auto       transformed_paths =
       selections->paths_common_upscale_for_maps | std::ranges::views::transform(operation) | std::ranges::views::filter(filter_dir);
     auto regular_paths = get_paths(selections, coo, root);
     regular_paths.insert(regular_paths.begin(), transformed_paths.begin(), transformed_paths.end());
     return regular_paths;
}

}// namespace ff_8