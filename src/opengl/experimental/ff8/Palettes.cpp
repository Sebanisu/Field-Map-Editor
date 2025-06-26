//
// Created by pcvii on 12/1/2021.
//
#include "Palettes.hpp"
#include "GenericCombo.hpp"
#include "imgui.h"
bool ff_8::Palettes::on_im_gui_update() const
{
  return glengine::GenericCombo("Palette", m_current, m_strings);
}
ff_8::Palettes::operator std::uint8_t() const
{
  return m_values.at(static_cast<std::size_t>(m_current));
}
std::string_view ff_8::Palettes::string() const
{
  return m_strings.at(static_cast<std::size_t>(m_current));
}
int ff_8::Palettes::index() const
{
  return m_current;
}
