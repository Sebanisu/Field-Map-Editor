//
// Created by pcvii on 1/11/2022.
//

#ifndef FIELD_MAP_EDITOR_MAP_HPP
#define FIELD_MAP_EDITOR_MAP_HPP
#include "Application.hpp"
#include "BatchRenderer.hpp"
#include "BlendModeEquations.hpp"
#include "BlendModeParameters.hpp"
#include "DelayedTextures.hpp"
#include "Event/EventDispatcher.hpp"
#include "FF8LoadTextures.hpp"
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "FrameBufferBackup.hpp"
#include "FrameBufferRenderer.hpp"
#include "GenericCombo.hpp"
#include "ImGuiDisabled.hpp"
#include "ImGuiIndent.hpp"
#include "ImGuiPushID.hpp"
#include "ImGuiTileDisplayWindow.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "MapBlends.hpp"
#include "MapDims.hpp"
#include "MapFilters.hpp"
#include "MapTileAdjustments.hpp"
#include "OrthographicCamera.hpp"
#include "OrthographicCameraController.hpp"
#include "PixelBuffer.hpp"
#include "tile_operations.hpp"
#include "TransformedSortedUniqueCopy.hpp"
#include "UniqueTileValues.hpp"
#include "Window.hpp"
#include <Counter.hpp>
#include <type_traits>
namespace ff8
{
template<typename TileFunctions>
class Map
{
public:
  Map() = default;
  Map(const Fields &fields)
    : Map(fields, {})
  {
  }
  Map(const Fields &fields, std::string upscale_path)
    : m_upscale_path(std::move(upscale_path))
    , m_mim(LoadMim(fields.Field(), fields.Coo(), m_mim_path, m_mim_choose_coo))
    , m_map(LoadMap(
        fields.Field(),
        fields.Coo(),
        m_mim,
        m_map_path,
        m_map_choose_coo))
    , m_filters(m_map)
  {
    if (std::empty(m_mim_path))
    {
      return;
    }
    if (!std::ranges::empty(m_upscale_path))
    {
      const auto stem = std::filesystem::path(m_map_path).parent_path().stem();
      m_upscale_path  = (std::filesystem::path(m_upscale_path)
                        / stem.string().substr(0, 2) / stem)
                         .string();
      fmt::print("Upscale Location: {}\n", m_upscale_path);
    }
    fmt::print(
      "Loaded Map: {}\nLoaded Mim: {}\nBegin Loading "
      "Textures from Mim.\n",
      m_map_path,
      m_mim_path);
    m_delayed_textures         = LoadTextures(m_mim);
    m_upscale_delayed_textures = LoadTextures(m_upscale_path);
    SetCameraBoundsToEdgesOfImage();
    m_tile_button_state =
      std::vector<bool>(VisitUnSortedUnFilteredTilesCount(), false);
  }
  void OnUpdate(float ts) const
  {

    if (m_delayed_textures.OnUpdate() || m_upscale_delayed_textures.OnUpdate())
    {
      if (!std::ranges::empty(m_upscale_path))
      {
        const auto current_max = (std::ranges::max_element)(
          *m_upscale_delayed_textures.textures,
          {},
          [](const glengine::Texture &texture) { return texture.height(); });
        if (
          static_cast<float>(m_mim.get_height()) * m_map_dims.tile_scale
          < static_cast<float>(current_max->height()))
        {
          m_map_dims.tile_scale = static_cast<float>(current_max->height())
                                  / static_cast<float>(m_mim.get_height());
          SetCameraBoundsToEdgesOfImage();
        }
      }
      m_changed = true;
    }
    m_imgui_viewport_window.OnUpdate(ts);
    m_imgui_viewport_window.Fit(s_fit_width, s_fit_height);
    m_batch_renderer.OnUpdate(ts);
  }
  void OnRender() const
  {
    if (std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path))
    {
      return;
    }
    const auto not_changed =
      glengine::scope_guard_captures([&]() { m_changed = false; });
    if (m_changed)
    {
      m_offscreen_drawing = true;
      const auto not_offscreen_drawing =
        glengine::scope_guard_captures([&]() { m_offscreen_drawing = false; });
      const auto fbb = glengine::FrameBufferBackup{};
      m_frame_buffer.Bind();

      GLCall{}(
        glViewport,
        0,
        0,
        m_frame_buffer.Specification().width,
        m_frame_buffer.Specification().height);
      glengine::Renderer::Clear();
      RenderTiles();
      if (!m_saving)
      {
        RenderFrameBufferGrid();
      }
    }
    // RestoreViewPortToFrameBuffer();
    m_imgui_viewport_window.OnRender([this]() { RenderFrameBuffer(); });
    GetViewPortPreview().OnRender(m_imgui_viewport_window.HasHover(), [this]() {
      m_preview = true;
      RenderFrameBuffer();
      m_preview = false;
    });
    ff8::ImGuiTileDisplayWindow::TakeControl(
      m_imgui_viewport_window.HasHover(), m_id);
  }
  void OnImGuiUpdate() const
  {
    const auto popid = glengine::ImGuiPushID();
    {
      const auto disable = glengine::ImGuiDisabled(
        std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path));

      (void)ImGui::Checkbox("Fit Height", &s_fit_height);
      (void)ImGui::Checkbox("Fit Width", &s_fit_width);
      m_changed = std::ranges::any_of(
        std::array{ ImGui::Checkbox("Draw Grid", &s_draw_grid),
                    [&]() -> bool {
                      return m_map.visit_tiles([](auto &&) -> bool {
                        if constexpr (!typename TileFunctions::use_blending{})
                        {
                          return true;
                        }
                        else
                        {
                          const bool checkbox_changed =
                            ImGui::Checkbox("Blending", &s_blending);
                          const bool blend_options_changed =
                            s_blends.OnImGuiUpdate();
                          return checkbox_changed || blend_options_changed;
                        }
                      });
                    }(),
                    m_filters.OnImGuiUpdate() },
        std::identity{});


      if (ImGui::Button("Save"))
      {
        Save();
      }
    }
    ImGui::Separator();
    m_imgui_viewport_window.OnImGuiUpdate();
    ImGui::Separator();

