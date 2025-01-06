#ifndef A36FA8B2_05FC_4730_B3F1_3B7B04340687
#define A36FA8B2_05FC_4730_B3F1_3B7B04340687
#include "filebrowser.hpp"
#include "format_imgui_text.hpp"
#include "map_directory_mode.hpp"
#include "map_sprite.hpp"
#include "Selections.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
#include <memory>
#include <open_viii/graphics/background/Map.hpp>
#include <SFML/Graphics/Image.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <variant>


namespace fme
{

class [[nodiscard]] import
{
   private:
     using variant_tile_t = std::variant<
       open_viii::graphics::background::Tile1,
       open_viii::graphics::background::Tile2,
       open_viii::graphics::background::Tile3,
       std::monostate>;
     static constexpr std::int8_t                 tile_size_px                  = { 16 };
     static constexpr std::uint8_t                tile_size_px_unsigned         = { 16U };

     mutable std::weak_ptr<Selections>            m_selections                  = {};
     mutable std::weak_ptr<map_sprite>            m_map_sprite                  = {};
     mutable open_viii::graphics::background::Map m_import_image_map            = {};
     mutable std::string                          m_import_image_path           = {};

     mutable sf::Texture                          m_loaded_image_texture        = {};
     mutable sf::RenderTexture                    m_loaded_image_render_texture = {};
     mutable sf::Image                            m_loaded_image_cpu            = {};

     mutable map_directory_mode                   m_modified_directory_map      = {};


     mutable ImGui::FileBrowser                   m_load_file_browser{ ImGuiFileBrowserFlags_EditPathString };

     mutable ImGui::FileBrowser m_directory_browser{ ImGuiFileBrowserFlags_SelectDirectory | ImGuiFileBrowserFlags_CreateNewDir
                                                     | ImGuiFileBrowserFlags_EditPathString };

   public:
     import();
     void                          render() const;
     [[nodiscard]] variant_tile_t &combo_selected_tile(bool &changed) const;
     [[nodiscard]] bool            browse_for_image_display_preview() const;
     [[nodiscard]] bool            combo_tile_size() const;
     [[nodiscard]] bool            checkbox_render_imported_image() const;
     void                          generate_map_for_imported_image(const variant_tile_t &current_tile, bool changed) const;
     void                          collapsing_header_generated_tiles() const;
     void                          update_scaled_up_render_texture() const;
     void                          update_imported_render_texture() const;
     void                          save_swizzle_textures() const;
     void                          reset_imported_image() const;
     void                          filter_empty_import_tiles() const;
     void                          find_selected_tile_for_import(variant_tile_t &current_tile) const;

     // Update functions for m_selections and m_map_sprite
     void                          update(const std::shared_ptr<Selections> &new_selections) const;
     void                          update(const std::shared_ptr<map_sprite> &new_map_sprite) const;

     void adjust_source_xy_texture_page_for_import_map(uint8_t next_source_y, const uint8_t next_texture_page) const;

     template<std::ranges::range tiles_t>
     std::pair<std::uint8_t, std::uint8_t> get_next_unused_y_and_texture_page(const tiles_t &tiles) const
     {
          const auto max_texture_id_tile = (std::ranges::max)(tiles, {}, [](const auto &tile) { return tile.texture_id(); });
          const auto max_source_y_tile   = (std::ranges::max)(
            tiles | std::ranges::views::filter([&max_texture_id_tile](const auto &tile) {
                 return tile.texture_id() == max_texture_id_tile.texture_id();
            }),
            {},
            [](const auto &tile) { return tile.source_y(); });
          int const tile_y = max_source_y_tile.source_y() / tile_size_px;
          format_imgui_text("Last Used Texture Page {}, and Source Y / 16 = {}", max_texture_id_tile.texture_id(), tile_y);
          const auto         next_source_y = static_cast<uint8_t>((tile_y + 1) % tile_size_px_unsigned);
          const std::uint8_t next_texture_page =
            tile_y + 1 == tile_size_px_unsigned ? max_texture_id_tile.texture_id() + 1 : max_texture_id_tile.texture_id();
          return { next_source_y, next_texture_page };
     }
};
}// namespace fme
#endif /* A36FA8B2_05FC_4730_B3F1_3B7B04340687 */
