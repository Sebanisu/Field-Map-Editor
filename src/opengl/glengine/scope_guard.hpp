//
// Created by pcvii on 11/15/2021.
//

#ifndef MYPROJECT_SCOPE_GUARD_HPP
#define MYPROJECT_SCOPE_GUARD_HPP
namespace glengine
{
struct [[nodiscard]] scope_guard
{
  constexpr scope_guard(void (*t)())
    : func(std::move(t))
  {
  }

  ~scope_guard()
  {// bug in gcc making this not constexpr with -Werror=useless-cast
    exec();
  }

  constexpr scope_guard &operator=(void (*t)())
  {
    exec();
    func = std::move(t);
    return *this;
  }

  constexpr scope_guard(const scope_guard &other)
    : func(other.func)
  {
  }
  constexpr scope_guard &operator=(const scope_guard &other)
  {
    exec();
    func = other.func;
    return *this;
  }
  constexpr scope_guard(scope_guard &&other) noexcept
    : scope_guard()
  {
    swap(*this, other);
  }
  constexpr scope_guard &operator=(scope_guard &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  constexpr friend void swap(
    scope_guard &first,
    scope_guard &second) noexcept// nothrow
  {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    // by swapping the members of two objects,
    // the two objects are effectively swapped
    swap(first.func, second.func);
  }
  template<size_t count>
  [[nodiscard]] static constexpr auto array(void (*t)())
  {
    std::array<scope_guard, count> r{};
    std::ranges::for_each(r, [&t](scope_guard &guard) { guard = t; });
    return r;
  }
  constexpr scope_guard() = default;

private:
  constexpr void exec() const
  {
    if (func != nullptr)
    {
      func();
    }
  }
  void (*func)() = nullptr;
};

struct [[nodiscard]] scope_guard_expensive
{
  scope_guard_expensive(std::function<void()> t);
  scope_guard_expensive(const scope_guard_expensive &other);

  scope_guard_expensive &operator=(const scope_guard_expensive &other);

  scope_guard_expensive(scope_guard_expensive &&other) noexcept;

  scope_guard_expensive &operator=(scope_guard_expensive &&other) noexcept;

  scope_guard_expensive &operator=(std::function<void()> t);

  ~scope_guard_expensive();

  friend void
    swap(scope_guard_expensive &first, scope_guard_expensive &second) noexcept;

  template<std::convertible_to<std::function<void()>> T, size_t count>
  [[nodiscard]] static std::array<scope_guard_expensive, count> array(T t)
  {
    std::array<scope_guard_expensive, count> r{};
    std::ranges::for_each(r, [&t](scope_guard_expensive &guard) { guard = t; });
    return r;
  }

  scope_guard_expensive() = default;

private:
  std::function<void()> func{};
};
}// namespace glengine
#endif// MYPROJECT_SCOPE_GUARD_HPP
