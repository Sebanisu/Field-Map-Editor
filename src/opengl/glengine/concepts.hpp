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


}// namespace glengine
#endif// FIELD_MAP_EDITOR_CONCEPTS_HPP