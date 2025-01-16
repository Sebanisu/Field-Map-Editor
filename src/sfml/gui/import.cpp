#include "import.hpp"
#include "archives_group.hpp"
#include "as_string.hpp"
#include "collapsing_tile_info.hpp"
#include "compact_type.hpp"
#include "Configuration.hpp"
#include "create_tile_button.hpp"
#include "cstdint"
#include "events.hpp"
#include "filebrowser.hpp"
#include "filter.hpp"
#include "format_imgui_text.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"
#include "imgui_color.hpp"
#include "push_pop_id.hpp"
#include "RangeConsumer.hpp"
#include "safedir.hpp"
#include "tool_tip.hpp"
#include <array>
#include <imgui.h>
#include <open_viii/graphics/BPPT.hpp>
#include <SFML/System/Time.hpp>
#include <toml++/toml.h>


namespace fme
{

void import::render() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }

     if (!selections->display_import_image)
     {
          return;
     }

     auto map_sprite = m_map_sprite.lock();
     if (!map_sprite)
     {
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     // begin imgui window
     const auto the_end = scope_guard([]() { ImGui::End(); });
     if (!ImGui::Begin(gui_labels::import_image.data()))
     {
          return;
     }
     bool                         changed      = false;
     //   * So I need to choose an existing tile to base the new tiles on.
     [[maybe_unused]] const auto &current_tile = combo_selected_tile(changed);
     // add text showing the tile's info.
     collapsing_tile_info(map_sprite, current_tile);
     //   * I need to browse for an image file.
     changed = browse_for_image_display_preview() || changed;
     //   * We need to adjust the scale to fit
     // maybe i can just create an imgui window filled with the image
     // scale the image to be the selected tile size. 16,32,64,128,256.
     changed = combo_tile_size() || changed;
     //   * We need to adjust the position
     // have a px offset? or something?
     generate_map_for_imported_image(current_tile, changed);
     collapsing_header_generated_tiles();
     // I need to detect the last used texture page and the highest source_y.
     map_sprite->const_visit_working_tiles([&](const auto &tiles) {
          if (std::ranges::empty(tiles))
          {
               return;
          }
          const auto [next_source_y, next_texture_page] = get_next_unused_y_and_texture_page(tiles);
          if (changed)
          {
               adjust_source_xy_texture_page_for_import_map(next_source_y, next_texture_page);
               update_scaled_up_render_texture();
               update_imported_render_texture();
          }
     });
     // I think I have the new tiles coords generated correctly for the swizzle and
     // for drawing in game. just need to render it into an image and merge the new
     // tiles into the `.map`
     //    * Then we can swap between swizzle and deswizzle views to show what they
     //    look like
     //    * At the end we need to be able to save and merge them with the '.map'
     //    file.
     //    * They'll probably insert before the last tile.
     //  Save button that'll save the swizzled images with new '.map'
     if (ImGui::Button(gui_labels::save_swizzle.data()))
     {
          save_swizzle_textures();
     }
     tool_tip(gui_labels::save_swizzle_import_tool_tip);
     // have a cancel button to hide window.
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::cancel.data()))
     {
          // hide window and save that it's hidden.
          selections->display_import_image = false;
          Configuration config{};
          config->insert_or_assign("selections_display_import_image", selections->display_import_image);
          config.save();
          reset_imported_image();
     }
     tool_tip(gui_labels::cancel_tool_tip);
     // have a reset button to reset window state?
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::reset.data()))
     {
          reset_imported_image();
     }
     tool_tip(gui_labels::reset_tool_tip);
}

