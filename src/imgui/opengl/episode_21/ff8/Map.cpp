//
// Created by pcvii on 11/30/2021.
//

#include "Map.hpp"
#include "OrthographicCameraController.hpp"

static OrthographicCameraController camera              = { 16 / 9 };
static bool                         snap_zoom_to_height = true;
static bool                         draw_grid           = false;
static bool                         saving              = false;

ff8::Map::Map(const ff8::Fields &fields)
  : m_mim(LoadMim(fields.Field(), fields.Coo(), m_mim_path, m_mim_choose_coo))
  , m_map(LoadMap(
      fields.Field(),
      fields.Coo(),
      m_mim,
      m_map_path,
      m_map_choose_coo))
  , m_delayed_textures(LoadTextures(m_mim))
{
}

void ff8::Map::OnUpdate(float ts) const
{
  m_delayed_textures.check();

  m_map.visit_tiles([&](const auto &tiles) {
    auto [i_min_x, i_max_x] = std::ranges::minmax_element(
      tiles, {}, [](const auto &tile) { return tile.x(); });
    auto [i_min_y, i_max_y] = std::ranges::minmax_element(
      tiles, {}, [](const auto &tile) { return tile.y(); });

    if (i_min_x == i_max_x || i_min_y == i_max_y)
    {
      return;
    }
    const auto min_x  = i_min_x->x();
    const auto max_x  = i_max_x->x();
    const auto min_y  = i_min_y->y();
    const auto max_y  = i_max_y->y();
    const auto width  = max_x - min_x + 16;
    const auto height = max_y - min_y + 16;
    camera.SetMaxBounds({ static_cast<float>(min_x),
                          static_cast<float>(max_x + 16),
                          static_cast<float>(min_y),
                          static_cast<float>(max_y + 16) });
  });
  if (snap_zoom_to_height)
  {
    camera.SetZoom();
  }
  camera.OnUpdate(ts);
  m_batch_renderer.OnUpdate(ts);
}
void ff8::Map::SetUniforms() const
{
  m_batch_renderer.Bind();
  //  if (saving)
  //  {
  //    batch_renderer.Shader().SetUniform(
  //      "u_MVP",
  //      OrthographicCamera{ { 0, 0 }, { texture->width(), texture->height() }
  //      }
  //        .ViewProjectionMatrix());
  //  }
  //  else
  {
    m_batch_renderer.Shader().SetUniform(
      "u_MVP", camera.Camera().ViewProjectionMatrix());
  }
  if (!draw_grid || saving)
  {
    m_batch_renderer.Shader().SetUniform("u_Grid", 0.F, 0.F);
  }
  else
  {
    m_batch_renderer.Shader().SetUniform("u_Grid", 16.F, 16.F);
  }
  m_batch_renderer.Shader().SetUniform("u_Color", 1.F, 1.F, 1.F, 1.F);
}

void ff8::Map::OnRender() const
{
  camera.OnRender();
  SetUniforms();
  m_batch_renderer.Clear();
  m_map.visit_tiles([&](const auto &tiles) {
    for (const auto &tile : tiles)
    {
      const auto bpp                = tile.depth();
      const auto palette            = tile.palette_id();

      int        texture_index      = {};
      int        texture_page_width = 256;
      if (bpp.bpp4())
      {
        texture_index = palette;
      }
      else if (bpp.bpp8())
      {
        texture_index      = 16 + palette;
        texture_page_width = 128;
      }
      else if (bpp.bpp16())
      {
        texture_index      = 16 * 2;
        texture_page_width = 64;
      }
      //      else if (bpp.bpp24())
      //      {
      //        texture_index = 16 * 2 + 1;
      //        texture_page_width
      //      }
      auto       texture_page_offset = tile.texture_id() * texture_page_width;

      auto      &texture     = m_delayed_textures.textures->at(texture_index);
      SubTexture sub_texture = {
        texture,
        glm::vec2{ tile.source_x() + texture_page_offset, texture.height() - tile.source_y() },
        glm::vec2{ tile.source_x() + texture_page_offset + 16,
                   texture.height() - (tile.source_y() + 16) }
      };

      m_batch_renderer.DrawQuad(
        sub_texture, glm::vec2(tile.x(), tile.y()), glm::vec2(16, 16));
    }
  });
  m_batch_renderer.Draw();
  m_batch_renderer.OnRender();
}
void ff8::Map::OnEvent(const Event::Item &e) const
{
  camera.OnEvent(e);
  m_batch_renderer.OnEvent(e);
}
