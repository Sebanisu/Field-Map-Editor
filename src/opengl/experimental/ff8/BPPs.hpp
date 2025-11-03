//
// Created by pcvii on 12/1/2021.
//

#ifndef FIELD_MAP_EDITOR_BPPS_HPP
#define FIELD_MAP_EDITOR_BPPS_HPP
#include <glengine/Event/EventItem.hpp>
#include <glengine/Renderable.hpp>
#include <open_viii/graphics/BPPT.hpp>
namespace ff_8
{
class Bpps
{
   public:
     void on_update(float) const {}
     void on_render() const {}
     bool on_im_gui_update() const;
     void on_event(const glengine::event::Item &) const {}
     const open_viii::graphics::BPPT *operator->() const;
     std::string_view                 string() const;
     int                              index() const;

   private:
     static constexpr auto m_values = []()
     {
          using namespace open_viii::graphics::literals;
          return std::array{ 4_bpp, 8_bpp, 16_bpp };
     }();
     static constexpr auto m_strings = []()
     {
          using namespace std::string_view_literals;
          return std::array{ "4"sv, "8"sv, "16"sv };
     }();
     mutable int m_current{};
};
static_assert(glengine::Renderable<Bpps>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_BPPS_HPP
