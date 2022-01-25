//
// Created by pcvii on 12/1/2021.
//
#include "BPPs.hpp"
#include "GenericCombo.hpp"
bool ff8::BPPs::OnImGuiUpdate() const
{
  return glengine::GenericCombo("BPP",m_current,m_strings);
}
