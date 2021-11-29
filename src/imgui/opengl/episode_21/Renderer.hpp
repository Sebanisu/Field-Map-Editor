//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_RENDERER_HPP
#define MYPROJECT_RENDERER_HPP
#include "concepts.hpp"
#include <concepts>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <optional>
#include <source_location>
#include <utility>
void
  BeginErrorCallBack();
void
  EndErrorCallback();
void
  GLClearError(
    const std::source_location location = std::source_location::current());

bool
  GLCheckError(
    const std::source_location location = std::source_location::current());

void
  GLGetError(
    const std::source_location location = std::source_location::current());


template<typename T>
struct optional_holder
{
  T return_value{};
};
template<>
struct optional_holder<void>
{
  std::nullopt_t return_value{ std::nullopt };
};

template<typename FuncT, typename... ArgsT>
requires std::invocable<FuncT, ArgsT...>
struct GLCall
{
public:
  using return_value_type = std::decay_t<std::invoke_result_t<FuncT, ArgsT...>>;
  GLCall(
    FuncT &&func,
    ArgsT &&...args,
    std::source_location location = std::source_location::current())
  {
    GLClearError(location);
    if constexpr (!Void<return_value_type>)
    {
      holder.return_value =
        std::invoke(std::forward<FuncT>(func), std::forward<ArgsT>(args)...);
    }
    else
    {
      std::invoke(std::forward<FuncT>(func), std::forward<ArgsT>(args)...);
    }
    GLGetError(std::move(location));
  }

  [[nodiscard]] auto
    operator()()
    && requires(!Void<return_value_type>)
  {
    return std::move(holder.return_value);
  }
  [[nodiscard]] auto
    operator()()
    & requires(!Void<return_value_type>)
  {
    return holder.return_value;
  }

private : optional_holder<return_value_type> holder{};
};
template<typename... Ts>
GLCall(Ts &&...) -> GLCall<Ts...>;

class Renderer
{
public:
  void
    Clear() const
  {
    GLCall{ glClearColor, 0.F, 0.F, 0.F, 0.F };
    GLCall{ glClear, GL_COLOR_BUFFER_BIT };
  }
  template<Bindable... Ts>
  static void
    Draw(const Ts &...ts)
  {
    ((void)ts.Bind(), ...);

    size_t size = (
      [](const auto &bindable)
        -> size_t
             {
               bindable.Bind();
               if constexpr (SizedBindable<std::decay_t<decltype(bindable)>>)
               {
                 return bindable.size();
               }
               else
               {
                 return 0U;
               }
             }(ts)
             + ...);

    if (size != 0)
    {
      GLCall{ glDrawElements,
              GL_TRIANGLES,
              static_cast<std::int32_t>(size),
              GL_UNSIGNED_INT,
              nullptr };
    }
  }
};
#endif// MYPROJECT_RENDERER_HPP
