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
template<typename T, std::size_t sizeT>
class unique_value_array
{
public:
  using ParameterT      = std::array<unsigned int, sizeT> &;
  using ConstParameterT = const std::array<unsigned int, sizeT> &;
  unique_value_array()  = default;
  template<decay_same_as<T>... Us>
  unique_value_array(void (*destroy)(ParameterT), Us... ts)
    : m_value{ std::forward<Us>(ts)... }
    , m_function(std::move(destroy))
  {
  }
  template<std::invocable<ParameterT> createT>
  unique_value_array(void (*destroy)(ParameterT), createT &&create)
    : m_function(move(destroy))
  {
    std::invoke(create, m_value);
  }
  unique_value_array(const unique_value_array &) = delete;
  ~unique_value_array() noexcept
  {
    if (m_function != nullptr)
    {
      std::invoke(m_function, m_value);
    }
  }
  unique_value_array(unique_value_array &&other) noexcept
    : unique_value_array()
  {
    swap(*this, other);
  }
  unique_value_array &operator=(const unique_value_array &) = delete;
  unique_value_array &operator=(unique_value_array &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }
  friend void swap(unique_value_array &left, unique_value_array &right) noexcept
  {
    using std::swap;
    swap(left.m_value, right.m_value);
    swap(left.m_function, right.m_function);
  }
  const T &operator[](std::size_t index) const
  {
    return m_value[index];
  }
  const T &at(std::size_t index) const
  {
    return m_value.at(index);
  }
  auto size() const
  {
    return m_value.size();
  }
  const auto *data() const
  {
    return m_value.data();
  }
  auto begin() const
  {
    return m_value.begin();
  }
  auto end() const
  {
    return m_value.end();
  }
  auto cbegin() const
  {
    return m_value.cbegin();
  }
  auto cend() const
  {
    return m_value.cend();
  }
  operator T() const noexcept
  {
    return m_value;
  }
  friend weak_value<std::array<T, sizeT>>;

private:
  std::array<T, sizeT> m_value   = {};
  void (*m_function)(ParameterT) = nullptr;
};

static_assert(
  std::movable<unique_value<
    std::uint32_t>> && !std::copyable<unique_value<std::uint32_t>>);
static_assert(
  std::movable<unique_value_array<
    std::uint32_t,
    1>> && !std::copyable<unique_value_array<std::uint32_t, 1>>);
using GLID = unique_value<std::uint32_t>;
template<std::size_t sizeT>
using GLID_array = unique_value_array<std::uint32_t, sizeT>;

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
