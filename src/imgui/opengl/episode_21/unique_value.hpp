//
// Created by pcvii on 12/5/2021.
//

#ifndef MYPROJECT_UNIQUE_VALUE_HPP
#define MYPROJECT_UNIQUE_VALUE_HPP
template<typename T>
class weak_value;
template<typename T>
class unique_value
{
public:
  unique_value() = default;
  unique_value(T t, void (*f)(T))
    : m_value(std::move(t))
    , m_function(std::move(f))
  {
  }
  unique_value(const unique_value &) = delete;
  ~unique_value() noexcept
  {
    if (m_function != nullptr)
    {
      std::invoke(m_function, std::move(m_value));
    }
  }
  unique_value(unique_value &&other) noexcept
    : unique_value()
  {
    swap(*this, other);
  }
  unique_value &operator=(const unique_value &) = delete;
  unique_value &operator                        =(unique_value &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }
  friend void swap(unique_value &left, unique_value &right) noexcept
  {
    using std::swap;
    swap(left.m_value, right.m_value);
    swap(left.m_function, right.m_function);
  }

  operator T() const noexcept
  {
    return m_value;
  }
  friend weak_value<T>;

private:
  T m_value             = {};
  void (*m_function)(T) = nullptr;
};

static_assert(
  std::movable<unique_value<
    std::uint32_t>> && !std::copyable<unique_value<std::uint32_t>>);
using GLID = unique_value<std::uint32_t>;

template<typename T, std::invocable<T> F>
unique_value(T t, F f) -> unique_value<T>;
template<typename T>
class weak_value
{
public:
  weak_value() = default;
  weak_value(const unique_value<T> &t)
    : m_value(t.m_value)
  {
  }
  operator T() const noexcept
  {
    return m_value;
  }

private:
  T m_value = {};
};
static_assert(
  std::movable<
    weak_value<std::uint32_t>> && std::copyable<weak_value<std::uint32_t>>);
using GLID_copy = weak_value<std::uint32_t>;
#endif// MYPROJECT_UNIQUE_VALUE_HPP
