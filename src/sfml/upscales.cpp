//
// Created by pcvii on 9/29/2021.
//
#include "upscales.hpp"

// upscales() = default;
upscales::upscales(std::weak_ptr<fme::Selections> selections)
  : m_selections(selections)
{
}
upscales::upscales(std::string root, std::optional<open_viii::LangT> coo, std::weak_ptr<fme::Selections> selections)
  : m_coo(coo)
  , m_root(std::move(root))
  , m_selections(selections)
{
}

[[nodiscard]] std::vector<std::filesystem::path>
  upscales::generate_upscale_paths(const std::filesystem::path &field_root, fme::key_value_data copy_data) const
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
                           else if (exists)
                           {
                                result.push_back(path_str);
                                return;
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


[[nodiscard]] bool upscales::has_upscale_path(const std::filesystem::path &field_root, fme::key_value_data copy_data) const
{

     namespace v           = std::ranges::views;
     const auto selections = m_selections.lock();
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
                   const auto      exists = std::filesystem::exists(path_str, ec);
                   if (ec)
                   {
                        spdlog::info("Failed to check path '{}': error={}", path_str, ec.message());
                        return false;
                   }
                   return exists;
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


[[nodiscard]] std::vector<std::filesystem::path> upscales::get_paths() const
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
     const auto filter_dir = [](safedir path) { return path.is_exists() && path.is_dir(); };


     return selections->paths_common_upscale | std::ranges::views::transform(operation) | std::ranges::views::filter(filter_dir)
            | std::ranges::to<std::vector>();
}

[[nodiscard]] std::vector<std::filesystem::path> upscales::get_map_paths() const
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
     const auto filter_dir = [](safedir path) { return path.is_exists() && path.is_dir(); };


     auto       transformed_paths =
       selections->paths_common_upscale_for_maps | std::ranges::views::transform(operation) | std::ranges::views::filter(filter_dir);
     auto regular_paths = get_paths();
     regular_paths.insert(regular_paths.begin(), transformed_paths.begin(), transformed_paths.end());
     return regular_paths;
}
