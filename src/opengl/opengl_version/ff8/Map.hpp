//
// Created by pcvii on 1/11/2022.
//

#ifndef FIELD_MAP_EDITOR_MAP_HPP
#define FIELD_MAP_EDITOR_MAP_HPP
#include "MapChild.hpp"
namespace ff8
{
template<typename TileFunctions>
class Map
{
private:
  MapChild m_child = {};

public:
  Map() = default;
  Map(const Fields &fields)
    : Map(fields, {})
  {
  }
  Map(const Fields &fields, std::string upscale_path)
    : m_child{ TileFunctions::Label, fields, upscale_path }
  {
    m_child.m_using_blending = do_we_use_blending();
    SetCameraBoundsToEdgesOfImage();
  }
  void OnUpdate(float ts) const
  {
    m_child.OnUpdate(ts);
  }
  void OnRender() const
  {
    m_child.RenderTiles([this]() { RenderTiles(); });
  }
  void OnImGuiUpdate() const
  {
    m_child.OnImGuiUpdate();
    m_child.SaveButton([this]() { RenderTiles(); });
  }
  void OnEvent(const glengine::Event::Item &event) const
  {
    m_child.OnEvent(event);
  }

private:

//  std::optional<glengine::SubTexture> TileToSubTexture(const auto &tile) const
//  {
//    const auto bpp                                 = tile.depth();
//    const auto palette                             = tile.palette_id();
//    const auto texture_page_id                     = tile.texture_id();
//    const auto [texture_index, texture_page_width] = [&]() {
//      if (std::ranges::empty(m_upscale_path))
//        return IndexAndPageWidth(bpp, palette);
//      return IndexAndPageWidth(palette, texture_page_id);
//    }();
//
//    const auto texture_page_offset =
//      [&, texture_page_width_copy = texture_page_width]() {
//        if (std::ranges::empty(m_upscale_path))
//          return texture_page_id * texture_page_width_copy;
//        return 0;
//      }();
//    const auto &texture =
//      std::ranges::empty(m_upscale_path)
//        ? m_delayed_textures.textures->at(texture_index)
//        : m_upscale_delayed_textures.textures->at(texture_index);
//    if (texture.width() == 0 || texture.height() == 0)
//      return std::nullopt;
//    const auto  texture_dims = glm::vec2{ texture.width(), texture.height() };
//    const float tile_scale   = static_cast<float>(texture.height())
//                             / static_cast<float>(m_mim.get_height());
//    const float tile_size = tile_scale * 16.F;
//    // glm::vec2(m_mim.get_width(tile.depth()), m_mim.get_height());
//    return std::optional<glengine::SubTexture>{
//      std::in_place_t{},
//      texture,
//      glm::vec2{ tile.source_x() * tile_scale
//                   + static_cast<float>(texture_page_offset),
//                 texture_dims.y - (tile.source_y() * tile_scale + tile_size) }
//        / texture_dims,
//      glm::vec2{ tile.source_x() * tile_scale
//                   + static_cast<float>(texture_page_offset) + tile_size,
//                 texture_dims.y - tile.source_y() * tile_scale }
//        / texture_dims
//    };
//  }

//  glm::vec2 TileSize() const
//  {
//    const auto scaled_size = m_tile_scale * 16.F;
//    return { scaled_size, scaled_size };
//  }
//  auto VisitTiles(auto &&lambda) const
//  {
//    return m_map.back().visit_tiles([&](const auto &tiles) {
//      auto f_tiles = tiles | std::views::filter(filter_invalid)
//                     | std::views::filter(filter_use_blending_and_draw)
//                     | std::views::filter(m_filters.TestTile());
//      std::vector<std::uint16_t> unique_z{};
//      {
//        // unique_z.reserve(std::ranges::size(tiles));
//        std::ranges::transform(f_tiles, std::back_inserter(unique_z), get_z);
//        std::ranges::sort(unique_z);
//        auto [begin, end] = std::ranges::unique(unique_z);
//        unique_z.erase(begin, end);
//      }
//
//      auto unique_z_reverse = unique_z | std::views::reverse;
//      for (const auto z : unique_z_reverse)
//      {
//        auto f_tiles_reverse_filter_z =
//          f_tiles | std::views::reverse | std::views::filter(get_match_z(z));
//        for (const auto &tile : f_tiles_reverse_filter_z)
//        {
//          if (!lambda(tile))
//            return;
//        }
//      }
//    });
//  }
  // draws tiles
  template<typename TileT>
  glm::vec3 TileToDrawPos(const TileT &tile) const
  {
    static constexpr auto x            = get_x<TileT>();
    static constexpr auto y            = get_y<TileT>();
    static constexpr auto texture_page = get_texture_page<TileT>();
    return { (static_cast<float>(
                x(tile) + texture_page(tile) * m_child.s_texture_page_width)
              - m_child.m_offset_x)
               * m_child.m_tile_scale,
             (m_child.m_offset_y - static_cast<float>(y(tile))) * m_child.m_tile_scale,
             0.F };
  }
  void RenderTiles() const
  {
    using open_viii::graphics::background::BlendModeT;
    m_child.m_uniform_color = m_child.s_default_uniform_color;
    glengine::Window::DefaultBlend();
    m_child.m_imgui_viewport_window.OnRender();
    m_child.SetUniforms();
    m_child.m_batch_renderer.Clear();
    m_child.VisitTiles(
      [this,
       last_blend_mode{ BlendModeT::none }](const auto &tile) mutable -> bool {
        if (const auto sub_texture = m_child.TileToSubTexture(tile); sub_texture)
        {
          m_child.UpdateBlendMode(tile, last_blend_mode);
          m_child.m_batch_renderer.DrawQuad(
            *sub_texture, TileToDrawPos(tile), m_child.TileSize());
        }
        return true;
      });
    m_child.m_batch_renderer.Draw();
    m_child.m_batch_renderer.OnRender();
    glengine::Window::DefaultBlend();
    m_child.m_uniform_color = m_child.s_default_uniform_color;
  }
//  void UpdateBlendMode(
//    const auto                                  &tile,
//    open_viii::graphics::background::BlendModeT &last_blend_mode) const
//  {
//    if (!do_we_use_blending() || !s_blending)
//      return;
//    auto blend_mode = tile.blend_mode();
//    if (blend_mode != last_blend_mode)
//    {
//      m_batch_renderer.Draw();// flush buffer.
//      last_blend_mode = blend_mode;
//      if (s_blends.PercentBlendEnabled())
//      {
//        switch (blend_mode)
//        {
//          case open_viii::graphics::background::BlendModeT::half_add:
//            m_uniform_color = s_half_uniform_color;
//            break;
//          case open_viii::graphics::background::BlendModeT::quarter_add:
//            m_uniform_color = s_quarter_uniform_color;
//            break;
//          default:
//            m_uniform_color = s_default_uniform_color;
//            break;
//        }
//      }
//      switch (blend_mode)
//      {
//        case open_viii::graphics::background::BlendModeT::half_add:
//        case open_viii::graphics::background::BlendModeT::quarter_add:
//        case open_viii::graphics::background::BlendModeT::add: {
//          s_blends.SetAddBlend();
//        }
//        break;
//        case open_viii::graphics::background::BlendModeT ::subtract: {
//          s_blends.SetSubtractBlend();
//        }
//        break;
//        default:
//          glengine::Window::DefaultBlend();
//      }
//    }
//  }
//  struct IndexAndPageWidthReturn
//  {
//    std::size_t  texture_index      = {};
//    std::int16_t texture_page_width = { s_texture_page_width };
//  };

//  auto
//    IndexAndPageWidth(open_viii::graphics::BPPT bpp, std::uint8_t palette) const
//  {
//    IndexAndPageWidthReturn r = { .texture_index = palette };
//    if (bpp.bpp8())
//    {
//      r.texture_index      = 16 + palette;
//      r.texture_page_width = s_texture_page_width / 2;
//    }
//    else if (bpp.bpp16())
//    {
//      r.texture_index      = 16 * 2;
//      r.texture_page_width = s_texture_page_width / 4;
//    }
//    return r;
//  }
//  auto IndexAndPageWidth(std::uint8_t palette, std::uint8_t texture_page) const
//  {
//    IndexAndPageWidthReturn r = { .texture_index = static_cast<size_t>(
//                                    texture_page + 13U * (palette + 1U)) };
//    if (!m_upscale_delayed_textures.textures->at(r.texture_index))
//    {
//      r.texture_index = texture_page;
//    }
//    return r;
//  }
//
//  void RenderFrameBuffer() const
//  {
//    glengine::Window::DefaultBlend();
//    m_child.m_imgui_viewport_window.OnRender();
//    m_child.SetUniforms();
//    m_child.m_batch_renderer.Clear();
//    m_child.m_batch_renderer.DrawQuad(
//      m_child.m_frame_buffer.GetColorAttachment(),
//      m_child.m_position * m_child.m_tile_scale,
//      glm::vec2(
//        m_child.m_frame_buffer.Specification().width,
//        m_child.m_frame_buffer.Specification().height));
//    m_child.m_batch_renderer.Draw();
//    m_child.m_batch_renderer.OnRender();
//  }
//  void Save() const
//  {
//    const auto stop_saving = start_saving();
//    OnRender();
//    const auto path = std::filesystem::path(m_map_path);
//    auto       string =
//      fmt::format("{}_map.png", (path.parent_path() / path.stem()).string());
//    glengine::PixelBuffer pixel_buffer{ m_frame_buffer.Specification() };
//    pixel_buffer.         operator()(m_frame_buffer, string);
//    while (pixel_buffer.operator()(&glengine::Texture::save))
//      ;
//  }
//  [[nodiscard]] auto PushColor(const ImVec4 &color) const
//  {
//    ImGui::PushStyleColor(ImGuiCol_Button, color);
//    // ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
//    // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
//    return glengine::scope_guard([]() { ImGui::PopStyleColor(1); });
//  }
//  [[nodiscard]] auto start_saving() const
//  {
//    m_saving = true;
//    return glengine::scope_guard_captures([&]() { m_saving = false; });
//  }
//  [[nodiscard]] auto VisitUnSortedUnFilteredTilesCount() const
//  {
//    return m_map.back().visit_tiles([&](const auto &tiles) -> std::size_t {
//      auto f_tiles = tiles | std::views::filter(filter_invalid);
//      return static_cast<std::size_t>(std::ranges::count_if(f_tiles, all_true));
//    });
//  }
//  bool VisitUnSortedUnFilteredTiles(auto &&lamda) const
//  {
//    return m_map.back().visit_tiles([&](auto &&tiles) -> bool {
//      auto f_tiles = tiles | std::views::filter(filter_invalid);
//      bool changed = false;
//      for (auto &tile : f_tiles)
//      {
//        changed = lamda(tile) || changed;
//      }
//      return changed;
//    });
//  }
  void SetCameraBoundsToEdgesOfImage()
  {
    m_child.m_map.back().visit_tiles([&]<typename TilesT>(const TilesT &tiles) {
      static constexpr auto x            = get_x<TilesT>();
      static constexpr auto y            = get_y<TilesT>();
      static constexpr auto texture_page = get_texture_page<TilesT>();

      auto f_tiles            = tiles | std::views::filter(filter_invalid);
      auto [i_min_x, i_max_x] = std::ranges::minmax_element(f_tiles, {}, x);
      auto [i_min_y, i_max_y] = std::ranges::minmax_element(f_tiles, {}, y);
      auto [true_i_min_x, true_i_max_x] =
        std::ranges::minmax_element(f_tiles, {}, true_x);
      auto [true_i_min_y, true_i_max_y] =
        std::ranges::minmax_element(f_tiles, {}, true_y);
      auto i_max_texture_page =
        std::ranges::max_element(f_tiles, {}, texture_page);

      if (i_min_x == i_max_x || i_min_y == i_max_y)
      {
        return;
      }
      m_child.true_min_xy = glm::vec2(true_x(*true_i_min_x), true_y(*true_i_min_y));
      m_child.true_max_xy = glm::vec2(true_x(*true_i_max_x), true_y(*true_i_max_y));
      m_child.min_x       = x(*i_min_x);
      m_child.max_x       = static_cast<float>(
        m_child.m_using_blending
                ? x(*i_max_x)
                : (texture_page(*i_max_texture_page) + 1) * m_child.s_texture_page_width);
      m_child.min_y             = y(*i_min_y);
      m_child.max_y             = y(*i_max_y);
      const auto width  = m_child.max_x - m_child.min_x + 16.F;
      const auto height = m_child.max_y - m_child.min_y + 16.F;
      m_child.m_offset_x        = width / 2.F + m_child.min_x;
      m_child.m_offset_y        = height / 2.F + m_child.min_y - 16.F;
      m_child.m_position        = glm::vec3(-width / 2.F, -height / 2.F, 0.F);
      m_child.m_imgui_viewport_window.SetImageBounds(glm::vec2{ width, height });
      m_child.m_fixed_render_camera.SetProjection({ width, height });
      m_child.m_frame_buffer = glengine::FrameBuffer(glengine::FrameBufferSpecification{
        .width  = static_cast<int>(abs(width)),
        .height = static_cast<int>(abs(height)) });
    });
  }
  constexpr static auto filter_invalid =
    open_viii::graphics::background::Map::filter_invalid();
//  constexpr static auto filter_use_blending_and_draw =
//    [](const auto &tile) -> bool {
//    static constexpr typename TileFunctions::template Bounds<
//      std::remove_cvref_t<decltype(tile)>>::use_blending use_blending{};
//    if (use_blending)
//    {
//      return tile.draw();
//    }
//    return true;
//  };
//  constexpr static auto all_true       = [](auto &&) { return true; };
//  constexpr static auto get_palette_id = [](const auto &tile) {
//    return tile.palette_id();
//  };
//  constexpr static auto get_z = [](const auto &tile) { return tile.z(); };
  template<typename T>
    requires(std::ranges::range<std::remove_cvref_t<T>>)
  static constexpr auto get_x()
  {
    using function_t =
      typename TileFunctions::template Bounds<std::ranges::range_value_t<T>>::x;
    return function_t{};
  }
  template<typename T>
    requires(!std::ranges::range<std::remove_cvref_t<T>>)
  static constexpr auto get_x()
  {
    using function_t =
      typename TileFunctions::template Bounds<std::remove_cvref_t<T>>::x;
    return function_t{};
  }
  template<typename T>
    requires(std::ranges::range<std::remove_cvref_t<T>>)
  static constexpr auto get_y()
  {
    using function_t =
      typename TileFunctions::template Bounds<std::ranges::range_value_t<T>>::y;
    return function_t{};
  }
  template<typename T>
    requires(!std::ranges::range<std::remove_cvref_t<T>>)
  static constexpr auto get_y()
  {
    using function_t =
      typename TileFunctions::template Bounds<std::remove_cvref_t<T>>::y;
    return function_t{};
  }

