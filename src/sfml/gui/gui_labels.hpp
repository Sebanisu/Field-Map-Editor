//
// Created by pcvii on 11/2/2021.
//

#ifndef FIELD_MAP_EDITOR_GUI_LABELS_HPP
#define FIELD_MAP_EDITOR_GUI_LABELS_HPP
#include <string_view>
struct gui_labels
{
     static constexpr std::string_view window_title                           = { "Field-Map-Editor - SFML" };
     static constexpr std::string_view remove                                 = { "Remove" };
     static constexpr std::string_view browse                                 = { "Browse" };
     static constexpr std::string_view palette                                = { "Palette" };
     static constexpr std::string_view bpp                                    = { "BPP" };
     static constexpr std::string_view flatten                                = { "Flatten" };
     static constexpr std::string_view all                                    = { "All" };
     static constexpr std::string_view map_order                              = { "Map Order" };
     static constexpr std::string_view compact                                = { "Compact" };
     static constexpr std::string_view rows                                   = { "Rows" };
     static constexpr std::string_view width                                  = { "Width" };
     static constexpr std::string_view max_tiles                              = { "Max Tiles" };
     static constexpr std::string_view filters                                = { "Filters" };
     static constexpr std::string_view background                             = { "Background" };
     static constexpr std::string_view control_panel                          = { "Control Panel" };
     static constexpr std::string_view upscale_path                           = { "Upscale Path" };
     static constexpr std::string_view deswizzle_path                         = { "Deswizzle Path" };
     static constexpr std::string_view processing                             = { "Processing" };
     static constexpr std::string_view animation_frame                        = { "Animation Frame" };
     static constexpr std::string_view z                                      = { "Z" };
     static constexpr std::string_view blend_other                            = { "Blend Other" };
     static constexpr std::string_view blend_mode                             = { "Blend Mode" };
     static constexpr std::string_view animation_id                           = { "Animation ID" };
     static constexpr std::string_view texture_page                           = { "Texture Page" };
     static constexpr std::string_view layers                                 = { "Layers" };
     static constexpr std::string_view pupu_id                                = { "Pupu ID" };
     static constexpr std::string_view set                                    = { "Set" };
     static constexpr std::string_view disable                                = { "Disable" };
     static constexpr std::string_view enable                                 = { "Enable" };
     static constexpr std::string_view language                               = { "Language" };
     static constexpr std::string_view draw                                   = { "Draw" };
     static constexpr std::string_view draw_bit                               = { "draw-bit" };
     static constexpr std::string_view path                                   = { "Path" };
     static constexpr std::string_view field                                  = { "Field" };
     static constexpr std::string_view directory_created                      = { "Directory Created" };
     static constexpr std::string_view directory_exists                       = { "Directory Exists" };
     static constexpr std::string_view saving_textures                        = { "Saving Textures" };
     static constexpr std::string_view saving                                 = { "Saving" };
     static constexpr std::string_view file                                   = { "file" };
     static constexpr std::string_view appends                                = { "appends" };
     static constexpr std::string_view start                                  = { "Start" };
     static constexpr std::string_view count_of_maps                          = { "count of maps" };
     static constexpr std::string_view draw_grid                              = { "Draw Grid" };
     static constexpr std::string_view selected_image_preview                 = { "Selected Image Preview" };
     static constexpr std::string_view load_image_file                        = { "Load Image File..." };
     static constexpr std::string_view cancel                                 = { "Cancel" };
     static constexpr std::string_view cancel_tool_tip                        = { "Hide window and reset window state." };
     static constexpr std::string_view reset                                  = { "Reset" };
     static constexpr std::string_view reset_tool_tip                         = { "Reset window state" };
     static constexpr std::string_view save_swizzle                           = { "Save Swizzle" };
     static constexpr std::string_view swizzle                                = { "Swizzle" };
     static constexpr std::string_view disable_blending                       = { "Disable Blending" };
     static constexpr std::string_view selected_tile_info                     = { "Selected Tile Info" };
     static constexpr std::string_view hovered_tiles                          = { "Hovered Tiles" };
     static constexpr std::string_view tile_size                              = { "Tile Size" };
     static constexpr std::string_view import_image                           = { "Import Image" };
     static constexpr std::string_view select_existing_tile                   = { "Select Existing Tile" };
     static constexpr std::string_view save_map_modified                      = { "Save Map File (modified)" };
     static constexpr std::string_view save_map_unmodified                    = { "Save Map File (unmodified)" };
     static constexpr std::string_view save_mim_unmodified                    = { "Save Mim File (unmodified)" };
     static constexpr std::string_view save_displayed_texture                 = { "Save Displayed Texture Image" };
     static constexpr std::string_view locate_a_custom_upscale_directory      = { "Locate a Custom Upscale Directory" };
     static constexpr std::string_view choose_a_custom_upscale_directory      = { "Choose a Custom Upscale Directory..." };
     static constexpr std::string_view locate_a_ff8_install                   = { "Locate a FF8 Install Directory" };
     static constexpr std::string_view choose_a_ff8_install                   = { "Choose a FF8 Install Directory..." };
     static constexpr std::string_view remove_the_selected_ff8_path           = { "Remove the selected FF8 path" };
     static constexpr std::string_view batch_deswizzle_title                  = { "Batch saving deswizzle textures..." };
     static constexpr std::string_view batch_deswizzle_finish                 = { "Finished the batch deswizzle..." };
     static constexpr std::string_view load_map                               = { "Load Map..." };
     static constexpr std::string_view save_map_as                            = { "Save Map as..." };
     static constexpr std::string_view save_mim_as                            = { "Save Mim as..." };
     static constexpr std::string_view save_texture_as                        = { "Save Texture as..." };
     static constexpr std::string_view save_swizzled_textures                 = { "Save Swizzled Textures" };
     static constexpr std::string_view save_deswizzled_textures               = { "Save Deswizzled Textures (Pupu)" };
     static constexpr std::string_view possible_tiles                         = { "Possible Tiles" };
     static constexpr std::string_view generated_tiles                        = { "Generated Tiles" };
     static constexpr std::string_view choose_directory_to_load_textures_from = { "Choose directory to load textures from" };
     static constexpr std::string_view choose_directory_to_save_textures_to   = { "Choose directory to save textures to" };
     static constexpr std::string_view draw_palette_texture                   = { "Draw Palette Texture" };
     static constexpr std::string_view compact_flatten                        = { "Compact and Flatten" };
     static constexpr std::string_view batch_operation                        = { "Batch Operation" };
     static constexpr std::string_view save_map_files                         = { "Save `.map` files" };

