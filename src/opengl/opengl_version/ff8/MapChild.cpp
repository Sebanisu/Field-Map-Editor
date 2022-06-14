//
// Created by pcvii on 6/7/2022.
//

#include "MapChild.hpp"


namespace ff8
{
constexpr static auto all_true = [](auto &&) { return true; };
static std::pair<float, float> generate_inner_width(int components)
{
  components = std::clamp(components, 1, std::numeric_limits<int>::max());
  const float f_count    = static_cast<float>(components);
  const auto &style      = ImGui::GetStyle();
  const float w_full     = ImGui::CalcItemWidth();
  const float w_item_one = (std::max)(
    1.0f,
    std::floor(
      (w_full - (style.ItemInnerSpacing.x) * static_cast<float>(components - 1))
      / f_count));
  const float w_item_last = (std::max)(
    1.0f,
    std::floor(
      w_full
      - (w_item_one + style.ItemInnerSpacing.x)
          * static_cast<float>(components - 1)));
  return { w_item_one, w_item_last };
}

[[nodiscard]] static std::size_t
  VisitUnSortedUnFilteredTilesCount(MapHistory &map)
{
  return map.back().visit_tiles([&](const auto &tiles) -> std::size_t {
    auto f_tiles = tiles | std::views::filter(MapChild::filter_invalid);
    return static_cast<std::size_t>(std::ranges::count_if(f_tiles, all_true));
  });
}

[[nodiscard]] bool VisitUnSortedUnFilteredTiles(MapHistory &map, auto &&lamda)
{
  return map.back().visit_tiles([&](auto &&tiles) -> bool {
    auto f_tiles = tiles | std::views::filter(MapChild::filter_invalid);
    bool changed = false;
    for (auto &tile : f_tiles)
    {
      changed = lamda(tile) || changed;
    }
    return changed;
  });
}
static void CheckBoxDraw(const bool using_blending, auto &tile, bool &changed)
{
  bool draw = tile.draw();
  if (ImGui::Checkbox("Draw?", &draw))
  {
    if (using_blending)
    {
      // this won't display change on swizzle because if we skip
      // those tiles they won't output to the image file.
      changed = true;
    }
    tile = tile.with_draw(static_cast<decltype(tile.draw())>(draw));
  }
}
static void InputsReadOnly(const auto &tile, const int index, const int id)
{
  // const auto         disabled  = glengine::ImGuiDisabled(true);
  std::array<int, 2> tile_dims = { static_cast<int>(tile.width()),
                                   static_cast<int>(tile.height()) };
  ImGui::InputInt2(
    "Tile Dimensions", tile_dims.data(), ImGuiInputTextFlags_ReadOnly);
  std::string index_str = fmt::format("{}", index);
  ImGui::InputText(
    "Index", index_str.data(), index_str.size(), ImGuiInputTextFlags_ReadOnly);
  std::string id_str = fmt::format("{}", id);
  ImGui::InputText(
    "OpenGL Texture ID",
    id_str.data(),
    id_str.size(),
    ImGuiInputTextFlags_ReadOnly);
  std::string hex = [&]() -> std::string {
    std::stringstream ss = {};
    tile.to_hex(ss);
    return ss.str();
  }();
  ImGui::InputText(
    "Raw Hex", hex.data(), hex.size(), ImGuiInputTextFlags_ReadOnly);
}
static int
  ComboBPP(MapHistory &map, MapFilters &filters, auto &tile, bool &changed)
{
  std::array<const char *, 3> bpp_options           = { "4", "8", "16" };
  int                         current_bpp_selection = [&]() -> int {
    switch (static_cast<int>(tile.depth()))
    {
      case 4:
      default:
        return 0;
      case 8:
        return 1;
      case 16:
        return 2;
    }
  }();
  if (ImGui::Combo("BPP", &current_bpp_selection, bpp_options.data(), 3))
  {
    switch (current_bpp_selection)
    {
      case 0:
      default:
        tile = tile.with_depth(open_viii::graphics::BPPT(false, false, true));
        break;
      case 1:
        tile = tile.with_depth(open_viii::graphics::BPPT(true, false, true));
        break;
      case 2:
        tile = tile.with_depth(open_viii::graphics::BPPT(false, true, false));
        break;
    }
    filters.unique_tile_values().refresh_bpp(map.back());
    changed = true;
  }
  return current_bpp_selection;
}
static void
  SliderInt2SourceXY(auto &tile, bool &changed, const int current_bpp_selection)
{
  std::array<int, 2> source_xy = {
    static_cast<int>(tile.source_xy().x() / tile.width()),
    static_cast<int>(tile.source_xy().y() / tile.height())
  };
  const std::pair<float, float> item_width = generate_inner_width(2);
  {
    const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
    if (ImGui::SliderInt(
          "##Source (X)",
          &source_xy[0],
          0,
          (static_cast<int>(std::pow(2, (2 - current_bpp_selection) + 2) - 1))))
    {
      source_xy[0] *= tile.width();
      changed = true;
      tile    = tile.with_source_x(
        static_cast<decltype(tile.source_xy().x())>(source_xy[0]));
    }
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  {
    const auto pop_width = glengine::ImGuiPushItemWidth(item_width.second);
    if (ImGui::SliderInt(
          "##Source (Y)",
          &source_xy[1],
          std::numeric_limits<
            std::remove_cvref_t<decltype(tile.source_xy().y())>>::min()
            / tile.height(),
          std::numeric_limits<
            std::remove_cvref_t<decltype(tile.source_xy().y())>>::max()
            / tile.height()))
    {
      changed = true;
      source_xy[1] *= tile.height();
      tile = tile.with_source_y(
        static_cast<decltype(tile.source_xy().y())>(source_xy[1]));
    }
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text(
    "%s",
    fmt::format(
      "Source Pos: ({}, {})", tile.source_xy().x(), tile.source_xy().y())
      .c_str());
  // todo add second source for moving a tile to a new location in
  // the mim / swizzled map. Without changing the image.
}
static void SliderInt3XYZ(
  MapHistory      &map,
  MapFilters      &filters,
  const glm::vec2 &true_min_xy,
  const glm::vec2 &true_max_xy,
  auto            &tile,
  bool            &changed)
{
  std::array<int, 3> xyz = { static_cast<int>(tile.xy().x() / tile.width()),
                             static_cast<int>(tile.xy().y() / tile.height()),
                             static_cast<int>(tile.z()) };

  const std::pair<float, float> item_width = generate_inner_width(3);
  {
    const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
    if (ImGui::SliderInt(
          "##Destination (X)",
          &xyz[0],
          static_cast<int>(true_min_xy.x / tile.width()),
          static_cast<int>(true_max_xy.x / tile.width())))
    {
      changed = true;
      xyz[0] *= tile.width();
      tile = tile.with_x(static_cast<decltype(tile.xy().x())>(xyz[0]));
    }
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  {
    const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
    if (ImGui::SliderInt(
          "##Destination (Y)",
          &xyz[1],
          static_cast<int>(true_min_xy.y / tile.height()),
          static_cast<int>(true_max_xy.y / tile.height())))
    {
      changed = true;
      xyz[1] *= tile.height();
      tile = tile.with_y(static_cast<decltype(tile.xy().y())>(xyz[1]));
    }
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  {
    const auto pop_width = glengine::ImGuiPushItemWidth(item_width.second);
    if (ImGui::SliderInt("##Destination (Z)", &xyz[2], 0, 0xFFF))
    {
      changed = true;
      tile    = tile.with_z(static_cast<decltype(tile.z())>(xyz[2]));
      filters.unique_tile_values().refresh_z(map.back());
    }
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text(
    "%s",
    fmt::format(
      "Destination Pos: ({}, {}, {})", tile.xy().x(), tile.xy().y(), tile.z())
      .c_str());
}
static void ComboBlendModes(
  MapHistory &map,
  MapFilters &filters,
  auto       &tile,
  bool       &changed)
{
  using namespace open_viii::graphics::background;
  using TileT           = std::remove_cvref_t<decltype(tile)>;
  const auto disabled   = glengine::ImGuiDisabled(!has_with_blend_mode<TileT>);
  const auto blend_mode = tile.blend_mode();
  int        current_blend_mode_selection = static_cast<int>(blend_mode);
  const std::array<std::string_view, 5> blend_mode_str = {
    "half_add", "add", "subtract", "quarter_add", "none",
  };
  if (glengine::GenericCombo(
        "Blend Mode", current_blend_mode_selection, blend_mode_str))
  {
    if constexpr (has_with_blend_mode<TileT>)
    {
      changed = true;
      tile    = tile.with_blend_mode(
        static_cast<decltype(tile.blend_mode())>(current_blend_mode_selection));
      filters.unique_tile_values().refresh_blend_mode(map.back());
    }
  }
}
static void SliderIntLayerID(
  MapHistory &map,
  MapFilters &filters,
  auto       &tile,
  bool       &changed)
{
  using namespace open_viii::graphics::background;
  int        layer_id = tile.layer_id();
  const auto disabled = glengine::ImGuiDisabled(
    !has_with_layer_id<std::remove_cvref_t<decltype(tile)>>);
  if (ImGui::SliderInt(
        "Layer ID",
        &layer_id,
        std::numeric_limits<
          std::remove_cvref_t<decltype(tile.layer_id())>>::min(),
        std::numeric_limits<
          std::remove_cvref_t<decltype(tile.layer_id())>>::max()))
  {
    if constexpr (has_with_layer_id<std::remove_cvref_t<decltype(tile)>>)
    {
      changed = true;
      tile =
        tile.with_layer_id(static_cast<decltype(tile.layer_id())>(layer_id));
      filters.unique_tile_values().refresh_layer_id(map.back());
    }
  }
}
static void SliderIntTexturePageID(
  MapHistory &map,
  MapFilters &filters,
  auto       &tile,
  bool       &changed)
{
  int texture_page_id = static_cast<int>(tile.texture_id());
  if (ImGui::SliderInt("Texture Page ID", &texture_page_id, 0, 13))
  {
    changed = true;
    tile    = tile.with_texture_id(
      static_cast<decltype(tile.texture_id())>(texture_page_id));
    filters.unique_tile_values().refresh_texture_page_id(map.back());
  }
}
static void SliderIntPaletteID(
  MapHistory &map,
  MapFilters &filters,
  auto       &tile,
  bool       &changed)
{
  int palette_id = static_cast<int>(tile.palette_id());
  if (ImGui::SliderInt("Palette ID", &palette_id, 0, 16))
  {
    changed = true;
    tile    = tile.with_palette_id(
      static_cast<decltype(tile.palette_id())>(palette_id));
    filters.unique_tile_values().refresh_palette_id(map.back());
  }
}
static void SliderIntBlendOther(
  MapHistory &map,
  MapFilters &filters,
  auto       &tile,
  bool       &changed)
{
  int blend = tile.blend();
  if (ImGui::SliderInt(
        "Blend Other",
        &blend,
        std::numeric_limits<std::remove_cvref_t<decltype(tile.blend())>>::min(),
        std::numeric_limits<
          std::remove_cvref_t<decltype(tile.blend())>>::max()))
  {
    changed = true;
    tile    = tile.with_blend(static_cast<decltype(tile.blend())>(blend));
    filters.unique_tile_values().refresh_blend_other(map.back());
  }
}
static void SliderInt2Animation(
  MapHistory &map,
  MapFilters &filters,
  auto       &tile,
  bool       &changed)
{
  int                           animation_id    = tile.animation_id();
  int                           animation_state = tile.animation_state();
  const std::pair<float, float> item_width      = generate_inner_width(2);
  using namespace open_viii::graphics::background;
  using TileT = std::remove_cvref_t<decltype(tile)>;
  {
    const auto disabled =
      glengine::ImGuiDisabled(!has_with_animation_id<TileT>);
    const auto pop_width = glengine::ImGuiPushItemWidth(item_width.first);
    if (ImGui::SliderInt(
          "##Animation ID",
          &animation_id,
          std::numeric_limits<
            std::remove_cvref_t<decltype(tile.animation_id())>>::min(),
          std::numeric_limits<
            std::remove_cvref_t<decltype(tile.animation_id())>>::max()))
    {
      if constexpr (has_with_animation_id<TileT>)
      {
        changed = true;
        tile    = tile.with_animation_id(
          static_cast<decltype(tile.animation_id())>(animation_id));
        filters.unique_tile_values().refresh_animation_id(map.back());
      }
    }
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  {
    const auto disabled =
      glengine::ImGuiDisabled(!has_with_animation_state<TileT>);
    const auto pop_width = glengine::ImGuiPushItemWidth(item_width.second);
    if (ImGui::SliderInt(
          "##Animation State",
          &animation_state,
          std::numeric_limits<
            std::remove_cvref_t<decltype(tile.animation_state())>>::min(),
          std::numeric_limits<
            std::remove_cvref_t<decltype(tile.animation_state())>>::max()))
    {
      if constexpr (has_with_animation_state<TileT>)
      {
        changed = true;
        tile    = tile.with_animation_state(
          static_cast<decltype(tile.animation_state())>(animation_state));
        filters.unique_tile_values().refresh_animation_frame(map.back());
      }
    }
  }
  ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text(
    "%s",
    fmt::format("Animation: ({}, {})", animation_id, animation_state).c_str());
}

}// namespace ff8

[[maybe_unused]] ff8::MapChild::MapChild(const char *label, const ff8::Fields &fields)
  : MapChild(label, fields, {})
{
}
ff8::MapChild::MapChild(
  const char        *label,
  const ff8::Fields &fields,
  std::string        upscale_path)
  : m_upscale_path(std::move(upscale_path))
  , m_mim(LoadMim(fields.Field(), fields.Coo(), m_mim_path, m_mim_choose_coo))
  , m_map(LoadMap(
      fields.Field(),
      fields.Coo(),
      m_mim,
      m_map_path,
      m_map_choose_coo))
  , m_filters(m_map.front())
  , m_label(label)
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
  // GetUniqueValues();
  m_tile_button_state =
    std::vector<bool>(VisitUnSortedUnFilteredTilesCount(m_map), false);
}
void ff8::MapChild::OnUpdate(float ts) const
{
  if (m_delayed_textures.OnUpdate() || m_upscale_delayed_textures.OnUpdate())
  {
    if (!std::ranges::empty(m_upscale_path))
    {
      const auto current_max = (std::ranges::max_element)(
        *m_upscale_delayed_textures.textures, {}, get_texture_height);
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
void ff8::MapChild::OnRender() const
{
  RenderTiles([](){});
}
void ff8::MapChild::OnImGuiUpdate() const
{
  const auto popid = glengine::ImGuiPushID();
  {
    const auto disable = glengine::ImGuiDisabled(
      std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path));

    m_changed = std::ranges::any_of(
      std::array{ ImGui::Checkbox("Draw Grid", &s_draw_grid),
                  ImGui::Checkbox("Fit Height", &s_fit_height),
                  ImGui::Checkbox("Fit Width", &s_fit_width),
                  [&]() -> bool {
                    return !m_using_blending
                           || std::ranges::any_of(
                             std::array{
                               ImGui::Checkbox("Blending", &s_blending),
                               s_blends.OnImGuiUpdate() },
                             std::identity{});
                  }(),
                  m_filters.OnImGuiUpdate() },
      std::identity{});


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
    float      text_width = 0.F;
    ImVec2     last_pos   = {};
    const auto render_sub_texture =
      [&text_width, &last_pos, this](
        const glengine::SubTexture &sub_texture) -> bool {
      text_width = 0.F;
      const auto imgui_texture_id_ref =
        ConvertGLIDtoImTextureID(sub_texture.ID());
      const auto uv        = sub_texture.ImGuiUV();
      const auto id_pop    = glengine::ImGuiPushID();
      const auto color     = ImVec4(0.F, 0.F, 0.F, 0.F);
      last_pos             = ImGui::GetCursorPos();
      text_width           = ImGui::GetItemRectMax().x;
      const auto pop_color = PushColor(color);
      bool       value =
        ImGui::ImageButton(imgui_texture_id_ref, ImVec2(16, 16), uv[0], uv[1]);
      text_width = ImGui::GetStyle().ItemSpacing.x + ImGui::GetItemRectMax().x
                   - text_width;
      return value;
    };
    const auto  dims = ImGui::GetContentRegionAvail();
    std::size_t i    = {};
    if (VisitUnSortedUnFilteredTiles(m_map, [&](auto &tile) -> bool {
          using namespace open_viii::graphics::background;
          const auto id_pop_2    = glengine::ImGuiPushID();
          const auto sub_texture = TileToSubTexture(tile);
          const auto increment = glengine::scope_guard_captures([&]() { ++i; });
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
            CheckBoxDraw(m_using_blending, tile, changed);
            int current_bpp_selection =
              ComboBPP(m_map, m_filters, tile, changed);
            SliderInt2SourceXY(tile, changed, current_bpp_selection);
            SliderInt3XYZ(
              m_map, m_filters, true_min_xy, true_max_xy, tile, changed);
            SliderIntLayerID(m_map, m_filters, tile, changed);
            SliderIntBlendOther(m_map, m_filters, tile, changed);
            SliderIntPaletteID(m_map, m_filters, tile, changed);
            SliderIntTexturePageID(m_map, m_filters, tile, changed);
            SliderInt2Animation(m_map, m_filters, tile, changed);
            ComboBlendModes(m_map, m_filters, tile, changed);
            InputsReadOnly(
              tile,
              static_cast<int>(i),
              static_cast<int>(static_cast<uint32_t>(sub_texture->ID())));
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
void ff8::MapChild::OnEvent(const glengine::Event::Item &event) const
{
  m_imgui_viewport_window.OnEvent(event);
  m_batch_renderer.OnEvent(event);
}
void ff8::MapChild::SetUniforms() const
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
    m_uniform_color.r,
    m_uniform_color.g,
    m_uniform_color.b,
    m_uniform_color.a);
}
glm::vec2 ff8::MapChild::TileSize() const
{
  const auto scaled_size = m_tile_scale * 16.F;
  return { scaled_size, scaled_size };
}
ff8::MapChild::IndexAndPageWidthReturn ff8::MapChild::IndexAndPageWidth(
  open_viii::graphics::BPPT bpp,
  std::uint8_t              palette) const
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
ff8::MapChild::IndexAndPageWidthReturn ff8::MapChild::IndexAndPageWidth(
  std::uint8_t palette,
  std::uint8_t texture_page) const

{
  IndexAndPageWidthReturn r = { .texture_index = static_cast<size_t>(
                                  texture_page + 13U * (palette + 1U)) };
  if (!m_upscale_delayed_textures.textures->at(r.texture_index))
  {
    r.texture_index = texture_page;
  }
  return r;
}
void ff8::MapChild::RenderFrameBuffer() const
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
glengine::scope_guard ff8::MapChild::PushColor(const ImVec4 &color) const
{
  ImGui::PushStyleColor(ImGuiCol_Button, color);
  // ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
  // ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
  return glengine::scope_guard{ []() { ImGui::PopStyleColor(1); } };
}
