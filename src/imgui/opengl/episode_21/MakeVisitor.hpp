//
// Created by pcvii on 12/7/2021.
//

#ifndef MYPROJECT_MAKEVISITOR_HPP
#define MYPROJECT_MAKEVISITOR_HPP
#include <utility>
template<typename... B>
[[nodiscard]] inline auto make_visitor(B &&...b) noexcept
{
  struct visitor final : public std::decay_t<B>...
  {
    using B::operator()...;
  };
  return visitor{ std::forward<B>(b)... };
}
#endif// MYPROJECT_MAKEVISITOR_HPP
