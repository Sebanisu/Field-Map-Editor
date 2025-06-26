#ifndef A047A03B_B70D_4490_89A6_07E5D157223D
#define A047A03B_B70D_4490_89A6_07E5D157223D
#include "map_sprite.hpp"
#include "mim_sprite.hpp"
#include "mouse_positions.hpp"
#include "Selections.hpp"
#include <BatchRenderer.hpp>
#include <FrameBuffer.hpp>
#include <imgui.h>
#include <OrthographicCamera.hpp>
namespace fme
{
struct [[nodiscard]] draw_window
{
     draw_window(
       std::weak_ptr<Selections>       in_selections,
       std::weak_ptr<const mim_sprite> in_mim_sprite,
       std::weak_ptr<map_sprite>       in_map_sprite)
       : m_selections(std::move(in_selections))
       , m_mim_sprite(std::move(in_mim_sprite))
       , m_map_sprite(std::move(in_map_sprite))
       , m_checkerboard_batchrenderer{ 4, []() -> std::filesystem::path {
                                           std::error_code       ec;

                                           // Get current path safely
                                           std::filesystem::path currentPath = std::filesystem::current_path(ec);
                                           if (ec)
                                           {
                                                spdlog::error("Failed to get current path: {}", ec.message());
                                                return {};
                                           }

                                           // Construct the target path
                                           std::filesystem::path shaderPath = currentPath / "res" / "shader" / "checkerboard.shader";

                                           // Check if it exists
                                           if (!std::filesystem::exists(shaderPath, ec))
                                           {
                                                if (ec)
                                                {
                                                     spdlog::error(
                                                       "Failed to check existence of path '{}': {}", shaderPath.string(), ec.message());
                                                }
                                                else
                                                {
                                                     spdlog::error("Shader file does not exist: {}", shaderPath.string());
                                                }
                                                return {};
                                           }
                                           return shaderPath;
                                      }() }
     {
     }
     void                            update(std::weak_ptr<Selections> in_selections);
     void                            update(std::weak_ptr<const mim_sprite> in_mim_sprite);
     void                            update(std::weak_ptr<map_sprite> in_map_sprite);
     void                            render() const;
     void                            hovered_index(std::ptrdiff_t index);
     const std::vector<std::size_t> &hovered_tiles_indices() const;
     const MousePositions           &mouse_positions() const;
     void                            update_mouse_positions();
     const std::vector<std::size_t> &clicked_tile_indices() const;
     void                            clear_clicked_tile_indices();

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
     std::ptrdiff_t                       m_hovered_index              = { -1 };
     void                                 update_hover_and_mouse_button_status_for_map(const ImVec2 &img_start, const float scale) const;
     void draw_map_grid_lines_for_tiles(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const;
     void draw_map_grid_for_conflict_tiles(const ImVec2 &screen_pos, const float scale) const;
     void draw_map_grid_lines_for_texture_page(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const;
     void draw_mim_grid_lines_for_tiles(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const;
     void draw_mim_grid_lines_for_texture_page(const ImVec2 &screen_pos, const ImVec2 &scaled_size, const float scale) const;
     void draw_mouse_positions_sprite(const float scale, const ImVec2 &screen_pos) const;
     void on_click_not_imgui() const;
};
}// namespace fme

#endif /* A047A03B_B70D_4490_89A6_07E5D157223D */
