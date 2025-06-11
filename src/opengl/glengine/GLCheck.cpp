//
// Created by pcvii on 12/19/2021.
//

#include "GLCheck.hpp"
bool GlCheckError(std::string_view prefix, const std::source_location location)
{
     if (GLenum error = glGetError(); error != GL_NO_ERROR)
     {
          using namespace std::string_view_literals;
          spdlog::error(
            "{} {}:{}:{} {}: 0x{:>04X}:{}",
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
void GlClearError(const std::source_location location)
{
     while (GlCheckError("Cleared Error", location))
     {
          // do nothing;
     }
}


void GlGetError(const std::source_location location)
{
     while (GlCheckError("Error", location))
     {
          assert(false);
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
         [[maybe_unused]] const void *user_param) {
            using namespace std::string_view_literals;
            static const std::unordered_map<GLenum, std::string_view> error_source_map{
                 { GL_DEBUG_SOURCE_API, "SOURCE_API"sv },
                 { GL_DEBUG_SOURCE_WINDOW_SYSTEM, "WINDOW_SYSTEM"sv },
                 { GL_DEBUG_SOURCE_SHADER_COMPILER, "SHADER_COMPILER"sv },
                 { GL_DEBUG_SOURCE_THIRD_PARTY, "THIRD_PARTY"sv },
                 { GL_DEBUG_SOURCE_APPLICATION, "APPLICATION"sv },
                 { GL_DEBUG_SOURCE_OTHER, "OTHER"sv }
            };
            static const std::unordered_map<GLenum, std::string_view> error_type_map{
                 { GL_DEBUG_TYPE_ERROR, "ERROR"sv },
                 { GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR, "DEPRECATED_BEHAVIOR"sv },
                 { GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR, "UNDEFINED_BEHAVIOR"sv },
                 { GL_DEBUG_TYPE_PORTABILITY, "PORTABILITY"sv },
                 { GL_DEBUG_TYPE_PERFORMANCE, "PERFORMANCE"sv },
                 { GL_DEBUG_TYPE_OTHER, "OTHER"sv },
                 { GL_DEBUG_TYPE_MARKER, "MARKER"sv }
            };
            static const std::unordered_map<GLenum, std::string_view> severity_map{ { GL_DEBUG_SEVERITY_HIGH, "HIGH"sv },
                                                                                    { GL_DEBUG_SEVERITY_MEDIUM, "MEDIUM"sv },
                                                                                    { GL_DEBUG_SEVERITY_LOW, "LOW"sv },
                                                                                    { GL_DEBUG_SEVERITY_NOTIFICATION, "NOTIFICATION"sv } };
            if (GL_DEBUG_SEVERITY_NOTIFICATION == severity)
            {
                 return;
            }
            const auto src = error_source_map.at(source);
            const auto tp  = error_type_map.at(type);
            const auto sv  = severity_map.at(severity);
            const auto error_message =
              fmt::format("GL CALLBACK: {0:s} type = {1:s}, severity = {2:s}, message = {3:s}", src, tp, sv, message);
            switch (severity)
            {
                 case GL_DEBUG_SEVERITY_NOTIFICATION: {
                      spdlog::info(error_message);
                      break;
                 }
                 case GL_DEBUG_SEVERITY_LOW: {
                      spdlog::warn(error_message);
                      break;
                 }
                 case GL_DEBUG_SEVERITY_MEDIUM: {
                      spdlog::error(error_message);
                      break;
                 }
                 case GL_DEBUG_SEVERITY_HIGH:
                 default: {
                      spdlog::critical(error_message);
                      break;
                 }
            }

            // if (GL_DEBUG_SEVERITY_HIGH == severity)
            // throw std::exception{};
       },
       0);
}
[[maybe_unused]] void EndErrorCallback()
{
     glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
