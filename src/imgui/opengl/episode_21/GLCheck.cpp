//
// Created by pcvii on 12/19/2021.
//

#include "GLCheck.hpp"
bool GLCheckError(std::string_view prefix, const std::source_location location)
{
  if (GLenum error = glGetError(); error != GL_NO_ERROR)
  {
    using namespace std::string_view_literals;
    fmt::print(
      stderr,
      "{} {}:{}:{} {}: 0x{:>04X}:{}\n",
      prefix,
      location.file_name(),
      location.line(),
      location.column(),
      location.function_name(),
      error,
      [&error]() {
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
    return true;
  }
  return false;
}
void GLClearError(const std::source_location location)
{
  while (GLCheckError("Cleared Error", location))
  {
    // do nothing;
  }
}


void GLGetError(const std::source_location location)
{
  while (GLCheckError("Error", location))
  {
    throw std::exception{};
  }
}

void BeginErrorCallBack()
{
  glEnable(GL_DEBUG_OUTPUT);
  // will SagFault here if GLEW isn't initiated.
  glDebugMessageCallback(
    [](
      GLenum                       source,
      GLenum                       type,
      [[maybe_unused]] GLuint      id,
      GLenum                       severity,
      [[maybe_unused]] GLsizei     length,
      const GLchar                *message,
      [[maybe_unused]] const void *userParam) {
      using namespace std::string_literals;
      static const std::unordered_map<GLenum, std::string> errorSourceMap{
        { GL_DEBUG_SOURCE_API, "SOURCE_API"s },
        { GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"s },
        { GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"s },
        { GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"s },
        { GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"s },
        { GL_DEBUG_SOURCE_OTHER, "OTHER"s }
      };
      static const std::unordered_map<GLenum, std::string> errorTypeMap{
        { GL_DEBUG_TYPE_ERROR, "ERROR"s },
        { GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"s },
        { GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"s },
        { GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"s },
        { GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"s },
        { GL_DEBUG_TYPE_OTHER, "OTHER"s },
        { GL_DEBUG_TYPE_MARKER, "MARKER"s }
      };
      static const std::unordered_map<GLenum, std::string> severityMap{
        { GL_DEBUG_SEVERITY_HIGH, "HIGH"s },
        { GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM"s },
        { GL_DEBUG_SEVERITY_LOW, "LOW"s },
        { GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"s }
      };
      if (GL_DEBUG_SEVERITY_NOTIFICATION == severity)
      {
        return;
      }
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
      if (GL_DEBUG_SEVERITY_HIGH == severity)
        throw std::exception{};
    },
    0);
}
void EndErrorCallback()
{
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
