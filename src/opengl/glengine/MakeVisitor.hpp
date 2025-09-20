//
// Created by pcvii on 12/7/2021.
//

#ifndef FIELD_MAP_EDITOR_MAKEVISITOR_HPP
#define FIELD_MAP_EDITOR_MAKEVISITOR_HPP
namespace glengine
{
template<typename... B>
[[nodiscard]] inline constexpr auto MakeVisitor(B &&...b) noexcept
{
     struct visitor final : public std::remove_cvref_t<B>...
     {
          using B::operator()...;
     };
     return visitor{ std::forward<B>(b)... };
}
}// namespace glengine
#endif// FIELD_MAP_EDITOR_MAKEVISITOR_HPP
