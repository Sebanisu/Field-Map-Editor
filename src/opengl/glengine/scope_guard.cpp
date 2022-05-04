//
// Created by pcvii on 11/29/2021.
//
#include "scope_guard.hpp"
namespace glengine
{
scope_guard_expensive::scope_guard_expensive(std::function<void()> t)
  : func(std::move(t))
{
}
scope_guard_expensive::scope_guard_expensive(const scope_guard_expensive &other)
  : func(other.func)
{
}
scope_guard_expensive &
  scope_guard_expensive::operator=(const scope_guard_expensive &other)
{
  scope_guard_expensive tmp{ other };
  *this = std::move(tmp);
  return *this;
}
scope_guard_expensive::scope_guard_expensive(
  scope_guard_expensive &&other) noexcept
  : scope_guard_expensive()
{
  swap(*this, other);
}
scope_guard_expensive &
  scope_guard_expensive::operator=(scope_guard_expensive &&other) noexcept
{
  swap(*this, other);
  return *this;
}
scope_guard_expensive::~scope_guard_expensive()
{
  if (func)
  {
    try
    {
      func();
    }
    catch (std::exception &e)
    {
      fmt::print("{}:{} - exception is {}", __FILE__, __LINE__, e.what());
    }
  }
}
scope_guard_expensive &scope_guard_expensive::operator=(std::function<void()> t)
{
  scope_guard_expensive tmp{ t };
  *this = std::move(tmp);
  return *this;
}
void swap(
  scope_guard_expensive &first,
  scope_guard_expensive &second) noexcept// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.func, second.func);
}
}// namespace glengine