import::variant_tile_t &import::combo_selected_tile(bool &changed) const
{
     auto                  selections   = m_selections.lock();
     auto                  map_sprite   = m_map_sprite.lock();

     static variant_tile_t current_tile = { std::monostate{} };
     if (!selections)
     {
          current_tile = std::monostate{};
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return current_tile;
     }

     if (!map_sprite)
     {
          current_tile = std::monostate{};
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return current_tile;
     }

     static std::string current_item_str = {};
     const auto         save_config      = [&]() {
          Configuration config{};
          config->insert_or_assign("selections_selected_tile", selections->selected_tile);
          config.save();
          current_item_str = std::holds_alternative<std::monostate>(current_tile) ? "" : fmt::format("{}", selections->selected_tile);
     };
     const auto  spacing      = ImGui::GetStyle().ItemInnerSpacing.x;

     const float button_size  = ImGui::GetFrameHeight();
     const float button_count = 2.0f;
     const auto  end_action =
       scope_guard([&, current_tile_id = selections->selected_tile, this]() { changed = current_tile_id != selections->selected_tile; });
     // combo box with all the tiles.
     find_selected_tile_for_import(current_tile);
     save_config();


     ImVec2 const combo_pos    = ImGui::GetCursorScreenPos();
     const auto   the_end_id_0 = PushPopID();
     static bool  was_hovered  = false;

     ImGui::PushItemWidth(ImGui::CalcItemWidth() - spacing * button_count - button_size * button_count);
     const auto pop_item_width = scope_guard(&ImGui::PopItemWidth);


     if (ImGui::BeginCombo("##Select Existing Tile", "", ImGuiComboFlags_HeightLarge))
     {
          static constexpr int  columnWidth   = 100;// Adjust as needed
          const auto            num_columns   = std::max(1, static_cast<int>(ImGui::GetContentRegionAvail().x / columnWidth));
          static constexpr auto tooltips_size = 256.F;
          const auto            cols_pop      = scope_guard([]() { ImGui::Columns(1); });
          ImGui::Columns(num_columns, "##columns", false);
          const auto the_end_combo = scope_guard([]() { ImGui::EndCombo(); });
          map_sprite->const_visit_original_tiles([&](const auto &tiles) {
               for (int tile_id = {}; const auto &tile : tiles)
               {
                    const auto next_col_pop = scope_guard([]() { ImGui::NextColumn(); });
                    const auto the_end_id_1 = PushPopID();
                    const auto iterate      = scope_guard([&tile_id]() { ++tile_id; });
                    bool       is_selected  = (selections->selected_tile == tile_id);// You can store your selection however you
                                                                              // want, outside or inside your objects
                    if (std::ranges::any_of(
                          std::array{ [&]() -> bool {
                                          bool const selected = ImGui::Selectable("", is_selected);
                                          if (ImGui::IsItemHovered())
                                          {
                                               const auto end_tooltip = scope_guard(&ImGui::EndTooltip);
                                               ImGui::BeginTooltip();
                                               format_imgui_text("{}", tile_id);
                                               const tile_button_options options = { .size = { tooltips_size, tooltips_size } };
                                               (void)create_tile_button(map_sprite, tile, options);
                                               map_sprite->enable_square(tile);
                                               was_hovered = true;
                                          }
                                          return selected;
                                     }(),
                                      []() -> bool {
                                           ImGui::SameLine();
                                           return false;
                                      }(),
                                      create_tile_button(map_sprite, tile),
                                      []() -> bool {
                                           ImGui::SameLine();
                                           return false;
                                      }(),
                                      [&tile_id]() -> bool {
                                           format_imgui_text("{}", tile_id);
                                           return false;
                                      }() },
                          std::identity{}))
                    {
                         selections->selected_tile = tile_id;
                         current_tile              = tile;
                         save_config();
                    }
                    if (is_selected)
                    {
                         ImGui::SetItemDefaultFocus();// You may set the initial focus when
                                                      // opening the combo (scrolling + for
                                                      // keyboard navigation support)
                    }
               }
          });
     }
     else if (was_hovered)
     {
          was_hovered = false;
          map_sprite->disable_square();
     }
     map_sprite->const_visit_original_tiles([&](const auto &tiles) {
          {
               // Left
               const auto pop_id_left = PushPopID();
               ImGui::SameLine(0, spacing);
               const bool disabled = std::cmp_less_equal(selections->selected_tile, 0)
                                     || std::cmp_greater_equal(selections->selected_tile - 1, std::ranges::size(tiles));
               ImGui::BeginDisabled(disabled);
               if (ImGui::ArrowButton("##l", ImGuiDir_Left))
               {
                    --selections->selected_tile;
                    current_tile = tiles[selections->selected_tile];
                    save_config();
                    changed = true;
               }
               ImGui::EndDisabled();
          }
          {
               // Right
               const auto pop_id_right = PushPopID();
               ImGui::SameLine(0, spacing);
               const bool disabled = std::cmp_greater_equal(selections->selected_tile + 1, std::ranges::size(tiles));
               ImGui::BeginDisabled(disabled);
               if (ImGui::ArrowButton("##r", ImGuiDir_Right))
               {
                    ++selections->selected_tile;
                    current_tile = tiles[selections->selected_tile];
                    save_config();
                    changed = true;
               }
               ImGui::EndDisabled();
          }
     });

     ImGui::SameLine(0, spacing);
     format_imgui_text("{}", gui_labels::select_existing_tile);
     ImVec2 const      backup_pos = ImGui::GetCursorScreenPos();
     ImGuiStyle const &style      = ImGui::GetStyle();
     ImGui::SetCursorScreenPos(ImVec2(combo_pos.x + style.FramePadding.x, combo_pos.y /*+ style.FramePadding.y*/));
     using namespace open_viii::graphics::background;
     (void)std::visit(
       events::make_visitor(
         [&](const is_tile auto &tile) -> bool { return create_tile_button(map_sprite, tile); },
         [](const std::monostate &) -> bool { return false; }),
       current_tile);
     ImGui::SameLine();
     format_imgui_text("{}", current_item_str);
     ImGui::SetCursorScreenPos(backup_pos);
     return current_tile;
}

