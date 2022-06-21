//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_SCOPEGUARD_HPP
#define FIELD_MAP_EDITOR_SCOPEGUARD_HPP
namespace glengine
{
struct [[nodiscard]] ScopeGuard
{
  constexpr ScopeGuard(void (*t)())
    : func(std::move(t))
  {
  }

  constexpr ~ScopeGuard()
  {// bug in gcc making this not constexpr with -Werror=useless-cast
    exec();
  }

  constexpr ScopeGuard &operator=(void (*t)())
  {
    exec();
    func = std::move(t);
    return *this;
  }

  constexpr ScopeGuard(const ScopeGuard &other)
    : func(other.func)
  {
  }
  constexpr ScopeGuard &operator=(const ScopeGuard &other)
  {
    exec();
    func = other.func;
    return *this;
  }
  constexpr ScopeGuard(ScopeGuard &&other) noexcept
    : ScopeGuard()
  {
    swap(*this, other);
  }
  constexpr ScopeGuard &operator=(ScopeGuard &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }

  constexpr friend void swap(
    ScopeGuard &first,
    ScopeGuard &second) noexcept// nothrow
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
    std::array<ScopeGuard, count> r{};
    std::ranges::for_each(r, [&t](ScopeGuard &guard) { guard = t; });
    return r;
  }
  constexpr ScopeGuard() = default;

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
template<std::invocable functT>
struct [[nodiscard]] ScopeGuardCaptures
{
  constexpr ScopeGuardCaptures(functT &&in_funct)
    : func(std::forward<functT>(in_funct))
    , enabled(true)
  {
  }

  ~ScopeGuardCaptures()
  {// bug in gcc making this not constexpr with -Werror=useless-cast
    exec();
  }

  constexpr ScopeGuardCaptures &operator=(functT &&in_funct)
  {
    exec();
    func    = std::forward<functT>(in_funct);
    enabled = true;
    return *this;
  }

  constexpr ScopeGuardCaptures(const ScopeGuardCaptures &other)
    : func(other.func)
  {
  }
  constexpr ScopeGuardCaptures &operator=(const ScopeGuardCaptures &other)
  {
    exec();
    func    = other.func;
    enabled = other.enabled;
    return *this;
  }
  constexpr ScopeGuardCaptures(ScopeGuardCaptures &&other) noexcept
    : ScopeGuardCaptures()
  {
    swap(*this, other);
  }
  constexpr ScopeGuardCaptures &operator=(ScopeGuardCaptures &&other) noexcept
  {
    exec();
    swap(*this, other);
    return *this;
  }

  constexpr friend void swap(
    ScopeGuardCaptures &first,
    ScopeGuardCaptures &second) noexcept// nothrow
  {
    // enable ADL (not necessary in our case, but good practice)
    using std::swap;

    // by swapping the members of two objects,
    // the two objects are effectively swapped
    swap(first.func, second.func);
    swap(first.enabled, second.enabled);
  }
  template<size_t count>
  [[nodiscard]] static constexpr auto array(void (*t)())
  {
    std::array<ScopeGuardCaptures, count> r{};
    std::ranges::for_each(r, [&t](ScopeGuardCaptures &guard) { guard = t; });
    return r;
  }
  constexpr ScopeGuardCaptures() = default;

private:
  constexpr void exec() const
  {
    if (enabled)
    {
      func();
      enabled = false;
    }
  }
  functT       func    = {};
  mutable bool enabled = { false };
};

// struct [[nodiscard]] scope_guard_expensive
//{
//   scope_guard_expensive(std::function<void()> t);
//   scope_guard_expensive(const scope_guard_expensive &other);
//
//   scope_guard_expensive &operator=(const scope_guard_expensive &other);
//
//   scope_guard_expensive(scope_guard_expensive &&other) noexcept;
//
//   scope_guard_expensive &operator=(scope_guard_expensive &&other) noexcept;
//
//   scope_guard_expensive &operator=(std::function<void()> t);
//
//   ~scope_guard_expensive();
//
//   friend void
//     swap(scope_guard_expensive &first, scope_guard_expensive &second)
//     noexcept;
//
//   template<std::convertible_to<std::function<void()>> T, size_t count>
//   [[nodiscard]] static std::array<scope_guard_expensive, count> array(T t)
//   {
//     std::array<scope_guard_expensive, count> r{};
//     std::ranges::for_each(r, [&t](scope_guard_expensive &guard) { guard = t;
//     }); return r;
//   }
//
//   scope_guard_expensive() = default;
//
// private:
//   std::function<void()> func{};
// };
}// namespace glengine
#endif// FIELD_MAP_EDITOR_SCOPEGUARD_HPP
