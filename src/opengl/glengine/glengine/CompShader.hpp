#ifndef BC882A74_AA62_484B_A1DD_0524A057427E
#define BC882A74_AA62_484B_A1DD_0524A057427E
#include "GLCheck.hpp"
#include "Renderer.hpp"
#include "UniqueValue.hpp"
#include <filesystem>
#include <fstream>
#include <glengine/ScopeGuard.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace glengine
{
struct CompShader
{
   private:
     std::filesystem::path m_path{};
     Glid                  m_program_id{};

     static GLuint         create_compute_shader(const std::string &source);
     static GLuint         create_compute_program(const GLuint &shader);

     // cache for uniforms
     mutable std::unordered_map<std::string_view, std::int32_t> m_cache{};

   public:
     CompShader() = default;
     CompShader(std::filesystem::path in_path)
       : m_path(std::move(in_path))
       , m_program_id(
           Glid{ create(m_path),
                 destroy })
     {
     }
     void     bind() const;
     GlidCopy id() const;
     void     execute(
       GLuint,
       GLuint,
       GLbitfield) const;

     [[nodiscard]] static auto backup()
     {
          GLint program_binding{ 0 };// save original
          GlCall{}(glGetIntegerv, GL_CURRENT_PROGRAM, &program_binding);
          return ScopeGuard{
               [=]() { GlCall{}(glUseProgram, program_binding); }
          };// restore original shader. this might not be doing anything.
     }

     // Set Uniforms
     [[nodiscard]] std::int32_t
          get_uniform_location(std::string_view name) const;
     void set_uniform(
       std::string_view name,
       glm::vec1        v) const;
     void set_uniform(
       std::string_view name,
       glm::vec2        v) const;
     void set_uniform(
       std::string_view name,
       glm::vec3        v) const;
     void set_uniform(
       std::string_view name,
       glm::vec4        v) const;
     void set_uniform(
       std::string_view name,
       const glm::mat4 &matrix) const;
     template<typename... T>
          requires((sizeof...(T) >= 1U) && (sizeof...(T) <= 4U))
                  && ((std::floating_point<T> && ...) || (std::unsigned_integral<T> && ...) || (std::signed_integral<T> && ...))
     void set_uniform(
       std::string_view name,
       T... v) const
     {
          const int32_t location = get_uniform_location(name);
          if (location == -1)
               return;
          const auto perform = [&]<typename NT>(auto &&fun)
          {
               GlCall{}(
                 std::forward<decltype(fun)>(fun),
                 location,
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

     template<std::ranges::contiguous_range T>
          requires(
            (decay_same_as<
              std::ranges::range_value_t<T>,
              float>)
            || (decay_same_as<
                std::ranges::range_value_t<T>,
                std::uint32_t>)
            || (decay_same_as<
                std::ranges::range_value_t<T>,
                std::int32_t>))
     void set_uniform(
       std::string_view name,
       T                v) const
     {
          const int32_t location = get_uniform_location(name);
          if (location == -1)
               return;
          const auto perform = [&](auto &&fun)
          {
               GlCall{}(
                 std::forward<decltype(fun)>(fun),
                 location,
                 static_cast<GLsizei>(std::ranges::ssize(v)),
                 std::ranges::data(v));
          };

          assert(!std::ranges::empty(v));
          if constexpr (decay_same_as<std::ranges::range_value_t<T>, float>)
          {
               perform(glUniform1fv);
          }
          else if constexpr (decay_same_as<
                               std::ranges::range_value_t<T>,
                               std::uint32_t>)
          {
               perform(glUniform1uiv);
          }
          else if constexpr (decay_same_as<
                               std::ranges::range_value_t<T>,
                               std::int32_t>)
          {
               perform(glUniform1iv);
          }
     }

     static GLuint create(const std::filesystem::path &path);
     static void   destroy(const GLuint id);
};
}// namespace glengine
#endif /* BC882A74_AA62_484B_A1DD_0524A057427E */
