//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_TESTCLEARCOLOR_HPP
#define MYPROJECT_TESTCLEARCOLOR_HPP


namespace test
{
class TestClearColor
{
public:
  TestClearColor();
  void OnUpdate(float) const {}
  void OnRender() const;
  void OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const {}

private:
  mutable glm::vec4 m_clear_color = { 0.2F, 0.3F, 0.8F, 1.F };
};
}// namespace test
#endif// MYPROJECT_TESTCLEARCOLOR_HPP
