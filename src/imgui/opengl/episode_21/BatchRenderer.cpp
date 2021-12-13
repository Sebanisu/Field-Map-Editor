//
// Created by pcvii on 11/30/2021.
//

#include "BatchRenderer.hpp"
#include "scope_guard.hpp"
#include "tests/Test.hpp"


static_assert(test::Test<BatchRenderer>);
static const Renderer renderer{};
static std::uint32_t  draw_count = 0U;
BatchRenderer::BatchRenderer()
  : BatchRenderer(100U)
{
}
BatchRenderer::BatchRenderer(std::size_t quad_count)
  : m_quad_count(quad_count)
  , m_shader(
      std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{
  m_vertex_array.Bind();
  m_vertex_array.push_back(m_vertex_buffer, Vertex::Layout());
  m_texture_slots.reserve(
    static_cast<std::uint32_t>(Max_Texture_Image_Units()));
}

void BatchRenderer::OnUpdate(float ts) const
{
  m_camera.OnUpdate(ts);
  draw_count = 0U;
}
void BatchRenderer::Draw() const
{
  FlushVertices();
}
void BatchRenderer::Draw(Quad quad) const
{
  if (std::ranges::size(m_vertices) == VERT_COUNT())
  {
    FlushVertices();
  }
  m_vertices += std::move(quad);
}
void BatchRenderer::OnImGuiUpdate() const
{
  if (m_camera.OnImGuiUpdate())
  {
  }
  ImGui::Text("%s", fmt::format("Total Draws: {}", draw_count).c_str());
}
void BatchRenderer::FlushVertices() const
{
  index_buffer_size = m_vertex_buffer.Update(m_vertices);
  BindTextures();
  DrawVertices();
  m_vertices.clear();
}
void BatchRenderer::DrawVertices() const
{
  if (std::ranges::empty(m_vertices))
  {
    return;
  }
  renderer.Draw(index_buffer_size, m_vertex_array, m_index_buffer, m_shader);
  ++draw_count;
}
void BatchRenderer::Clear() const
{
  m_texture_slots.clear();
  m_texture_slots.push_back(m_blank.ID());
  m_vertices.clear();
}
std::size_t BatchRenderer::QUAD_COUNT() const noexcept
{
  return m_quad_count;
}
std::size_t BatchRenderer::VERT_COUNT() const noexcept
{
  return m_quad_count * 4U;
}
std::size_t BatchRenderer::INDEX_COUNT() const noexcept
{
  return m_quad_count * 6U;
}
const std::int32_t &BatchRenderer::Max_Texture_Image_Units()
{
  static const std::int32_t number = []() {
    std::int32_t texture_units{};
    GLCall{}(glGetIntegerv, GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units);
    return texture_units;
  }();
  return number;
}
void BatchRenderer::DrawQuad(
  glm::vec2      offset,
  const Texture &texture,
  glm::vec2      size) const
{
  DrawQuad(offset, { 1.F, 1.F, 1.F, 1.F }, texture, 1.F, size);
}
void BatchRenderer::DrawQuad(glm::vec2 offset, const SubTexture &texture) const
{
  DrawQuad(offset, { 1.F, 1.F, 1.F, 1.F }, texture);
}
void BatchRenderer::DrawQuad(
  glm::vec2         offset,
  glm::vec4         color,
  const SubTexture &texture,
  const float       tiling_factor,
  glm::vec2         size) const
{
  if (const auto result = std::ranges::find(m_texture_slots, texture.ID());
      result != std::ranges::end(m_texture_slots))
  {
    Draw(CreateQuad(
      offset,
      color,
      static_cast<int>(result - std::ranges::begin(m_texture_slots)),
      tiling_factor,
      texture.UV(),
      size));
  }
  else
  {
    if (std::cmp_equal(
          std::ranges::size(m_texture_slots), Max_Texture_Image_Units()))
    {
      FlushVertices();
    }
    m_texture_slots.push_back(texture.ID());
    Draw(CreateQuad(
      offset,
      color,
      static_cast<int>(std::ranges::size(m_texture_slots) - 1U),
      tiling_factor,
      texture.UV(),
      size));
  }
}
void BatchRenderer::DrawQuad(glm::vec2 offset, glm::vec4 color) const
{
  Draw(CreateQuad(offset, color, 0));
}
const Shader &BatchRenderer::Shader() const
{
  return m_shader;
}
const std::vector<std::uint32_t> &BatchRenderer::TextureSlots() const
{
  return m_texture_slots;
}
void BatchRenderer::OnRender() const
{
  m_camera.OnRender();
  FlushVertices();
}
void BatchRenderer::BindTextures() const
{
  m_shader.Bind();
  m_uniform_texture_slots.clear();
  for (std::int32_t i{}; const std::uint32_t id : m_texture_slots)
  {
    GLCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + i));
    GLCall{}(glBindTexture, GL_TEXTURE_2D, id);
    m_uniform_texture_slots.push_back(i++);
  }
  m_shader.SetUniform("u_Textures", m_uniform_texture_slots);
}
void BatchRenderer::OnEvent(const Event::Item &e) const
{
  m_camera.OnEvent(e);
}
void BatchRenderer::Bind() const
{
  m_shader.Bind();
  m_shader.SetUniform("u_MVP", m_camera.Camera().ViewProjectionMatrix());
}
void BatchRenderer::UnBind()
{
  ::Shader::UnBind();
}
OrthographicCameraController &BatchRenderer::Camera() const
{
  return m_camera;
}
