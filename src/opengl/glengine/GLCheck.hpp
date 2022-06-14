//
// Created by pcvii on 12/19/2021.
//

#ifndef FIELD_MAP_EDITOR_GLCHECK_HPP
#define FIELD_MAP_EDITOR_GLCHECK_HPP
#include "concepts.hpp"
#include <functional>
#include <source_location>
#include <utility>
void BeginErrorCallBack();
void EndErrorCallback();
void GLClearError(
  const std::source_location location = std::source_location::current());

bool GLCheckError(
  const std::source_location location = std::source_location::current());

void GLGetError(
  const std::source_location location = std::source_location::current());

struct GLCall
{
public:
  GLCall(std::source_location location = std::source_location::current())
    : m_source_location(std::move(location))
  {
  }
  template<typename FuncT, typename... ArgsT>
    requires std::invocable<FuncT, ArgsT...> [
      [nodiscard]] auto
    operator()(FuncT &&func, ArgsT &&...args) &&
  {
    using return_value_type =
      std::decay_t<std::invoke_result_t<FuncT, ArgsT...>>;
    GLClearError(m_source_location);
    if constexpr (!glengine::Void<return_value_type>)
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
#endif// FIELD_MAP_EDITOR_GLCHECK_HPP