     static constexpr std::string_view compact_flatten_warning                = {
          "The compact and flatten functions modify the `.map` file. For these changes to take effect in the game, the user must load the "
                         "updated `.map` file into the field's (.fi.fl.fs) files."
     };

     static constexpr std::string_view compact_tooltip = {
          "Reorder tiles to reduce overlapping. The game often reuses tiles with different palettes."
     };

     static constexpr std::string_view flatten_tooltip = {
          "Simplify `.map` files by reducing the number of different bits per pixel (bpp) and/or palettes. Upscales generally don't "
          "require these."
     };

     static constexpr std::string_view compact_rows_tooltip = {
          "Rows: Sort tiles from 8-bit to 4-bit and separate conflicting palettes, attempting to apply the sort to each row individually."
     };

     static constexpr std::string_view compact_all_tooltip = {
          "All: Sort tiles from 8-bit to 4-bit and separate conflicting palettes, applying the sort to all tiles at once."
     };

     static constexpr std::string_view compact_map_order_tooltip = {
          "Map Order: Creates a tile for each map entry with 16 columns and 16 rows per texture page. Tiles are changed to 4bpp, "
          "allowing 16 tiles per row."
     };

     static constexpr std::string_view flatten_bpp_tooltip = {
          "BPP: Changes all tiles to 4 bits per pixel (bpp) to maximize the number of tiles per texture page. This is applied "
          "automatically by Map Order compacting."
     };

     static constexpr std::string_view flatten_palette_tooltip = {
          "Palette: Changes all palettes to 0, potentially reducing the need to reload textures."
     };

     static constexpr std::string_view flatten_both_tooltip       = { "Both: Changes the bpp to 4 and the palette to 0." };


     static constexpr std::string_view language_dropdown_tool_tip = {
          "This Language dropdown might not change anything unless it's the remaster version of the fields archive because they have all "
          "the languages in the same file. You could change the path directly to a lang- path. Then this will override this dropdown for "
          "older versions of FF8."
     };
     static constexpr std::string_view save_swizzle_import_tool_tip = {
          "Save swizzled image wtih new `.map`. Including imported texture."
     };


     static constexpr std::string_view swizzle_tooltip = {
          "Swizzle displays the tiles in their original positions as defined in the `.mim` file, or in their current swizzled positions "
          "if "
          "they have been rearranged."
     };

     static constexpr std::string_view disable_blending_tooltip = {
          "Use Disable blending to turn off the effect that emulates PSX-style blending for tiles with semi-transparent parts, such as "
          "lights or colored glass."
     };
     static constexpr std::string_view draw_palette_texture_tooltip = {
          "Draw Palette Texture: The .mim file uses palettes to draw tiles. 4-bit palettes have 16 colors, 8-bit palettes support up to "
          "256, and 16-bit tiles don’t use palettes. This setting shows the raw palettes in a grid."
     };


     gui_labels() = delete;
};
#endif// FIELD_MAP_EDITOR_GUI_LABELS_HPP
