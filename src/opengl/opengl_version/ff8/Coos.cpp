//
// Created by pcvii on 11/30/2021.
//

#include "Coos.hpp"
#include "GenericCombo.hpp"
bool ff8::Coos::OnImGuiUpdate() const
{
  return glengine::GenericCombo("Language",m_current,m_coos);
}
