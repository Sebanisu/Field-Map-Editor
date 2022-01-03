//
// Created by pcvii on 11/30/2021.
//

#include "Map.hpp"
#include "Application.hpp"
#include "FrameBufferBackup.hpp"
#include "OrthographicCameraController.hpp"
#include "PixelBuffer.hpp"
#include "Window.hpp"
static OrthographicCameraController camera               = { 16 / 9 };
static glengine::OrthographicCamera fixed_render_camera  = {};
static bool                         snap_zoom_to_height  = true;
static bool                         draw_grid            = false;
static bool                         saving               = false;
static bool                         offscreen_drawing    = false;
static bool                         enable_percent_blend = true;
static float                        offset_y             = 0.F;
static constexpr glm::vec4 default_uniform_color = { 1.F, 1.F, 1.F, 1.F };
static constexpr glm::vec4 half_uniform_color    = { .5F, .5F, .5F, .5F };
static constexpr glm::vec4 quarter_uniform_color = { .25F, .25F, .25F, .25F };
static glm::vec4           uniform_color         = default_uniform_color;


static constexpr auto      parameters            = std::array{ GL_ZERO,
                                               GL_ONE,
                                               GL_SRC_COLOR,
                                               GL_ONE_MINUS_SRC_COLOR,
                                               GL_DST_COLOR,
                                               GL_ONE_MINUS_DST_COLOR,
                                               GL_SRC_ALPHA,
                                               GL_ONE_MINUS_SRC_ALPHA,
                                               GL_DST_ALPHA,
                                               GL_ONE_MINUS_DST_ALPHA,
                                               GL_CONSTANT_COLOR,
                                               GL_ONE_MINUS_CONSTANT_COLOR,
                                               GL_CONSTANT_ALPHA,
                                               GL_ONE_MINUS_CONSTANT_ALPHA,
                                               GL_SRC_ALPHA_SATURATE,
                                               GL_SRC1_COLOR,
                                               GL_ONE_MINUS_SRC_COLOR,
                                               GL_SRC1_ALPHA,
                                               GL_ONE_MINUS_SRC1_ALPHA };
static constexpr auto      parameters_string =
  std::array{ "GL_ZERO",// 0
              "GL_ONE",// 1
              "GL_SRC_COLOR",// 2
              "GL_ONE_MINUS_SRC_COLOR",// 3
              "GL_DST_COLOR",// 4
              "GL_ONE_MINUS_DST_COLOR",// 5
              "GL_SRC_ALPHA",// 6
              "GL_ONE_MINUS_SRC_ALPHA",// 7
              "GL_DST_ALPHA",// 8
              "GL_ONE_MINUS_DST_ALPHA",// 9
              "GL_CONSTANT_COLOR",// 10
              "GL_ONE_MINUS_CONSTANT_COLOR",// 11
              "GL_CONSTANT_ALPHA",// 12
              "GL_ONE_MINUS_CONSTANT_ALPHA",// 13
              "GL_SRC_ALPHA_SATURATE",// 14
              "GL_SRC1_COLOR",// 15
              "GL_ONE_MINUS_SRC_COLOR",// 16
              "GL_SRC1_ALPHA",// 17
              "GL_ONE_MINUS_SRC1_ALPHA" };// 18
static constexpr auto equations = std::array{ GL_FUNC_ADD,
                                              GL_FUNC_SUBTRACT,
                                              GL_FUNC_REVERSE_SUBTRACT,
                                              GL_MIN,
                                              GL_MAX };
static constexpr auto equations_string =
  std::array{ "GL_FUNC_ADD",// 0
              "GL_FUNC_SUBTRACT",// 1
              "GL_FUNC_REVERSE_SUBTRACT",// 2
              "GL_MIN",// 3
              "GL_MAX" };// 4
static std::array<int, 4> add_parameter_selections{ 2, 1, 6, 7 };
static std::array<int, 2> add_equation_selections{};
static std::array<int, 4> subtract_parameter_selections{ 4, 1, 6, 7 };
static std::array<int, 2> subtract_equation_selections{ 2, 0 };

