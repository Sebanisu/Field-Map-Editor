//
// Created by pcvii on 12/1/2021.
//
#include "Palettes.hpp"
#include "GenericCombo.hpp"
#include "imgui.h"
bool ff8::Palettes::OnImGuiUpdate() const
{
  return glengine::GenericCombo("Palette", m_current, m_strings);
}
std::uint8_t ff8::Palettes::Palette() const
{
  return m_values.at(static_cast<std::size_t>(m_current));
}
std::string_view ff8::Palettes::String() const
{
  return m_strings.at(static_cast<std::size_t>(m_current));
}
int ff8::Palettes::Index() const
{
  return m_current;
}