    ImGui::Text(
      "%s",
      fmt::format(
        "DrawPos ({}, {}, {}), Width {}, Height {}"
        "\n\tOffset ({}, {}),\n\tMin ({}, {}), Max ({},{})\n",
        m_map_dims.position.x,
        m_map_dims.position.y,
        m_map_dims.position.z,
        m_frame_buffer.Specification().width,
        m_frame_buffer.Specification().height,
        m_map_dims.offset.x,
        m_map_dims.offset.y,
        m_map_dims.min.x,
        m_map_dims.min.y,
        m_map_dims.max.x,
        m_map_dims.max.y)
        .c_str());

    m_batch_renderer.OnImGuiUpdate();
    ImGui::Separator();
    ImGui::Text("%s", "Fixed Prerender Camera: ");
    m_fixed_render_camera.OnImGuiUpdate();

    ff8::ImGuiTileDisplayWindow::OnImGuiUpdateForward(m_id, [this]() {
      ImGui::Text(
        "%s", fmt::format("Map {}", static_cast<uint32_t>(m_id)).c_str());
      float      text_width = 0.F;
      ImVec2     last_pos   = {};
      const auto render_sub_texture =
        [&text_width,
         &last_pos](const glengine::SubTexture &sub_texture) -> bool {
        text_width = 0.F;
        const auto imgui_texture_id_ref =
          ConvertGLIDtoImTextureID(sub_texture.ID());
        const auto uv     = sub_texture.ImGuiUV();
        const auto id_pop = glengine::ImGuiPushID();
        const auto color  = ImVec4(0.F, 0.F, 0.F, 0.F);
        last_pos          = ImGui::GetCursorPos();
        text_width        = ImGui::GetItemRectMax().x;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        const auto pop_color =
          glengine::scope_guard([]() { ImGui::PopStyleColor(1); });
        // ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        bool value = ImGui::ImageButton(
          imgui_texture_id_ref, ImVec2(16, 16), uv[0], uv[1]);
        text_width = ImGui::GetStyle().ItemSpacing.x + ImGui::GetItemRectMax().x
                     - text_width;
        return value;
      };
      const auto                        dims = ImGui::GetContentRegionAvail();
      std::size_t                       i    = {};
      const MapTileAdjustments<TileFunctions> mta  = { m_map, m_filters, m_map_dims };
      if (VisitUnSortedUnFilteredTiles([&](auto &tile) -> bool {
            using namespace open_viii::graphics::background;
            const auto id_pop_2    = glengine::ImGuiPushID();
            const auto sub_texture = TileToSubTexture(tile);
            const auto increment =
              glengine::scope_guard_captures([&]() { ++i; });
            if (!sub_texture)
            {
              return false;
            }
            if (render_sub_texture(*sub_texture))
            {
              m_tile_button_state.at(i).flip();
            }
            bool changed = false;
            if (m_tile_button_state.at(i))
            {
              ImGui::SameLine();
              mta(tile,changed,i,sub_texture);
            }
            else if (
              dims.x - (last_pos.x + text_width - ImGui::GetCursorPos().x)
              > text_width)
            {
              if (
                m_tile_button_state.size() != i + 1
                && m_tile_button_state.at(i + 1))
              {
              }
              else
              {
                ImGui::SameLine();
              }
            }
            return changed;
          }))
      {
        m_changed = true;
      }
    });
  }
  void OnEvent(const glengine::Event::Item &event) const
  {
    m_imgui_viewport_window.OnEvent(event);
    m_batch_renderer.OnEvent(event);
  }

