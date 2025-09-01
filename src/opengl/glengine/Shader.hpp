//
// Created by pcvii on 11/22/2021.
//

#ifndef FIELD_MAP_EDITOR_SHADER_HPP
#define FIELD_MAP_EDITOR_SHADER_HPP
#include "GLCheck.hpp"
#include "Renderer.hpp"
#include "ScopeGuard.hpp"
#include "UniqueValue.hpp"
#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace glengine
{
struct [[nodiscard]] Shader
{
   private:
     struct ShaderProgramSource
     {
          std::string vertex_shader{};
          std::string fragment_shader{};
     };
     std::filesystem::path                                      m_file_path{};
     Glid                                                       m_renderer_id{};
     // cache for uniforms
     mutable std::unordered_map<std::string_view, std::int32_t> m_cache{};
     [[nodiscard]] static  ShaderProgramSource                   parse_shader(const std::filesystem::path &path);
     [[nodiscard]] static  std::uint32_t                         compile_shader(const std::uint32_t type, const std::string_view source);
     [[nodiscard]] static  std::uint32_t                         create_shader(const std::string_view, const std::string_view);
     [[nodiscard]] std::int32_t                                 get_uniform_location(std::string_view name) const;

   public:
     Shader() = default;
     Shader(std::filesystem::path file_path)
       : m_file_path(std::move(file_path))
       , m_renderer_id(Glid{ create(m_file_path), destroy })
     {
     }

     static GLuint             create(const std::filesystem::path &path);
     static void               destroy(const GLuint id);

     void                      bind() const;
     static void               unbind();
     [[nodiscard]] static auto backup()
     {
          GLint program_binding{ 0 };// save original
          GlCall{}(glGetIntegerv, GL_CURRENT_PROGRAM, &program_binding);
          return ScopeGuard{ [=]() {
               GlCall{}(glUseProgram, program_binding);
          } };// restore original shader. this might not be doing anything.
     }

     // Set Uniforms
     void set_uniform(std::string_view name, glm::vec1 v) const;
     void set_uniform(std::string_view name, glm::vec2 v) const;
     void set_uniform(std::string_view name, glm::vec3 v) const;
     void set_uniform(std::string_view name, glm::vec4 v) const;
     void set_uniform(std::string_view name, const glm::mat4 &matrix) const;
     template<typename... T>
          requires((sizeof...(T) >= 1U) && (sizeof...(T) <= 4U))
                  && ((std::floating_point<T> && ...) || (std::unsigned_integral<T> && ...) || (std::signed_integral<T> && ...))
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
};
static_assert(Bindable<Shader>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_SHADER_HPP
