#include "CompShader.hpp"
namespace glengine
{
GLuint CompShader::create_compute_shader(const std::string &source)
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

GLuint CompShader::create_compute_program(const GLuint &shader)
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

void CompShader::bind() const
{
     GlCall{}(glUseProgram, m_program_id);
}

GlidCopy CompShader::id() const
{
     return m_program_id;
}

void CompShader::execute(GLuint width, GLuint height, GLbitfield bf) const
{
     // Dispatch compute shader
     // glDispatchCompute((width + 15) / 16, (height + 15) / 16, 1);
     // glDispatchCompute((width + 7) / 8, (height + 3) / 4, 1);
     glDispatchCompute((width + 7) / 8, (height + 7) / 8, 1);

     // Ensure compute shader writes are complete
     glMemoryBarrier(bf);
}


std::int32_t CompShader::get_uniform_location(std::string_view name) const
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
void CompShader::set_uniform(std::string_view name, glm::vec1 v) const
{
     set_uniform(name, v.x);
}
void CompShader::set_uniform(std::string_view name, glm::vec2 v) const
{
     set_uniform(name, v.x, v.y);
}
void CompShader::set_uniform(std::string_view name, glm::vec3 v) const
{
     set_uniform(name, v.x, v.y, v.z);
}
void CompShader::set_uniform(std::string_view name, glm::vec4 v) const
{
     set_uniform(name, v.r, v.g, v.b, v.a);
}
void CompShader::set_uniform(std::string_view name, const glm::mat4 &matrix) const
{
     const int32_t location = get_uniform_location(name);
     if (location == -1)
          return;
     GlCall{}(glUniformMatrix4fv, location, 1, GLboolean{ GL_FALSE }, glm::value_ptr(matrix));
}

GLuint CompShader::create(const std::filesystem::path &path)
{
     GLuint     temp_program_id = {};
     const auto pop_shader      = backup();
     try
     {
          const auto st = std::filesystem::status(path);
          if (!std::filesystem::is_regular_file(st))
          {
               spdlog::error("{}:{} - Shader path is invalid (not a regular file)\n\t\"{}\"", __FILE__, __LINE__, path.string());
               return {};
          }

          std::ifstream fs(path, std::ios::binary | std::ios::in);
          if (!fs.is_open())
          {
               spdlog::error("{}:{} - Failed to open shader\n\t\"{}\"", __FILE__, __LINE__, path.string());
               return {};
          }

          const auto  size = std::filesystem::file_size(path);
          std::string content(size, '\0');
          fs.read(content.data(), static_cast<std::streamsize>(size));

          if (!fs)
          {
               spdlog::error("{}:{} - Failed to read shader file\n\t\"{}\"", __FILE__, __LINE__, path.string());
               return {};
          }

          const GLuint shader_id = create_compute_shader(content);
          if (shader_id)
          {
               temp_program_id = create_compute_program(shader_id);
               GlCall{}(glDeleteShader, shader_id);
          }
          else
          {
               spdlog::error("{}:{} - Failed to create compute shader\n\t\"{}\"", __FILE__, __LINE__, path.string());
          }
     }
     catch (const std::filesystem::filesystem_error &e)
     {
          spdlog::error("{}:{} - Filesystem error: {}\n\t\"{}\"", __FILE__, __LINE__, e.what(), path.string());
     }
     return temp_program_id;
}

void CompShader::destroy(const GLuint id)
{
     GlCall{}(glDeleteProgram, id);
}
}// namespace glengine