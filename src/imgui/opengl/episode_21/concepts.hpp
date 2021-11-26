//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_CONCEPTS_HPP
#define MYPROJECT_CONCEPTS_HPP
#include <concepts>
template<typename T, typename U>
concept decay_same_as = std::is_same_v<std::decay_t<T>, std::decay_t<U>>;
template<typename T>
concept Void = std::is_void_v<T>;
template<typename T>
concept Bindable = requires(T t)
{
  {
    t.Bind()
    } -> Void;
  {
    t.UnBind()
    } -> Void;
};
template<typename T>
concept SizedBindable = Bindable<T> && requires(T t)
{
  {
    t.size()
    } -> std::integral;
};
#endif// MYPROJECT_CONCEPTS_HPP
