#ifndef D606CA11_FB30_432B_BF0C_B3184209EE6E
#define D606CA11_FB30_432B_BF0C_B3184209EE6E
#include "map_sprite.hpp"
#include "Selections.hpp"
#include <functional>
#include <imgui.h>
#include <memory>
#include <optional>
namespace fme
{
class [[nodiscard]] textures_window
{
     mutable std::weak_ptr<map_sprite> m_map_sprite       = {};
     mutable std::weak_ptr<Selections> m_selections       = {};
     mutable float                     m_thumb_size_width = { 96.f };
     mutable float                     m_aspect_ratio     = { 1.f };

   public:
     textures_window() = default;
     void render() const;
     void handle_thumbnail_size_adjustment() const;
     int  calc_column_count(float) const;
     void render_thumbnail_button(
       const std::string &,
       const glengine::Texture &,
       const bool,
       std::move_only_function<void()>) const;
     void render_thumbnail_button(
       const std::string &,
       const uint32_t &,
       const glm::ivec2 &,
       const bool,
       std::move_only_function<void()>) const;
     void draw_thumbnail_label(const std::string &) const;
     void update(const std::shared_ptr<map_sprite> &) const;
     void update(const std::shared_ptr<Selections> &) const;
};
}// namespace fme
#endif /* D606CA11_FB30_432B_BF0C_B3184209EE6E */
