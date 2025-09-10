//
// Created by pcvii on 11/21/2021.
//

#ifndef FIELD_MAP_EDITOR_RENDERER_HPP
#define FIELD_MAP_EDITOR_RENDERER_HPP
#include "GLCheck.hpp"
#include "IndexType.hpp"
#include "ScopeGuard.hpp"
#include <glm/glm.hpp>
#include <memory>
namespace glengine
{
#if __cpp_if_consteval
#define FME_NOT_CONSTEVAL !consteval// c++23
#else
#define FME_NOT_CONSTEVAL (!std::is_constant_evaluated())// c++20
#endif
class Clear_impl
{
   public:
     constexpr Clear_impl() = default;
     constexpr Clear_impl(glm::vec4 color)
       : m_color(std::move(color))
     {
     }
     constexpr void Color(glm::vec4 color)
     {
          m_color = std::move(color);
     }
     constexpr void operator()() const
     {
          if FME_NOT_CONSTEVAL
          {
               GlCall{}(glClearColor, m_color.r, m_color.g, m_color.b, m_color.a);
               GlCall{}(glClear, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
          }
     }

   private:
     glm::vec4 m_color = {};
};
#undef FME_NOT_CONSTEVAL
struct Renderer
{
     constexpr Renderer() = delete;


     template<typename T>
          requires(!SizedBindable<T>)
     static consteval std::size_t CountSize() noexcept
     {
          return {};
     }
     template<SizedBindable T>
     static consteval std::size_t CountSize() noexcept
     {
          return 1U;
     }
     template<typename... T>
     static consteval bool check_SizedBindable()
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
     template<Bindable... T>
     static void bind(const T &...bindables)
     {
          ((void)bindables.bind(), ...);
     }
     template<typename T>
          requires(!SizedBindable<T>)
     static std::size_t size(const T &) noexcept
     {
          return {};
     }
     template<SizedBindable T>
     static std::size_t size(const T &sized_bindable)
     {
          return sized_bindable.size();
     }
     template<typename... T>
          requires(sizeof...(T) > 1U && check_SizedBindable<T...>())
     static std::size_t size(const T &...bindables)
     {
          return (size<T>(bindables) + ...);
     }
     static inline constinit Clear_impl Clear = {};
     template<Bindable... Ts>
     static void Draw(const Ts &...ts)
     {
          std::vector<std::unique_ptr<AnyScopeGuard>> backups;
          backups.reserve(sizeof...(Ts));// Reserve space for efficiency

          // Call backup() for each argument and store in-place
          (
            [&]()
            {
                 using T = std::decay_t<decltype(ts)>;
                 if constexpr (HasInstanceBackup<T>)
                 {
                      backups.emplace_back(std::make_unique<std::decay_t<decltype(ts.backup())>>(ts.backup()));
                 }
                 else if constexpr (HasStaticBackup<T>)
                 {
                      backups.emplace_back(std::make_unique<std::decay_t<decltype(T::backup())>>(T::backup()));
                 }
            }(),
            ...);

          Renderer::bind<Ts...>(ts...);
          size_t size = Renderer::size<Ts...>(ts...);
          auto   type = glengine::Type<Ts...>(ts...);

          assert(
            type == glengine::IndexType::UnsignedInt || type == glengine::IndexType::UnsignedShort
            || type == glengine::IndexType::UnsignedByte);
          assert(size != 0);
          GlCall{}(glDrawElements, GL_TRIANGLES, static_cast<std::int32_t>(size), +type, nullptr);
     }
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_RENDERER_HPP
