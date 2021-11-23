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