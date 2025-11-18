#ifndef A047A03B_B70D_4490_89A6_07E5D157223D
#define A047A03B_B70D_4490_89A6_07E5D157223D
#include "map_sprite.hpp"
#include "mim_sprite.hpp"
#include "mouse_positions.hpp"
#include "Selections.hpp"
#include <glengine/BatchRenderer.hpp>
#include <glengine/FrameBuffer.hpp>
#include <glengine/OrthographicCamera.hpp>
#include <imgui.h>
namespace fme
{
struct [[nodiscard]] draw_window
{
     draw_window() = default;

     draw_window(
       const std::shared_ptr<Selections> &in_selections,
       const std::shared_ptr<mim_sprite> &in_mim_sprite,
       const std::shared_ptr<map_sprite> &in_map_sprite)
       : m_selections(in_selections)
       , m_mim_sprite(in_mim_sprite)
       , m_map_sprite(in_map_sprite)
       , m_checkerboard_batchrenderer{
            4,
            []() -> std::filesystem::path
            {
                 std::error_code       ec;

                 // Get current path safely
                 std::filesystem::path currentPath
                   = std::filesystem::current_path(ec);
                 if (ec)
                 {
                      spdlog::error(
                        "Failed to get current path: {}", ec.message());
                      return {};
                 }

                 // Construct the target path
                 std::filesystem::path shaderPath
                   = currentPath / "res" / "shader" / "checkerboard.shader";

                 // Check if it exists
                 if (!std::filesystem::exists(shaderPath, ec))
                 {
                      if (ec)
                      {
                           spdlog::error(
                             "Failed to check existence of path '{}': {}",
                             shaderPath.string(),
                             ec.message());
                      }
                      else
                      {
                           spdlog::error(
                             "Shader file does not exist: {}",
                             shaderPath.string());
                      }
                      return {};
                 }
                 return shaderPath;
            }()
       }
     {
     }
     void update(std::weak_ptr<Selections> in_selections);
     void update(std::weak_ptr<const mim_sprite> in_mim_sprite);
     void update(std::weak_ptr<map_sprite> in_map_sprite);
     void on_im_gui_update() const;
     void hovered_index(std::ptrdiff_t index) const;
     const std::vector<std::size_t> &hovered_tiles_indices() const;
     const MousePositions           &mouse_positions() const;
     const std::vector<std::size_t> &clicked_tile_indices() const;
     void                            clear_clicked_tile_indices() const;
     void                            remove_clicked_index(std::size_t) const;
     void                            on_update(float) const;
     void                            on_im_gui_window_menu() const;

   private:
     std::weak_ptr<Selections>            m_selections                 = {};
     std::weak_ptr<const mim_sprite>      m_mim_sprite                 = {};
     std::weak_ptr<map_sprite>            m_map_sprite                 = {};
     mutable MousePositions               m_mouse_positions            = {};
     mutable glengine::FrameBuffer        m_checkerboard_framebuffer   = {};
     glengine::BatchRenderer              m_checkerboard_batchrenderer = {};
     mutable glengine::OrthographicCamera m_fixed_render_camera        = {};
     mutable std::vector<std::size_t>     m_clicked_tile_indices       = {};
     mutable std::vector<std::size_t>     m_hovered_tiles_indices      = {};
     mutable std::ptrdiff_t               m_hovered_index              = { -1 };
     mutable glm::ivec2                   m_location_backup            = {};
     mutable bool                         m_translation_in_progress = { false };
     void update_hover_and_mouse_button_status_for_map(
       const ImVec2 &img_start,
       const float   scale) const;
     void draw_map_grid_lines_for_tiles(
       const ImVec2 &screen_pos,
       const ImVec2 &scaled_size,
       const float   scale) const;
     void draw_map_grid_for_conflict_tiles(
       const ImVec2 &screen_pos,
       const float   scale) const;
     void draw_map_grid_lines_for_texture_page(
       const ImVec2 &screen_pos,
       const ImVec2 &scaled_size,
       const float   scale) const;
     void draw_mim_grid_lines_for_tiles(
       const ImVec2 &screen_pos,
       const ImVec2 &scaled_size,
       const float   scale) const;
     void draw_mim_grid_lines_for_texture_page(
       const ImVec2 &screen_pos,
       const ImVec2 &scaled_size,
       const float   scale) const;
     void UseImGuizmo(
       const float   scale,
       const ImVec2 &screen_pos) const;
};
}// namespace fme
static_assert(
  glengine::Renderable<fme::draw_window>,
  "draw_window must satisfy Renderable concept");

#endif /* A047A03B_B70D_4490_89A6_07E5D157223D */