import::import() {

};

bool import::browse_for_image_display_preview() const
{
     auto selections = m_selections.lock();
     auto map_sprite = m_map_sprite.lock();

     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return false;
     }

     if (!map_sprite)
     {
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return false;
     }

     bool changed = false;
     ImGui::InputText("##image_path", m_import_image_path.data(), m_import_image_path.size(), ImGuiInputTextFlags_ReadOnly);
     ImGui::SameLine();
     if (ImGui::Button(gui_labels::browse.data()))
     {
          m_load_file_browser.Open();
          m_load_file_browser.SetTitle(gui_labels::load_image_file.data());
          m_load_file_browser.SetTypeFilters({ ".png" });
          m_load_file_browser.SetPwd(Configuration{}["load_image_path"].value_or(std::filesystem::current_path().string()));
          m_load_file_browser.SetInputName(m_import_image_path.data());
     }
     m_load_file_browser.Display();

     if (m_load_file_browser.HasSelected())
     {
          Configuration config{};
          config->insert_or_assign("load_image_path", m_load_file_browser.GetPwd().string());
          config.save();
          [[maybe_unused]] const auto selected_path = m_load_file_browser.GetSelected();
          m_import_image_path                       = selected_path.string();
          m_load_file_browser.ClearSelected();
          m_loaded_image_texture.loadFromFile(m_import_image_path);// stored on gpu.
          m_loaded_image_texture.setRepeated(false);
          m_loaded_image_texture.setSmooth(false);
          m_loaded_image_texture.generateMipmap();
          changed = true;
     }
     if (m_loaded_image_texture.getSize().x == 0 || m_loaded_image_texture.getSize().y == 0)
     {
          return false;
     }
     if (ImGui::CollapsingHeader(gui_labels::selected_image_preview.data()))
     {
          sf::Sprite const sprite(m_loaded_image_texture);
          const float      width             = std::max((ImGui::GetContentRegionAvail().x), 1.0F);
          const auto       size              = m_loaded_image_texture.getSize();

          float const      scale             = width / static_cast<float>(size.x);
          const float      height            = static_cast<float>(size.y) * scale;
          ImVec2 const     cursor_screen_pos = ImGui::GetCursorScreenPos();
          const auto       pop_id            = PushPopID();
          const auto       str_id            = fmt::format("id2668{}", get_imgui_id());
          ImGui::ImageButton(str_id.c_str(), sprite, sf::Vector2f(width, height));
          if (ImGui::Checkbox(gui_labels::draw_grid.data(), &selections->import_image_grid))
          {
               Configuration config{};
               config->insert_or_assign("selections_import_image_grid", selections->import_image_grid);
               config.save();
          }
          if (selections->import_image_grid)
          {
               static constexpr float thickness = 2.0F;
               static const ImU32     color_32  = imgui_color32(sf::Color::Red);
               for (auto x_pos = static_cast<std::uint32_t>(selections->tile_size_value); x_pos < size.x;
                    x_pos += static_cast<std::underlying_type_t<tile_sizes>>(selections->tile_size_value))
               {
                    ImGui::GetWindowDrawList()->AddLine(
                      ImVec2(cursor_screen_pos.x + (static_cast<float>(x_pos) * scale), cursor_screen_pos.y),
                      ImVec2(
                        cursor_screen_pos.x + (static_cast<float>(x_pos) * scale),
                        cursor_screen_pos.y + (static_cast<float>(size.y) * scale)),
                      color_32,
                      thickness);
               }

               for (auto y_pos = static_cast<std::uint32_t>(selections->tile_size_value); y_pos < size.y;
                    y_pos += static_cast<std::underlying_type_t<tile_sizes>>(selections->tile_size_value))
               {
                    ImGui::GetWindowDrawList()->AddLine(
                      ImVec2(cursor_screen_pos.x, cursor_screen_pos.y + (static_cast<float>(y_pos) * scale)),
                      ImVec2(
                        cursor_screen_pos.x + (static_cast<float>(size.x) * scale),
                        cursor_screen_pos.y + (static_cast<float>(y_pos) * scale)),
                      color_32,
                      thickness);
               }
          }
     }
     return changed;
}


