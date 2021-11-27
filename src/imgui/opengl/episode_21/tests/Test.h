//
// Created by pcvii on 11/23/2021.
//

#ifndef MYPROJECT_TEST_H
#define MYPROJECT_TEST_H
#include "concepts.hpp"
namespace test
{
template<typename T>
concept Test =
  std::default_initializable<T> && std::movable<T> &&(// requires(const T &t)
    //{
    //  {
    //    t.OnUpdate(float{})
    //    } -> Void;
    //  {
    //    t.OnRender()
    //    } -> Void;
    //  {
    //    t.OnImGuiRender()
    //    } -> Void;
    //} ||
    requires(const T &t)
    {
      {
        OnUpdate(t, float{})
        } -> Void;
      {
        OnRender(t)
        } -> Void;
      {
        OnImGuiRender(t)
        } -> Void;
    });
}// namespace test
#endif// MYPROJECT_TEST_H
