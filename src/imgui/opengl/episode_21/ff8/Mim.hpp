//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_MIM_HPP
#define MYPROJECT_MIM_HPP
#include "Fields.hpp"
namespace ff8
{
class Mim
{
public:
  friend void
    OnUpdate(const Mim &, float);
  friend void
    OnRender(const Mim &);
  friend void
    OnImGuiRender(const Mim &);
  Mim() = default;
  Mim(const Fields &fields)
    : m_mim(fields.Mim(m_path, m_choose_coo))
  {
  }

private:
  std::string                          m_path       = {};
  bool                                 m_choose_coo = {};
  open_viii::graphics::background::Mim m_mim        = {};
};
static_assert(test::Test<Mim>);
inline void
  OnUpdate(const Mim &, float)
{
}
inline void
  OnRender(const Mim &)
{
}
inline void
  OnImGuiRender(const Mim &)
{
}
}// namespace ff8
#endif// MYPROJECT_MIM_HPP
