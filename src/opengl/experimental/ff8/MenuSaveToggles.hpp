//
// Created by pcvii on 9/5/2022.
//

#ifndef FIELD_MAP_EDITOR_MENUSAVETOGGLES_HPP
#define FIELD_MAP_EDITOR_MENUSAVETOGGLES_HPP
#include "Configuration.hpp"
#include "Menu.hpp"
#include <fmt/format.h>
#include <ranges>
static inline void MenuSaveToggles(const glengine::Menu &menu, Configuration &config)
{
     const auto  string = fmt::format("{}_menu", menu.title());
     toml::array array{};
     array.reserve(menu.size());
     const auto toggles = menu.get_toggles();
     for (const auto toggle : toggles)
     {
          array.push_back(bool{ toggle });
     }
     config->insert_or_assign(string, array);
     config.save();
}
static inline void MenuSaveToggles(const glengine::Menu &menu)
{
     Configuration config{};
     MenuSaveToggles(menu, config);
}
static inline void MenuLoadToggles(glengine::Menu &menu, Configuration &config)
{
     const auto string = fmt::format("{}_menu", menu.title());
     if (config->contains(string))
     {
          toml::array *array = (config->get(string)->as_array());
          array->resize(menu.size(), false);
          config.save();
          menu.toggle_items((*array) | std::ranges::views::transform([](toml::node &val) { return val.value_or(false); }));
     }
     //  else
     //  {
     //    MenuSaveToggles(menu, config);
     //  }
}
static inline void MenuLoadToggles(glengine::Menu &menu)
{
     Configuration config{};
     MenuLoadToggles(menu, config);
}

#endif// FIELD_MAP_EDITOR_MENUSAVETOGGLES_HPP
