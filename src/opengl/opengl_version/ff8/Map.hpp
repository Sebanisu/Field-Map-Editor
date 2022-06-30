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
#include "MapHistory.hpp"
#include "MapTileAdjustments.hpp"
#include "OrthographicCamera.hpp"
#include "OrthographicCameraController.hpp"
#include "PixelBuffer.hpp"
#include "SimilarAdjustments.hpp"
#include "tile_operations.hpp"
#include "TransformedSortedUniqueCopy.hpp"
#include "UniqueTileValues.hpp"
#include "Window.hpp"
#include <Counter.hpp>
#include <source_location>
#include <type_traits>
namespace ff_8
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
  Map(const Fields &fields, std::filesystem::path upscale_path)
    : m_upscale_path(std::move(upscale_path))
    , m_mim(LoadMim(fields, fields.coo(), m_mim_path, m_mim_choose_coo))
    , m_map(LoadMap(fields, fields.coo(), m_mim, m_map_path, m_map_choose_coo))
    , m_filters(m_map.back())
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
      spdlog::debug("Upscale Location: \"{}\"", m_upscale_path.string());
    }
    spdlog::debug("Loaded Map: \"{}\"", m_map_path);
    spdlog::debug("Loaded Mim: \"{}\"", m_mim_path);
    spdlog::debug("Begin Loading Textures from Mim.");
    m_delayed_textures         = LoadTextures(m_mim);
    m_upscale_delayed_textures = LoadTextures(m_upscale_path);
    visit_unsorted_unfiltered_tiles();
    m_tile_button_state =
      std::vector<bool>(visit_unsorted_unfiltered_tiles_count(), false);
  }
  void on_update(float ts) const
  {

    if (
      m_delayed_textures.on_update() || m_upscale_delayed_textures.on_update())
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
          visit_unsorted_unfiltered_tiles();
        }
      }
      m_changed();
    }
    m_imgui_viewport_window.on_update(ts);
    m_imgui_viewport_window.fit(s_fit_width, s_fit_height);
    m_batch_renderer.on_update(ts);
  }
  void on_render() const
  {
    if (std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path))
    {
      return;
    }
    const auto not_changed = m_changed.unset();
    if (m_changed)
    {
      m_offscreen_drawing = true;
      const auto not_offscreen_drawing =
        glengine::ScopeGuardCaptures([&]() { m_offscreen_drawing = false; });
      const auto fbb = glengine::FrameBufferBackup{};
      m_frame_buffer.bind();

      GlCall{}(
        glViewport,
        0,
        0,
        m_frame_buffer.specification().width,
        m_frame_buffer.specification().height);
      glengine::Renderer::Clear();
      render_tiles();
      if (!m_saving)
      {
        render_frame_buffer_grid();
      }
    }
    // RestoreViewPortToFrameBuffer();
    m_imgui_viewport_window.on_render([this]() { render_frame_buffer(); });
    GetViewPortPreview().on_render(m_imgui_viewport_window, [this]() {
      m_preview = true;
      const auto pop_preview =
        glengine::ScopeGuardCaptures([&]() { m_preview = false; });
      render_frame_buffer();
    });
    ff_8::ImGuiTileDisplayWindow::take_control(
      m_imgui_viewport_window.has_hover(), m_id);
  }
  void on_im_gui_update() const
  {
    const auto pop_id = glengine::ImGuiPushId();
    {
      const auto disable = glengine::ImGuiDisabled(
        std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path));

      (void)ImGui::Checkbox("fit Height", &s_fit_height);
      (void)ImGui::Checkbox("fit Width", &s_fit_width);
      m_changed.set_if_true(std::ranges::any_of(
        std::array{ ImGui::Checkbox("draw Grid", &s_draw_grid),
                    [&]() -> bool {
                      if constexpr (!typename TileFunctions::UseBlending{})
                      {
                        return false;
                      }
                      else
                      {
                        const bool checkbox_changed =
                          ImGui::Checkbox("Blending", &s_blending);
                        const bool blend_options_changed =
                          s_blends.on_im_gui_update();
                        return checkbox_changed || blend_options_changed;
                      }
                    }(),
                    m_filters.on_im_gui_update() },
        std::identity{}));


      if (ImGui::Button("Save"))
      {
        save();
      }
    }
    ImGui::Separator();
    m_imgui_viewport_window.on_im_gui_update();
    ImGui::Separator();

    ImGui::Text(
      "%s",
      fmt::format(
        "DrawPos ({}, {}, {}), Width {}, Height {}"
        "\n\tOffset ({}, {}),\n\tMin ({}, {}), Max ({},{})\n",
        m_map_dims.position.x,
        m_map_dims.position.y,
        m_map_dims.position.z,
        m_frame_buffer.specification().width,
        m_frame_buffer.specification().height,
        m_map_dims.offset.x,
        m_map_dims.offset.y,
        m_map_dims.min.x,
        m_map_dims.min.y,
        m_map_dims.max.x,
        m_map_dims.max.y)
        .c_str());

    m_batch_renderer.on_im_gui_update();
    ImGui::Separator();
    ImGui::Text("%s", "Fixed Prerender camera: ");
    m_fixed_render_camera.on_im_gui_update();

    ff_8::ImGuiTileDisplayWindow::on_im_gui_update_forward(m_id, [this]() {
      ImGui::Text(
        "%s", fmt::format("Map {}", static_cast<uint32_t>(m_id)).c_str());
      float      text_width = 0.F;
      ImVec2     last_pos   = {};
      const auto render_sub_texture =
        [&text_width,
         &last_pos](const glengine::SubTexture &sub_texture) -> bool {
        text_width = 0.F;
        const auto imgui_texture_id_ref =
          ConvertGliDtoImTextureId(sub_texture.id());
        const auto uv     = sub_texture.im_gui_uv();
        const auto id_pop = glengine::ImGuiPushId();
        const auto color  = ImVec4(0.F, 0.F, 0.F, 0.F);
        last_pos          = ImGui::GetCursorPos();
        text_width        = ImGui::GetItemRectMax().x;
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        const auto pop_color =
          glengine::ScopeGuard([]() { ImGui::PopStyleColor(1); });
        // ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        bool value = ImGui::ImageButton(
          imgui_texture_id_ref, ImVec2(16, 16), uv[0], uv[1]);
        text_width = ImGui::GetStyle().ItemSpacing.x + ImGui::GetItemRectMax().x
                     - text_width;
        return value;
      };
      const auto  dims = ImGui::GetContentRegionAvail();
      std::size_t i    = {};

      //      if (!m_changed.previous())
      //      {
      //        (void)m_map.copy_back_preemptive();
      //      }
      const auto  mta  = MapTileAdjustments<TileFunctions>(
        m_map, m_filters, m_map_dims, m_similar);
      if (visit_unsorted_unfiltered_tiles(
            [&](auto &tile, VisitState &visit_state) -> bool {
              using namespace open_viii::graphics::background;
              const auto id_pop_2    = glengine::ImGuiPushId();
              const auto sub_texture = tile_to_sub_texture(tile);
              const auto increment =
                glengine::ScopeGuardCaptures([&]() { ++i; });
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
                visit_state = mta(tile, changed, i, sub_texture);
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
        m_changed();
      }
      //      else if (m_changed.previous() && !m_changed.undo())
      //      {
      //        m_map.end_preemptive_copy_mode();
      //      }
      //      else
      //      {
      //        m_changed.disable_undo();
      //      }
    });
  }
  void on_event(const glengine::event::Item &event) const
  {
    m_imgui_viewport_window.on_event(event);
    m_batch_renderer.on_event(event);
  }

