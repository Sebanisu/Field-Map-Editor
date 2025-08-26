//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_COOS_HPP
#define FIELD_MAP_EDITOR_COOS_HPP
#include "Configuration.hpp"
#include "ScopeGuard.hpp"
#include <Event/EventItem.hpp>
#include <open_viii/strings/LangCommon.hpp>
#include <Renderable.hpp>
namespace ff_8
{
class Coos
{
   public:
     Coos();
     void on_update(float) const {}
     void on_render() const {}
     bool on_im_gui_update() const;
     void on_event(const glengine::event::Item &) const {}
     //[[nodiscard]] std::string_view operator*() const;
     [[nodiscard]] operator std::string_view() const;

   private:
     static constexpr auto m_coos = open_viii::LangCommon::to_string_array();
     mutable int           m_current{};
};
static_assert(glengine::Renderable<Coos>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_COOS_HPP
