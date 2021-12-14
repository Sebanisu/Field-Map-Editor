//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_BATCHRENDERER_HPP
#define MYPROJECT_BATCHRENDERER_HPP
#include "IndexBufferDynamic.hpp"
#include "OrthographicCameraController.hpp"
#include "Shader.hpp"
#include "SubTexture.hpp"
#include "Test.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"


class BatchRenderer
{
public:
  BatchRenderer();
  BatchRenderer(std::size_t quad_count);

  void                      OnUpdate(float) const;
  void                      OnRender() const;
  void                      OnImGuiUpdate() const;
  void                      OnEvent(const Event::Item &e) const;

  [[nodiscard]] std::size_t QUAD_COUNT() const noexcept;
  [[nodiscard]] std::size_t VERT_COUNT() const noexcept;
  [[nodiscard]] [[maybe_unused]] std::size_t INDEX_COUNT() const noexcept;
  [[nodiscard]] static const std::int32_t   &Max_Texture_Image_Units();
  void                                       Clear() const;
  void                                       DrawQuad(
                                          glm::vec2      offset,
                                          const Texture &texture,
                                          glm::vec2      size = glm::vec2{ 1.F }) const;
  void DrawQuad(glm::vec2 offset, const SubTexture &texture) const;
  void DrawQuad(
    glm::vec2         offset,
    glm::vec4         color,
    const SubTexture &texture,
    const float       tiling_factor = 1.F,
    glm::vec2         size          = glm::vec2{ 1.F }) const;
  [[maybe_unused]] void DrawQuad(glm::vec2 offset, glm::vec4 color) const;
  void                  Draw(Quad quad) const;
  void                  Draw() const;

  const ::Shader       &Shader() const;
  OrthographicCameraController     &Camera() const;
  const std::vector<std::uint32_t> &TextureSlots() const;
  void                              Bind() const;
  static void                       UnBind();

private:
  void                        FlushVertices() const;
  void                        DrawVertices() const;
  void                        BindTextures() const;
  std::size_t                 m_quad_count    = { 100U };
  VertexBufferDynamic         m_vertex_buffer = { QUAD_COUNT() };
  IndexBufferDynamic          m_index_buffer  = { QUAD_COUNT() };
  mutable std::vector<Vertex> m_vertices      = { [this]() {
    std::vector<Vertex>       r{};
    r.reserve(VERT_COUNT());
    return r;
  }() };
  mutable IndexBufferDynamicSize       index_buffer_size       = {};
  ::Shader                             m_shader                = {};
  VertexArray                          m_vertex_array          = {};
  mutable std::vector<std::uint32_t>   m_texture_slots         = {};
  mutable std::vector<std::int32_t>    m_uniform_texture_slots = {};
  inline static OrthographicCameraController m_camera                = {16/9};
  Texture m_blank = { (std::numeric_limits<std::uint32_t>::max)() };
};
static_assert(Renderable<BatchRenderer>);
static_assert(Bindable<BatchRenderer>);
#endif// MYPROJECT_BATCHRENDERER_HPP