private:
  // set uniforms
  void set_uniforms() const
  {
    m_batch_renderer.bind();
    if (m_offscreen_drawing || m_saving)
    {
      m_batch_renderer.shader().set_uniform(
        "u_MVP", m_fixed_render_camera.view_projection_matrix());
    }
    else if (m_preview)
    {
      m_batch_renderer.shader().set_uniform(
        "u_MVP", m_imgui_viewport_window.preview_view_projection_matrix());
    }
    else
    {
      m_batch_renderer.shader().set_uniform(
        "u_MVP", m_imgui_viewport_window.view_projection_matrix());
    }
    m_batch_renderer.shader().set_uniform("u_Grid", 0.F, 0.F);
    //    if (!s_draw_grid || m_offscreen_drawing || m_saving)
    //    {
    //      m_batch_renderer.shader().set_uniform("u_Grid", 0.F, 0.F);
    //    }
    //    else
    //    {
    //      m_batch_renderer.shader().set_uniform(
    //        "u_Grid", m_map_dims.scaled_tile_size());
    //    }
    m_batch_renderer.shader().set_uniform("u_Color", m_uniform_color);
  }
  std::optional<glengine::SubTexture>
    tile_to_sub_texture(const auto &tile) const
  {
    const auto bpp                                 = tile.depth();
    const auto palette                             = tile.palette_id();
    const auto texture_page_id                     = tile.texture_id();
    const auto [texture_index, texture_page_width] = [&]() {
      if (std::ranges::empty(m_upscale_path))
      {
        return index_and_page_width(bpp, palette);
      }
      return index_and_page_width(palette, texture_page_id);
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
    const float tile_size = tile_scale * map_dims_statics::TileSize;
    // glm::vec2(m_mim.get_width(tile.depth()), m_mim.get_height());
    return m_map.get_front_version_of_back_tile(
      tile, [&](const auto &front_tile) {
        // todo maybe should have a toggle to force back tile.
        return std::optional<glengine::SubTexture>{
          std::in_place_t{},
          texture,
          glm::vec2{ front_tile.source_x() * tile_scale
                       + static_cast<float>(texture_page_offset),
                     texture_dims.y
                       - (front_tile.source_y() * tile_scale + tile_size) }
            / texture_dims,
          glm::vec2{ front_tile.source_x() * tile_scale
                       + static_cast<float>(texture_page_offset) + tile_size,
                     texture_dims.y - front_tile.source_y() * tile_scale }
            / texture_dims
        };
      });
  }
  glm::vec3 tile_to_draw_pos(const auto &tile) const
  {
    static constexpr typename TileFunctions::X           x{};
    static constexpr typename TileFunctions::Y           y{};
    static constexpr typename TileFunctions::TexturePage texture_page{};
    return { (static_cast<float>(
                x(tile)
                + texture_page(tile) * map_dims_statics::TexturePageWidth)
              - m_map_dims.offset.x)
               * m_map_dims.tile_scale,
             (m_map_dims.offset.y - static_cast<float>(y(tile)))
               * m_map_dims.tile_scale,
             0.F };
  }
  auto visit_tiles(auto &&lambda) const
  {
    return m_map.back().visit_tiles([&](const auto &tiles) {
      auto f_tiles =
        tiles | std::views::filter(tile_operations::InvalidTile{})
        | std::views::filter([]([[maybe_unused]] const auto &tile) -> bool {
            if constexpr (typename TileFunctions::UseBlending{})
            {
              return tile.draw();
            }
            else
            {
              return true;
            }
          })
        | std::views::filter(m_filters);
      std::vector<std::uint16_t> unique_z{};
      {
        // unique_z.reserve(std::ranges::size(tiles));
        std::ranges::transform(
          f_tiles, std::back_inserter(unique_z), tile_operations::Z{});
        std::ranges::sort(unique_z);
        auto [begin, end] = std::ranges::unique(unique_z);
        unique_z.erase(begin, end);
      }

      auto unique_z_reverse = unique_z | std::views::reverse;
      for (const auto z : unique_z_reverse)
      {
        auto f_tiles_reverse_filter_z =
          f_tiles | std::views::reverse
          | std::views::filter(tile_operations::ZMatch{ z });
        for (const auto &tile : f_tiles_reverse_filter_z)
        {
          if (!lambda(tile))
            return;
        }
      }
    });
  }
  // draws tiles
  void render_tiles() const
  {
    using open_viii::graphics::background::BlendModeT;
    BlendModeT last_blend_mode{ BlendModeT::none };
    m_uniform_color = s_default_color;
    glengine::Window::default_blend();
    m_imgui_viewport_window.on_render();
    set_uniforms();
    m_batch_renderer.clear();
    visit_tiles([this, &last_blend_mode](const auto &tile) -> bool {
      auto sub_texture = tile_to_sub_texture(tile);
      if (!sub_texture)
      {
        return true;
      }
      update_blend_mode(tile, last_blend_mode);
      m_batch_renderer.draw_quad(
        *sub_texture, tile_to_draw_pos(tile), m_map_dims.scaled_tile_size());
      return true;
    });
    m_batch_renderer.draw();
    m_batch_renderer.on_render();
    glengine::Window::default_blend();
    m_uniform_color = s_default_color;
  }
  void update_blend_mode(
    [[maybe_unused]] const auto &tile,
    [[maybe_unused]] open_viii::graphics::background::BlendModeT
      &last_blend_mode) const
  {
    if constexpr (typename TileFunctions::UseBlending{})
    {
      if (!s_blending)
      {
        return;
      }
      auto blend_mode = tile.blend_mode();
      if (blend_mode != last_blend_mode)
      {
        m_batch_renderer.draw();// flush buffer.
        last_blend_mode = blend_mode;
        if (s_blends.percent_blend_enabled())
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
            s_blends.set_add_blend();
          }
          break;
          case open_viii::graphics::background::BlendModeT ::subtract: {
            s_blends.set_subtract_blend();
          }
          break;
          default:
            glengine::Window::default_blend();
        }
      }
    }
  }
  struct [[nodiscard]] IndexAndPageWidthReturn
  {
    std::size_t  texture_index      = {};
    std::int16_t texture_page_width = { map_dims_statics::TexturePageWidth };
  };

  [[nodiscard]] static auto
    index_and_page_width(open_viii::graphics::BPPT bpp, std::uint8_t palette)
  {
    IndexAndPageWidthReturn r = { .texture_index = palette };
    if (bpp.bpp8())
    {
      r.texture_index      = 16 + palette;
      r.texture_page_width = map_dims_statics::TexturePageWidth / 2;
    }
    else if (bpp.bpp16())
    {
      r.texture_index      = 16 * 2;
      r.texture_page_width = map_dims_statics::TexturePageWidth / 4;
    }
    return r;
  }
  [[maybe_unused]] [[nodiscard]] auto
    index_and_page_width(std::uint8_t palette, std::uint8_t texture_page) const
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

  void render_frame_buffer() const
  {
    glengine::Window::default_blend();
    m_imgui_viewport_window.on_render();
    set_uniforms();
    m_batch_renderer.clear();
    m_batch_renderer.draw_quad(
      m_frame_buffer.get_color_attachment(),
      m_map_dims.scaled_position(),
      glm::vec2(
        m_frame_buffer.specification().width,
        m_frame_buffer.specification().height));
    m_batch_renderer.draw();
    m_batch_renderer.on_render();
  }

  void render_frame_buffer_grid() const
  {
    // todo remove unneeded code
    // todo maybe use transparent texture to render grid
    if (!s_draw_grid)
    {
      return;
    }
    const auto fbb = glengine::FrameBufferBackup{};
    const auto offscreen_pop =
      glengine::ScopeGuardCaptures([&]() { m_offscreen_drawing = false; });
    m_offscreen_drawing = true;
    glengine::Window::default_blend();
    m_imgui_viewport_window.on_render();
    set_uniforms();
    m_frame_buffer.bind();
    GlCall{}(
      glViewport,
      0,
      0,
      m_frame_buffer.specification().width,
      m_frame_buffer.specification().height);
    m_batch_renderer.shader().set_uniform(
      "u_Grid", m_map_dims.scaled_tile_size());
    m_batch_renderer.clear();
    m_batch_renderer.draw_quad(
      m_frame_buffer.get_color_attachment(),
      m_map_dims.scaled_position(),
      glm::vec2(
        m_frame_buffer.specification().width,
        m_frame_buffer.specification().height));
    m_batch_renderer.draw();
    m_batch_renderer.on_render();
  }
  void save() const
  {
    m_saving = true;
    const auto not_saving =
      glengine::ScopeGuardCaptures([&]() { m_saving = false; });
    if (s_draw_grid)
    {
      m_changed();
    }
    const auto changed = glengine::ScopeGuardCaptures([&]() {
      if (s_draw_grid)
      {
        m_changed();
      }
    });
    on_render();
    const auto path = std::filesystem::path(m_map_path);
    auto       string =
      fmt::format("{}_map.png", (path.parent_path() / path.stem()).string());
    glengine::PixelBuffer pixel_buffer{ m_frame_buffer.specification() };
    pixel_buffer.         operator()(m_frame_buffer, string);
    while (pixel_buffer.operator()(&glengine::Texture::save))
      ;
  }
  auto visit_unsorted_unfiltered_tiles_count() const
  {
    return m_map.back().visit_tiles([&](const auto &tiles) -> std::size_t {
      auto f_tiles = tiles | std::views::filter(tile_operations::InvalidTile{});
      return static_cast<std::size_t>(
        std::ranges::count_if(f_tiles, [](auto &&) { return true; }));
    });
  }
  bool visit_unsorted_unfiltered_tiles(auto &&lambda) const
  {
    return m_map.back().visit_tiles([&](auto &&tiles) -> bool {
      auto f_tiles = tiles | std::views::filter(tile_operations::InvalidTile{});
      bool changed = false;
      VisitState visit_state = {};
      for (auto &tile : f_tiles)
      {
        changed = lambda(tile, visit_state) || changed;
        if (visit_state == VisitState::ShortCircuit)
        {
          break;
        }
      }
      if (visit_state == VisitState::Undo)
      {
        changed = m_map.undo();
        //        m_changed.enable_undo();
      }
      if (visit_state == VisitState::UndoAll)
      {
        changed = true;
        m_map.undo_all();
      }
      return changed;
    });
  }
  void visit_unsorted_unfiltered_tiles() const
  {
    // s_camera.RefreshAspectRatio(m_imgui_viewport_window.ViewPortAspectRatio());
    const glm::vec2 size = m_map_dims.scaled_size();
    m_imgui_viewport_window.set_image_bounds(size);
    m_fixed_render_camera.set_projection(size);
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

  std::filesystem::path                m_upscale_path   = {};
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
  MapHistory                           m_map            = {};
  // dimensions of map
  MapDims<TileFunctions>               m_map_dims       = { m_map.back() };
  // loads the textures overtime instead of forcing them to load at start.
  glengine::DelayedTextures<35U>       m_delayed_textures = {};
  glengine::DelayedTextures<17U * 13U>
    m_upscale_delayed_textures = {};// 20 is detected max 16(+1)*13 is
                                    // possible max. 0 being no palette and
                                    // 1-17 being with palettes
  // takes quads and draws them to the frame buffer or screen.
  glengine::BatchRenderer           m_batch_renderer    = { 1000 };
  // holds rendered image at 1:1 scale to prevent gaps when scaling.
  mutable glengine::FrameBuffer     m_frame_buffer      = {};
  mutable bool                      m_offscreen_drawing = { false };
  mutable bool                      m_saving            = { false };
  mutable bool                      m_preview           = { false };
  inline constinit static MapBlends s_blends            = {};
  mutable MapFilters                m_filters           = { m_map.back() };
  struct Changed
  {
    void operator=(bool) const = delete;
    void set_if_true(
      bool                 in,
      std::source_location source_location =
        std::source_location::current()) const
    {
      if (in)
        operator()(source_location);
    }
    void operator()(
      std::source_location source_location =
        std::source_location::current()) const
    {
      if (!m_current)
      {
        spdlog::debug(
          "Changed\n\r{}:{}",
          source_location.file_name(),
          source_location.line());
        m_previous = m_current;
        m_current  = true;
      }
    }
    [[nodiscard]] operator bool() const
    {
      return m_current;
    }
    [[nodiscard]] bool previous() const
    {
      return m_previous;
    }
    [[nodiscard]] auto unset() const
    {
      return glengine::ScopeGuardCaptures([this] {
        m_previous = m_current;
        m_current  = false;
      });
    }
    //    void enable_undo() const
    //    {
    //      m_undo = true;
    //    }
    //    [[nodiscard]] bool undo() const
    //    {
    //      return m_undo;
    //    }
    //    void disable_undo() const
    //    {
    //      m_undo = false;
    //      // return glengine::ScopeGuardCaptures([this] { m_undo = false;
    //      });
    //    }

  private:
    mutable bool m_current        = { true };
    mutable bool m_previous       = { false };
    mutable bool m_was_mouse_down = { false };
    mutable bool m_undo           = { false };
  };
  Changed                       m_changed               = {};
  glengine::Counter             m_id                    = {};
  mutable std::vector<bool>     m_tile_button_state     = {};
  glengine::ImGuiViewPortWindow m_imgui_viewport_window = {
    TileFunctions::label
  };
  mutable SimilarAdjustments m_similar = {};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAP_HPP