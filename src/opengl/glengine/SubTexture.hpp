//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_SUBTEXTURE_HPP
#define FIELD_MAP_EDITOR_SUBTEXTURE_HPP
#include "Texture.hpp"
namespace glengine
{
class SubTexture
{
public:
  SubTexture() = default;
  SubTexture(GLID_copy id);
  SubTexture(const Texture &texture);
  SubTexture(
    const Texture   &texture,
    const glm::vec2 &min,
    const glm::vec2 &max);
  static SubTexture CreateFromCoords(
    const Texture   &texture,
    const glm::vec2 &coords,
    const glm::vec2 &cell_size,
    const glm::vec2 &sprite_size = { 1.F, 1.F },
    float            scale       = 1.F);
  GLID_copy ID() const
  {
    return m_render_id;
  }
  std::array<glm::vec2, 4U> UV() const
  {
    return m_uv;
  }
  void        Bind(std::int32_t slot = 0) const;
  static void UnBind();

private:
  static constexpr auto Default_UV        = std::array{ glm::vec2{ 0.F, 0.F },
                                                 glm::vec2{ 1.F, 0.F },
                                                 glm::vec2{ 1.F, 1.F },
                                                 glm::vec2{ 0.F, 1.F } };
  GLID_copy             m_render_id       = {};
  std::decay_t<decltype(Default_UV)> m_uv = { Default_UV };
};
static_assert(Bindable<SubTexture>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_SUBTEXTURE_HPP