ff8::Map::Map(const ff8::Fields &fields)
  : m_mim(LoadMim(fields.Field(), fields.Coo(), m_mim_path, m_mim_choose_coo))
  , m_map(LoadMap(
      fields.Field(),
      fields.Coo(),
      m_mim,
      m_map_path,
      m_map_choose_coo))
{
  if (!std::empty(m_mim_path))
  {
    fmt::print("Loaded {}\n", m_mim_path);
    fmt::print("Loaded {}\n", m_map_path);
    fmt::print("Loading Textures from Mim \n");
    m_delayed_textures = LoadTextures(m_mim);
  }
  camera.RefreshAspectRatio();
  m_map.visit_tiles([&](const auto &tiles) {
    auto f_tiles = tiles
                   | std::views::filter(
                     open_viii::graphics::background::Map::filter_invalid());
    auto [i_min_x, i_max_x] = std::ranges::minmax_element(
      f_tiles, {}, [](const auto &tile) { return tile.x(); });
    auto [i_min_y, i_max_y] = std::ranges::minmax_element(
      f_tiles, {}, [](const auto &tile) { return tile.y(); });

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
    offset_y          = static_cast<float>(min_y + max_y);

    camera.SetMaxBounds({ static_cast<float>(min_x),
                          static_cast<float>(max_x + 16),
                          static_cast<float>(min_y),
                          static_cast<float>(max_y + 16) });

    fixed_render_camera.SetProjection(
      static_cast<float>(min_x),
      static_cast<float>(max_x + 16),
      static_cast<float>(min_y),
      static_cast<float>(max_y + 16));
    m_frame_buffer = glengine::FrameBuffer(glengine::FrameBufferSpecification{
      .width = std::abs(width), .height = std::abs(height) });
  });
}

void ff8::Map::OnUpdate(float ts) const
{
  m_delayed_textures.check();


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
  if (offscreen_drawing || saving)
  {
    m_batch_renderer.Shader().SetUniform(
      "u_MVP", fixed_render_camera.ViewProjectionMatrix());
  }
  else
  {
    m_batch_renderer.Shader().SetUniform(
      "u_MVP", camera.Camera().ViewProjectionMatrix());
  }
  if (!draw_grid || offscreen_drawing || saving)
  {
    m_batch_renderer.Shader().SetUniform("u_Grid", 0.F, 0.F);
  }
  else
  {
    m_batch_renderer.Shader().SetUniform("u_Grid", 16.F, 16.F);
  }
  m_batch_renderer.Shader().SetUniform(
    "u_Color",
    uniform_color.r,
    uniform_color.g,
    uniform_color.b,
    uniform_color.a);
}

