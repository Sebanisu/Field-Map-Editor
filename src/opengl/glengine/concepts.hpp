//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_CONCEPTS_HPP
#define FIELD_MAP_EDITOR_CONCEPTS_HPP
namespace glengine
{
template<typename T, typename U>
concept decay_same_as = std::is_same_v < std::remove_cvref_t<T>,
std::remove_cvref_t < U >> ;
template<typename T>
concept Void = std::is_void_v<T>;
template<typename T>
concept Bindable =
  std::default_initializable<T> && std::movable<T> && requires(const T t) {
                                                        {
                                                          t.bind()
                                                          } -> Void;
                                                        {
                                                          T::unbind()
                                                          } -> Void;
                                                      };
template<typename T>
concept SizedBindable = Bindable<T> && requires(const T t) {
                                         {
                                           t.size()
                                           } -> std::integral;
                                       };

template<typename T, typename... Ts>
  requires(std::same_as<
             std::ranges::range_value_t<T>,
             std::ranges::range_value_t<Ts>>,
           ...)
          || (sizeof...(Ts) == 0U)
struct all_range_value : std::true_type
{
  using type = std::ranges::range_value_t<T>;
  // static all_range_value value = {};
};
template<typename... Ts>
using all_range_value_t = typename all_range_value<Ts...>::type;
template<typename... Ts>
static constexpr auto all_range_value_v = all_range_value<Ts...>::value;
template<typename T, typename... Ts>
  requires(std::same_as<T, Ts>, ...) || (sizeof...(Ts) == 0U)
struct all_same_as : std::true_type
{
  using type = T;
};
template<typename... Ts>
using all_same_as_t = typename all_same_as<Ts...>::type;
template<typename... Ts>
static constexpr auto all_same_as_v = all_same_as<Ts...>::value;
template<typename T, typename...>
struct first : std::true_type
{
  using type = T;
};
template<typename... Ts>
using first_t = typename first<Ts...>::type;
}// namespace glengine
#endif// FIELD_MAP_EDITOR_CONCEPTS_HPP