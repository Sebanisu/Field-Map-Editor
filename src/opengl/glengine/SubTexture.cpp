//
// Created by pcvii on 11/30/2021.
//

#include "SubTexture.hpp"
void glengine::SubTexture::Bind(std::int32_t slot) const
{
  GLCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot));
  GLCall{}(glBindTexture, GL_TEXTURE_2D, m_render_id);
}
void glengine::SubTexture::UnBind()
{
  Texture::UnBind();
}
glengine::SubTexture glengine::SubTexture::CreateFromCoords(
  const Texture   &texture,
  const glm::vec2 &coords,
  const glm::vec2 &cell_size,
  const glm::vec2 &sprite_size,
  float            scale)
{
  return {
    texture,
    { (coords.x * cell_size.x * scale) / static_cast<float>(texture.width()),
      (coords.y * cell_size.y * scale) / static_cast<float>(texture.height()) },
    { ((coords.x + sprite_size.x) * cell_size.x * scale)
        / static_cast<float>(texture.width()),
      ((coords.y + sprite_size.y) * cell_size.y * scale)
        / static_cast<float>(texture.height()) }
  };
}
glengine::SubTexture::SubTexture(
  const Texture   &texture,
  const glm::vec2 &min,
  const glm::vec2 &max)
  : m_render_id(texture.ID())
{
  m_uv = { (glm::vec2{ min.x, min.y }),
           (glm::vec2{ max.x, min.y }),
           (glm::vec2{ max.x, max.y }),
           (glm::vec2{ min.x, max.y }) };
}
glengine::SubTexture::SubTexture(const Texture &texture)
  : m_render_id(texture.ID())
{
}
glengine::SubTexture::SubTexture(GLID_copy id)
  : m_render_id(std::move(id))
{
}
