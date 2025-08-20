#include "CompShader.hpp"


glengine::CompShader::CompShader(std::filesystem::path in_path)
  : m_path(std::move(in_path))
{
     const auto pop_shader = backup();
     try
     {
          const auto st = std::filesystem::status(m_path);
          if (!std::filesystem::is_regular_file(st))
          {
               spdlog::error("{}:{} - Shader path is invalid (not a regular file)\n\t\"{}\"", __FILE__, __LINE__, m_path.string());
               return;
          }

          std::ifstream fs(m_path, std::ios::binary | std::ios::in);
          if (!fs.is_open())
          {
               spdlog::error("{}:{} - Failed to open shader\n\t\"{}\"", __FILE__, __LINE__, m_path.string());
               return;
          }

          const auto  size = std::filesystem::file_size(m_path);
          std::string content(size, '\0');
          fs.read(content.data(), size);

          if (!fs)
          {
               spdlog::error("{}:{} - Failed to read shader file\n\t\"{}\"", __FILE__, __LINE__, m_path.string());
               return;
          }

          const GLuint shader_id = create_compute_shader(content);
          if (shader_id)
          {
               m_program_id = create_compute_program(shader_id);
               GlCall{}(glDeleteShader, shader_id);
          }
          else
          {
               spdlog::error("{}:{} - Failed to create compute shader\n\t\"{}\"", __FILE__, __LINE__, m_path.string());
          }
     }
     catch (const std::filesystem::filesystem_error &e)
     {
          spdlog::error("{}:{} - Filesystem error: {}\n\t\"{}\"", __FILE__, __LINE__, e.what(), m_path.string());
     }
}

glengine::CompShader::~CompShader() noexcept
{
     if (m_program_id == 0)
     {
          return;
     }
     GlCall{}(glDeleteProgram, m_program_id);
}


GLuint glengine::CompShader::create_compute_shader(const std::string &source)
{
     const char *ptr    = source.c_str();
     GLuint      shader = GlCall{}(glCreateShader, GL_COMPUTE_SHADER);
     GlCall{}(glShaderSource, shader, 1, &ptr, nullptr);
     GlCall{}(glCompileShader, shader);

     // Check compilation status
     GLint success;
     GlCall{}(glGetShaderiv, shader, GL_COMPILE_STATUS, &success);
     if (!success)
     {
          GLchar infoLog[512];
          GlCall{}(glGetShaderInfoLog, shader, 512, nullptr, infoLog);
          spdlog::error("Compute shader compilation failed: {}", infoLog);
          return 0;
     }
     return shader;
}

GLuint glengine::CompShader::create_compute_program(const GLuint &shader)
{
     GLuint program = GlCall{}(glCreateProgram);
     GlCall{}(glAttachShader, program, shader);
     GlCall{}(glLinkProgram, program);

     // Check linking status
     GLint success;
     GlCall{}(glGetProgramiv, program, GL_LINK_STATUS, &success);
     if (!success)
     {
          GLchar infoLog[512];
          GlCall{}(glGetProgramInfoLog, program, 512, nullptr, infoLog);
          spdlog::error("Compute program linking failed: {}", infoLog);
          return 0;
     }
     return program;
}

void glengine::CompShader::bind() const
{
     GlCall{}(glUseProgram, m_program_id);
}

GLuint glengine::CompShader::id() const
{
     return m_program_id;
}

void glengine::CompShader::execute(GLuint width, GLuint height) const
{
     // Dispatch compute shader
     // glDispatchCompute((width + 15) / 16, (height + 15) / 16, 1);
     // glDispatchCompute((width + 7) / 8, (height + 3) / 4, 1);
     glDispatchCompute((width + 7) / 8, (height + 7) / 8, 1);

     // Ensure compute shader writes are complete
     glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}


std::int32_t glengine::CompShader::get_uniform_location(std::string_view name) const
{
     if (m_cache.contains(name))
     {
          return m_cache.at(name);
     }

     std::int32_t location = GlCall{}(glGetUniformLocation, m_program_id, std::ranges::data(name));

     if (location == -1)
     {
          spdlog::warn("{}:{} uniform name {} doesn't exist, Invalid uniform location {}", __FILE__, __LINE__, name, location);
     }

     m_cache.emplace(std::move(name), location);

     return location;
}
void glengine::CompShader::set_uniform(std::string_view name, glm::vec1 v) const
{
     set_uniform(name, v.x);
}
void glengine::CompShader::set_uniform(std::string_view name, glm::vec2 v) const
{
     set_uniform(name, v.x, v.y);
}
void glengine::CompShader::set_uniform(std::string_view name, glm::vec3 v) const
{
     set_uniform(name, v.x, v.y, v.z);
}
void glengine::CompShader::set_uniform(std::string_view name, glm::vec4 v) const
{
     set_uniform(name, v.r, v.g, v.b, v.a);
}
void glengine::CompShader::set_uniform(std::string_view name, const glm::mat4 &matrix) const
{
     const int32_t location = get_uniform_location(name);
     if (location == -1)
          return;
     GlCall{}(glUniformMatrix4fv, location, 1, GLboolean{ GL_FALSE }, glm::value_ptr(matrix));
}