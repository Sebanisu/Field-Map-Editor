//
// Created by pcvii on 11/15/2021.
//

#ifndef MYPROJECT_SCOPE_GUARD_HPP
#define MYPROJECT_SCOPE_GUARD_HPP
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
#endif// MYPROJECT_SCOPE_GUARD_HPP
