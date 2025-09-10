//
// Created by pcvii on 11/30/2021.
//

#include "SubTexture.hpp"
void glengine::SubTexture::bind(std::int32_t slot) const
{
     GlCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot));
     GlCall{}(glBindTexture, GL_TEXTURE_2D, m_render_id);
}


void glengine::SubTexture::bind_read_only(int slot) const
{
     GlCall{}(glBindImageTexture, slot, m_render_id, 0, static_cast<GLboolean>(GL_FALSE), 0, GL_READ_ONLY, Texture::s_sized_interal_format);
     // bind(slot);
}
void glengine::SubTexture::bind_write_only(int slot) const
{
     GlCall{}(
       glBindImageTexture, slot, m_render_id, 0, static_cast<GLboolean>(GL_FALSE), 0, GL_WRITE_ONLY, Texture::s_sized_interal_format);
}


void glengine::SubTexture::unbind()
{
     Texture::unbind();
}
glengine::SubTexture glengine::SubTexture::create_from_coords(
  const Texture   &texture,
  const glm::vec2 &coords,
  const glm::vec2 &cell_size,
  const glm::vec2 &sprite_size,
  float            scale)
{
     return { texture,
              { (coords.x * cell_size.x * scale) / static_cast<float>(texture.width()),
                (coords.y * cell_size.y * scale) / static_cast<float>(texture.height()) },
              { ((coords.x + sprite_size.x) * cell_size.x * scale) / static_cast<float>(texture.width()),
                ((coords.y + sprite_size.y) * cell_size.y * scale) / static_cast<float>(texture.height()) } };
}
glengine::SubTexture::SubTexture(const Texture &texture, const glm::vec2 &min, const glm::vec2 &max)
  : m_render_id(texture.id())
  , m_width(texture.width())
  , m_height(texture.height())
  , m_uv{ (glm::vec2{ min.x, min.y }), (glm::vec2{ max.x, min.y }), (glm::vec2{ max.x, max.y }), (glm::vec2{ min.x, max.y }) }
{
}

static constexpr GLint Miplevel = {};
glengine::SubTexture::SubTexture(const Texture &texture)
  : m_render_id(texture.id())
  , m_width(texture.width())
  , m_height(texture.height())
{
}
glengine::SubTexture::SubTexture(GlidCopy id)
  : m_render_id(std::move(id))
{
     bind();
     GlCall{}(glGetTexLevelParameteriv, GL_TEXTURE_2D, Miplevel, GL_TEXTURE_WIDTH, &m_width);
     GlCall{}(glGetTexLevelParameteriv, GL_TEXTURE_2D, Miplevel, GL_TEXTURE_HEIGHT, &m_height);
}


int glengine::SubTexture::width() const noexcept
{
     if (m_uv == default_uv)
     {
          return m_width;
     }
     auto [min_uv, max_uv] = minmax_uv();
     return static_cast<int>(std::round((max_uv.x - min_uv.x) * static_cast<float>(m_width)));
}

int glengine::SubTexture::height() const noexcept
{
     if (m_uv == default_uv)
     {
          return m_height;
     }
     auto [min_uv, max_uv] = minmax_uv();
     return static_cast<int>(std::round((max_uv.y - min_uv.y) * static_cast<float>(m_height)));
}

glm::ivec2 glengine::SubTexture::get_size() const noexcept
{
     return { width(), height() };
}

std::pair<glm::vec2, glm::vec2> glengine::SubTexture::minmax_uv() const noexcept
{
     glm::vec2 min_uv = m_uv[0];
     glm::vec2 max_uv = m_uv[0];
     for (auto const &uv : m_uv)
     {
          min_uv = glm::min(min_uv, uv);
          max_uv = glm::max(max_uv, uv);
     }
     return { min_uv, max_uv };
}