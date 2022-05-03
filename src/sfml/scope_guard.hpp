//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
#define FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
struct [[nodiscard]] scope_guard
{
  scope_guard(void (*t)())
    : func(std::move(t))
  {
  }

  ~scope_guard()
  {
    func();
  }

private:
  void (*func)();
};
#endif// FIELD_MAP_EDITOR_SCOPE_GUARD_HPP
