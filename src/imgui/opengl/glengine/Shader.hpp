//
// Created by pcvii on 11/22/2021.
//

#ifndef MYPROJECT_SHADER_HPP
#define MYPROJECT_SHADER_HPP
#include "Renderer.hpp"
class Shader
{
private:
  std::uint32_t                                              m_renderer_id{};
  std::filesystem::path                                      m_file_path{};
  // cache for uniforms
  mutable std::unordered_map<std::string_view, std::int32_t> m_cache{};

public:
  Shader() = default;
  Shader(std::filesystem::path file_path);
  ~Shader();

  Shader(const Shader &) = delete;
  Shader &operator=(const Shader &) = delete;

  Shader(Shader &&other) noexcept;
  Shader     &operator=(Shader &&other) noexcept;

  friend void swap(Shader &first, Shader &second) noexcept;

  void        Bind() const;
  static void UnBind();

  // Set Uniforms
  template<typename... T>
  // clang-format off
  requires
    ((sizeof...(T) >= 1U)
      && (sizeof...(T) <= 4U))
    && ((std::floating_point<T> && ...)
      || (std::unsigned_integral<T> && ...)
      || (std::signed_integral<T> && ...))
    // clang-format on
    void SetUniform(std::string_view name, T... v) const
  {
    const auto perform = [&]<typename NT>(auto &&fun) {
      GLCall{}(
        std::forward<decltype(fun)>(fun),
        get_uniform_location(name),
        static_cast<NT>(v)...);
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
  // Set Uniforms
  template<std::ranges::contiguous_range T>
  requires(
    (decay_same_as<std::ranges::range_value_t<T>, float>)
    || (decay_same_as<std::ranges::range_value_t<T>, std::uint32_t>)
    || (decay_same_as<
        std::ranges::range_value_t<T>,
        std::int32_t>)) void SetUniform(std::string_view name, T v) const
  {
    const auto perform = [&]<typename NT>(auto &&fun) {
      GLCall{}(
        std::forward<decltype(fun)>(fun),
        get_uniform_location(name),
        static_cast<GLsizei>(std::ranges::ssize(v)),
        std::ranges::data(v));
    };

    assert(!std::ranges::empty(v));
    if constexpr (decay_same_as<std::ranges::range_value_t<T>, float>)
    {
      perform.template operator()<float>(glUniform1fv);
    }
    else if constexpr (decay_same_as<
                         std::ranges::range_value_t<T>,
                         std::uint32_t>)
    {
      perform.template operator()<float>(glUniform1uiv);
    }
    else if constexpr (decay_same_as<
                         std::ranges::range_value_t<T>,
                         std::int32_t>)
    {
      perform.template operator()<float>(glUniform1iv);
    }
  }
  void SetUniform(std::string_view name, const glm::mat4 &matrix) const
  {
    GLCall{}(
      glUniformMatrix4fv,
      get_uniform_location(name),
      1,
      GLboolean{ GL_FALSE },
      &matrix[0][0]);
  }

private:
  struct ShaderProgramSource
  {
    std::string vertex_shader{};
    std::string fragment_shader{};
  };
  [[nodiscard]] ShaderProgramSource ParseShader();
  [[nodiscard]] std::uint32_t
    CompileShader(const std::uint32_t type, const std::string_view source);
  std::uint32_t CreateShader(
    const std::string_view vertexShader,
    const std::string_view fragmentShader);
  std::int32_t get_uniform_location(std::string_view name) const;
};
static_assert(Bindable<Shader>);
#endif// MYPROJECT_SHADER_HPP
