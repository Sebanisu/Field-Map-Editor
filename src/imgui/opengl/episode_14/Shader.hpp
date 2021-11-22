//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_SHADER_HPP
#define MYPROJECT_SHADER_HPP
#include "Renderer.hpp"
#include <cassert>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
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
  void
    SetUniform(std::string_view name, float f0, float f1, float f2, float f3)
      const;

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
