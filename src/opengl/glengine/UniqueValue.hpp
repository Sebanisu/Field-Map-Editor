//
// Created by pcvii on 12/5/2021.
//

#ifndef FIELD_MAP_EDITOR_UNIQUEVALUE_HPP
#define FIELD_MAP_EDITOR_UNIQUEVALUE_HPP
namespace glengine
{
template<typename T>
class WeakValue;
template<typename T>
class UniqueValue
{
   public:
     constexpr UniqueValue() = default;
     constexpr UniqueValue(
       T t,
       void (*f)(T))
       : m_value(std::move(t))
       , m_function(std::move(f))
     {
     }
     UniqueValue(const UniqueValue &) = delete;
     constexpr ~UniqueValue() noexcept
     {
          // value of 0 is valid because sometimes nothing is bound.
          if (m_function != nullptr)
          {
               std::invoke(m_function, std::move(m_value));
               m_value = 0;
          }
     }
     constexpr UniqueValue(UniqueValue &&other) noexcept
       : UniqueValue()
     {
          swap(*this, other);
     }
     constexpr UniqueValue &operator=(const UniqueValue &) = delete;
     constexpr UniqueValue &operator=(UniqueValue &&other) noexcept
     {
          swap(*this, other);
          return *this;
     }
     friend constexpr void swap(
       UniqueValue &left,
       UniqueValue &right) noexcept
     {
          using std::swap;
          swap(left.m_value, right.m_value);
          swap(left.m_function, right.m_function);
     }
     constexpr T operator*() const noexcept
     {
          return m_value;
     }
     constexpr operator T() const noexcept
     {
          return m_value;
     }
     friend WeakValue<T>;

   private:
     T m_value             = {};
     void (*m_function)(T) = nullptr;
};
using Glid = UniqueValue<std::uint32_t>;
static_assert(std::movable<Glid> && !std::copyable<Glid>);
static_assert(
  Glid(
    1,
    [](std::uint32_t) {})
  == std::uint32_t{ 1 });

template<typename T, std::size_t sizeT>
class UniqueValueArray
{
   public:
     using ValueT          = std::array<T, sizeT>;
     using ParameterT      = ValueT &;
     using ConstParameterT = const ValueT &;
     UniqueValueArray()    = default;
     template<decay_same_as<T>... Us>
     constexpr UniqueValueArray(
       void (*destroy)(ParameterT),
       Us... ts)
       : m_value{ std::forward<Us>(ts)... }
       , m_function(std::move(destroy))
     {
     }
     template<std::invocable createT>
          requires decay_same_as<
                     ValueT,
                     std::invoke_result_t<createT>>
     constexpr UniqueValueArray(
       void (*destroy)(ParameterT),
       createT &&create)
       : m_value(std::invoke(create))
       , m_function(std::move(destroy))
     {
     }
     constexpr ~UniqueValueArray() noexcept
     {
          if (m_function != nullptr)
          {
               std::invoke(m_function, m_value);
          }
     }
     UniqueValueArray(const UniqueValueArray &)            = delete;
     UniqueValueArray &operator=(const UniqueValueArray &) = delete;
     constexpr UniqueValueArray(UniqueValueArray &&other) noexcept
       : UniqueValueArray()
     {
          swap(*this, other);
     }
     constexpr UniqueValueArray &operator=(UniqueValueArray &&other) noexcept
     {
          swap(*this, other);
          return *this;
     }
     constexpr friend void swap(
       UniqueValueArray &left,
       UniqueValueArray &right) noexcept
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
     friend WeakValue<ValueT>;

   private:
     ValueT m_value                 = {};
     void (*m_function)(ParameterT) = nullptr;
};
template<
  typename T,
  decay_same_as<T>... Ts>
UniqueValueArray(
  void (*)(std::array<
           T,
           sizeof...(Ts) + 1U> &),
  T,
  Ts...)
  -> UniqueValueArray<
    T,
    sizeof...(Ts) + 1U>;

template<std::invocable CreatorT>
UniqueValueArray(
  void (*)(std::invoke_result_t<CreatorT> &),
  CreatorT)
  -> UniqueValueArray<
    typename std::invoke_result_t<CreatorT>::value_type,
    std::ranges::size(typename std::invoke_result_t<CreatorT>{})>;
template<std::size_t sizeT>
using GlidArray = UniqueValueArray<std::uint32_t, sizeT>;
static_assert(std::movable<GlidArray<1>> && !std::copyable<GlidArray<1>>);
static_assert(
  static_cast<std::array<
    std::uint32_t,
    1>>(
    GlidArray<1>(
      [](GlidArray<1>::ParameterT) {},
      1U))
  == std::array{ 1U });
static_assert(
  static_cast<std::array<
    std::uint32_t,
    1>>(
    GlidArray<1>(
      [](GlidArray<1>::ParameterT) {},
      []()
      {
           GlidArray<1>::ValueT out{};
           out[0] = 1U;
           return out;
      }))
  == std::array{ 1U });
static_assert(
  static_cast<std::array<
    std::uint32_t,
    1>>(
    UniqueValueArray(
      [](GlidArray<1>::ParameterT) {},
      []()
      {
           GlidArray<1>::ValueT out{};
           out[0] = 1U;
           return out;
      }))
  == std::array{ 1U });
template<
  typename T,
  std::invocable<T> F>
UniqueValue(
  T t,
  F f) -> UniqueValue<T>;
template<typename T, typename U, std::size_t Usz>
concept array_type_and_size_are_same
  = decay_same_as<std::ranges::range_value_t<T>, U>
    && std::ranges::size(T{}) == Usz;
template<typename T>
class WeakValue
{
   public:
     constexpr WeakValue() = default;
     constexpr WeakValue(const UniqueValue<T> &t)
       : m_value(t.m_value)
     {
     }
     template<
       typename U,
       std::size_t Usz>
          requires array_type_and_size_are_same<
            T,
            U,
            Usz>
     constexpr WeakValue(
       const UniqueValueArray<
         U,
         Usz> &t)
       : m_value(t.m_value)
     {
     }
     constexpr T operator*() const noexcept
     {
          return m_value;
     }
     constexpr operator T() const noexcept
     {
          return m_value;
     }

   private:
     T m_value = {};
};
using GlidCopy = WeakValue<std::uint32_t>;
static_assert(std::movable<GlidCopy> && std::copyable<GlidCopy>);
static_assert(
  GlidCopy{ Glid(
    1,
    [](std::uint32_t) {}) }
  == std::uint32_t{ 1 });
static_assert(
  static_cast<std::array<
    std::uint32_t,
    1U>>(
    WeakValue<std::array<
      std::uint32_t,
      1U>>{
      GlidArray<1U>(
        [](typename GlidArray<1U>::ParameterT) {},
        1U) })
  == std::array{ 1U });
}// namespace glengine
#endif// FIELD_MAP_EDITOR_UNIQUEVALUE_HPP