private:
  // set uniforms
  void SetUniforms() const
  {
    m_batch_renderer.Bind();
    if (m_offscreen_drawing || m_saving)
    {
      m_batch_renderer.Shader().SetUniform(
        "u_MVP", m_fixed_render_camera.ViewProjectionMatrix());
    }
    else if (m_preview)
    {
      m_batch_renderer.Shader().SetUniform(
        "u_MVP", m_imgui_viewport_window.PreviewViewProjectionMatrix());
    }
    else
    {
      m_batch_renderer.Shader().SetUniform(
        "u_MVP", m_imgui_viewport_window.ViewProjectionMatrix());
    }
    m_batch_renderer.Shader().SetUniform("u_Grid", 0.F, 0.F);
    //    if (!s_draw_grid || m_offscreen_drawing || m_saving)
    //    {
    //      m_batch_renderer.Shader().SetUniform("u_Grid", 0.F, 0.F);
    //    }
    //    else
    //    {
    //      m_batch_renderer.Shader().SetUniform(
    //        "u_Grid", m_map_dims.scaled_tile_size());
    //    }
    m_batch_renderer.Shader().SetUniform("u_Color", m_uniform_color);
  }
  std::optional<glengine::SubTexture> TileToSubTexture(const auto &tile) const
  {
    const auto bpp                                 = tile.depth();
    const auto palette                             = tile.palette_id();
    const auto texture_page_id                     = tile.texture_id();
    const auto [texture_index, texture_page_width] = [&]() {
      if (std::ranges::empty(m_upscale_path))
      {
        return IndexAndPageWidth(bpp, palette);
      }
      return IndexAndPageWidth(palette, texture_page_id);
    }();
    const auto texture_page_offset =
      [&, texture_page_width_copy = texture_page_width]() {
        if (std::ranges::empty(m_upscale_path))
        {
          return texture_page_id * texture_page_width_copy;
        }
        return 0;
      }();
    const auto &texture = [&,
                           texture_index = texture_index]() -> decltype(auto) {
      if (std::ranges::empty(m_upscale_path))
      {
        return m_delayed_textures.textures->at(texture_index);
      }
      return m_upscale_delayed_textures.textures->at(texture_index);
    }();
    if (texture.width() == 0 || texture.height() == 0)
      return std::nullopt;
    const auto  texture_dims = glm::vec2{ texture.width(), texture.height() };
    const float tile_scale   = static_cast<float>(texture.height())
                             / static_cast<float>(m_mim.get_height());
    const float tile_size = tile_scale * MapDimsStatics::tile_size;
    // glm::vec2(m_mim.get_width(tile.depth()), m_mim.get_height());
    return std::optional<glengine::SubTexture>{
      std::in_place_t{},
      texture,
      glm::vec2{ tile.source_x() * tile_scale
                   + static_cast<float>(texture_page_offset),
                 texture_dims.y - (tile.source_y() * tile_scale + tile_size) }
        / texture_dims,
      glm::vec2{ tile.source_x() * tile_scale
                   + static_cast<float>(texture_page_offset) + tile_size,
                 texture_dims.y - tile.source_y() * tile_scale }
        / texture_dims
    };
  }
  glm::vec3 TileToDrawPos(const auto &tile) const
  {
    static constexpr typename TileFunctions::x            x{};
    static constexpr typename TileFunctions::y            y{};
    static constexpr typename TileFunctions::texture_page texture_page{};
    return { (static_cast<float>(
                x(tile)
                + texture_page(tile) * MapDimsStatics::texture_page_width)
              - m_map_dims.offset.x)
               * m_map_dims.tile_scale,
             (m_map_dims.offset.y - static_cast<float>(y(tile)))
               * m_map_dims.tile_scale,
             0.F };
  }
  auto VisitTiles(auto &&lambda) const
  {
    return m_map.visit_tiles([&](const auto &tiles) {
      auto f_tiles = tiles | std::views::filter(tile_operations::invalid_tile{})
                     | std::views::filter([](const auto &tile) -> bool {
                         static constexpr
                           typename TileFunctions::use_blending use_blending{};
                         if (use_blending)
                         {
                           return tile.draw();
                         }
                         return true;
                       })
                     | std::views::filter(m_filters.TestTile());
      std::vector<std::uint16_t> unique_z{};
      {
        // unique_z.reserve(std::ranges::size(tiles));
        std::ranges::transform(
          f_tiles, std::back_inserter(unique_z), tile_operations::z{});
        std::ranges::sort(unique_z);
        auto [begin, end] = std::ranges::unique(unique_z);
        unique_z.erase(begin, end);
      }

      auto unique_z_reverse = unique_z | std::views::reverse;
      for (const auto z : unique_z_reverse)
      {
        auto f_tiles_reverse_filter_z =
          f_tiles | std::views::reverse
          | std::views::filter(tile_operations::z_match{ z });
        for (const auto &tile : f_tiles_reverse_filter_z)
        {
          if (!lambda(tile))
            return;
        }
      }
    });
  }
  // draws tiles
  void RenderTiles() const
  {
    using open_viii::graphics::background::BlendModeT;
    BlendModeT last_blend_mode{ BlendModeT::none };
    m_uniform_color = s_default_color;
    glengine::Window::DefaultBlend();
    m_imgui_viewport_window.OnRender();
    SetUniforms();
    m_batch_renderer.Clear();
    VisitTiles([this, &last_blend_mode](const auto &tile) -> bool {
      auto sub_texture = TileToSubTexture(tile);
      if (!sub_texture)
      {
        return true;
      }
      UpdateBlendMode(tile, last_blend_mode);
      m_batch_renderer.DrawQuad(
        *sub_texture, TileToDrawPos(tile), m_map_dims.scaled_tile_size());
      return true;
    });
    m_batch_renderer.Draw();
    m_batch_renderer.OnRender();
    glengine::Window::DefaultBlend();
    m_uniform_color = s_default_color;
  }
  void UpdateBlendMode(
    [[maybe_unused]] const auto &tile,
    [[maybe_unused]] open_viii::graphics::background::BlendModeT
      &last_blend_mode) const
  {
    if constexpr (typename TileFunctions::use_blending{})
    {
      if (!s_blending)
      {
        return;
      }
      auto blend_mode = tile.blend_mode();
      if (blend_mode != last_blend_mode)
      {
        m_batch_renderer.Draw();// flush buffer.
        last_blend_mode = blend_mode;
        if (s_blends.PercentBlendEnabled())
        {
          switch (blend_mode)
          {
            case open_viii::graphics::background::BlendModeT::half_add:
              m_uniform_color = s_half_color;
              break;
            case open_viii::graphics::background::BlendModeT::quarter_add:
              m_uniform_color = s_quarter_color;
              break;
            default:
              m_uniform_color = s_default_color;
              break;
          }
        }
        switch (blend_mode)
        {
          case open_viii::graphics::background::BlendModeT::half_add:
          case open_viii::graphics::background::BlendModeT::quarter_add:
          case open_viii::graphics::background::BlendModeT::add: {
            s_blends.SetAddBlend();
          }
          break;
          case open_viii::graphics::background::BlendModeT ::subtract: {
            s_blends.SetSubtractBlend();
          }
          break;
          default:
            glengine::Window::DefaultBlend();
        }
      }
    }
  }
  struct [[nodiscard]] IndexAndPageWidthReturn
  {
    std::size_t  texture_index      = {};
    std::int16_t texture_page_width = { MapDimsStatics::texture_page_width };
  };

  [[nodiscard]] static auto
    IndexAndPageWidth(open_viii::graphics::BPPT bpp, std::uint8_t palette)
  {
    IndexAndPageWidthReturn r = { .texture_index = palette };
    if (bpp.bpp8())
    {
      r.texture_index      = 16 + palette;
      r.texture_page_width = MapDimsStatics::texture_page_width / 2;
    }
    else if (bpp.bpp16())
    {
      r.texture_index      = 16 * 2;
      r.texture_page_width = MapDimsStatics::texture_page_width / 4;
    }
    return r;
  }
  [[nodiscard]] auto
    IndexAndPageWidth(std::uint8_t palette, std::uint8_t texture_page) const
  {
    IndexAndPageWidthReturn r = { .texture_index = static_cast<size_t>(
                                    texture_page + 13U * (palette + 1U)) };
    if (!m_upscale_delayed_textures.textures->at(r.texture_index))
    {
      // no palette with texture page combo was found. So attempt to load
      // texture page without palette.
      r.texture_index = texture_page;
    }
    return r;
  }

  void RenderFrameBuffer() const
  {
    glengine::Window::DefaultBlend();
    m_imgui_viewport_window.OnRender();
    SetUniforms();
    m_batch_renderer.Clear();
    m_batch_renderer.DrawQuad(
      m_frame_buffer.GetColorAttachment(),
      m_map_dims.scaled_position(),
      glm::vec2(
        m_frame_buffer.Specification().width,
        m_frame_buffer.Specification().height));
    m_batch_renderer.Draw();
    m_batch_renderer.OnRender();
  }

  void RenderFrameBufferGrid() const
  {
    if (!s_draw_grid)
    {
      return;
    }
    const auto fbb = glengine::FrameBufferBackup{};
    const auto offscreen_pop =
      glengine::scope_guard_captures([&]() { m_offscreen_drawing = false; });
    m_offscreen_drawing = true;
    glengine::Window::DefaultBlend();
    m_imgui_viewport_window.OnRender();
    SetUniforms();
    m_frame_buffer.Bind();
    GLCall{}(
      glViewport,
      0,
      0,
      m_frame_buffer.Specification().width,
      m_frame_buffer.Specification().height);
    m_batch_renderer.Shader().SetUniform(
      "u_Grid", m_map_dims.scaled_tile_size());
    m_batch_renderer.Clear();
    m_batch_renderer.DrawQuad(
      m_frame_buffer.GetColorAttachment(),
      m_map_dims.scaled_position(),
      glm::vec2(
        m_frame_buffer.Specification().width,
        m_frame_buffer.Specification().height));
    m_batch_renderer.Draw();
    m_batch_renderer.OnRender();
  }
  void Save() const
  {
    m_saving = true;
    const auto not_saving =
      glengine::scope_guard_captures([&]() { m_saving = false; });
    if (s_draw_grid)
    {
      m_changed = true;
    }
    const auto changed = glengine::scope_guard_captures([&]() {
      if (s_draw_grid)
      {
        m_changed = true;
      }
    });
    OnRender();
    const auto path = std::filesystem::path(m_map_path);
    auto       string =
      fmt::format("{}_map.png", (path.parent_path() / path.stem()).string());
    glengine::PixelBuffer pixel_buffer{ m_frame_buffer.Specification() };
    pixel_buffer.         operator()(m_frame_buffer, string);
    while (pixel_buffer.operator()(&glengine::Texture::save))
      ;
  }
  auto VisitUnSortedUnFilteredTilesCount() const
  {
    return m_map.visit_tiles([&](const auto &tiles) -> std::size_t {
      auto f_tiles =
        tiles | std::views::filter(tile_operations::invalid_tile{});
      return static_cast<std::size_t>(
        std::ranges::count_if(f_tiles, [](auto &&) { return true; }));
    });
  }
  bool VisitUnSortedUnFilteredTiles(auto &&lambda) const
  {
    return m_map.visit_tiles([&](auto &&tiles) -> bool {
      auto f_tiles =
        tiles | std::views::filter(tile_operations::invalid_tile{});
      bool changed = false;
      for (auto &tile : f_tiles)
      {
        changed = lambda(tile) || changed;
      }
      return changed;
    });
  }
  void SetCameraBoundsToEdgesOfImage() const
  {
    // s_camera.RefreshAspectRatio(m_imgui_viewport_window.ViewPortAspectRatio());
    const glm::vec2 size = m_map_dims.scaled_size();
    m_imgui_viewport_window.SetImageBounds(size);
    m_fixed_render_camera.SetProjection(size);
    m_frame_buffer = glengine::FrameBuffer(glengine::FrameBufferSpecification{
      .width  = static_cast<int>(abs(size.x)),
      .height = static_cast<int>(abs(size.y)) });
  }
  mutable glengine::OrthographicCamera m_fixed_render_camera = {};
  inline constinit static auto         s_fit_height          = bool{ true };
  inline constinit static auto         s_fit_width           = bool{ true };
  inline constinit static auto         s_draw_grid           = bool{ false };
  inline constinit static auto         s_blending            = bool{ true };

  static constexpr auto                s_default_color       = glm::vec4{ 1.F };
  static constexpr auto                s_half_color     = s_default_color / 2.F;
  static constexpr auto                s_quarter_color  = s_half_color / 2.F;
  mutable glm::vec4                    m_uniform_color  = s_default_color;

  std::string                          m_upscale_path   = {};
  // internal mim file path
  std::string                          m_mim_path       = {};
  // internal map file path
  std::string                          m_map_path       = {};
  // if coo was chosen instead of default.
  bool                                 m_mim_choose_coo = {};
  // if coo was chosen instead of default.
  bool                                 m_map_choose_coo = {};
  // container for field textures
  open_viii::graphics::background::Mim m_mim            = {};
  // container for field tile information
  mutable open_viii::graphics::background::Map m_map    = {};
  // dimensions of map
  MapDims<TileFunctions>                       m_map_dims         = { m_map };
  // loads the textures overtime instead of forcing them to load at start.
  glengine::DelayedTextures<35U>               m_delayed_textures = {};
  glengine::DelayedTextures<17U * 13U>
    m_upscale_delayed_textures = {};// 20 is detected max 16(+1)*13 is possible
                                    // max. 0 being no palette and 1-17 being
                                    // with palettes
  // takes quads and draws them to the frame buffer or screen.
  glengine::BatchRenderer           m_batch_renderer        = { 1000 };
  // holds rendered image at 1:1 scale to prevent gaps when scaling.
  mutable glengine::FrameBuffer     m_frame_buffer          = {};
  mutable bool                      m_offscreen_drawing     = { false };
  mutable bool                      m_saving                = { false };
  mutable bool                      m_preview               = { false };
  inline constinit static MapBlends s_blends                = {};
  mutable MapFilters                m_filters               = { m_map };
  mutable bool                      m_changed               = { true };
  glengine::Counter                 m_id                    = {};
  mutable std::vector<bool>         m_tile_button_state     = {};
  glengine::ImGuiViewPortWindow     m_imgui_viewport_window = {
        TileFunctions::Label
  };
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAP_HPP