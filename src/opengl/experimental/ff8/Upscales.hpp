//
// Created by pcvii on 5/2/2022.
//

#ifndef FIELD_MAP_EDITOR_UPSCALES_HPP
#define FIELD_MAP_EDITOR_UPSCALES_HPP
#include "Configuration.hpp"
#include <Event/EventItem.hpp>
#include <Renderable.hpp>
namespace ff_8
{
class Upscales
{
   public:
     void               on_update(float) const {}
     void               on_render() const {}
     [[nodiscard]] bool on_im_gui_update() const;
     void               on_event(const glengine::event::Item &) const {}
     operator std::filesystem::path() const;
     Upscales();
     Upscales(Configuration);

   private:
     [[nodiscard]] const std::string &string() const;
     mutable int                      m_current = {};
     mutable toml::array              m_paths   = {};
};
static_assert(glengine::Renderable<Upscales>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_UPSCALES_HPP
