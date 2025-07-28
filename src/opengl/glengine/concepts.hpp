//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_CONCEPTS_HPP
#define FIELD_MAP_EDITOR_CONCEPTS_HPP
#include <concepts>
#include <ranges>
#include <vector>
namespace glengine
{

template<typename T>
struct is_std_vector_impl : std::false_type
{
};

template<typename T, typename Alloc>
struct is_std_vector_impl<std::vector<T, Alloc>> : std::true_type
{
};

template<typename T>
static const constexpr bool is_std_vector_v = is_std_vector_impl<T>::value;

template<typename T>
concept is_std_vector = is_std_vector_v<T>;

template<typename T>
struct vector_elem_type
{
     using type = void;
};

template<typename T, typename Alloc>
struct vector_elem_type<std::vector<T, Alloc>>
{
     using type = std::remove_cvref_t<T>;
};

template<typename T>
using vector_elem_type_t = typename vector_elem_type<T>::type;

template<typename T>
concept is_enum = std::is_enum_v<T>;
template<typename T>
concept is_enum_or_integral = is_enum<T> || std::integral<T>;
template<typename T, typename U>
concept decay_same_as = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;
template<typename T>
concept Void = std::is_void_v<T>;
template<typename T>
concept Bindable = std::default_initializable<T> && std::movable<T> && requires(const T t) {
     { t.bind() } -> Void;
     { T::unbind() } -> Void;
};

template<typename T>
concept HasInstanceBackup = requires(T t) {
     { t.backup() };
};
template<typename T>
concept HasStaticBackup = requires {
     { T::backup() };
};
template<typename T>
concept HasAnyBackup = HasInstanceBackup<T> || HasStaticBackup<T>;
template<typename T>
concept SizedBindable = Bindable<T> && requires(const T t) {
     { t.size() } -> std::integral;
};

template<typename T, typename... Ts>
     requires(std::same_as<std::ranges::range_value_t<T>, std::ranges::range_value_t<Ts>>, ...) || (sizeof...(Ts) == 0U)
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