//
// Created by pcvii on 11/23/2021.
//

#ifndef MYPROJECT_TEST_HPP
#define MYPROJECT_TEST_HPP
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
        } -> std::same_as<bool>;
    } ||
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
#endif// MYPROJECT_TEST_HPP
