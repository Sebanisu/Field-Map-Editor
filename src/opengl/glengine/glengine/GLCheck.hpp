//
// Created by pcvii on 12/19/2021.
//

#ifndef FIELD_MAP_EDITOR_GLCHECK_HPP
#define FIELD_MAP_EDITOR_GLCHECK_HPP
#include "concepts.hpp"
#include <concepts>
#include <GL/glew.h>
#include <source_location>
#include <spdlog/spdlog.h>
namespace glengine
{
void                  BeginErrorCallBack();
[[maybe_unused]] void EndErrorCallback();
void                  GlClearError(
                   const std::source_location location = std::source_location::current());

bool GlCheckError(
  const std::source_location location = std::source_location::current());

void GlGetError(
  const std::source_location location = std::source_location::current());

struct GlCall
{
   public:
     GlCall(std::source_location location = std::source_location::current())
       : m_source_location(location)
     {
     }
     template<
       typename FuncT,
       typename... ArgsT>
          requires std::invocable<
            FuncT,
            ArgsT...>
     [[nodiscard]] auto operator()(
       FuncT &&func,
       ArgsT &&...args) &&
     {
          using ReturnValueT
            = std::remove_cvref_t<std::invoke_result_t<FuncT, ArgsT...>>;
          GlClearError(m_source_location);
          if constexpr (!glengine::Void<ReturnValueT>)
          {
               ReturnValueT return_value = std::invoke(
                 std::forward<FuncT>(func), std::forward<ArgsT>(args)...);
               GlGetError(m_source_location);
               return return_value;
          }
          else
          {
               std::invoke(
                 std::forward<FuncT>(func), std::forward<ArgsT>(args)...);
               GlGetError(m_source_location);
          }
     }

   private:
     std::source_location m_source_location = {};
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_GLCHECK_HPP
