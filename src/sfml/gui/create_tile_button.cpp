#include "create_tile_button.hpp"
static constexpr auto toImColor = [](const sf::Color &c) -> ImColor {
     return {static_cast<int>(c.r), static_cast<int>(c.g), static_cast<int>(c.b), static_cast<int>(c.a)};
};
using namespace open_viii::graphics::background;
template<is_tile tileT>
[[nodiscard]] bool
  fme::create_tile_button(std::weak_ptr<const fme::map_sprite> map_ptr, const tileT &tile, const tile_button_options &options)
{
     const auto map = map_ptr.lock();
     if (!map)
     {
          spdlog::error("{}", "map_sprite is null");
          return false;
     }
     const auto *texture = map->get_texture(tile);
     if (texture == nullptr)
     {
          spdlog::error("{}", "Failed to get texture for tile.");
          return false;
     }
     const auto tile_texture_size = map->get_tile_texture_size(texture);
     const auto src_x             = [&]() -> std::uint32_t {
          if (map->filter().deswizzle.enabled())
          {
               return static_cast<std::uint32_t>(tile.x());
          }
          auto       source_texture_page_width = tileT::texture_page_width(tile.depth());
          const auto texture_page_x_offset     = [&]() -> std::uint32_t {
               if (map->filter().upscale.enabled())
               {
                    return 0;
               }
               return tile.texture_id() * source_texture_page_width;
          }();
          return tile.source_x() + texture_page_x_offset;
     }();
     const auto src_y = [&]() -> std::uint32_t {
          if (map->filter().deswizzle.enabled())
          {
               return static_cast<std::uint32_t>(tile.y());
          }
          return tile.source_y();
     }();
     static constexpr float tile_size = 16.F;
     sf::Sprite             sprite(
       *texture,
       sf::IntRect(
         static_cast<int>((static_cast<float>(src_x) / tile_size) * static_cast<float>(tile_texture_size.x)),
         static_cast<int>((static_cast<float>(src_y) / tile_size) * static_cast<float>(tile_texture_size.y)),
         static_cast<int>(tile_texture_size.x),
         static_cast<int>(tile_texture_size.y)));

     int pop_count     = {};
     int pop_var_count = {};
     if (options.color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ toImColor(options.color.value()) });
          ++pop_count;
     }
     if (options.hover_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ toImColor(options.hover_color.value()) });
          ++pop_count;
     }
     if (options.active_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ toImColor(options.active_color.value()) });
          ++pop_count;
     }
     if (options.padding_size.has_value())
     {
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, options.padding_size.value());
          ++pop_var_count;
     }

     // Use scope_guard to safely pop style colors and variables
     const auto pop_style = scope_guard{ [&]() {
          if (pop_count > 0)
               ImGui::PopStyleColor(pop_count);
          if (pop_var_count > 0)
               ImGui::PopStyleVar(pop_var_count);
     } };
     const auto pop_id = PushPopID();

     return ImGui::ImageButton(
       "##tile_image_button",
       sprite,
       options.size == sf::Vector2f{} ? sf::Vector2f{ ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() } : options.size,
       options.background_color,
       options.tint_color);
}


// Explicit instantiation for Tiles
template [[nodiscard]] bool fme::create_tile_button(std::weak_ptr<const fme::map_sprite>, const Tile1 &, const tile_button_options &);
template [[nodiscard]] bool fme::create_tile_button(std::weak_ptr<const fme::map_sprite>, const Tile2 &, const tile_button_options &);
template [[nodiscard]] bool fme::create_tile_button(std::weak_ptr<const fme::map_sprite>, const Tile3 &, const tile_button_options &);


[[nodiscard]] bool          fme::create_color_button(const tile_button_options &options)
{
     // needs to be in a std::shared_ptr to be returned from a lambda. it has no move or copy operations. Also it won't just use copy
     // elision.
     static std::shared_ptr<sf::RenderTexture> transparent_texture = []() {
          auto t = std::make_shared<sf::RenderTexture>();
          t->create(1, 1);
          t->clear(sf::Color::Transparent);
          t->display();
          return t;
     }();

     if (!transparent_texture)
     {
          spdlog::error("{}", "transparent_texture is null");
          return false;
     }

     int pop_count     = {};
     int pop_var_count = {};
     if (options.color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ toImColor(options.color.value()) });
          ++pop_count;
     }
     if (options.hover_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ toImColor(options.hover_color.value()) });
          ++pop_count;
     }
     if (options.active_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ toImColor(options.active_color.value()) });
          ++pop_count;
     }
     if (options.padding_size.has_value())
     {
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, options.padding_size.value());
          ++pop_var_count;
     }

     // Use scope_guard to safely pop style colors and variables
     const auto pop_style = scope_guard{ [&]() {
          if (pop_count > 0)
               ImGui::PopStyleColor(pop_count);
          if (pop_var_count > 0)
               ImGui::PopStyleVar(pop_var_count);
     } };
     const auto pop_id = PushPopID();
     return ImGui::ImageButton(
       "##color_button",
       *transparent_texture.get(),
       options.size == sf::Vector2f{} ? sf::Vector2f{ ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() } : options.size,
       options.background_color,
       options.tint_color);
}