//
// Created by pcvii on 11/22/2021.
//

#ifndef FIELD_MAP_EDITOR_SHADER_HPP
#define FIELD_MAP_EDITOR_SHADER_HPP
#include "Renderer.hpp"
#include <filesystem>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
namespace glengine
{
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

     Shader(const Shader &)            = delete;
     Shader &operator=(const Shader &) = delete;

     Shader(Shader &&other) noexcept;
     Shader     &operator=(Shader &&other) noexcept;

     friend void swap(Shader &first, Shader &second) noexcept;

     void        bind() const;
     static void unbind();

     void        set_uniform(std::string_view name, glm::vec1 v) const;
     void        set_uniform(std::string_view name, glm::vec2 v) const;
     void        set_uniform(std::string_view name, glm::vec3 v) const;
     void        set_uniform(std::string_view name, glm::vec4 v) const;
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
     void set_uniform(std::string_view name, T... v) const
     {
          const int32_t location = get_uniform_location(name);
          if (location == -1)
               return;
          const auto perform = [&]<typename NT>(auto &&fun) {
               GlCall{}(std::forward<decltype(fun)>(fun), location, static_cast<NT>(v)...);
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
            (decay_same_as<std::ranges::range_value_t<T>, float>) || (decay_same_as<std::ranges::range_value_t<T>, std::uint32_t>)
            || (decay_same_as<std::ranges::range_value_t<T>, std::int32_t>))
     void set_uniform(std::string_view name, T v) const
     {
          const int32_t location = get_uniform_location(name);
          if (location == -1)
               return;
          const auto perform = [&](auto &&fun) {
               GlCall{}(std::forward<decltype(fun)>(fun), location, static_cast<GLsizei>(std::ranges::ssize(v)), std::ranges::data(v));
          };

          assert(!std::ranges::empty(v));
          if constexpr (decay_same_as<std::ranges::range_value_t<T>, float>)
          {
               perform(glUniform1fv);
          }
          else if constexpr (decay_same_as<std::ranges::range_value_t<T>, std::uint32_t>)
          {
               perform(glUniform1uiv);
          }
          else if constexpr (decay_same_as<std::ranges::range_value_t<T>, std::int32_t>)
          {
               perform(glUniform1iv);
          }
     }
     void set_uniform(std::string_view name, const glm::mat4 &matrix) const
     {
          const int32_t location = get_uniform_location(name);
          if (location == -1)
               return;
          GlCall{}(glUniformMatrix4fv, location, 1, GLboolean{ GL_FALSE }, glm::value_ptr(matrix));
     }

   private:
     struct ShaderProgramSource
     {
          std::string vertex_shader{};
          std::string fragment_shader{};
     };
     [[nodiscard]] ShaderProgramSource parse_shader();
     [[nodiscard]] std::uint32_t       compile_shader(const std::uint32_t type, const std::string_view source);
     std::uint32_t                     create_shader(const std::string_view, const std::string_view);
     std::int32_t                      get_uniform_location(std::string_view name) const;
};
static_assert(Bindable<Shader>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_SHADER_HPP
