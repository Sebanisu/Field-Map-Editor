//
// Created by pcvii on 12/1/2021.
//
#include "BPPs.hpp"
#include "GenericCombo.hpp"
bool ff_8::BPPs::on_im_gui_update() const
{
  return glengine::GenericCombo("BPP", m_current, m_strings);
}
