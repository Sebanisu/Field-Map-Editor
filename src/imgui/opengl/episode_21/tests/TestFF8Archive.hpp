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
    OnImGuiUpdate(const TestFF8Archive &);
  friend void
    OnEvent(const TestFF8Archive &, const Event::Item &);

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
  OnImGuiUpdate(const TestFF8Archive &self)
{
  ff8::OnImGuiUpdate(self.ff8_menu);
}
inline void
  OnEvent(const TestFF8Archive &self, const Event::Item &e)
{
  ff8::OnEvent(self.ff8_menu, e);
}
}// namespace test
#endif// MYPROJECT_TESTFF8ARCHIVE_HPP
