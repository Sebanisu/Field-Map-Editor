//
// Created by pcvii on 11/15/2021.
//

#ifndef MYPROJECT_SCOPE_GUARD_HPP
#define MYPROJECT_SCOPE_GUARD_HPP
#include <functional>
struct [[nodiscard]] scope_guard
{
  scope_guard(void (*t)())
    : func(std::move(t))
  {
  }
  scope_guard(const scope_guard &other)
    : func(other.func)
  {
  }

  scope_guard &
    operator=(const scope_guard &other)
  {
    func = other.func;
    return *this;
  }

  scope_guard(scope_guard &&other)
    : scope_guard()
  {
    swap(*this, other);
  }

  scope_guard &
    operator=(scope_guard &&other)
  {
    swap(*this, other);
    return *this;
  }

  ~scope_guard()
  {
    if (func != nullptr)
    {
      func();
    }
  }

  void
    swap(scope_guard &first, scope_guard &second)// nothrow
  {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    // by swapping the members of two objects,
    // the two objects are effectively swapped
    swap(first.func, second.func);
  }

private:
  scope_guard() = default;
  void (*func)();
};

struct [[nodiscard]] scope_guard_expensive
{
  scope_guard_expensive(std::function<void()> t)
    : func(std::move(t))
  {
  }
  scope_guard_expensive(const scope_guard_expensive &other)
    : func(other.func)
  {
  }
  
  scope_guard_expensive &
    operator=(const scope_guard_expensive &other)
  {
    func = other.func;
    return *this;
  }

  scope_guard_expensive(scope_guard_expensive &&other)
    : scope_guard_expensive()
  {
    swap(*this, other);
  }

  scope_guard_expensive &
    operator=(scope_guard_expensive &&other)
  {
    swap(*this, other);
    return *this;
  }

  ~scope_guard_expensive()
  {
    if (func != nullptr)
    {
      func();
    }
  }

  void
    swap(scope_guard_expensive &first, scope_guard_expensive &second)// nothrow
  {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    // by swapping the members of two objects,
    // the two objects are effectively swapped
    swap(first.func, second.func);
  }

private:
  scope_guard_expensive() = default;
  std::function<void()> func;
};
#endif// MYPROJECT_SCOPE_GUARD_HPP
