//
// Created by pcvii on 11/30/2021.
//

#include "BatchRenderer.hpp"
namespace glengine
{
static_assert(Renderable<BatchRenderer>);
static std::uint32_t draw_count = 0U;
BatchRenderer::BatchRenderer()
  : BatchRenderer(100U)
{
}
BatchRenderer::BatchRenderer(std::size_t quad_count, Shader shader)
  : m_max_textures(max_texture_image_units())
  , m_quad_count(quad_count)
  , m_shader(std::move(shader))
{
     const auto pop_backup = m_vertex_array.backup();
     m_vertex_array.bind();
     m_vertex_array.push_back(m_vertex_buffer, Vertex::layout());
     m_texture_slots.reserve(m_max_textures);
}

void BatchRenderer::on_update(float) const
{
     draw_count = 0U;
}
void BatchRenderer::draw() const
{
     flush_vertices();
}
void BatchRenderer::draw(Quad quad) const
{
     if (std::ranges::size(m_vertices) == vert_count())
     {
          flush_vertices();
     }
     m_vertices += std::move(quad);
}
void BatchRenderer::on_im_gui_update() const
{
     // ImGui::Text("%s", fmt::format("Total Draws: {}", draw_count).c_str());
}
void BatchRenderer::flush_vertices() const
{
     index_buffer_size = m_vertex_buffer.update(m_vertices);
     bind_textures();
     draw_vertices();
     m_vertices.clear();
}
void BatchRenderer::draw_vertices() const
{
     if (std::ranges::empty(m_vertices))
     {
          return;
     }
     Renderer::Draw(index_buffer_size, m_vertex_array, m_index_buffer, m_shader);
     ++draw_count;
}
void BatchRenderer::clear() const
{
     m_texture_slots.clear();
     m_texture_slots.push_back(m_blank.id());
     m_vertices.clear();
}
std::size_t BatchRenderer::quad_count() const noexcept
{
     return m_quad_count;
}
std::size_t BatchRenderer::vert_count() const noexcept
{
     return m_quad_count * 4U;
}
std::size_t BatchRenderer::index_count() const noexcept
{
     return m_quad_count * 6U;
}
std::uint32_t BatchRenderer::max_texture_image_units()
{
     std::int32_t texture_units{};
     GlCall{}(glGetIntegerv, GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
     return static_cast<std::uint32_t>(texture_units);
}
void BatchRenderer::draw_quad(const Texture &texture, glm::vec3 offset, glm::vec2 size) const
{
     draw_quad(offset, { 1.F, 1.F, 1.F, 1.F }, texture, 1.F, size);
}
void BatchRenderer::draw_quad(const SubTexture &texture, glm::vec3 offset, glm::vec2 size) const
{
     draw_quad(offset, { 1.F, 1.F, 1.F, 1.F }, texture, 1.F, size);
}
void BatchRenderer::draw_quad(const SubTexture &texture, glm::vec3 offset, glm::vec2 size, int id, unsigned int pupu_id) const
{
     draw_quad(offset, { 1.F, 1.F, 1.F, 1.F }, texture, 1.F, size, id, pupu_id);
}
void BatchRenderer::draw_quad(
  glm::vec3         offset,
  glm::vec4         color,
  const SubTexture &texture,
  const float       tiling_factor,
  glm::vec2         size,
  int               id,
  unsigned int      pupu_id) const
{
     if (const auto result = std::ranges::find(m_texture_slots, texture.id()); result != std::ranges::end(m_texture_slots))
     {
          draw(CreateQuad(
            offset, color, static_cast<int>(result - std::ranges::begin(m_texture_slots)), tiling_factor, texture.uv(), size, id, pupu_id));
     }
     else
     {
          if (std::cmp_equal(std::ranges::size(m_texture_slots), m_max_textures))
          {
               // when we reach the max amount of unique textures we go ahead and empty the queue and draw to the frame buffer.
               flush_vertices();
          }
          draw(CreateQuad(
            offset, color, static_cast<int>(result - std::ranges::begin(m_texture_slots)), tiling_factor, texture.uv(), size, id, pupu_id));
     }
}
void BatchRenderer::draw_quad(glm::vec3 offset, glm::vec4 color) const
{
     draw(CreateQuad(offset, color, 0));
}

[[maybe_unused]] void BatchRenderer::draw_quad(glm::vec3 offset, glm::vec4 color, glm::vec2 size) const
{
     draw(CreateQuad(
       offset,
       color,
       {},
       1.F,
       std::array<glm::vec2, 4U>{ glm::vec2{ 0.F, 0.F }, glm::vec2{ 1.F, 0.F }, glm::vec2{ 1.F, 1.F }, glm::vec2{ 0.F, 1.F } },
       size));
}
const Shader &BatchRenderer::shader() const
{
     return m_shader;
}
[[maybe_unused]] const std::vector<std::uint32_t> &BatchRenderer::texture_slots() const
{
     return m_texture_slots;
}
void BatchRenderer::on_render() const
{
     flush_vertices();
}
void BatchRenderer::bind_textures() const
{
     m_shader.bind();
     m_uniform_texture_slots.clear();
     for (std::int32_t i{}; const std::uint32_t id : m_texture_slots)
     {
          GlCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + i));
          GlCall{}(glBindTexture, GL_TEXTURE_2D, id);
          m_uniform_texture_slots.push_back(i++);
     }
     m_shader.set_uniform("u_Textures", m_uniform_texture_slots);
}
void BatchRenderer::on_event(const event::Item &) const {}
void BatchRenderer::bind() const
{
     m_shader.bind();
}
void BatchRenderer::unbind()
{
     glengine::Shader::unbind();
}

}// namespace glengine