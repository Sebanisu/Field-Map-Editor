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
#include "FF8LoadTextures.hpp"
#include "Fields.hpp"
#include "FrameBuffer.hpp"
#include "FrameBufferBackup.hpp"
#include "GenericCombo.hpp"
#include "ImGuiDisabled.hpp"
#include "ImGuiIndent.hpp"
#include "ImGuiPushID.hpp"
#include "OrthographicCamera.hpp"
#include "OrthographicCameraController.hpp"
#include "PixelBuffer.hpp"
#include "TransformedSortedUniqueCopy.hpp"
#include "UniqueTileValues.hpp"
#include "Window.hpp"
#include <type_traits>
namespace ff8
{
template<typename TileFunctions>
class Map
{
public:
  Map() = default;
  Map(const Fields &fields)
    : m_mim(LoadMim(fields.Field(), fields.Coo(), m_mim_path, m_mim_choose_coo))
    , m_map(LoadMap(
        fields.Field(),
        fields.Coo(),
        m_mim,
        m_map_path,
        m_map_choose_coo))
    , m_unique_tile_values(m_map)
  {
    if (std::empty(m_mim_path))
    {
      return;
    }
    fmt::print("Loaded {}\n", m_mim_path);
    fmt::print("Loaded {}\n", m_map_path);
    fmt::print("Loading Textures from Mim \n");
    m_delayed_textures = LoadTextures(m_mim);
    SetCameraBoundsToEdgesOfImage();
    GetUniqueValues();
  }
  void OnUpdate(float ts) const
  {
    s_camera.RefreshAspectRatio();
    m_delayed_textures.OnUpdate();
    if (s_snap_zoom_to_height)
    {
      s_camera.SetZoom();
    }
    s_camera.OnUpdate(ts);
    m_batch_renderer.OnUpdate(ts);
  }
  void OnRender() const
  {
    if (std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path))
    {
      return;
    }
    {
      m_offscreen_drawing = true;
      const auto not_offscreen_drawing =
        glengine::scope_guard_expensive([&]() { m_offscreen_drawing = false; });
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
    RestoreViewPortToFrameBuffer();
    RenderFrameBuffer();
  }
  void OnImGuiUpdate() const
  {
    {
      const auto disable = glengine::ImGuiDisabled(
        std::ranges::empty(m_map_path) || std::ranges::empty(m_mim_path));
      ImGui::Checkbox("Draw Grid", &s_draw_grid);
      ImGui::Checkbox("Snap Zoom to Height", &s_snap_zoom_to_height);
      if (ImGui::CollapsingHeader("Add Blend"))
      {
        const auto un_indent = glengine::ImGuiIndent();
        ImGui::Checkbox("Percent Blends (50%,25%)", &s_enable_percent_blend);
        const auto pop = glengine::ImGuiPushID();
        Blend_Combos(add_parameter_selections, add_equation_selections);
      }
      if (ImGui::CollapsingHeader("Subtract Blend"))
      {
        const auto un_indent = glengine::ImGuiIndent();
        const auto pop       = glengine::ImGuiPushID();
        Blend_Combos(
          subtract_parameter_selections, subtract_equation_selections);
      }
      if (ImGui::Button("Save"))
      {
        Save();
      }
    }
    ImGui::Separator();
    s_camera.OnImGuiUpdate();
    ImGui::Separator();
    m_batch_renderer.OnImGuiUpdate();
    ImGui::Separator();
    if (ImGui::CollapsingHeader("Filters"))
    {
      const auto            un_indent0 = glengine::ImGuiIndent();
      static constexpr auto common =
        [](
          const char                             *label,
          std::ranges::random_access_range auto  &bool_range,
          std::ranges::random_access_range auto &&possible_value_range,
          std::ranges::random_access_range auto &&possible_value_string_range,
          std::ranges::random_access_range auto &&used_value_range,
          std::uint32_t                           line_count) {
          assert(
            std::ranges::size(possible_value_range)
            == std::ranges::size(possible_value_string_range));
          assert(
            std::ranges::size(bool_range)
            == std::ranges::size(possible_value_range));
          assert(
            std::ranges::size(possible_value_range)
            >= std::ranges::size(used_value_range));
          if (ImGui::CollapsingHeader(label))
          {
            const auto un_indent1 = glengine::ImGuiIndent();

            auto       boolptr    = std::ranges::begin(bool_range);
            const auto boolsent   = std::ranges::end(bool_range);
            auto current_value    = std::ranges::cbegin(possible_value_range);
            auto current_string =
              std::ranges::cbegin(possible_value_string_range);

            for (std::uint32_t i = 0; boolptr != boolsent; ++i,
                               (void)++boolptr,
                               (void)++current_value,
                               (void)++current_string)
            {
              auto found = std::ranges::find(used_value_range, *current_value);

              const auto disabled = glengine::ImGuiDisabled(
                found == std::ranges::end(used_value_range));

              const auto pop       = glengine::ImGuiPushID();
              const auto string    = fmt::format("{:>4}", *current_string);
              auto       size      = ImGui::CalcTextSize(string.c_str());
              bool       same_line = line_count > 0 && i % line_count != 0;
              if (same_line)
                ImGui::SameLine();
              ImGui::Dummy(ImVec2(2.F, 2.F));
              if (same_line)
                ImGui::SameLine();
              if (ImGui::Selectable(string.c_str(), *boolptr, 0, size))
              {
                *boolptr = !*boolptr;
              }
            }
            ImGui::Dummy(ImVec2(2.F, 2.F));
            {
              const auto pop = glengine::ImGuiPushID();
              if (ImGui::Button("All"))
              {
                std::ranges::fill(bool_range, true);
              }
            }
            ImGui::SameLine();
            ImGui::Dummy(ImVec2(2.F, 2.F));
            ImGui::SameLine();
            {
              const auto pop = glengine::ImGuiPushID();
              if (ImGui::Button("None"))
              {
                std::ranges::fill(bool_range, false);
              }
            }
            ImGui::Dummy(ImVec2(2.F, 2.F));
          }
        };

      common(
        "BPP",
        m_possible_tile_values.bpp_enabled,
        m_possible_tile_values.bpp.values(),
        m_possible_tile_values.bpp.strings(),
        m_unique_tile_values.bpp.values(),
        4);

      common(
        "Palettes",
        m_possible_tile_values.palette_id_enabled,
        m_possible_tile_values.palette_id.values(),
        m_possible_tile_values.palette_id.strings(),
        m_unique_tile_values.palette_id.values(),
        8);

      common(
        "Blend Mode",
        m_possible_tile_values.blend_mode_enabled,
        m_possible_tile_values.blend_mode.values(),
        m_possible_tile_values.blend_mode.strings(),
        m_unique_tile_values.blend_mode.values(),
        3);
    }
  }
  void OnEvent(const glengine::Event::Item &) const {}

private:
  static void Blend_Combos(
    glengine::BlendModeParameters &parameters_selections,
    glengine::BlendModeEquations  &equation_selections)
  {
    if (parameters_selections.OnImGuiUpdate())
    {
      // something changed
    }
    ImGui::Separator();
    if (equation_selections.OnImGuiUpdate())
    {
      // something changed
    }
  }
  // set uniforms
  void SetUniforms() const
  {
    m_batch_renderer.Bind();
    if (m_offscreen_drawing || m_saving)
    {
      m_batch_renderer.Shader().SetUniform(
        "u_MVP", s_fixed_render_camera.ViewProjectionMatrix());
    }
    else
    {
      m_batch_renderer.Shader().SetUniform(
        "u_MVP", s_camera.Camera().ViewProjectionMatrix());
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
  // draws tiles
  void RenderTiles() const
  {
    using open_viii::graphics::background::BlendModeT;
    BlendModeT last_blend_mode{ BlendModeT::none };
    s_uniform_color = s_default_uniform_color;
    glengine::Window::DefaultBlend();
    s_camera.OnRender();
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
            IndexAndPageWidth(bpp, palette);

          auto  texture_page_offset = tile.texture_id() * texture_page_width;

          auto &texture = m_delayed_textures.textures->at(texture_index);
          if (texture.width() == 0 || texture.height() == 0)
            continue;
          const auto texture_dims =
            glm::vec2(m_mim.get_width(tile.depth()), m_mim.get_height());
          glengine::SubTexture sub_texture = {
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
            if (s_enable_percent_blend)
            {
              switch (blend_mode)
              {
                case BlendModeT::half_add:
                  s_uniform_color = s_half_uniform_color;
                  break;
                case BlendModeT::quarter_add:
                  s_uniform_color = s_quarter_uniform_color;
                  break;
                default:
                  s_uniform_color = s_default_uniform_color;
                  break;
              }
            }
            switch (blend_mode)
            {
              case BlendModeT::half_add:
              case BlendModeT::quarter_add:
              case BlendModeT::add: {
                SetBlendModeSelections(
                  add_parameter_selections, add_equation_selections);
              }
              break;
              case BlendModeT ::subtract: {
                SetBlendModeSelections(
                  subtract_parameter_selections, subtract_equation_selections);
              }
              break;
              default:
                glengine::Window::DefaultBlend();
            }
          }

          using tileT = std::ranges::range_value_t<decltype(tiles)>;
          static constexpr
            typename TileFunctions::template Bounds<tileT>::x x{};
          static constexpr
            typename TileFunctions::template Bounds<tileT>::y y{};
          static constexpr
            typename TileFunctions::template Bounds<tileT>::texture_page
              texture_page{};
          m_batch_renderer.DrawQuad(
            sub_texture,
            glm::vec3(
              static_cast<float>(
                x(tile) + texture_page(tile) * s_texture_page_width),
              m_offset_y - static_cast<float>(y(tile)),
              0.F),
            glm::vec2(16.F, 16.F));
        }
      }
    });
    m_batch_renderer.Draw();
    m_batch_renderer.OnRender();
    glengine::Window::DefaultBlend();
  }
  static void SetBlendModeSelections(
    const glengine::BlendModeParameters &parameters_selections,
    const glengine::BlendModeEquations  &equation_selections)
  {
    GLCall{}(
      glBlendFuncSeparate,
      parameters_selections[0].current_value(),
      parameters_selections[1].current_value(),
      parameters_selections[2].current_value(),
      parameters_selections[3].current_value());
    GLCall{}(
      glBlendEquationSeparate,
      equation_selections[0].current_value(),
      equation_selections[1].current_value());
  }

  auto
    IndexAndPageWidth(open_viii::graphics::BPPT bpp, std::uint8_t palette) const
  {
    struct
    {
      size_t texture_index      = {};
      int    texture_page_width = { s_texture_page_width };
    } r = { .texture_index = palette };
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

  void RenderFrameBuffer() const
  {
    using open_viii::graphics::background::BlendModeT;
    BlendModeT last_blend_mode{ BlendModeT::none };
    s_uniform_color = s_default_uniform_color;
    glengine::Window::DefaultBlend();
    s_camera.OnRender();
    SetUniforms();
    m_batch_renderer.Clear();
    m_map.visit_tiles([&](const auto &tiles) {
      auto f_tiles = tiles
                     | std::views::filter(
                       open_viii::graphics::background::Map::filter_invalid());
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
            IndexAndPageWidth(bpp, palette);

          auto  texture_page_offset = tile.texture_id() * texture_page_width;

          auto &texture = m_delayed_textures.textures->at(texture_index);
          if (texture.width() == 0 || texture.height() == 0)
            continue;
          const auto texture_dims =
            glm::vec2(m_mim.get_width(tile.depth()), m_mim.get_height());
          glengine::SubTexture sub_texture = {
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
            if (s_enable_percent_blend)
            {
              switch (blend_mode)
              {
                case BlendModeT::half_add:
                  s_uniform_color = s_half_uniform_color;
                  break;
                case BlendModeT::quarter_add:
                  s_uniform_color = s_quarter_uniform_color;
                  break;
                default:
                  s_uniform_color = s_default_uniform_color;
                  break;
              }
            }
            switch (blend_mode)
            {
              case BlendModeT::half_add:
              case BlendModeT::quarter_add:
              case BlendModeT::add: {
                SetBlendModeSelections(
                  add_parameter_selections, add_equation_selections);
              }
              break;
              case BlendModeT ::subtract: {
                SetBlendModeSelections(
                  subtract_parameter_selections, subtract_equation_selections);
              }
              break;
              default:
                glengine::Window::DefaultBlend();
            }
          }

          using tileT = std::ranges::range_value_t<decltype(tiles)>;
          static constexpr
            typename TileFunctions::template Bounds<tileT>::x x{};
          static constexpr
            typename TileFunctions::template Bounds<tileT>::y y{};
          static constexpr
            typename TileFunctions::template Bounds<tileT>::texture_page
              texture_page{};
          m_batch_renderer.DrawQuad(
            sub_texture,
            glm::vec3(
              static_cast<float>(
                x(tile) + texture_page(tile) * s_texture_page_width),
              m_offset_y - static_cast<float>(y(tile)),
              0.F),
            glm::vec2(16.F, 16.F));
        }
      }
    });
    m_batch_renderer.Draw();
    m_batch_renderer.OnRender();
    glengine::Window::DefaultBlend();
  }
  void Save() const
  {
    m_saving = true;
    const auto not_saving =
      glengine::scope_guard_expensive([&]() { m_saving = false; });
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
    s_camera.RefreshAspectRatio();
    m_map.visit_tiles([&](const auto &tiles) {
      using tileT = std::ranges::range_value_t<decltype(tiles)>;
      static constexpr typename TileFunctions::template Bounds<tileT>::x x{};
      static constexpr typename TileFunctions::template Bounds<tileT>::y y{};
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
      const auto min_x = x(*i_min_x);
      const auto max_x =
        x(*i_max_x) + texture_page(*i_max_texture_page) * s_texture_page_width;
      const auto min_y  = y(*i_min_y);
      const auto max_y  = y(*i_max_y);
      const auto width  = max_x - min_x + 16;
      const auto height = max_y - min_y + 16;
      m_offset_y        = static_cast<float>(min_y + max_y);

      s_camera.SetMaxBounds({ static_cast<float>(min_x),
                              static_cast<float>(max_x + 16),
                              static_cast<float>(min_y),
                              static_cast<float>(max_y + 16) });

      s_fixed_render_camera.SetProjection(
        static_cast<float>(min_x),
        static_cast<float>(max_x + 16),
        static_cast<float>(min_y),
        static_cast<float>(max_y + 16));
      m_frame_buffer = glengine::FrameBuffer(glengine::FrameBufferSpecification{
        .width = abs(width), .height = abs(height) });
    });
  }

  inline static glengine::OrthographicCameraController s_camera    = { 16 / 9 };
  inline static glengine::OrthographicCamera s_fixed_render_camera = {};
  inline static bool                         s_snap_zoom_to_height = { true };
  inline static bool                         s_enable_percent_blend = { true };
  inline static bool                         s_draw_grid            = { false };

  static constexpr int16_t                   s_texture_page_width   = 256;


  static constexpr glm::vec4 s_default_uniform_color = { 1.F, 1.F, 1.F, 1.F };
  static constexpr glm::vec4 s_half_uniform_color    = { .5F, .5F, .5F, .5F };
  static constexpr glm::vec4 s_quarter_uniform_color = { .25F,
                                                         .25F,
                                                         .25F,
                                                         .25F };
  inline static glm::vec4    s_uniform_color         = s_default_uniform_color;

  inline static constinit glengine::BlendModeParameters
    add_parameter_selections{ 2, 1, 6, 7 };
  inline static constinit glengine::BlendModeEquations
    add_equation_selections{};
  inline static constinit glengine::BlendModeParameters
    subtract_parameter_selections{ 4, 1, 6, 7 };
  inline static constinit glengine::BlendModeEquations
                                       subtract_equation_selections{ 2, 0 };
  // inline static std::array<int, 4> add_parameter_selections{ 2, 1, 6, 7 };
  // inline static std::array<int, 2> add_equation_selections{};
  //  inline static std::array<int, 4> subtract_parameter_selections{ 4, 1, 6, 7
  //  }; inline static std::array<int, 2> subtract_equation_selections{ 2, 0 };

  // internal mim file path
  std::string                          m_mim_path             = {};
  // internal map file path
  std::string                          m_map_path             = {};
  // if coo was chosen instead of default.
  bool                                 m_mim_choose_coo       = {};
  // if coo was chosen instead of default.
  bool                                 m_map_choose_coo       = {};
  // container for field textures
  open_viii::graphics::background::Mim m_mim                  = {};
  // container for field tile information
  open_viii::graphics::background::Map m_map                  = {};
  // loads the textures overtime instead of forcing them to load at start.
  glengine::DelayedTextures<35U>       m_delayed_textures     = {};
  // takes quads and draws them to the frame buffer or screen.
  glengine::BatchRenderer              m_batch_renderer       = {};
  // holds rendered image at 1:1 scale to prevent gaps when scaling.
  glengine::FrameBuffer                m_frame_buffer         = {};
  float                                m_offset_y             = {};
  mutable bool                         m_offscreen_drawing    = { false };
  mutable bool                         m_saving               = { false };
  UniqueTileValues                     m_unique_tile_values   = { m_map };
  TilePossibleValues                   m_possible_tile_values = {};
};
}// namespace ff8
#endif// FIELD_MAP_EDITOR_MAP_HPP