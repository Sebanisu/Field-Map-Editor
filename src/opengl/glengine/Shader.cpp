//
// Created by pcvii on 11/22/2021.
//
#include "Shader.hpp"
#include "Formatters.hpp"
#include <iostream>
namespace glengine
{

GLuint Shader::create(const std::filesystem::path &path)
{
     const auto          pop_backup = backup();
     ShaderProgramSource source     = parse_shader(path);
     return create_shader(source.vertex_shader, source.fragment_shader);
}

void Shader::destroy(const GLuint id)
{
     GlCall{}(glDeleteProgram, id);
     unbind();
}

void Shader::bind() const
{
     GlCall{}(glUseProgram, m_renderer_id);
}

void Shader::unbind()
{
     GlCall{}(glUseProgram, 0U);
}

Shader::ShaderProgramSource Shader::parse_shader(const std::filesystem::path &path)
{
     std::ifstream fs(path, std::ios::binary | std::ios::in);
     if (!fs.is_open())
     {
          spdlog::error("{}:{} - Failed to open shader \n\t\"{}\"", __FILE__, __LINE__, path.string());
          return {};
     }
     enum class ShaderType
     {
          none     = -1,
          vertex   = 0,
          fragment = 1,
     };
     std::stringstream ss[2U] = {};
     {
          std::string line{};
          ShaderType  mode = ShaderType::none;
          while (std::getline(fs, line))
          {
               if (line.find("#shader") != std::string::npos)
               {
                    if (line.find("vertex") != std::string::npos)
                    {
                         // set mode to vertex.
                         mode = ShaderType::vertex;
                    }
                    else if (line.find("fragment") != std::string::npos)
                    {
                         // set mode to fragment.
                         mode = ShaderType::fragment;
                    }
               }
               else
               {
                    if (std::string::size_type pos = line.find("#"); pos != std::string::npos)
                    {
                         line.erase(0, pos);
                    }
                    ss[static_cast<std::size_t>(mode)] << line << '\n';
               }
          }
     }

     return { ss[0].str(), ss[1].str() };
}

std::uint32_t Shader::compile_shader(
  const std::uint32_t    type,
  const std::string_view source)
{
     using namespace std::string_view_literals;
     const std::uint32_t id  = GlCall{}(glCreateShader, type);
     const char         *src = std::data(source);
     GlCall{}(glShaderSource, id, 1, &src, nullptr);
     GlCall{}(glCompileShader, id);

     int result{};
     GlCall{}(glGetShaderiv, id, GL_COMPILE_STATUS, &result);
     if (result == GL_FALSE)
     {
          int length{};
          GlCall{}(glGetShaderiv, id, GL_INFO_LOG_LENGTH, &length);
          std::string message(static_cast<std::string::size_type>(length), '\0');
          GlCall{}(glGetShaderInfoLog, id, length, &length, std::data(message));
          spdlog::error(
            "Error {}:{} - Failed to compile shader {} - {}",
            __FILE__,
            __LINE__,
            (type == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER"sv : "GL_FRAGMENT_SHADER"sv),
            message);
          GlCall{}(glDeleteShader, id);
          spdlog::error("{}", std::stacktrace::current());
          return 0U;
     }

     return id;
}
std::uint32_t Shader::create_shader(
  const std::string_view vertex_shader,
  const std::string_view fragment_shader)
{
     const std::uint32_t vs      = compile_shader(GL_VERTEX_SHADER, vertex_shader);
     const std::uint32_t fs      = compile_shader(GL_FRAGMENT_SHADER, fragment_shader);
     const std::uint32_t program = GlCall{}(glCreateProgram);
     GlCall{}(glAttachShader, program, vs);
     GlCall{}(glAttachShader, program, fs);
     GlCall{}(glLinkProgram, program);
     GlCall{}(glValidateProgram, program);
     GlCall{}(glDeleteShader, vs);
     GlCall{}(glDeleteShader, fs);
     return program;
}
std::int32_t Shader::get_uniform_location(std::string_view name) const
{
     if (m_cache.contains(name))
     {
          return m_cache.at(name);
     }

     std::int32_t location = GlCall{}(glGetUniformLocation, m_renderer_id, std::ranges::data(name));

     if (location == -1)
     {
          spdlog::warn("{}:{} uniform name {} doesn't exist, Invalid uniform location {}", __FILE__, __LINE__, name, location);
     }

     m_cache.emplace(std::move(name), location);

     return location;
}
void Shader::set_uniform(
  std::string_view name,
  glm::vec1        v) const
{
     set_uniform(name, v.x);
}
void Shader::set_uniform(
  std::string_view name,
  glm::vec2        v) const
{
     set_uniform(name, v.x, v.y);
}
void Shader::set_uniform(
  std::string_view name,
  glm::vec3        v) const
{
     set_uniform(name, v.x, v.y, v.z);
}
void Shader::set_uniform(
  std::string_view name,
  glm::vec4        v) const
{
     set_uniform(name, v.r, v.g, v.b, v.a);
}
void Shader::set_uniform(
  std::string_view name,
  const glm::mat4 &matrix) const
{
     const int32_t location = get_uniform_location(name);
     if (location == -1)
          return;
     GlCall{}(glUniformMatrix4fv, location, 1, GLboolean{ GL_FALSE }, glm::value_ptr(matrix));
}
}// namespace glengine