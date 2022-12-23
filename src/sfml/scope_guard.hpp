//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
#define FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
template<std::invocable functionT>
struct [[nodiscard]] scope_guard
{
  scope_guard(functionT &&t)
    : func(std::forward<functionT>(t))
  {
  }

  ~scope_guard()
  {
    std::invoke(func);
  }

private:
  functionT func;
};
#endif// FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
