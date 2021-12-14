//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_PATHS_HPP
#define MYPROJECT_PATHS_HPP
#include "scope_guard.hpp"


namespace ff8
{
class Paths
{
public:
  void                             OnUpdate(float) const {}
  void                             OnRender() const {}
  bool                             OnImGuiUpdate() const;
  void                             OnEvent(const Event::Item &) const {}
  [[nodiscard]] const std::string &Path() const;
  Paths();

private:
  mutable std::vector<std::string> m_paths   = {};
  mutable int                      m_current = {};
};
static_assert(Renderable<Paths>);
}// namespace ff8
#endif// MYPROJECT_PATHS_HPP
