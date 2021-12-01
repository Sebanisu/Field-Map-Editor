//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_BATCHRENDERER_HPP
#define MYPROJECT_BATCHRENDERER_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "SubTexture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"
#include <glm/glm.hpp>
#include <vector>

class BatchRenderer
{
public:
  BatchRenderer();
  BatchRenderer(std::size_t quad_count);

  friend void
    OnUpdate(const BatchRenderer &, float);
  friend void
    OnRender(const BatchRenderer &);
  friend void
    OnImGuiRender(const BatchRenderer &);


  [[nodiscard]] std::size_t
    QUAD_COUNT() const noexcept
  {
    return m_quad_count;
  };
  [[nodiscard]] std::size_t
    VERT_COUNT() const noexcept
  {
    return m_quad_count * 4U;
  };
  [[nodiscard]] [[maybe_unused]] std::size_t
    INDEX_COUNT() const noexcept
  {
    return m_quad_count * 6U;
  };

  [[nodiscard]] static const std::int32_t &
    Max_Texture_Image_Units()
  {
    static const std::int32_t number = []()
    {
      std::int32_t texture_units{};
      GLCall{ glGetIntegerv, GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units };
      return texture_units;
    }();
    return number;
  }
  void
    Clear() const;
  void
    DrawQuad(glm::vec2 offset, const Texture &texture) const
  {
    DrawQuad(offset, { 1.F, 1.F, 1.F, 1.F }, texture);
  }
  void
    DrawQuad(glm::vec2 offset, const SubTexture &texture) const
  {
    DrawQuad(offset, { 1.F, 1.F, 1.F, 1.F }, texture);
  }
  void
    DrawQuad(
      glm::vec2      offset,
      glm::vec4      color,
      const SubTexture &texture,
      const float    tiling_factor = 1.F) const
  {
    if (const auto result = std::ranges::find(m_texture_slots, texture.ID());
        result != std::ranges::end(m_texture_slots))
    {
      Draw(CreateQuad(
        offset,
        color,
        static_cast<int>(result - std::ranges::begin(m_texture_slots)),
        tiling_factor,texture.UV()));
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
        tiling_factor));
    }
  }
  void
    DrawQuad(glm::vec2 offset, glm::vec4 color) const
  {
    Draw(CreateQuad(offset, color, 0));
  }
  void
    Draw(Quad quad) const;
  void
    Draw() const;

  const auto &
    Shader() const
  {
    return m_shader;
  }
  const auto &
    TextureSlots() const
  {
    return m_texture_slots;
  }

private:
  void
    FlushVertices() const;
  void
                              DrawVertices() const;
  std::size_t                 m_quad_count    = { 100U };
  VertexBufferDynamic         m_vertex_buffer = { QUAD_COUNT() };
  IndexBufferDynamic          m_index_buffer  = { QUAD_COUNT() };
  mutable std::vector<Vertex> m_vertices      = { [this]()
                                             {
                                               std::vector<Vertex>r{};
                                               r.reserve(VERT_COUNT());
                                               return r;
                                             }() };
  mutable IndexBufferDynamicSize     index_buffer_size       = {};
  ::Shader                           m_shader                = {};
  VertexArray                        m_vertex_array          = {};
  mutable std::vector<std::uint32_t> m_texture_slots         = {};
  mutable std::vector<std::int32_t>  m_uniform_texture_slots = {};
  Texture m_blank = { (std::numeric_limits<std::uint32_t>::max)() };
};
void
  OnUpdate(const BatchRenderer &, float);
void
  OnRender(const BatchRenderer &);
void
  OnImGuiRender(const BatchRenderer &);


#endif// MYPROJECT_BATCHRENDERER_HPP
