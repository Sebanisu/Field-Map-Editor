//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_RENDERER_HPP
#define MYPROJECT_RENDERER_HPP
#include <concepts>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <source_location>
#include <functional>
void
  GLClearError();

void
  GLGetError(
    const std::source_location location = std::source_location::current());

template<typename FuncT, typename... ArgsT>
requires std::invocable<FuncT, ArgsT...>
struct GLCall
{
  GLCall(
    FuncT &&func,
    ArgsT &&...args,
    std::source_location location = std::source_location::current())
  {
    GLClearError();
    if constexpr (!std::is_void_v<std::invoke_result_t<FuncT, ArgsT...>>)
    {
      r() =
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
    && requires(!std::is_void_v<std::invoke_result_t<FuncT, ArgsT...>>)
  {
    return std::move(r());
  }
  [[nodiscard]] auto
    operator()()
    & requires(!std::is_void_v<std::invoke_result_t<FuncT, ArgsT...>>)
  {
    return r();
  }
  [[nodiscard]] static auto &
    r() requires(!std::is_void_v<std::invoke_result_t<FuncT, ArgsT...>>)
  {
    static std::invoke_result_t<FuncT, ArgsT...> temp;
    return temp;
  }

private:
};
template<typename... Ts>
GLCall(Ts &&...) -> GLCall<Ts...>;
#endif// MYPROJECT_RENDERER_HPP
