//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_PATHS_HPP
#define FIELD_MAP_EDITOR_PATHS_HPP
#include "scope_guard.hpp"
#include <Event/EventItem.hpp>
#include <Renderable.hpp>
namespace ff8
{
class Paths
{
public:
  void OnUpdate(float) const {}
  void OnRender() const {}
  bool OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const {}
  [[nodiscard]] const std::string &Path() const;
  Paths();

private:
  mutable std::vector<std::string> m_paths   = {};
  mutable int                      m_current = {};
};
static_assert(glengine::Renderable<Paths>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_PATHS_HPP
