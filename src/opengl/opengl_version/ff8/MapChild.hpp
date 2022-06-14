//
// Created by pcvii on 6/7/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPCHILD_HPP
#define FIELD_MAP_EDITOR_MAPCHILD_HPP
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
#include "MapFilters.hpp"
#include "MapHistory.hpp"
#include "OrthographicCamera.hpp"
#include "OrthographicCameraController.hpp"
#include "PixelBuffer.hpp"
#include "TransformedSortedUniqueCopy.hpp"
#include "UniqueTileValues.hpp"
#include "Window.hpp"
#include <Counter.hpp>
#include <type_traits>
namespace ff8
{
struct MapChild
{
  struct IndexAndPageWidthReturn
  {
    std::size_t  texture_index      = {};
    std::int16_t texture_page_width = { s_texture_page_width };
  };
  MapChild() = default;
  [[maybe_unused]] [[maybe_unused]] MapChild(
    const char   *label,
    const Fields &fields);
  MapChild(const char *label, const Fields &fields, std::string upscale_path);
  void                    OnUpdate(float ts) const;
  void                    OnRender() const;
  void                    OnImGuiUpdate() const;
  void                    OnEvent(const glengine::Event::Item &event) const;
  void                    SetUniforms() const;
  glm::vec2               TileSize() const;
  IndexAndPageWidthReturn IndexAndPageWidth(
    open_viii::graphics::BPPT bpp,
    std::uint8_t              palette) const;
  IndexAndPageWidthReturn
    IndexAndPageWidth(std::uint8_t palette, std::uint8_t texture_page) const;
  template<typename T>
  void SaveButton(T &&lambda) const
  {
    const auto popid = glengine::ImGuiPushID();
    if (ImGui::Button("Save"))
    {
      Save(std::forward<decltype(lambda)>(lambda));
    }
  }
  template<typename T>
  void Save(T &&lambda) const
  {
    const auto stop_saving = [this]() {
      m_saving = true;
      return glengine::scope_guard_captures([this]() { m_saving = false; });
    }();
    RenderTiles(std::forward<decltype(lambda)>(lambda));
    const auto path = std::filesystem::path(m_map_path);
    auto       string =
      fmt::format("{}_map.png", (path.parent_path() / path.stem()).string());
    glengine::PixelBuffer pixel_buffer{ m_frame_buffer.Specification() };
    pixel_buffer.         operator()(m_frame_buffer, string);
    while (pixel_buffer.operator()(&glengine::Texture::save))
      ;
  }
  [[nodiscard]] glengine::scope_guard PushColor(const ImVec4 &color) const;
  void                                RenderFrameBuffer() const;
  template<typename T>
  void RenderTiles(T &&lambda) const
  {
    if (std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path))
    {
      return;
    }
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
      lambda();
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
    m_changed = false;
  }
  template<typename TileT>
  void UpdateBlendMode(
    const TileT                                 &tile,
    open_viii::graphics::background::BlendModeT &last_blend_mode) const
  {
    if (!m_using_blending || !s_blending)
      return;
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
            m_uniform_color = s_half_uniform_color;
            break;
          case open_viii::graphics::background::BlendModeT::quarter_add:
            m_uniform_color = s_quarter_uniform_color;
            break;
          default:
            m_uniform_color = s_default_uniform_color;
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
  static auto filter_use_blending_and_draw(bool using_blending)
  {
    return [=](const auto &tile) -> bool {
      if (using_blending)
      {
        return tile.draw();
      }
      return true;
    };
  }
  constexpr static auto filter_invalid =
    open_viii::graphics::background::Map::filter_invalid();
  constexpr static auto get_z = [](const auto &tile) { return tile.z(); };
  template<typename T>
  constexpr static auto get_match_z(T z)
  {
    return [z](const auto &tile) { return z == get_z(tile); };
  }
  template<typename T>
  auto VisitTiles(T &&lambda) const
  {
    return m_map.back().visit_tiles([&](const auto &tiles) {
      auto f_tiles =
        tiles | std::views::filter(filter_invalid)
        | std::views::filter(filter_use_blending_and_draw(m_using_blending))
        | std::views::filter(m_filters.TestTile());
      std::vector<std::uint16_t> unique_z{};
      {
        // unique_z.reserve(std::ranges::size(tiles));
        std::ranges::transform(f_tiles, std::back_inserter(unique_z), get_z);
        std::ranges::sort(unique_z);
        auto [begin, end] = std::ranges::unique(unique_z);
        unique_z.erase(begin, end);
      }

      auto unique_z_reverse = unique_z | std::views::reverse;
      for (const auto z : unique_z_reverse)
      {
        auto f_tiles_reverse_filter_z =
          f_tiles | std::views::reverse | std::views::filter(get_match_z(z));
        for (const auto &tile : f_tiles_reverse_filter_z)
        {
          if (!lambda(tile))
            return;
        }
      }
    });
  }
  //  template<typename TileT>
  //  [[nodiscard]] glm::vec3 TileToDrawPos(const TileT &tile) const
  //  {
  //    static constexpr auto x            = get_x<TileT>();
  //    static constexpr auto y            = get_y<TileT>();
  //    static constexpr auto texture_page = get_texture_page<TileT>();
  //    return { (static_cast<float>(
  //                x(tile) + texture_page(tile) * s_texture_page_width)
  //              - m_offset_x)
  //               * m_tile_scale,
  //             (m_offset_y - static_cast<float>(y(tile))) * m_tile_scale,
  //             0.F };
  //  }
  template <typename TileT>
  [[nodiscard]] std::optional<glengine::SubTexture>
    TileToSubTexture(const TileT &tile) const
  {
    const auto bpp                                 = tile.depth();
    const auto palette                             = tile.palette_id();
    const auto texture_page_id                     = tile.texture_id();
    const auto [texture_index, texture_page_width] = [&]() {
      if (std::ranges::empty(m_upscale_path))
        return IndexAndPageWidth(bpp, palette);
      return IndexAndPageWidth(palette, texture_page_id);
    }();

    const auto texture_page_offset =
      [&, texture_page_width_copy = texture_page_width]() {
        if (std::ranges::empty(m_upscale_path))
          return texture_page_id * texture_page_width_copy;
        return 0;
      }();
    const auto &texture =
      std::ranges::empty(m_upscale_path)
        ? m_delayed_textures.textures->at(texture_index)
        : m_upscale_delayed_textures.textures->at(texture_index);
    if (texture.width() == 0 || texture.height() == 0)
      return std::nullopt;
    const auto  texture_dims = glm::vec2{ texture.width(), texture.height() };
    const float tile_scale   = static_cast<float>(texture.height())
                             / static_cast<float>(m_mim.get_height());
    const float tile_size = tile_scale * 16.F;
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
  constexpr static auto get_palette_id = [](const auto &tile) {
    return tile.palette_id();
  };
  constexpr static auto get_texture_height =
    [](const glengine::Texture &texture) { return texture.height(); };
  static constexpr auto true_x = [](const auto &tile) { return tile.x(); };
  static constexpr auto true_y = [](const auto &tile) { return tile.y(); };

  mutable glengine::OrthographicCamera m_fixed_render_camera = {};
  inline static bool                   s_fit_height          = { true };
  inline static bool                   s_fit_width           = { true };
  inline static bool                   s_draw_grid           = { false };
  inline static bool                   s_blending            = { true };

  static constexpr int16_t             s_texture_page_width  = 256;


  static constexpr glm::vec4 s_default_uniform_color = { 1.F, 1.F, 1.F, 1.F };
  static constexpr glm::vec4 s_half_uniform_color    = { .5F, .5F, .5F, .5F };
  static constexpr glm::vec4 s_quarter_uniform_color = { .25F,
                                                         .25F,
                                                         .25F,
                                                         .25F };
  mutable glm::vec4          m_uniform_color         = s_default_uniform_color;

  std::string                m_upscale_path          = {};
  // internal mim file path
  std::string                m_mim_path              = {};
  // internal map file path
  std::string                m_map_path              = {};
  // if coo was chosen instead of default.
  bool                       m_mim_choose_coo        = {};
  // if coo was chosen instead of default.
  bool                       m_map_choose_coo        = {};
  // container for field textures
  open_viii::graphics::background::Mim m_mim         = {};
  // container for field tile information
  mutable MapHistory                   m_map         = {};
  // loads the textures overtime instead of forcing them to load at start.
  glengine::DelayedTextures<35U>       m_delayed_textures = {};
  glengine::DelayedTextures<17U * 13U>
    m_upscale_delayed_textures = {};// 20 is detected max 16(+1)*13 is possible
  // max. 0 being no palette and 1-17 being
  // with palettes
  // takes quads and draws them to the frame buffer or screen.
  glengine::BatchRenderer           m_batch_renderer        = { 1000 };
  // holds rendered image at 1:1 scale to prevent gaps when scaling.
  mutable glengine::FrameBuffer     m_frame_buffer          = {};
  mutable float                     m_offset_x              = 0.F;
  mutable float                     m_offset_y              = -16.F;
  mutable bool                      m_offscreen_drawing     = { false };
  mutable bool                      m_saving                = { false };
  mutable bool                      m_preview               = { false };
  glm::vec3                         m_position              = {};
  inline constinit static MapBlends s_blends                = {};
  mutable MapFilters                m_filters               = { m_map.front() };
  mutable bool                      m_changed               = { true };
  mutable glm::vec2                 true_min_xy             = {};
  mutable glm::vec2                 true_max_xy             = {};
  mutable float                     min_x                   = {};
  mutable float                     min_y                   = {};
  mutable float                     max_x                   = {};
  mutable float                     max_y                   = {};
  mutable float                     m_tile_scale            = { 1.F };
  glengine::Counter                 m_id                    = {};
  mutable std::vector<bool>         m_tile_button_state     = {};
  const char                       *m_label                 = {};
  glengine::ImGuiViewPortWindow     m_imgui_viewport_window = { m_label };
  bool                              m_using_blending        = { false };
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAPCHILD_HPP
