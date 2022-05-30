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
#include "MapFilters.hpp"
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
    GetUniqueValues();
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
          static_cast<float>(m_mim.get_height()) * m_tile_scale
          < static_cast<float>(current_max->height()))
        {

          float old_height =
            static_cast<float>(m_frame_buffer.Specification().height)
            / m_tile_scale;
          float old_width =
            static_cast<float>(m_frame_buffer.Specification().width)
            / m_tile_scale;
          m_tile_scale = static_cast<float>(current_max->height())
                         / static_cast<float>(m_mim.get_height());
          float height = old_height * m_tile_scale;
          float width  = old_width * m_tile_scale;
          m_imgui_viewport_window.SetImageBounds(glm::vec2{ width, height });
          m_fixed_render_camera.SetProjection({ width, height });
          m_frame_buffer =
            glengine::FrameBuffer(glengine::FrameBufferSpecification{
              .width  = static_cast<int>(width),
              .height = static_cast<int>(height) });
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
  void OnImGuiUpdate() const
  {
    const auto popid = glengine::ImGuiPushID();
    {
      const auto disable = glengine::ImGuiDisabled(
        std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path));

      m_changed = std::ranges::any_of(
        std::array{ ImGui::Checkbox("Draw Grid", &s_draw_grid),
                    ImGui::Checkbox("Fit Height", &s_fit_height),
                    ImGui::Checkbox("Fit Width", &s_fit_width),
                    s_blends.OnImGuiUpdate(),
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
        "DrawPos - X: {}, Y: {} Z: {}, Width {}, Height {}"
        "\n\tOffset X {} Offset Y {},\n\tMin X {}, Max X {}, Min Y {}, Max Y "
        "{}\n",
        m_position.x,
        m_position.y,
        m_position.z,
        m_frame_buffer.Specification().width,
        m_frame_buffer.Specification().height,
        m_offset_x,
        m_offset_y,
        min_x,
        max_x,
        min_y,
        max_y)
        .c_str());

    m_batch_renderer.OnImGuiUpdate();
    ImGui::Separator();
    ImGui::Text("%s", "Fixed Prerender Camera: ");
    m_fixed_render_camera.OnImGuiUpdate();
    ff8::ImGuiTileDisplayWindow::OnImGuiUpdateForward(m_id, [this]() {
      ImGui::Text(
        "%s", fmt::format("Map {}", static_cast<uint32_t>(m_id)).c_str());
      //      const auto *imgui_texture_id_ref = ConvertGLIDtoImTextureID();
      //      m_packed.button_clicked          = ImGui::ImageButton(
      //        m_imgui_texture_id_ref,
      //        ImVec2(
      //          static_cast<float>(m_fb.Specification().width),
      //          static_cast<float>(m_fb.Specification().height)),
      //        ImVec2(0, 1),
      //        ImVec2(1, 0),
      //        0);
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
      //      m_batch_renderer.Shader().SetUniform(
      //        "u_MVP",
      //        GetViewPortPreview().SetPositionAndSizeAndGetMVP(
      //          s_camera.Camera().ScreenSpaceToWorldSpace(
      //            m_imgui_viewport_window.ViewPortMousePos()),
      //          glm::vec2{ m_frame_buffer.Specification().width,
      //                     m_frame_buffer.Specification().height }));
    }
    else
    {
      m_batch_renderer.Shader().SetUniform(
        "u_MVP", m_imgui_viewport_window.ViewProjectionMatrix());
    }
    if (!s_draw_grid || m_offscreen_drawing || m_saving)
    {
      m_batch_renderer.Shader().SetUniform("u_Grid", 0.F, 0.F);
    }
    else
    {
      m_batch_renderer.Shader().SetUniform("u_Grid", 16.F, 16.F);
    }
    m_batch_renderer.Shader().SetUniform(
      "u_Color",
      s_uniform_color.r,
      s_uniform_color.g,
      s_uniform_color.b,
      s_uniform_color.a);
  }
  std::optional<glengine::SubTexture> TileToSubTexture(const auto &tile) const
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
  glm::vec3 TileToDrawPos(const auto &tile) const
  {
    using tileT = std::decay_t<decltype(tile)>;
    static constexpr typename TileFunctions::template Bounds<tileT>::x x{};
    static constexpr typename TileFunctions::template Bounds<tileT>::y y{};
    static constexpr
      typename TileFunctions::template Bounds<tileT>::texture_page
        texture_page{};
    return { (static_cast<float>(
                x(tile) + texture_page(tile) * s_texture_page_width)
              - m_offset_x)
               * m_tile_scale,
             (m_offset_y - static_cast<float>(y(tile))) * m_tile_scale,
             0.F };
  }
  glm::vec2 TileSize() const
  {
    const auto scaled_size = m_tile_scale * 16.F;
    return { scaled_size, scaled_size };
  }
  auto VisitTiles(auto &&lambda) const
  {
    return m_map.visit_tiles([&](const auto &tiles) {
      auto f_tiles = tiles
                     | std::views::filter(
                       open_viii::graphics::background::Map::filter_invalid())
                     | std::views::filter(m_filters.TestTile());

      std::vector<std::uint16_t> unique_z{};
      {
        // unique_z.reserve(std::ranges::size(tiles));
        std::ranges::transform(
          f_tiles, std::back_inserter(unique_z), [](const auto &tile) {
            return tile.z();
          });
        std::ranges::sort(unique_z);
        auto [begin, end] = std::ranges::unique(unique_z);
        unique_z.erase(begin, end);
      }

      auto unique_z_reverse = unique_z | std::views::reverse;
      for (const auto z : unique_z_reverse)
      {
        auto f_tiles_reverse_filter_z =
          f_tiles | std::views::reverse
          | std::views::filter([z](const auto &t) { return z == t.z(); });
        for (const auto &tile : f_tiles_reverse_filter_z)
        {
          lambda(tile);
        }
      }
    });
  }
  // draws tiles
  void RenderTiles() const
  {
    using open_viii::graphics::background::BlendModeT;
    BlendModeT last_blend_mode{ BlendModeT::none };
    s_uniform_color = s_default_uniform_color;
    glengine::Window::DefaultBlend();
    m_imgui_viewport_window.OnRender();
    SetUniforms();
    m_batch_renderer.Clear();
    VisitTiles([this, &last_blend_mode](const auto &tile) {
      auto sub_texture = TileToSubTexture(tile);
      if (!sub_texture)
        return;
      UpdateBlendMode(tile, last_blend_mode);
      m_batch_renderer.DrawQuad(*sub_texture, TileToDrawPos(tile), TileSize());
    });
    m_batch_renderer.Draw();
    m_batch_renderer.OnRender();
    glengine::Window::DefaultBlend();
    s_uniform_color = s_default_uniform_color;
  }
  void UpdateBlendMode(
    const auto                                  &tile,
    open_viii::graphics::background::BlendModeT &last_blend_mode) const
  {
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
            s_uniform_color = s_half_uniform_color;
            break;
          case open_viii::graphics::background::BlendModeT::quarter_add:
            s_uniform_color = s_quarter_uniform_color;
            break;
          default:
            s_uniform_color = s_default_uniform_color;
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
  struct IndexAndPageWidthReturn
  {
    std::size_t  texture_index      = {};
    std::int16_t texture_page_width = { s_texture_page_width };
  };

  auto
    IndexAndPageWidth(open_viii::graphics::BPPT bpp, std::uint8_t palette) const
  {
    IndexAndPageWidthReturn r = { .texture_index = palette };
    if (bpp.bpp8())
    {
      r.texture_index      = 16 + palette;
      r.texture_page_width = s_texture_page_width / 2;
    }
    else if (bpp.bpp16())
    {
      r.texture_index      = 16 * 2;
      r.texture_page_width = s_texture_page_width / 4;
    }
    return r;
  }
  auto IndexAndPageWidth(std::uint8_t palette, std::uint8_t texture_page) const
  {
    IndexAndPageWidthReturn r = { .texture_index = static_cast<size_t>(
                                    texture_page + 13U * (palette + 1U)) };
    if (!m_upscale_delayed_textures.textures->at(r.texture_index))
    {
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
      m_position * m_tile_scale,
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
    OnRender();
    const auto path = std::filesystem::path(m_map_path);
    auto       string =
      fmt::format("{}_map.png", (path.parent_path() / path.stem()).string());
    glengine::PixelBuffer pixel_buffer{ m_frame_buffer.Specification() };
    pixel_buffer.         operator()(m_frame_buffer, string);
    while (pixel_buffer.operator()(&glengine::Texture::save))
      ;
  }
  void GetUniqueValues()
  {
    auto vector = m_map.visit_tiles([&](const auto &tiles) {
      auto f_tiles = tiles
                     | std::views::filter(
                       open_viii::graphics::background::Map::filter_invalid());
      return TransformedSortedUniqueCopy(
        f_tiles, [](const auto &tile) { return tile.palette_id(); });
    });
    for (const auto &value : vector)
      fmt::print("\t{}\n", value);
  }
  void SetCameraBoundsToEdgesOfImage()
  {
    // s_camera.RefreshAspectRatio(m_imgui_viewport_window.ViewPortAspectRatio());
    m_map.visit_tiles([&](const auto &tiles) {
      using tileT = std::ranges::range_value_t<decltype(tiles)>;
      static constexpr typename TileFunctions::template Bounds<tileT>::x x{};
      static constexpr typename TileFunctions::template Bounds<tileT>::y y{};
      static constexpr
        typename TileFunctions::template Bounds<tileT>::use_texture_page
          use_texture_page{};
      static constexpr
        typename TileFunctions::template Bounds<tileT>::texture_page
           texture_page{};
      auto f_tiles = tiles
                     | std::views::filter(
                       open_viii::graphics::background::Map::filter_invalid());
      auto [i_min_x, i_max_x] = std::ranges::minmax_element(f_tiles, {}, x);
      auto [i_min_y, i_max_y] = std::ranges::minmax_element(f_tiles, {}, y);
      auto i_max_texture_page =
        std::ranges::max_element(f_tiles, {}, texture_page);

      if (i_min_x == i_max_x || i_min_y == i_max_y)
      {
        return;
      }
      min_x = x(*i_min_x);
      max_x = static_cast<float>(
        use_texture_page
          ? x(*i_max_x)
          : (texture_page(*i_max_texture_page) + 1) * s_texture_page_width);
      min_y             = y(*i_min_y);
      max_y             = y(*i_max_y);
      const auto width  = max_x - min_x + 16.F;
      const auto height = max_y - min_y + 16.F;

      // m_offset_y        = static_cast<float>(min_y + max_y);
      m_offset_x        = width / 2.F + min_x;
      m_offset_y        = height / 2.F + min_y - 16.F;
      //  m_position        = glm::vec3(min_x, min_y, 0.F);
      m_position        = glm::vec3(-width / 2.F, -height / 2.F, 0.F);

      m_imgui_viewport_window.SetImageBounds(glm::vec2{ width, height });

      //      s_fixed_render_camera.SetProjection(
      //        static_cast<float>(min_x),
      //        static_cast<float>(max_x + 16),
      //        static_cast<float>(min_y),
      //        static_cast<float>(max_y + 16));
      m_fixed_render_camera.SetProjection({ width, height });
      m_frame_buffer = glengine::FrameBuffer(glengine::FrameBufferSpecification{
        .width  = static_cast<int>(abs(width)),
        .height = static_cast<int>(abs(height)) });
    });
  }
  mutable glengine::OrthographicCamera m_fixed_render_camera = {};
  inline static bool                   s_fit_height          = { true };
  inline static bool                   s_fit_width           = { true };
  inline static bool                   s_draw_grid           = { false };

  static constexpr int16_t             s_texture_page_width  = 256;


  static constexpr glm::vec4 s_default_uniform_color = { 1.F, 1.F, 1.F, 1.F };
  static constexpr glm::vec4 s_half_uniform_color    = { .5F, .5F, .5F, .5F };
  static constexpr glm::vec4 s_quarter_uniform_color = { .25F,
                                                         .25F,
                                                         .25F,
                                                         .25F };
  inline static glm::vec4    s_uniform_color         = s_default_uniform_color;

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
  open_viii::graphics::background::Map m_map         = {};
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
  inline static constinit float     m_offset_x              = 0.F;
  inline static constinit float     m_offset_y              = -16.F;
  mutable bool                      m_offscreen_drawing     = { false };
  mutable bool                      m_saving                = { false };
  mutable bool                      m_preview               = { false };
  glm::vec3                         m_position              = {};
  inline constinit static MapBlends s_blends                = {};
  MapFilters                        m_filters               = { m_map };
  mutable bool                      m_changed               = { true };
  inline static constinit float     min_x                   = {};
  inline static constinit float     min_y                   = {};
  inline static constinit float     max_x                   = {};
  inline static constinit float     max_y                   = {};
  mutable float                     m_tile_scale            = { 1.F };
  glengine::Counter                 m_id                    = {};
  glengine::ImGuiViewPortWindow     m_imgui_viewport_window = {
        TileFunctions::Label
  };
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAP_HPP