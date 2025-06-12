#include "create_tile_button.hpp"
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
     // sf::Sprite             sprite(
     //   *texture,
     //   sf::IntRect(
     //     static_cast<int>((static_cast<float>(src_x) / tile_size) * static_cast<float>(tile_texture_size.x)),
     //     static_cast<int>((static_cast<float>(src_y) / tile_size) * static_cast<float>(tile_texture_size.y)),
     //     static_cast<int>(tile_texture_size.x),
     //     static_cast<int>(tile_texture_size.y)));
     const auto             rect      = sf::IntRect(
       static_cast<int>((static_cast<float>(src_x) / tile_size) * static_cast<float>(tile_texture_size.x)),
       static_cast<int>((static_cast<float>(src_y) / tile_size) * static_cast<float>(tile_texture_size.y)),
       static_cast<int>(tile_texture_size.x),
       static_cast<int>(tile_texture_size.y));

     const ImVec2 texSize = { static_cast<float>(texture->width()), static_cast<float>(texture->height()) };

     const ImVec2 uv0     = { static_cast<float>(rect.left) / texSize.x, static_cast<float>(rect.top) / texSize.y };

     const ImVec2 uv1 = { static_cast<float>(rect.left + rect.width) / texSize.x, static_cast<float>(rect.top + rect.height) / texSize.y };

     int          pop_count     = {};
     int          pop_var_count = {};
     if (options.button_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_Button, options.button_color.value());
          ++pop_count;
     }
     if (options.button_hover_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, options.button_hover_color.value());
          ++pop_count;
     }
     if (options.button_active_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, options.button_active_color.value());
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
       glengine::ConvertGliDtoImTextureId<ImTextureID>(texture->id()),
       ((options.size.x == 0 || options.size.y == 0) ? ImVec2{ ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() } : options.size),
       uv0,
       uv1,
       ImVec4{ options.background_color },
       ImVec4{ options.tint_color });
}


// Explicit instantiation for Tiles
template [[nodiscard]] bool fme::create_tile_button(std::weak_ptr<const fme::map_sprite>, const Tile1 &, const tile_button_options &);
template [[nodiscard]] bool fme::create_tile_button(std::weak_ptr<const fme::map_sprite>, const Tile2 &, const tile_button_options &);
template [[nodiscard]] bool fme::create_tile_button(std::weak_ptr<const fme::map_sprite>, const Tile3 &, const tile_button_options &);


[[nodiscard]] bool          fme::create_color_button::operator()() const
{
     // needs to be in a std::shared_ptr to be returned from a lambda. it has no move or copy operations. Also it won't just use copy
     // elision.

     if (m_transparent_texture.width() == 0 || m_transparent_texture.height() == 0)
     {
          spdlog::error("{}", "transparent_texture is not loaded");
          return false;
     }

     int pop_count     = {};
     int pop_var_count = {};
     if (m_options.button_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_Button, m_options.button_color.value());
          ++pop_count;
     }
     if (m_options.button_hover_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonHovered, m_options.button_hover_color.value());
          ++pop_count;
     }
     if (m_options.button_active_color.has_value())
     {
          ImGui::PushStyleColor(ImGuiCol_ButtonActive, m_options.button_active_color.value());
          ++pop_count;
     }
     if (m_options.padding_size.has_value())
     {
          ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, m_options.padding_size.value());
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
     ImVec2     uv0    = ImVec2(0.0f, 0.0f);
     ImVec2     uv1    = ImVec2(1.0f, 1.0f);
     return ImGui::ImageButton(
       "##tile_image_button",
       glengine::ConvertGliDtoImTextureId<ImTextureID>(m_transparent_texture.id()),
       ((m_options.size.x == 0 || m_options.size.y == 0) ? ImVec2{ ImGui::GetTextLineHeight(), ImGui::GetTextLineHeight() }
                                                         : m_options.size),
       uv0,
       uv1,
       ImVec4{ m_options.background_color },
       ImVec4{ m_options.tint_color });
}