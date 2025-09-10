//
// Created by pcvii on 12/1/2021.
//
#include "BPPs.hpp"
#include "GenericCombo.hpp"
bool ff_8::Bpps::on_im_gui_update() const
{
     return glengine::GenericCombo("BPP", m_current, m_strings);
}
const open_viii::graphics::BPPT *ff_8::Bpps::operator->() const
{
     return &m_values.at(static_cast<std::size_t>(m_current));
}
std::string_view ff_8::Bpps::string() const
{
     return m_strings.at(static_cast<std::size_t>(m_current));
}
int ff_8::Bpps::index() const
{
     return m_current;
}
