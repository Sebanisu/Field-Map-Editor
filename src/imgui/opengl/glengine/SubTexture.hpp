//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_SUBTEXTURE_HPP
#define MYPROJECT_SUBTEXTURE_HPP
#include "Texture.hpp"

class SubTexture
{
public:
  SubTexture() = default;
  SubTexture(GLID_copy id)
    : m_render_id(std::move(id))
  {
  }
  SubTexture(const Texture &texture)
    : m_render_id(texture.ID())
  {
  }
  SubTexture(const Texture &texture, const glm::vec2 &min, const glm::vec2 &max)
    : m_render_id(texture.ID())
  {
    m_uv = { (glm::vec2{ min.x, min.y }),
             (glm::vec2{ max.x, min.y }),
             (glm::vec2{ max.x, max.y }),
             (glm::vec2{ min.x, max.y }) };
  }
  static SubTexture CreateFromCoords(
    const Texture   &texture,
    const glm::vec2 &coords,
    const glm::vec2 &cell_size,
    const glm::vec2 &sprite_size = { 1.F, 1.F },
    float            scale       = 1.F)
  {
    return { texture,
             { (coords.x * cell_size.x * scale)
                 / static_cast<float>(texture.width()),
               (coords.y * cell_size.y * scale)
                 / static_cast<float>(texture.height()) },
             { ((coords.x + sprite_size.x) * cell_size.x * scale)
                 / static_cast<float>(texture.width()),
               ((coords.y + sprite_size.y) * cell_size.y * scale)
                 / static_cast<float>(texture.height()) } };
  }
  GLID_copy ID() const
  {
    return m_render_id;
  }
  std::array<glm::vec2, 4U> UV() const
  {
    return m_uv;
  }
  void Bind(std::int32_t slot = 0) const
  {
    GLCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot));
    GLCall{}(glBindTexture, GL_TEXTURE_2D, m_render_id);
  }
  static void UnBind()
  {
    Texture::UnBind();
  };

private:
  static constexpr auto Default_UV        = std::array{ glm::vec2{ 0.F, 0.F },
                                                 glm::vec2{ 1.F, 0.F },
                                                 glm::vec2{ 1.F, 1.F },
                                                 glm::vec2{ 0.F, 1.F } };
  GLID_copy             m_render_id       = {};
  std::decay_t<decltype(Default_UV)> m_uv = { Default_UV };
};
static_assert(Bindable<SubTexture>);
#endif// MYPROJECT_SUBTEXTURE_HPP