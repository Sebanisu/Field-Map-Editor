//
// Created by pcvii on 12/21/2022.
//
#include "Selections.hpp"
#include "formatters.hpp"
#include "utilities.hpp"
#include <spdlog/spdlog.h>

fme::Selections::Selections(const Configuration config)
  : m_selections_array(load_selections_array(config))
{
     sort_paths();
     std::filesystem::path &path  = get<ConfigKey::FF8Path>();
     const auto            &paths = get<ConfigKey::FF8DirectoryPaths>();
     if (std::ranges::find(paths, path) == paths.end())
     {
          if (!paths.empty())
          {
               path = paths.front();
          }
     }
}

std::optional<fme::Configuration> fme::Selections::get_ffnx_config() const
{
     return get_ffnx_config(get<ConfigKey::FF8Path>());
}

std::optional<fme::Configuration> fme::Selections::get_ffnx_config(const std::filesystem::path &ff8_path) const
{
     const auto      ffnx_settings_toml = ff8_path / "FFNx.toml";
     std::error_code error_code         = {};
     bool            exists             = std::filesystem::exists(ffnx_settings_toml, error_code);
     if (error_code)
     {
          spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), ffnx_settings_toml);
          error_code.clear();
     }

     if (exists)
     {
          return ffnx_settings_toml;
     }
     return std::nullopt;
}

/**
 * @brief Sorts and deduplicates the stored paths.
 *
 * This function reads the current paths from `FF8DirectoryPaths`, sorts them alphabetically,
 * and removes duplicates. If the paths are already sorted and unique, no action is taken.
 * After modification, the updated paths are saved back into the configuration.
 *
 * @note Paths that do not exist on the filesystem are TODO: not yet removed.
 */
void fme::Selections::sort_paths()
{
     const bool remove_on_error = true;// or false depending on behavior you want

     auto       process         = [&]<ConfigKey... Keys>() {

          auto args           = std::forward_as_tuple(get<Keys>()...);// tuple of references
          auto args_with_bool = std::tuple_cat(std::make_tuple(remove_on_error), args);

          bool changed    = false;
          changed |= std::apply(remove_empty_values<std::remove_cvref_t<decltype(get<Keys>())>...>, args);
          changed |= std::apply(remove_nonexistent_paths<std::remove_cvref_t<decltype(get<Keys>())>...>, args_with_bool);
          changed |= std::apply(sort_and_remove_duplicates<std::remove_cvref_t<decltype(get<Keys>())>...>, args);

          if (changed)
               update<Keys...>();
     };

     process.template operator()<ConfigKey::FF8DirectoryPaths>();
     process.template operator()<ConfigKey::ExternalTexturesDirectoryPaths>();
     process.template operator()<ConfigKey::ExternalMapsDirectoryPaths>();

     process.template operator()<
       ConfigKey::CacheTexturePaths,
       ConfigKey::CacheSwizzlePathsEnabled,
       ConfigKey::CacheSwizzleAsOneImagePathsEnabled,
       ConfigKey::CacheDeswizzlePathsEnabled>();

     process.template operator()<ConfigKey::CacheMapPaths, ConfigKey::CacheMapPathsEnabled>();
}
