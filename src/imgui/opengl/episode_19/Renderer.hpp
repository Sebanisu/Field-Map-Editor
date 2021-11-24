//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_RENDERER_HPP
#define MYPROJECT_RENDERER_HPP
#include <concepts>
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <source_location>
#include <utility>
void
  BeginErrorCallBack();
void EndErrorCallback();
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
template<typename T>
concept Bindable = requires(T t)
{
  t.Bind();
  t.UnBind();
};
template<typename T>
concept SizedBindable = Bindable<T> && requires(T t)
{
  t.size();
};
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
