//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_CONCEPTS_HPP
#define MYPROJECT_CONCEPTS_HPP
#include <concepts>
template<typename T, typename U>
concept decay_same_as = std::is_same_v<std::decay_t<T>, std::decay_t<U>>;
#endif// MYPROJECT_CONCEPTS_HPP
