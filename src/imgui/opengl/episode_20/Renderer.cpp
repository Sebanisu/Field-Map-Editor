//
// Created by pcvii on 11/21/2021.
//

#include "Renderer.hpp"
#include <fmt/format.h>
void
  GLClearError()
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR)
  {
    using namespace std::string_view_literals;
    fmt::print(
      stderr,
      "Cleared Error: 0x{:>04X}:{}\n",
      error,
      [&error]()
      {
        switch (error)
        {
        case GL_INVALID_ENUM:
          return "GL_INVALID_ENUM"sv;
        case GL_INVALID_VALUE:
          return "GL_INVALID_VALUE"sv;
        case GL_INVALID_OPERATION:
          return "GL_INVALID_OPERATION"sv;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          return "GL_INVALID_FRAMEBUFFER_OPERATION"sv;
        case GL_OUT_OF_MEMORY:
          return "GL_OUT_OF_MEMORY"sv;
        case GL_STACK_UNDERFLOW:
          return "GL_STACK_UNDERFLOW"sv;
        case GL_STACK_OVERFLOW:
          return "GL_STACK_OVERFLOW"sv;
        }
        return ""sv;
      }());

    //throw std::exception{};
  }
}
void
  GLGetError(const std::source_location location)
{
  GLenum error;
  while ((error = glGetError()) != GL_NO_ERROR)
  {
    using namespace std::string_view_literals;
    fmt::print(
      stderr,
      "Error {}({}:{}) {}: 0x{:>04X}:{}\n",
      location.file_name(),
      location.line(),
      location.column(),
      location.function_name(),
      error,
      [&error]()
      {
        switch (error)
        {
        case GL_INVALID_ENUM:
          return "GL_INVALID_ENUM"sv;
        case GL_INVALID_VALUE:
          return "GL_INVALID_VALUE"sv;
        case GL_INVALID_OPERATION:
          return "GL_INVALID_OPERATION"sv;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
          return "GL_INVALID_FRAMEBUFFER_OPERATION"sv;
        case GL_OUT_OF_MEMORY:
          return "GL_OUT_OF_MEMORY"sv;
        case GL_STACK_UNDERFLOW:
          return "GL_STACK_UNDERFLOW"sv;
        case GL_STACK_OVERFLOW:
          return "GL_STACK_OVERFLOW"sv;
        }
        return ""sv;
      }());
    throw std::exception{};
  }
}

void
  BeginErrorCallBack()
{
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(
    [](
      GLenum                       source,
      GLenum                       type,
      [[maybe_unused]] GLuint      id,
      GLenum                       severity,
      [[maybe_unused]] GLsizei     length,
      const GLchar                *message,
      [[maybe_unused]] const void *userParam)
    {
      static const std::unordered_map<GLenum, std::string> errorSourceMap{
        { GL_DEBUG_SOURCE_API, "SOURCE_API" },
        { GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM" },
        { GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER" },
        { GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY" },
        { GL_DEBUG_SOURCE_APPLICATION, "APPLICATION" },
        { GL_DEBUG_SOURCE_OTHER, "OTHER" }
      };
      static const std::unordered_map<GLenum, std::string> errorTypeMap{
        { GL_DEBUG_TYPE_ERROR, "ERROR" },
        { GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR" },
        { GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR" },
        { GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY" },
        { GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE" },
        { GL_DEBUG_TYPE_OTHER, "OTHER" },
        { GL_DEBUG_TYPE_MARKER, "MARKER" }
      };
      static const std::unordered_map<GLenum, std::string> severityMap{
        { GL_DEBUG_SEVERITY_HIGH, "HIGH" },
        { GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM" },
        { GL_DEBUG_SEVERITY_LOW, "LOW" },
        { GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION" }
      };
      std::string src = errorSourceMap.at(source);
      std::string tp  = errorTypeMap.at(type);
      std::string sv  = severityMap.at(severity);
      fmt::print(
        stderr,
        "GL CALLBACK: {0:s} type = {1:s}, severity = {2:s}, message = "
        "{3:s}\n",
        src,
        tp,
        sv,
        message);
      //            if (GL_DEBUG_SEVERITY_HIGH == severity)
      //              throw std::exception{};
    },
    0);
}
void EndErrorCallback()
{
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}