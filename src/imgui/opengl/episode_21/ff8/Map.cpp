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
{
  if(!std::empty(m_mim_path))
  {
    fmt::print("Loaded {}\n", m_mim_path);
    fmt::print("Loaded {}\n", m_map_path);
    fmt::print("Loading Textures from Mim \n");
    m_delayed_textures = LoadTextures(m_mim);
  }
  camera.RefreshAspectRatio();
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
    const auto min_x = i_min_x->x();
    const auto max_x = i_max_x->x();
    const auto min_y = i_min_y->y();
    const auto max_y = i_max_y->y();
    //    const auto width  = max_x - min_x + 16;
    //    const auto height = max_y - min_y + 16;
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
  using open_viii::graphics::background::BlendModeT;
  BlendModeT last_blend_mode{BlendModeT::none};
  Window::DefaultBlend();
  camera.OnRender();
  SetUniforms();
  m_batch_renderer.Clear();
  m_map.visit_tiles([&](const auto &tiles) {
    for (const auto &tile : tiles | std::views::reverse)
    {
      const auto  bpp                = tile.depth();
      const auto  palette            = tile.palette_id();

      std::size_t texture_index      = palette;
      int         texture_page_width = 256;
      if (bpp.bpp8())
      {
        texture_index      = 16 + palette;
        texture_page_width = 128;
      }
      else if (bpp.bpp16())
      {
        texture_index      = 16 * 2;
        texture_page_width = 64;
      }
      auto  texture_page_offset = tile.texture_id() * texture_page_width;

      auto &texture = m_delayed_textures.textures->at(texture_index);
      if (texture.width() == 0 || texture.height() == 0)
        continue;
      const auto texture_dims =
        glm::vec2(m_mim.get_width(tile.depth()), m_mim.get_height());
      SubTexture sub_texture = {
        texture,
        glm::vec2{ tile.source_x() + texture_page_offset,
                   texture_dims.y - (tile.source_y() + 16) }
          / texture_dims,
        glm::vec2{ tile.source_x() + texture_page_offset + 16,
                   texture_dims.y - tile.source_y() }
          / texture_dims
      };
      auto blend_mode = tile.blend_mode();
      if(blend_mode != last_blend_mode)
      {
        last_blend_mode = blend_mode;
        switch(blend_mode)
        {
          case BlendModeT::add:
          case BlendModeT::half_add:
          case BlendModeT::quarter_add: {
            //Window::AddBlend();
          }
          break;
          case BlendModeT ::subtract:
          {
            //Window::SubtractBlend();
          }
          break;
          default:
            Window::DefaultBlend();
        }
        m_batch_renderer.Draw(); //flush buffer.
      }
      m_batch_renderer.DrawQuad(
        sub_texture,
        glm::vec2(tile.x(), (camera.Bounds().top - 16) - tile.y()),
        glm::vec2(16, 16));
    }
  });
  m_batch_renderer.Draw();
  m_batch_renderer.OnRender();
  Window::DefaultBlend();
}
void ff8::Map::OnEvent(const Event::Item &e) const
{
  camera.OnEvent(e);
  m_batch_renderer.OnEvent(e);
}
void ff8::Map::OnImGuiUpdate() const {
  m_batch_renderer.OnImGuiUpdate();
  camera.OnImGuiUpdate();
}
