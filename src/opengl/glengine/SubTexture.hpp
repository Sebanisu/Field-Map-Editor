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
  SubTexture(GlidCopy id);
  SubTexture(const Texture &texture);
  SubTexture(
    const Texture   &texture,
    const glm::vec2 &min,
    const glm::vec2 &max);
  static SubTexture create_from_coords(
    const Texture   &texture,
    const glm::vec2 &coords,
    const glm::vec2 &cell_size,
    const glm::vec2 &sprite_size = { 1.F, 1.F },
    float            scale       = 1.F);
  GlidCopy id() const
  {
    return m_render_id;
  }
  std::array<glm::vec2, 4U> uv() const
  {
    return m_uv;
  }
  /**
   * Only get the uv ImGui uses it's a min and max but with swapped y values.
   * @return array of two ImVec2
   */
  std::array<ImVec2, 2U> im_gui_uv() const
  {
    return { ImVec2{ m_uv[0].x, m_uv[2].y }, ImVec2{ m_uv[2].x, m_uv[0].y } };
  }
  void        bind(std::int32_t slot = 0) const;
  static void unbind();


private:
  static constexpr auto default_uv  = std::array{ glm::vec2{ 0.F, 0.F },
                                                 glm::vec2{ 1.F, 0.F },
                                                 glm::vec2{ 1.F, 1.F },
                                                 glm::vec2{ 0.F, 1.F } };
  GlidCopy              m_render_id = {};
  GLint                 m_width     = {};
  GLint                 m_height    = {};
  std::remove_cvref_t<decltype(default_uv)> m_uv = { default_uv };
};
static_assert(Bindable<SubTexture>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_SUBTEXTURE_HPP
