//
// Created by pcvii on 11/22/2021.
//
#include "Shader.hpp"
#include <fmt/format.h>
#include <fstream>
namespace glengine
{
Shader::Shader(std::filesystem::path file_path)
  : m_file_path(std::move(file_path))
{
  ShaderProgramSource source = ParseShader();
  m_renderer_id = CreateShader(source.vertex_shader, source.fragment_shader);
}
Shader::~Shader()
{
  GLCall{}(glDeleteProgram, m_renderer_id);
  UnBind();
}
Shader::Shader(Shader &&other) noexcept
  : Shader()
{
  swap(*this, other);
}
Shader &Shader::operator=(Shader &&other) noexcept
{
  swap(*this, other);
  return *this;
}
void swap(Shader &first, Shader &second) noexcept// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
  swap(first.m_file_path, second.m_file_path);
  swap(first.m_cache, second.m_cache);
}
void Shader::Bind() const
{
  GLCall{}(glUseProgram, m_renderer_id);
}
void Shader::UnBind()
{
  GLCall{}(glUseProgram, 0U);
}
Shader::ShaderProgramSource Shader::ParseShader()
{
  std::ifstream fs(m_file_path, std::ios::binary | std::ios::in);
  if (!fs.is_open())
  {
    fmt::print(
      stderr,
      "Error {}:{} - Failed to open shader \n\t\"{}\"\n",
      __FILE__,
      __LINE__,
      m_file_path.string());
    return {};
  }
  enum class ShaderType
  {
    None     = -1,
    Vertex   = 0,
    Fragment = 1,
  };
  std::stringstream ss[2U] = {};
  {
    std::string line{};
    ShaderType  mode = ShaderType::None;
    while (std::getline(fs, line))
    {
      if (line.find("#shader") != std::string::npos)
      {
        if (line.find("vertex") != std::string::npos)
        {
          // set mode to vertex.
          mode = ShaderType::Vertex;
        }
        else if (line.find("fragment") != std::string::npos)
        {
          // set mode to fragment.
          mode = ShaderType::Fragment;
        }
      }
      else
      {
        if (std::string::size_type pos = line.find("#");
            pos != std::string::npos)
        {
          line.erase(0, pos);
        }
        ss[static_cast<std::size_t>(mode)] << line << '\n';
      }
    }
  }

  return { ss[0].str(), ss[1].str() };
}
std::uint32_t
  Shader::CompileShader(const std::uint32_t type, const std::string_view source)
{
  using namespace std::string_view_literals;
  const std::uint32_t id  = GLCall{}(glCreateShader, type);
  const char         *src = std::data(source);
  GLCall{}(glShaderSource, id, 1, &src, nullptr);
  GLCall{}(glCompileShader, id);

  int result{};
  GLCall{}(glGetShaderiv, id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE)
  {
    int length{};
    GLCall{}(glGetShaderiv, id, GL_INFO_LOG_LENGTH, &length);
    std::string message(static_cast<std::string::size_type>(length), '\0');
    GLCall{}(glGetShaderInfoLog, id, length, &length, std::data(message));
    fmt::print(
      stderr,
      "Error {}:{} - Failed to compile shader {} - {}\n",
      __FILE__,
      __LINE__,
      (type == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER"sv
                                : "GL_FRAGMENT_SHADER"sv),
      message);
    GLCall{}(glDeleteShader, id);
    return 0U;
  }

  return id;
}
std::uint32_t Shader::CreateShader(
  const std::string_view vertexShader,
  const std::string_view fragmentShader)
{
  const std::uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  const std::uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
  const std::uint32_t program = GLCall{}(glCreateProgram);
  GLCall{}(glAttachShader, program, vs);
  GLCall{}(glAttachShader, program, fs);
  GLCall{}(glLinkProgram, program);
  GLCall{}(glValidateProgram, program);
  GLCall{}(glDeleteShader, vs);
  GLCall{}(glDeleteShader, fs);
  return program;
}
std::int32_t Shader::get_uniform_location(std::string_view name) const
{
  if (m_cache.contains(name))
  {
    return m_cache.at(name);
  }

  std::int32_t location =
    GLCall{}(glGetUniformLocation, m_renderer_id, std::ranges::data(name));

  if (location == -1)
  {
    fmt::print(
      stderr,
      "Warning {}:{} uniform name {} doesn't exist, Invalid uniform location "
      "{}\n",
      __FILE__,
      __LINE__,
      name,
      location);
  }

  m_cache.emplace(std::move(name), location);

  return location;
}
}// namespace glengine