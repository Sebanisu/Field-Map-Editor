//
// Created by pcvii on 11/23/2021.
//

#ifndef MYPROJECT_TEST_H
#define MYPROJECT_TEST_H
#include <concepts>
namespace test
{
template<typename T>
concept Test = std::default_initializable<T> && requires(T t)
{
  t.OnUpdate(float{});
  t.OnRender();
  t.OnImGuiRender();
};
}// namespace test
#endif// MYPROJECT_TEST_H
