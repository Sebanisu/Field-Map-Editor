//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_COOS_HPP
#define FIELD_MAP_EDITOR_COOS_HPP
#include "scope_guard.hpp"
#include <Event/EventItem.hpp>
#include <Renderable.hpp>

namespace ff8
{
class Coos
{
public:
  void                           OnUpdate(float) const {}
  void                           OnRender() const {}
  bool                           OnImGuiUpdate() const;
  void                           OnEvent(const glengine::Event::Item &) const {}
  [[nodiscard]] std::string_view Coo() const
  {
    return m_coos.at(static_cast<std::size_t>(m_current));
  }

private:
  static constexpr auto m_coos = open_viii::LangCommon::to_string_array();
  mutable int           m_current{};
};
static_assert(glengine::Renderable<Coos>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_COOS_HPP
