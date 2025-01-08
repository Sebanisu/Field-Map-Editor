//
// Created by pcvii on 12/21/2022.
//

#include "Selections.hpp"
fme::Selections::Selections()
{
     Configuration const config{};
     path          = config["selections_path"].value_or(path);
     window_width  = config["selections_window_width"].value_or(window_width);
     window_height = config["selections_window_width"].value_or(window_height);
     palette       = config["selections_palette"].value_or(palette);
     bpp           = config["selections_bpp"].value_or(bpp);
     draw          = static_cast<draw_mode>(config["selections_draw"].value_or(static_cast<std::underlying_type_t<draw_mode>>(draw)));
     coo = static_cast<open_viii::LangT>(config["selections_coo"].value_or(static_cast<std::underlying_type_t<open_viii::LangT>>(coo)));
     selected_tile          = config["selections_selected_tile"].value_or(selected_tile);
     draw_disable_blending  = config["selections_draw_disable_blending"].value_or(draw_disable_blending);
     draw_grid              = config["selections_draw_grid"].value_or(draw_grid);
     draw_palette           = config["selections_draw_palette"].value_or(draw_palette);
     draw_swizzle           = config["selections_draw_swizzle"].value_or(draw_swizzle);
     //  render_imported_image =
     //  config["selections_render_imported_image"].value_or(
     //    render_imported_image);
     draw_texture_page_grid = config["selections_draw_texture_page_grid"].value_or(draw_texture_page_grid);
     display_batch_window   = config["selections_display_batch_window"].value_or(display_batch_window);
     display_import_image   = config["selections_display_import_image"].value_or(display_import_image);
     import_image_grid      = config["selections_import_image_grid"].value_or(import_image_grid);
     tile_size_value        = static_cast<tile_sizes>(
       config["selections_tile_size_value"].value_or(static_cast<std::underlying_type_t<tile_sizes>>(tile_size_value)));
     display_history_window = config["selections_display_history_window"].value_or(display_history_window);
     display_control_panel_window = config["selections_display_control_panel_window"].value_or(display_control_panel_window);
     display_draw_window = config["selections_display_draw_window"].value_or(display_draw_window);
}
