//
// Created by pcvii on 11/30/2021.
//

#include "Coos.hpp"
#include "GenericCombo.hpp"
bool ff_8::Coos::on_im_gui_update() const
{
  return glengine::GenericCombo("Language", m_current, m_coos);
}
// std::string_view ff_8::Coos::operator*() const
//{
//   return m_coos.at(static_cast<std::size_t>(m_current));
// }
ff_8::Coos::operator std::string_view() const
{
  return m_coos.at(static_cast<std::size_t>(m_current));
}