bool import::combo_tile_size() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return false;
     }


     static constexpr auto values =
       std::array{ tile_sizes::default_size, tile_sizes::x_2_size, tile_sizes::x_4_size, tile_sizes::x_8_size, tile_sizes::x_16_size };
     const auto gcc = GenericComboClass(
       gui_labels::tile_size,
       []() -> decltype(auto) { return values; },
       []() { return values | std::views::transform(AsString{}); },
       selections->tile_size_value);
     if (!gcc.render())
     {
          return false;
     }
     Configuration config{};
     config->insert_or_assign("selections_tile_size_value", static_cast<std::underlying_type_t<tile_sizes>>(selections->tile_size_value));
     config.save();
     return true;
}
void import::generate_map_for_imported_image(const variant_tile_t &current_tile, bool changed) const
{//   * I'd probably store the new tiles in their own map.
     using namespace open_viii::graphics::background;
     using namespace open_viii::graphics;

     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     const auto tiles_wide = static_cast<uint32_t>(
       ceil(static_cast<double>(static_cast<float>(m_loaded_image_texture.getSize().x) / static_cast<float>(selections->tile_size_value))));
     const auto tiles_high = static_cast<uint32_t>(
       ceil(static_cast<double>(static_cast<float>(m_loaded_image_texture.getSize().y) / static_cast<float>(selections->tile_size_value))));
     format_imgui_text("{}: {} x {} = {}", gui_labels::possible_tiles, tiles_wide, tiles_high, tiles_wide * tiles_high);
     if (changed && tiles_wide * tiles_high != 0U && m_loaded_image_texture.getSize() != sf::Vector2u{})
     {
          m_import_image_map = open_viii::graphics::background::Map(
            [&current_tile, x_tile = uint8_t{}, y_tile = uint8_t{}, &tiles_high, &tiles_wide]() mutable {
                 return std::visit(
                   [&](auto tile) -> variant_tile_t {
                        if constexpr (is_tile<std::decay_t<decltype(tile)>>)
                        {
                             if (x_tile == tiles_wide)
                             {
                                  x_tile = 0;
                                  ++y_tile;
                             }
                             if (y_tile == tiles_high)
                             {
                                  return std::monostate{};
                             }
                             //   * Set new tiles to 4 bit to get max amount of tiles.
                             tile =
                               tile.with_depth(BPPT::BPP4_CONST())
                                 .with_source_xy({ static_cast<uint8_t>(x_tile * tile_size_px_unsigned),
                                                   static_cast<uint8_t>(y_tile * tile_size_px_unsigned) })
                                 .with_xy({ static_cast<int16_t>(x_tile * tile_size_px), static_cast<int16_t>(y_tile * tile_size_px) });

                             // iterate
                             ++x_tile;
                             return tile;
                        }
                        else
                        {
                             return std::monostate{};
                        }
                   },
                   current_tile);
            });
          filter_empty_import_tiles();
     }
}

