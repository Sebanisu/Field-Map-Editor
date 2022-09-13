//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_BATCHRENDERER_HPP
#define FIELD_MAP_EDITOR_BATCHRENDERER_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"
#include "SubTexture.hpp"

#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"
namespace glengine
{

class BatchRenderer
{
public:
  BatchRenderer();
  BatchRenderer(
    std::size_t quad_count,
    Shader      shader = { std::filesystem::current_path() / "res" / "shader"
                           / "basic3.shader" });

  void                      on_update(float) const;
  void                      on_render() const;
  void                      on_im_gui_update() const;
  void                      on_event(const event::Item &e) const;

  [[nodiscard]] std::size_t quad_count() const noexcept;
  [[nodiscard]] std::size_t vert_count() const noexcept;
  [[nodiscard]] [[maybe_unused]] std::size_t index_count() const noexcept;
  [[nodiscard]] static const std::int32_t   &max_texture_image_units();
  void                                       clear() const;
  void                                       draw_quad(
                                          const Texture &texture,
                                          glm::vec3      offset,
                                          glm::vec2      size = glm::vec2{ 1.F }) const;
  void draw_quad(
    const SubTexture &texture,
    glm::vec3         offset,
    glm::vec2         size,
    int               id) const;
  void draw_quad(
    const SubTexture &texture,
    glm::vec3         offset,
    glm::vec2         size = glm::vec2{ 1.F }) const;
  void draw_quad(
    glm::vec3         offset,
    glm::vec4         color,
    const SubTexture &texture,
    const float       tiling_factor = 1.F,
    glm::vec2         size          = glm::vec2{ 1.F },
    int               id            = -1) const;
  [[maybe_unused]] void   draw_quad(glm::vec3 offset, glm::vec4 color) const;
  void                    draw(Quad quad) const;
  void                    draw() const;

  const glengine::Shader &shader() const;
  [[maybe_unused]] const std::vector<std::uint32_t> &texture_slots() const;
  void                                               bind() const;
  static void                                        unbind();

private:
  void                        flush_vertices() const;
  void                        draw_vertices() const;
  void                        bind_textures() const;
  std::size_t                 m_quad_count    = { 100U };
  VertexBufferDynamic         m_vertex_buffer = { quad_count() };
  IndexBufferDynamic          m_index_buffer  = { quad_count() };
  mutable std::vector<Vertex> m_vertices      = { [this]() {
    std::vector<Vertex>       r{};
    r.reserve(vert_count());
    return r;
  }() };
  mutable IndexBufferDynamicSize     index_buffer_size       = {};
  glengine::Shader                   m_shader                = {};
  VertexArray                        m_vertex_array          = {};
  mutable std::vector<std::uint32_t> m_texture_slots         = {};
  mutable std::vector<std::int32_t>  m_uniform_texture_slots = {};
  Texture m_blank = { (std::numeric_limits<std::uint32_t>::max)() };
};
static_assert(Renderable<BatchRenderer>);
static_assert(Bindable<BatchRenderer>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_BATCHRENDERER_HPP
