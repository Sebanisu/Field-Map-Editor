//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_PATHS_HPP
#define FIELD_MAP_EDITOR_PATHS_HPP
#include "Configuration.hpp"
#include "ScopeGuard.hpp"
#include <Event/EventItem.hpp>
#include <Renderable.hpp>


namespace ff_8
{
class Paths
{
   public:
     void               on_update(float) const {}
     void               on_render() const {}
     [[nodiscard]] bool on_im_gui_update() const;
     void               on_event(const glengine::event::Item &) const {}
     [[nodiscard]]      operator std::filesystem::path() const;
     Paths();
     explicit Paths(Configuration);

   private:
     [[nodiscard]] const std::string &string() const;
     mutable toml::array              m_paths   = {};
     mutable int                      m_current = {};
};
static_assert(glengine::Renderable<Paths>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_PATHS_HPP
