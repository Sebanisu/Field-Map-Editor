//
// Created by pcvii on 11/30/2021.
//
#include "Paths.hpp"
#include "Configuration.hpp"
#include "GenericCombo.hpp"
#include <open_viii/paths/Paths.hpp>
static constexpr auto ff8_directory_paths
  = std::string_view{ "ff8_directory_paths" };
static constexpr auto ff8_directory_path_index
  = std::string_view{ "ff8_directory_path_index" };
bool ff_8::Paths::on_im_gui_update() const
{
     if (glengine::GenericCombo(
           "Path",
           m_current,
           m_paths
             | std::ranges::views::transform(
               [](auto &&value) -> decltype(auto)
               {
                    return value.template ref<std::string>();
               }))// todo filter by if is directory check
     )
     {
          auto config = Configuration{};
          config->insert_or_assign(ff8_directory_path_index, m_current);
          config.save();
          return true;
     }
     return false;
}
const std::string &ff_8::Paths::string() const
{
     if (std::cmp_less(m_current, std::ranges::size(m_paths)))
     {
          return m_paths[static_cast<std::size_t>(m_current)]
            .ref<std::string>();
     }
     const static auto empty = std::string("");
     return empty;
}
ff_8::Paths::Paths(Configuration config)
  : m_paths(
      [&]()
      {
           if (!config->contains(ff8_directory_paths))
           {
                const auto &default_paths = open_viii::Paths::get();
                // todo get all default paths for linux and windows.
                toml::array paths_array{};
                paths_array.reserve(default_paths.size());
                for (const auto &path : default_paths)
                {
                     paths_array.push_back(path.u8string());
                }
                config->insert_or_assign(
                  ff8_directory_paths, std::move(paths_array));
                config.save();
           }
           return *(config->get_as<toml::array>(ff8_directory_paths));
      }())
  , m_current(config[ff8_directory_path_index].value_or(int{}))
{
     // todo move path clean up to after loading the here.
}
ff_8::Paths::operator std::filesystem::path() const
{
     return string();
}
ff_8::Paths::Paths()
  : Paths(Configuration{})
{
}
