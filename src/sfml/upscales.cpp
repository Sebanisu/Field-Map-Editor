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
     const auto        filter_dir = [](safedir path) { return path.is_exists() && path.is_dir(); };


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
     const auto        filter_dir = [](safedir path) { return path.is_exists() && path.is_dir(); };

     static const auto paths =
       std::to_array<std::string>({ // todo ffnx uses a sepperate directory for map files which means we might not see it with our
                                    // current method of selecting one path ffnx_direct_mode_path might not want to be in the regular
                                    // paths list might need to be somewhere else. maybe a get paths map.
                                    "{selected_path}/{ffnx_direct_mode_path}/field/mapdata/" });

     auto transformed_paths = paths | std::ranges::views::transform(operation) | std::ranges::views::filter(filter_dir);
     auto regular_paths     = get_paths();
     regular_paths.insert(regular_paths.begin(), transformed_paths.begin(), transformed_paths.end());
     return regular_paths;
}