auto index_and_page_width(auto bpp, auto palette)
{
  struct
  {
    size_t texture_index      = {};
    int    texture_page_width = { 256 };
  } r = { .texture_index = palette };
  if (bpp.bpp8())
  {
    r.texture_index      = 16 + palette;
    r.texture_page_width = 128;
  }
  else if (bpp.bpp16())
  {
    r.texture_index      = 16 * 2;
    r.texture_page_width = 64;
  }
  return r;
}
void set_blend_mode_selections(
  const std::array<int, 4U> &parameters_selections,
  const std::array<int, 2U> &equation_selections)
{
  GLCall{}(
    glBlendFuncSeparate,
    parameters.at(static_cast<std::size_t>(parameters_selections[0])),
    parameters.at(static_cast<std::size_t>(parameters_selections[1])),
    parameters.at(static_cast<std::size_t>(parameters_selections[2])),
    parameters.at(static_cast<std::size_t>(parameters_selections[3])));
  GLCall{}(
    glBlendEquationSeparate,
    equations.at(static_cast<std::size_t>(equation_selections[0])),
    equations.at(static_cast<std::size_t>(equation_selections[1])));
}
void ff8::Map::OnRender() const
{
  if (std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path))
  {
    return;
  }
  {
    offscreen_drawing = true;
    const auto not_offscreen_drawing =
      scope_guard([]() { offscreen_drawing = false; });
    const auto fbb = glengine::FrameBufferBackup{};
    m_frame_buffer.Bind();
    GLCall{}(
      glViewport,
      0,
      0,
      m_frame_buffer.Specification().width,
      m_frame_buffer.Specification().height);
    Renderer::Clear();
    RenderTiles();
  }
  RestoreViewPortToFrameBuffer();
  RenderFrameBuffer();
}
void ff8::Map::RenderFrameBuffer() const
{
  if (saving)
  {
    return;
  }
  SetUniforms();
  m_batch_renderer.Clear();
  m_batch_renderer.DrawQuad(
    m_frame_buffer.GetColorAttachment(),
    glm::vec3(camera.MaxBounds()->left, camera.MaxBounds()->bottom, 0.F),
    glm::vec2(
      m_frame_buffer.Specification().width,
      m_frame_buffer.Specification().height));
  m_batch_renderer.OnRender();// flush buffer
}
void ff8::Map::RenderTiles() const
{
  using open_viii::graphics::background::BlendModeT;
  BlendModeT last_blend_mode{ BlendModeT::none };
  uniform_color = default_uniform_color;
  Window::DefaultBlend();
  camera.OnRender();
  SetUniforms();
  m_batch_renderer.Clear();
  m_map.visit_tiles([&](const auto &tiles) {
    auto f_tiles = tiles
                   | std::views::filter(
                     open_viii::graphics::background::Map::filter_invalid());
    //    const auto i_max_z = std::ranges::max_element(
    //      tiles, {}, [](const auto &tile) { return tile.z(); });
    //    if (i_max_z == std::ranges::end(tiles))
    //    {
    //      return;
    //    }
    //    const float                max_z = static_cast<float>(i_max_z->z());
    std::vector<std::uint16_t> unique_z{};
    {
      unique_z.reserve(std::ranges::size(tiles));
      std::ranges::transform(
        f_tiles, std::back_inserter(unique_z), [](const auto &tile) {
          return tile.z();
        });
      std::ranges::sort(unique_z);
      auto [begin, end] = std::ranges::unique(unique_z);
      unique_z.erase(begin, end);
    }

    for (const auto z : unique_z | std::views::reverse)
    {
      // fmt::print("z = {}\n", z);
      for (const auto &tile :
           f_tiles | std::views::reverse
             | std::views::filter([z](const auto &t) { return z == t.z(); }))
      {
        const auto bpp     = tile.depth();
        const auto palette = tile.palette_id();
        const auto [texture_index, texture_page_width] =
          index_and_page_width(bpp, palette);

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
        if (blend_mode != last_blend_mode)
        {
          m_batch_renderer.Draw();// flush buffer.
          last_blend_mode = blend_mode;
          if (enable_percent_blend)
          {
            switch (blend_mode)
            {
              case BlendModeT::half_add:
                uniform_color = half_uniform_color;
                break;
              case BlendModeT::quarter_add:
                uniform_color = quarter_uniform_color;
                break;
              default:
                uniform_color = default_uniform_color;
                break;
            }
          }
          switch (blend_mode)
          {
            case BlendModeT::half_add:
            case BlendModeT::quarter_add:
            case BlendModeT::add: {
              set_blend_mode_selections(
                add_parameter_selections, add_equation_selections);
            }
            break;
            case BlendModeT ::subtract: {
              set_blend_mode_selections(
                subtract_parameter_selections, subtract_equation_selections);
            }
            break;
            default:
              Window::DefaultBlend();
          }
        }
        m_batch_renderer.DrawQuad(
          sub_texture,
          glm::vec3(tile.x(), offset_y - tile.y(), 0.F),
          glm::vec2(16.F, 16.F));
      }
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
void Blend_Combos(
  std::array<int, 4U> &parameters_selections,
  std::array<int, 2U> &equation_selections)
{
  ImGui::Combo(
    "srcRGB",
    &parameters_selections[0],
    std::ranges::data(parameters_string),
    static_cast<int>(std::ranges::ssize(parameters_string)));
  ImGui::Combo(
    "dstRGB",
    &parameters_selections[1],
    std::ranges::data(parameters_string),
    static_cast<int>(std::ranges::ssize(parameters_string)));
  ImGui::Combo(
    "srcAlpha",
    &parameters_selections[2],
    std::ranges::data(parameters_string),
    static_cast<int>(std::ranges::ssize(parameters_string)));
  ImGui::Combo(
    "dstAlpha",
    &parameters_selections[3],
    std::ranges::data(parameters_string),
    static_cast<int>(std::ranges::ssize(parameters_string)));
  ImGui::Separator();
  ImGui::Combo(
    "modeRGB",
    &equation_selections[0],
    std::ranges::data(equations_string),
    static_cast<int>(std::ranges::ssize(equations_string)));
  ImGui::Combo(
    "modeAlpha",
    &equation_selections[1],
    std::ranges::data(equations_string),
    static_cast<int>(std::ranges::ssize(equations_string)));
}
void ff8::Map::OnImGuiUpdate() const
{
  {
    const auto disable = scope_guard(&ImGui::EndDisabled);
    ImGui::BeginDisabled(
      std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path));
    ImGui::Checkbox("Snap Zoom to Height", &snap_zoom_to_height);
    if (ImGui::CollapsingHeader("Add Blend"))
    {
      ImGui::Checkbox("Percent Blends (50%,25%)", &enable_percent_blend);
      ImGui::PushID(1);
      const auto pop = scope_guard(&ImGui::PopID);
      Blend_Combos(add_parameter_selections, add_equation_selections);
    }
    if (ImGui::CollapsingHeader("Subtract Blend"))
    {
      ImGui::PushID(2);
      const auto pop = scope_guard(&ImGui::PopID);
      Blend_Combos(subtract_parameter_selections, subtract_equation_selections);
    }
    if (ImGui::Button("Save"))
    {
      Save();
    }
  }
  ImGui::Separator();
  camera.OnImGuiUpdate();
  ImGui::Separator();
  m_batch_renderer.OnImGuiUpdate();
  ImGui::Separator();
}
void ff8::Map::Save() const
{
  saving                = true;
  const auto not_saving = scope_guard([]() { saving = false; });
  OnRender();
  const auto path = std::filesystem::path(m_map_path);
  auto       string =
    fmt::format("{}_map.png", (path.parent_path() / path.stem()).string());
  PixelBuffer  pixel_buffer{ m_frame_buffer.Specification() };
  pixel_buffer.operator()(m_frame_buffer, string);
  while (pixel_buffer.operator()(&Texture::save))
    ;
}
