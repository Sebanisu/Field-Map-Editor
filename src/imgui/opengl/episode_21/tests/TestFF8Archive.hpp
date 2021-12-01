//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_TESTFF8ARCHIVE_HPP
#define MYPROJECT_TESTFF8ARCHIVE_HPP
#include "ff8/FF8Menu.hpp"
namespace test
{
class TestFF8Archive
{
public:
  friend void
    OnUpdate(const TestFF8Archive &, float);
  friend void
    OnRender(const TestFF8Archive &);
  friend void
    OnImGuiRender(const TestFF8Archive &);

private:
  ff8::FF8Menu ff8_menu = {};
};
inline void
  OnUpdate(const TestFF8Archive &self, float ts)
{
  ff8::OnUpdate(self.ff8_menu, ts);
}
inline void
  OnRender(const TestFF8Archive &self)
{
  ff8::OnRender(self.ff8_menu);
}
inline void
  OnImGuiRender(const TestFF8Archive &self)
{
  ff8::OnImGuiRender(self.ff8_menu);
}
}// namespace test
#endif// MYPROJECT_TESTFF8ARCHIVE_HPP
