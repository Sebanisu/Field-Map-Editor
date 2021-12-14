//
// Created by pcvii on 11/21/2021.
//

#ifndef MYPROJECT_RENDERER_HPP
#define MYPROJECT_RENDERER_HPP

#include "IndexType.hpp"


void BeginErrorCallBack();
void EndErrorCallback();
void GLClearError(
  const std::source_location location = std::source_location::current());

bool GLCheckError(
  const std::source_location location = std::source_location::current());

void GLGetError(
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

struct GLCall
{
public:
  GLCall(std::source_location location = std::source_location::current())
    : m_source_location(std::move(location))
  {
  }
  template<typename FuncT, typename... ArgsT>
  requires std::invocable<FuncT, ArgsT...>
  [[nodiscard]] auto operator()(FuncT &&func, ArgsT &&...args) &&
  {
    using return_value_type =
      std::decay_t<std::invoke_result_t<FuncT, ArgsT...>>;
    GLClearError(m_source_location);
    if constexpr (!Void<return_value_type>)
    {
      return_value_type return_value =
        std::invoke(std::forward<FuncT>(func), std::forward<ArgsT>(args)...);
      GLGetError(m_source_location);
      return return_value;
    }
    else
    {
      std::invoke(std::forward<FuncT>(func), std::forward<ArgsT>(args)...);
      GLGetError(m_source_location);
    }
  }

private:
  std::source_location m_source_location = {};
};

class Renderer
{
private:
  mutable glm::vec4 m_clear_color{};

public:
  void ClearColor(glm::vec4 color) const
  {
    m_clear_color = std::move(color);
  }
  void
    ClearColor(const float r, const float g, const float b, const float a) const
  {
    ClearColor({ r, g, b, a });
  }
  void Clear() const
  {
    GLCall{}(
      glClearColor,
      m_clear_color.r,
      m_clear_color.g,
      m_clear_color.b,
      m_clear_color.a);
    GLCall{}(glClear, GL_COLOR_BUFFER_BIT);
  }
  template<Bindable... Ts>
  static void Draw(const Ts &...ts)
  {
    ((void)ts.Bind(), ...);

    size_t size = ([](const auto &bindable) -> size_t {
      bindable.Bind();
      if constexpr (SizedBindable<std::decay_t<decltype(bindable)>>)
      {
        return bindable.size();
      }
      else
      {
        return 0U;
      }
    }(ts) + ...);
    auto   type = ([](const auto &typed) -> IndexType {
      typed.Bind();
      if constexpr (has_Type_for_IndexType<std::decay_t<decltype(typed)>>)
      {
        return typed.Type();
      }
      else
      {
        return IndexType::none;
      }
    }(ts) + ...);

    assert(type != IndexType::none);
    if (size != 0)
    {
      GLCall{}(
        glDrawElements,
        GL_TRIANGLES,
        static_cast<std::int32_t>(size),
        +type,
        nullptr);
    }
  }
};
#endif// MYPROJECT_RENDERER_HPP
