//
// Created by pcvii on 12/5/2021.
//

#ifndef FIELD_MAP_EDITOR_UNIQUE_VALUE_HPP
#define FIELD_MAP_EDITOR_UNIQUE_VALUE_HPP
namespace glengine
{
template<typename T>
class weak_value;
template<typename T>
class unique_value
{
public:
  constexpr unique_value() = default;
  constexpr unique_value(T t, void (*f)(T))
    : m_value(std::move(t))
    , m_function(std::move(f))
  {
  }
  unique_value(const unique_value &) = delete;
  constexpr ~unique_value() noexcept
  {
    if (m_function != nullptr)
    {
      std::invoke(m_function, std::move(m_value));
    }
  }
  constexpr unique_value(unique_value &&other) noexcept
    : unique_value()
  {
    swap(*this, other);
  }
  constexpr unique_value &operator=(const unique_value &) = delete;
  constexpr unique_value &operator=(unique_value &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }
  friend constexpr void swap(unique_value &left, unique_value &right) noexcept
  {
    using std::swap;
    swap(left.m_value, right.m_value);
    swap(left.m_function, right.m_function);
  }

  constexpr operator T() const noexcept
  {
    return m_value;
  }
  friend weak_value<T>;

private:
  T m_value             = {};
  void (*m_function)(T) = nullptr;
};
using GLID = unique_value<std::uint32_t>;
static_assert(std::movable<GLID> && !std::copyable<GLID>);
static_assert(GLID(1, [](std::uint32_t) {}) == std::uint32_t{ 1 });

template<typename T, std::size_t sizeT>
class unique_value_array
{
public:
  using ValueT          = std::array<T, sizeT>;
  using ParameterT      = ValueT &;
  using ConstParameterT = const ValueT &;
  unique_value_array()  = default;
  template<decay_same_as<T>... Us>
  constexpr unique_value_array(void (*destroy)(ParameterT), Us... ts)
    : m_value{ std::forward<Us>(ts)... }
    , m_function(std::move(destroy))
  {
  }
  template<std::invocable createT>
  requires decay_same_as<ValueT, std::invoke_result_t<createT>>
  constexpr unique_value_array(void (*destroy)(ParameterT), createT &&create)
    : m_value(std::invoke(create))
    , m_function(move(destroy))
  {
  }
  constexpr ~unique_value_array() noexcept
  {
    if (m_function != nullptr)
    {
      std::invoke(m_function, m_value);
    }
  }
  unique_value_array(const unique_value_array &) = delete;
  unique_value_array &operator=(const unique_value_array &) = delete;
  constexpr unique_value_array(unique_value_array &&other) noexcept
    : unique_value_array()
  {
    swap(*this, other);
  }
  constexpr unique_value_array &operator=(unique_value_array &&other) noexcept
  {
    swap(*this, other);
    return *this;
  }
  constexpr friend void
    swap(unique_value_array &left, unique_value_array &right) noexcept
  {
    using std::swap;
    swap(left.m_value, right.m_value);
    swap(left.m_function, right.m_function);
  }
  constexpr const T &operator[](std::size_t index) const
  {
    return m_value[index];
  }
  constexpr const T &at(std::size_t index) const
  {
    return m_value.at(index);
  }
  constexpr auto size() const
  {
    return m_value.size();
  }
  constexpr const auto *data() const
  {
    return m_value.data();
  }
  constexpr auto begin() const
  {
    return m_value.begin();
  }
  constexpr auto end() const
  {
    return m_value.end();
  }
  constexpr auto cbegin() const
  {
    return m_value.cbegin();
  }
  constexpr auto cend() const
  {
    return m_value.cend();
  }
  constexpr operator ValueT() const noexcept
  {
    return m_value;
  }
  friend weak_value<ValueT>;

private:
  ValueT m_value                 = {};
  void (*m_function)(ParameterT) = nullptr;
};
template<typename T, decay_same_as<T>... Ts>
unique_value_array(void (*)(std::array<T, sizeof...(Ts) + 1U> &), T, Ts...)
  -> unique_value_array<T, sizeof...(Ts) + 1U>;

template<std::invocable CreatorT>
unique_value_array(void (*)(std::invoke_result_t<CreatorT> &), CreatorT)
  -> unique_value_array<
    typename std::invoke_result_t<CreatorT>::value_type,
    std::ranges::size(typename std::invoke_result_t<CreatorT>{})>;
template<std::size_t sizeT>
using GLID_array = unique_value_array<std::uint32_t, sizeT>;
static_assert(std::movable<GLID_array<1>> && !std::copyable<GLID_array<1>>);
static_assert(
  static_cast<std::array<std::uint32_t, 1>>(
    GLID_array<1>([](GLID_array<1>::ParameterT) {}, 1U))
  == std::array{ 1U });
static_assert(
  static_cast<std::array<std::uint32_t, 1>>(GLID_array<1>(
    [](GLID_array<1>::ParameterT) {},
    []() {
      GLID_array<1>::ValueT out{};
      out[0] = 1U;
      return out;
    }))
  == std::array{ 1U });
static_assert(
  static_cast<std::array<std::uint32_t, 1>>(unique_value_array(
    [](GLID_array<1>::ParameterT) {},
    []() {
      GLID_array<1>::ValueT out{};
      out[0] = 1U;
      return out;
    }))
  == std::array{ 1U });
template<typename T, std::invocable<T> F>
unique_value(T t, F f) -> unique_value<T>;
template<typename T, typename U, std::size_t Usz>
concept array_type_and_size_are_same =
  decay_same_as<std::ranges::range_value_t<T>, U> && std::ranges::size(T{})
== Usz;
template<typename T>
class weak_value
{
public:
  constexpr weak_value() = default;
  constexpr weak_value(const unique_value<T> &t)
    : m_value(t.m_value)
  {
  }
  template<typename U, std::size_t Usz>
  requires array_type_and_size_are_same<T, U, Usz>
  constexpr weak_value(const unique_value_array<U, Usz> &t)
    : m_value(t.m_value)
  {
  }
  constexpr operator T() const noexcept
  {
    return m_value;
  }

private:
  T m_value = {};
};
using GLID_copy = weak_value<std::uint32_t>;
static_assert(std::movable<GLID_copy> && std::copyable<GLID_copy>);
static_assert(GLID_copy{ GLID(1, [](std::uint32_t) {}) } == std::uint32_t{ 1 });
static_assert(
  static_cast<std::array<std::uint32_t, 1U>>(
    weak_value<std::array<std::uint32_t, 1U>>{
      GLID_array<1U>([](typename GLID_array<1U>::ParameterT) {}, 1U) })
  == std::array{ 1U });
}// namespace glengine
#endif// FIELD_MAP_EDITOR_UNIQUE_VALUE_HPP