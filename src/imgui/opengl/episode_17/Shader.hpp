//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_SHADER_HPP
#define MYPROJECT_SHADER_HPP
#include "Renderer.hpp"
#include <cassert>
#include <concepts>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <string_view>
#include <unordered_map>
class Shader
{
private:
  std::uint32_t                                              m_renderer_id{};
  std::filesystem::path                                      m_file_path{};
  // cache for uniforms
  mutable std::unordered_map<std::string_view, std::int32_t> m_cache{};
  Shader() = default;

public:
  Shader(std::filesystem::path file_path);
  ~Shader();

  Shader(const Shader &) = delete;
  Shader &
    operator=(const Shader &) = delete;

  Shader(Shader &&other);
  Shader &
    operator=(Shader &&other);

  void
    swap(Shader &first, Shader &second);

  void
    Bind() const;
  void
    UnBind() const;

  // Set Uniforms
  template<typename... T>
  requires((sizeof...(T) >= 1U) && (sizeof...(T) <= 4U))
    && ((std::floating_point<T> && ...) || (std::unsigned_integral<T> && ...) || (std::signed_integral<T> && ...)) void SetUniform(
      std::string_view name,
      T... v) const
  {
    const auto perform = [&]<typename NT>(auto &&fun)
    {
      GLCall{ std::forward<decltype(fun)>(fun),
              get_uniform_location(name),
              static_cast<NT>(v)... };
    };
    if constexpr ((std::floating_point<T> && ...))
    {
      if constexpr (sizeof...(T) == 1U)
      {
        perform.template operator()<float>(glUniform1f);
      }
      else if constexpr (sizeof...(T) == 2U)
      {
        perform.template operator()<float>(glUniform2f);
      }
      else if constexpr (sizeof...(T) == 3U)
      {
        perform.template operator()<float>(glUniform3f);
      }
      else if constexpr (sizeof...(T) == 4U)
      {
        perform.template operator()<float>(glUniform4f);
      }
    }
    else if constexpr ((std::unsigned_integral<T> && ...))
    {
      if constexpr (sizeof...(T) == 1U)
      {
        perform.template operator()<std::uint32_t>(glUniform1ui);
      }
      else if constexpr (sizeof...(T) == 2U)
      {
        perform.template operator()<std::uint32_t>(glUniform2ui);
      }
      else if constexpr (sizeof...(T) == 3U)
      {
        perform.template operator()<std::uint32_t>(glUniform3ui);
      }
      else if constexpr (sizeof...(T) == 4U)
      {
        perform.template operator()<std::uint32_t>(glUniform4ui);
      }
    }
    else if constexpr ((std::signed_integral<T> && ...))
    {
      if constexpr (sizeof...(T) == 1U)
      {
        perform.template operator()<std::int32_t>(glUniform1i);
      }
      else if constexpr (sizeof...(T) == 2U)
      {
        perform.template operator()<std::int32_t>(glUniform2i);
      }
      else if constexpr (sizeof...(T) == 3U)
      {
        perform.template operator()<std::int32_t>(glUniform3i);
      }
      else if constexpr (sizeof...(T) == 4U)
      {
        perform.template operator()<std::int32_t>(glUniform4i);
      }
    }
  }
  void
    SetUniform(std::string_view name, const glm::mat4 &matrix) const
  {
    GLCall{ glUniformMatrix4fv, get_uniform_location(name), 1, GLboolean{GL_FALSE}, &matrix[0][0] };
  }

private:
  struct ShaderProgramSource
  {
    std::string vertex_shader{};
    std::string fragment_shader{};
  };
  [[nodiscard]] ShaderProgramSource
    ParseShader();
  [[nodiscard]] std::uint32_t
    CompileShader(const std::uint32_t type, const std::string_view source);
  std::uint32_t
    CreateShader(
      const std::string_view vertexShader,
      const std::string_view fragmentShader);
  std::int32_t
    get_uniform_location(std::string_view name) const;
};
#endif// MYPROJECT_SHADER_HPP