  template<typename T>
    requires(std::ranges::range<std::remove_cvref_t<T>>)
  static constexpr auto get_texture_page()
  {
    using function_t = typename TileFunctions::template Bounds<
      std::ranges::range_value_t<T>>::texture_page;
    return function_t{};
  }
  template<typename T>
    requires(!std::ranges::range<std::remove_cvref_t<T>>)
  static constexpr auto get_texture_page()
  {
    using function_t = typename TileFunctions::template Bounds<
      std::remove_cvref_t<T>>::texture_page;
    return function_t{};
  }
//  constexpr static auto get_match_z(auto z)
//  {
//    return [z](const auto &tile) { return z == get_z(tile); };
//  }
  constexpr static auto get_texture_height =
    [](const glengine::Texture &texture) { return texture.height(); };
  static constexpr auto true_x = [](const auto &tile) { return tile.x(); };
  static constexpr auto true_y = [](const auto &tile) { return tile.y(); };
  bool                  do_we_use_blending() const
  {
    return m_child.m_map.back().visit_tiles([](auto &&tiles) -> bool {
      using tileT = std::ranges::range_value_t<decltype(tiles)>;
      return typename TileFunctions::template Bounds<
        std::remove_cvref_t<tileT>>::use_blending{};
    });
  }
  bool do_we_use_texture_page() const
  {
    return m_child.m_map.back().visit_tiles([](auto &&tiles) -> bool {
      using tileT = std::ranges::range_value_t<decltype(tiles)>;
      return typename TileFunctions::template Bounds<tileT>::use_texture_page{};
    });
  }
  //  mutable glengine::OrthographicCamera m_fixed_render_camera = {};
  //  inline static bool                   s_fit_height          = { true };
  //  inline static bool                   s_fit_width           = { true };
  //  inline static bool                   s_draw_grid           = { false };
  //  inline static bool                   s_blending            = { true };
  //
  //  static constexpr int16_t             s_texture_page_width  = 256;
  //
  //
  //  static constexpr glm::vec4 s_default_uniform_color = { 1.F, 1.F, 1.F, 1.F
  //  }; static constexpr glm::vec4 s_half_uniform_color    = { .5F, .5F, .5F,
  //  .5F }; static constexpr glm::vec4 s_quarter_uniform_color = { .25F,
  //                                                         .25F,
  //                                                         .25F,
  //                                                         .25F };
  //  inline static glm::vec4    m_uniform_color         =
  //  s_default_uniform_color;
  //
  //  std::string                m_upscale_path          = {};
  //  // internal mim file path
  //  std::string                m_mim_path              = {};
  //  // internal map file path
  //  std::string                m_map_path              = {};
  //  // if coo was chosen instead of default.
  //  bool                       m_mim_choose_coo        = {};
  //  // if coo was chosen instead of default.
  //  bool                       m_map_choose_coo        = {};
  //  // container for field textures
  //  open_viii::graphics::background::Mim m_mim         = {};
  //  // container for field tile information
  //  mutable MapHistory                   m_map         = {};
  //  // loads the textures overtime instead of forcing them to load at start.
  //  glengine::DelayedTextures<35U>       m_delayed_textures = {};
  //  glengine::DelayedTextures<17U * 13U>
  //    m_upscale_delayed_textures = {};// 20 is detected max 16(+1)*13 is
  //    possible
  //                                    // max. 0 being no palette and 1-17
  //                                    being
  //                                    // with palettes
  //  // takes quads and draws them to the frame buffer or screen.
  //  glengine::BatchRenderer           m_batch_renderer        = { 1000 };
  //  // holds rendered image at 1:1 scale to prevent gaps when scaling.
  //  mutable glengine::FrameBuffer     m_frame_buffer          = {};
  //  mutable float                     m_offset_x              = 0.F;
  //  mutable float                     m_offset_y              = -16.F;
  //  mutable bool                      m_offscreen_drawing     = { false };
  //  mutable bool                      m_saving                = { false };
  //  mutable bool                      m_preview               = { false };
  //  glm::vec3                         m_position              = {};
  //  inline constinit static MapBlends s_blends                = {};
  //  mutable MapFilters                m_filters               = {
  //  m_map.front() }; mutable bool                      m_changed = { true };
  //  mutable glm::vec2                 true_min_xy             = {};
  //  mutable glm::vec2                 true_max_xy             = {};
  //  mutable float                     min_x                   = {};
  //  mutable float                     min_y                   = {};
  //  mutable float                     max_x                   = {};
  //  mutable float                     max_y                   = {};
  //  mutable float                     m_tile_scale            = { 1.F };
  //  glengine::Counter                 m_id                    = {};
  //  mutable std::vector<bool>         m_tile_button_state     = {};
  //  glengine::ImGuiViewPortWindow     m_imgui_viewport_window = {
  //        TileFunctions::Label
  //  };
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAP_HPP