//
// Created by pcvii on 11/24/2021.
//

#ifndef MYPROJECT_CONCEPTS_HPP
#define MYPROJECT_CONCEPTS_HPP
namespace glengine
{
template<typename T, typename U>
concept decay_same_as = std::is_same_v<std::decay_t<T>, std::decay_t<U>>;
template<typename T>
concept Void = std::is_void_v<T>;
template<typename T>
concept Bindable =
  std::default_initializable<T> && std::movable<T> && requires(const T t)
{
  {
    t.Bind()
    } -> Void;
  {
    T::UnBind()
    } -> Void;
};
template<typename T>
concept SizedBindable = Bindable<T> && requires(const T t)
{
  {
    t.size()
    } -> std::integral;
};
template<Bindable... T>
inline void Bind(const T &...bindables)
{
  ((void)bindables.Bind(), ...);
}
template<typename T>
// clang-format off
requires (!SizedBindable<T>)
  // clang-format on
  inline std::size_t size(const T &) noexcept
{
  return {};
}
template<SizedBindable T>
inline std::size_t size(const T &sized_bindable)
{
  return sized_bindable.size();
}

template<typename T>
// clang-format off
requires (!SizedBindable<T>)
  // clang-format on
  inline consteval std::size_t CountSize() noexcept
{
  return {};
}
template<SizedBindable T>
inline consteval std::size_t CountSize() noexcept
{
  return 1U;
}
template<typename... T>
inline consteval bool check_SizedBindable()
{
  if constexpr (sizeof...(T) != 0U)
  {
    return (CountSize<T>() + ...) == std::size_t{ 1U };
  }
  else
  {
    return false;
  }
}
template<typename... T>
// clang-format off
  requires(sizeof...(T) > 1U && check_SizedBindable<T...>())
  // clang-format on
  inline std::size_t size(const T &...bindables)
{
  return (glengine::size<T>(bindables) + ...);
}
}// namespace glengine
#endif// MYPROJECT_CONCEPTS_HPP