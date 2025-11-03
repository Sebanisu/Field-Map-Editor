//
// Created by pcvii on 11/30/2021.
//

#include "Coos.hpp"
#include <glengine/GenericCombo.hpp>
static constexpr auto coo_index  = std::string_view("coo_index");
static constexpr auto coo_string = std::string_view("coo_string");
bool                  ff_8::Coos::on_im_gui_update() const
{
     if (glengine::GenericCombo("Language", m_current, m_coos))
     {
          auto config = Configuration{};
          config->insert_or_assign(coo_index, m_current);
          config->insert_or_assign(coo_string, std::string_view{ *this });
          config.save();
          return true;
     }
     return false;
}
// std::string_view ff_8::Coos::operator*() const
//{
//   return m_coos.at(static_cast<std::size_t>(m_current));
// }
ff_8::Coos::operator std::string_view() const
{
     return m_coos.at(static_cast<std::size_t>(m_current));
}
ff_8::Coos::Coos()
  : m_current(Configuration{}[coo_index].value_or(int{}))
{
}