void import::collapsing_header_generated_tiles() const
{

     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }

     if (!ImGui::CollapsingHeader(
           m_import_image_map.visit_tiles([](auto &&tiles) { return fmt::format("{}: {}", gui_labels::generated_tiles, std::size(tiles)); })
             .c_str()))
     {
          return;
     }


     static constexpr int columns = 9;
     if (!ImGui::BeginTable("import_tiles_table", columns))
     {
          return;
     }
     const auto the_end_tile_table = scope_guard([]() { ImGui::EndTable(); });
     m_import_image_map.visit_tiles([&](auto &tiles) {
          std::uint32_t i{};
          for (const auto &tile : tiles)
          {
               ImGui::TableNextColumn();
               sf::Sprite const sprite(
                 m_loaded_image_texture,
                 sf::IntRect(
                   static_cast<int>(tile.x() / tile_size_px * selections->tile_size_value),
                   static_cast<int>(tile.y() / tile_size_px * selections->tile_size_value),
                   static_cast<int>(selections->tile_size_value),
                   static_cast<int>(selections->tile_size_value)));
               const auto             the_end_tile_table_tile = PushPopID();
               static constexpr float button_size             = 32.F;

               const auto             str                     = fmt::format("tb{}", i++);
               ImGui::ImageButton(str.c_str(), sprite, sf::Vector2f(button_size, button_size));
          }
     });
}

void import::update_scaled_up_render_texture() const
{

     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }

     const auto scale_up_dim = [&](uint32_t dim) {
          return static_cast<uint32_t>(
            ceil(static_cast<double>(dim) / static_cast<double>(selections->tile_size_value))
            * static_cast<double>(selections->tile_size_value));
     };
     const auto size = m_loaded_image_texture.getSize();
     if (size == decltype(size){})
     {
          return;
     }
     m_loaded_image_render_texture.create(scale_up_dim(size.x), scale_up_dim(size.y));
     m_loaded_image_render_texture.setActive(true);
     m_loaded_image_render_texture.clear(sf::Color::Transparent);
     sf::Sprite sprite = sf::Sprite(m_loaded_image_texture);
     sprite.setScale(1.F, -1.F);
     sprite.setPosition(0.F, static_cast<float>(m_loaded_image_render_texture.getSize().y));
     m_loaded_image_render_texture.draw(sprite);
     m_loaded_image_render_texture.setRepeated(false);
     m_loaded_image_render_texture.setSmooth(false);
     m_loaded_image_render_texture.generateMipmap();
}


