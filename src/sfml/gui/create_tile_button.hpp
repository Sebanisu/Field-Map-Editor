#ifndef DF4C3AF7_C3A6_4F62_8336_B0EF72752E11
#define DF4C3AF7_C3A6_4F62_8336_B0EF72752E11

#include "colors.hpp"
#include "map_sprite.hpp"
#include "push_pop_id.hpp"
#include <fmt/format.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <open_viii/graphics/background/Map.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Vertex.hpp>
namespace fme
{
/**
 * @brief Options for configuring the appearance and behavior of an image button.
 */
struct tile_button_options
{
     /**
      * @brief Size of the button in pixels.
      */
     ImVec2                    size                = {};

     /**
      * @brief Background color of the button.
      * This is the color drawn before the image gets overlaid on top.
      * Defaults to `sf::Color::Transparent`.
      */
     fme::color                background_color    = { colors::Transparent };

     /**
      * @brief Tint color applied to the button image.
      * This color is applied over the image itself, modifying its appearance.
      * Defaults to `sf::Color::White` (no tint).
      */
     fme::color                tint_color          = { colors::White };

     /**
      * @brief Optional override for the default button color.
      * This affects the color of the button under and around the image.
      * If set, this value will be applied using `ImGui::PushStyleColor(ImGuiCol_Button)`.
      * If not set, the style's default button color is used.
      */
     std::optional<fme::color> button_color        = {};

     /**
      * @brief Optional color for the button when hovered.
      * This affects the color of the button under and around the image when hovered.
      * If set, this value will be applied using `ImGui::PushStyleColor(ImGuiCol_ButtonHovered)`.
      * If not set, the style's default hover color is used.
      */
     std::optional<fme::color> button_hover_color  = {};

     /**
      * @brief Optional color for the button when active (pressed).
      * This affects the color of the button under and around the image when pressed.
      * If set, this value will be applied using `ImGui::PushStyleColor(ImGuiCol_ButtonActive)`.
      * If not set, the style's default active color is used.
      */
     std::optional<fme::color> button_active_color = {};

     /**
      * @brief Optional padding size around the image in pixels.
      * Determines the frame around the image. The `x` value is applied to both left and right,
      * and the `y` value is applied to both top and bottom, effectively doubling the padding on each side.
      * If set, this value will be applied using `ImGui::PushStyleVar(ImGuiStyleVar_FramePadding)`.
      * If not set, the style's default padding size is used.
      */
     std::optional<ImVec2>     padding_size        = {};
};

template<open_viii::graphics::background::is_tile tileT>
[[nodiscard]] bool create_tile_button(std::weak_ptr<const map_sprite> map, const tileT &tile, const tile_button_options &options = {});
struct [[nodiscard]] create_color_button
{
   private:
     const tile_button_options &m_options             = {};
     glengine::Texture          m_transparent_texture = (std::uint32_t{ 0 });

   public:
     create_color_button(tile_button_options options = {})
       : m_options(std::move(options))
     {
     }
     bool operator()() const;
};
}// namespace fme
#endif /* DF4C3AF7_C3A6_4F62_8336_B0EF72752E11 */
