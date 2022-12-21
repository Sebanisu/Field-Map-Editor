//
// Created by pcvii on 12/21/2022.
//
#ifndef FIELD_MAP_EDITOR_SELECTIONS_HPP
#define FIELD_MAP_EDITOR_SELECTIONS_HPP
#include "Configuration.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>
struct selections
{
  std::string path = []() {
    std::error_code ec  = {};
    auto            str = std::filesystem::current_path(ec).string();
    if (ec)
    {
      spdlog::warn(
        "{}:{} - {}: {} path: \"{}\"",
        __FILE__,
        __LINE__,
        ec.value(),
        ec.message(),
        str);
      ec.clear();
    }
    return str;
  }();
  int           bpp                            = {};
  int           palette                        = {};
  int           field                          = {};
  int           coo                            = {};

  int           draw                           = { 1 };
  int           selected_tile                  = { -1 };
  std::uint32_t window_width                   = { 800 };
  std::uint32_t window_height                  = { 600 };
  std::uint16_t tile_size_value                = { 16U };
  bool          draw_palette                   = { false };
  bool          draw_grid                      = { false };
  bool          draw_texture_page_grid         = { false };
  bool          draw_swizzle                   = { false };
  bool          draw_disable_blending          = { false };
  bool          test_batch_window              = { false };
  bool          display_import_image           = { false };
  bool          import_image_grid              = { false };
  bool          render_imported_image          = { false };
  bool          batch_embed_map_warning_window = { false };
  selections()
  {
    Configuration config{};
    path          = config["selections_path"].value_or(path);
    window_width  = config["selections_window_width"].value_or(window_width);
    window_height = config["selections_window_width"].value_or(window_height);
    palette       = config["selections_palette"].value_or(palette);
    bpp           = config["selections_bpp"].value_or(bpp);
    draw          = config["selections_draw"].value_or(draw);
    coo           = config["selections_coo"].value_or(coo);
    selected_tile = config["selections_selected_tile"].value_or(selected_tile);
    draw_disable_blending = config["selections_draw_disable_blending"].value_or(
      draw_disable_blending);
    draw_grid    = config["selections_draw_grid"].value_or(draw_grid);
    draw_palette = config["selections_draw_palette"].value_or(draw_palette);
    draw_swizzle = config["selections_draw_swizzle"].value_or(draw_swizzle);
    //  render_imported_image =
    //  config["selections_render_imported_image"].value_or(
    //    render_imported_image);
    draw_texture_page_grid =
      config["selections_draw_texture_page_grid"].value_or(
        draw_texture_page_grid);
    test_batch_window =
      config["selections_test_batch_window"].value_or(test_batch_window);
    display_import_image =
      config["selections_display_import_image"].value_or(display_import_image);
    import_image_grid =
      config["selections_import_image_grid"].value_or(import_image_grid);
    tile_size_value =
      config["selections_tile_size_value"].value_or(tile_size_value);
  }
};
#endif// FIELD_MAP_EDITOR_SELECTIONS_HPP