void import::update_imported_render_texture() const
{

     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     auto map_sprite = m_map_sprite.lock();
     if (!map_sprite)
     {
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     if (selections->render_imported_image)
     {
          map_sprite->update_render_texture(&m_loaded_image_render_texture.getTexture(), m_import_image_map, selections->tile_size_value);
     }
}

void import::adjust_source_xy_texture_page_for_import_map(uint8_t next_source_y, const uint8_t next_texture_page) const
{
     m_import_image_map.visit_tiles([&](auto &&import_tiles) {
          auto       tile_i   = import_tiles.begin();
          const auto tile_end = import_tiles.end();
          for (uint8_t tp = next_texture_page; tp < tile_size_px; ++tp)
          {
               for (uint8_t pixel_y = next_source_y; pixel_y < tile_size_px; ++pixel_y)
               {
                    next_source_y = 0;
                    for (uint8_t pixel_x = 0; pixel_x < tile_size_px; ++pixel_x)
                    {
                         if (tile_i == tile_end)
                         {
                              return;
                         }
                         *tile_i = tile_i->with_source_xy(pixel_x * tile_size_px, pixel_y * tile_size_px).with_texture_id(tp);
                         ++tile_i;
                    }
               }
          }
     });
}


void import::save_swizzle_textures() const
{
     auto map_sprite = m_map_sprite.lock();
     if (!map_sprite)
     {
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     m_directory_browser.Open();
     m_directory_browser.SetTitle(map_sprite->appends_prefix_base_name(gui_labels::choose_directory_to_save_textures_to));
     m_directory_browser.SetPwd(Configuration{}["swizzle_path"].value_or(std::filesystem::current_path().string()));
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_modified_directory_map = map_directory_mode::save_swizzle_textures;
}

void import::reset_imported_image() const
{
     auto map_sprite = m_map_sprite.lock();
     if (!map_sprite)
     {
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     map_sprite->update_render_texture(nullptr, {}, tile_sizes::default_size);
     m_import_image_map                = {};
     m_loaded_image_texture            = {};
     m_loaded_image_cpu                = {};
     m_import_image_path               = {};
     selections->render_imported_image = false;
     Configuration config{};
     config->insert_or_assign("selections_render_imported_image", selections->render_imported_image);
     config.save();
}


void import::find_selected_tile_for_import(import::variant_tile_t &current_tile) const
{
     using namespace open_viii::graphics::background;
     auto map_sprite = m_map_sprite.lock();
     if (!map_sprite)
     {
          spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     map_sprite->const_visit_working_tiles([&](const auto &tiles) {
          if (selections->selected_tile < 0 || std::cmp_greater_equal(selections->selected_tile, tiles.size()))
          {
               current_tile = std::monostate{};
               return;
          }
          std::visit(
            [&](const auto &tile) {
                 if (std::cmp_less(selections->selected_tile, tiles.size()))
                 {
                      const auto &tmp_tile = tiles[static_cast<size_t>(selections->selected_tile)];
                      if constexpr (std::is_same_v<std::decay_t<decltype(tile)>, std::decay_t<decltype(tmp_tile)>>)
                      {
                           if (tile != tmp_tile)
                           {
                                current_tile = tmp_tile;
                           }
                      }
                      else if constexpr (!is_tile<std::decay_t<decltype(tile)>>)
                      {
                           current_tile = tmp_tile;
                      }
                 }
            },
            current_tile);
     });
}

void import::filter_empty_import_tiles() const
{//* Filter empty tiles
     auto selections = m_selections.lock();

     if (!selections)
     {
          spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
          return;
     }
     m_loaded_image_cpu = m_loaded_image_texture.copyToImage();
     m_import_image_map.visit_tiles([&](auto &tiles) {
          const auto rem_range = std::ranges::remove_if(tiles, [&](const auto &tile) -> bool {
               const auto          x_start = tile.x() / tile_size_px * selections->tile_size_value;
               const auto          y_start = tile.y() / tile_size_px * selections->tile_size_value;
               const int           x_max   = x_start + selections->tile_size_value;
               const sf::Vector2u &imgsize = m_loaded_image_cpu.getSize();
               const auto          x_end   = (std::min)(static_cast<int>(imgsize.x), x_max);
               const int           y_max   = y_start + selections->tile_size_value;
               const auto          y_end   = (std::min)(static_cast<int>(imgsize.y), y_max);
               for (auto pixel_x = x_start; std::cmp_less(pixel_x, x_end); ++pixel_x)
               {
                    for (auto pixel_y = y_start; std::cmp_less(pixel_y, y_end); ++pixel_y)
                    {
                         const auto color =
                           m_loaded_image_cpu.getPixel(static_cast<unsigned int>(pixel_x), static_cast<unsigned int>(pixel_y));
                         if (std::cmp_greater(color.a, 0U))
                         {
                              return false;
                         }
                    }
               }
               return true;
          });
          tiles.erase(rem_range.begin(), rem_range.end());
     });
     m_loaded_image_cpu = {};
}
bool import::checkbox_render_imported_image() const
{
     if (m_loaded_image_texture.getSize() != sf::Vector2u{})
     {
          auto selections = m_selections.lock();
          auto map_sprite = m_map_sprite.lock();

          if (!selections)
          {
               spdlog::error("m_selections is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
               return false;
          }

          if (!map_sprite)
          {
               spdlog::error("m_map_sprite is no longer valid. File: {}, Line: {}", __FILE__, __LINE__);
               return false;
          }

          if (ImGui::Checkbox(gui_labels::render_imported_image.data(), &selections->render_imported_image))
          {
               Configuration config{};
               config->insert_or_assign("selections_render_imported_image", selections->render_imported_image);
               config.save();

               // Pass texture and map and tile_size
               update_imported_render_texture();

               if (!selections->render_imported_image)
               {
                    map_sprite->update_render_texture(nullptr, {}, tile_sizes::default_size);
               }
               return true;
          }
     }
     return false;
}
// Update functions for m_selections and m_map_sprite
void import::update(const std::shared_ptr<Selections> &new_selections) const
{
     m_selections = new_selections;
}

void import::update(const std::shared_ptr<map_sprite> &new_map_sprite) const
{
     m_map_sprite = new_map_sprite;
}

}// namespace fme