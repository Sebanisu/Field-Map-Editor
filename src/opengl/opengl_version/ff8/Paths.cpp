//
// Created by pcvii on 11/30/2021.
//
#include "Paths.hpp"
#include "GenericCombo.hpp"
bool ff8::Paths::OnImGuiUpdate() const
{
  return glengine::GenericCombo("Path",m_current,m_paths);
}
const std::string &ff8::Paths::Path() const
{
  if (std::cmp_less(m_current, std::ranges::size(m_paths)))
  {
    return m_paths.at(static_cast<std::size_t>(m_current));
  }
  const static auto empty = std::string("");
  return empty;
}
ff8::Paths::Paths()
{
  m_paths = open_viii::Paths::get();
}
