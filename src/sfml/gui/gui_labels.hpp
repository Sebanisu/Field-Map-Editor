//
// Created by pcvii on 11/2/2021.
//

#ifndef FIELD_MAP_EDITOR_GUI_LABELS_HPP
#define FIELD_MAP_EDITOR_GUI_LABELS_HPP
#include <array>
#include <string_view>
namespace fme
{
struct gui_labels
{
     static constexpr std::string_view window_title                           = { "Field-Map-Editor - SFML" };
     static constexpr std::string_view remove                                 = { "Remove" };
     static constexpr std::string_view browse                                 = { "Browse" };
     static constexpr std::string_view palette                                = { "Palette" };
     static constexpr std::string_view bpp                                    = { "BPP" };
     static constexpr std::string_view bpp_and_palette                        = { "BPP & Palette" };
     static constexpr std::string_view hex                                    = { "Hex" };
     static constexpr std::string_view source                                 = { "Source" };
     static constexpr std::string_view destination                            = { "Destination" };
     static constexpr std::string_view flatten                                = { "Flatten" };
     static constexpr std::string_view all                                    = { "All" };
     static constexpr std::string_view map_order                              = { "Map Order" };
     static constexpr std::string_view compact                                = { "Compact" };
     static constexpr std::string_view move_conflicts                         = { "Move Conflicts" };
     static constexpr std::string_view move_conflicts_only                    = { "Move Conflicts Only" };
     static constexpr std::string_view rows                                   = { "Rows" };
     static constexpr std::string_view width                                  = { "Width" };
     static constexpr std::string_view windows                                = { "Windows" };
     static constexpr std::string_view max_tiles                              = { "Max Tiles" };
     static constexpr std::string_view filters                                = { "Filters" };
     static constexpr std::string_view background                             = { "Background" };
     static constexpr std::string_view control_panel                          = { "Control Panel" };
     static constexpr std::string_view upscale_path                           = { "Upscale Path" };
     static constexpr std::string_view deswizzle_path                         = { "Deswizzle Path" };
     static constexpr std::string_view upscale_map_path                           = { "Upscale .map Path" };
     static constexpr std::string_view deswizzle_map_path                         = { "Deswizzle .map Path" };
     static constexpr std::string_view processing                             = { "Processing" };
     static constexpr std::string_view display_history                        = { "History" };
     static constexpr std::string_view history                                = { "History" };
     static constexpr std::string_view animation_frame                        = { "Animation Frame" };
     static constexpr std::string_view z                                      = { "Z" };
     static constexpr std::string_view blend_other                            = { "Blend Other" };
     static constexpr std::string_view blend_mode                             = { "Blend Mode" };
     static constexpr std::string_view animation_id                           = { "Animation ID" };
     static constexpr std::string_view texture_page                           = { "Texture Page" };
     static constexpr std::string_view layer_id                               = { "Layer ID" };
     static constexpr std::string_view pupu_id                                = { "Pupu ID" };
     static constexpr std::string_view set                                    = { "Set" };
     static constexpr std::string_view disable                                = { "Disable" };
     static constexpr std::string_view enable                                 = { "Enable" };
     static constexpr std::string_view language                               = { "Language" };
     static constexpr std::string_view draw                                   = { "Draw" };
     static constexpr std::string_view draw_bit                               = { "Draw-Bit" };
     static constexpr std::string_view path                                   = { "Path" };
     static constexpr std::string_view mim                                    = { "Mim" };
     static constexpr std::string_view field                                  = { "Field" };
     static constexpr std::string_view directory_created                      = { "Directory Created" };
     static constexpr std::string_view directory_exists                       = { "Directory Exists" };
     static constexpr std::string_view saving_textures                        = { "Saving Textures" };
     static constexpr std::string_view saving                                 = { "Saving" };
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
     static constexpr std::string_view swizzle_as_one_image = {"Swizzle as one Image"};
     static constexpr std::string_view deswizzle                              = { "Deswizzle" };
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
     static constexpr std::string_view load_map_file                          = { "Load Map File" };
     static constexpr std::string_view batch_load_map                         = { "Load `.map` files" };
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
     static constexpr std::string_view batch_operation_window                 = { "Batch Swizzle/Deswizzle" };
     static constexpr std::string_view explore                                = { "Explore" };
     static constexpr std::string_view forced_on_while_swizzled               = { "Forced on while Swizzled..." };

     static constexpr std::string_view choose_input_directory                 = { "Choose directory to load textures from..." };
     static constexpr std::string_view choose_output_directory                = { "Choose directory to save textures to..." };

     static constexpr std::string_view input_type                             = { "Input Type" };
     static constexpr std::string_view input_root_path_type                   = { "Input Root Path Type" };
     
     static constexpr std::string_view output_type                            = { "Output Type" };
     static constexpr std::string_view output_root_path_type                  = { "Output Root Path Type" };

     static constexpr std::string_view input_path                             = { "Input Path" };
     static constexpr std::string_view output_path                            = { "Output Path" };
     static constexpr std::string_view select_none                            = { "Select None" };
     static constexpr std::string_view select_all                             = { "Select All" };
     static constexpr std::string_view draw_window_title                      = { "Draw" };
     static constexpr std::string_view display_draw_window                    = { "Draw" };
     static constexpr std::string_view display_control_panel_window           = { "Control Panel" };
     static constexpr std::string_view render_imported_image                  = { "Render Imported Image" };
     static constexpr std::string_view mouse_not_over                         = { "Mouse not over the image." };
     static constexpr std::string_view mouse_pos                              = { "Mouse Pos" };
     static constexpr std::string_view tile_pos                               = { "Tile Pos" };
     static constexpr std::string_view page                                   = { "Page" };
     static constexpr std::string_view number_of_tiles                        = { "Number of Tiles" };

     static constexpr std::string_view file                                   = { "File" };
     static constexpr std::string_view edit                                   = { "Edit" };
     static constexpr std::string_view import                                 = { "Import" };
     static constexpr std::string_view import_page                            = { "Import" };
     static constexpr std::string_view undo                                   = { "Undo" };
     static constexpr std::string_view redo                                   = { "Redo" };
     static constexpr std::string_view undo_all                               = { "Undo All" };
     static constexpr std::string_view redo_all                               = { "Redo All" };
     static constexpr std::string_view draw_tile_grid                         = { "Draw Tile Grid" };
     static constexpr std::string_view draw_texture_page_grid                 = { "Draw Texture Page Grid" };
     static constexpr std::string_view draw_tile_conflict_rects               = { "Draw Tile Conflict Rectangles" };
     static constexpr std::string_view update_position                        = { "Update Position" };


     static constexpr std::string_view begin_batch_operation                  = { "Begin Batch Operation..." };
     static constexpr std::string_view stop                                   = { "Stop" };

     static constexpr std::string_view save_map_files                         = { "Save `.map` files" };

     static constexpr std::string_view draw_bit_all_tooltip                   = { "Show all regardless of bit being enabled or disabled." };

     static constexpr std::string_view draw_bit_enabled_tooltip               = { "Show only tiles with draw bit enabled" };

     static constexpr std::string_view draw_bit_disabled_tooltip              = { "Show only tiles with draw bit disabled" };
     static constexpr std::string_view display_custom_paths_window            = { "Display Custom Paths Window" };
     static constexpr std::string_view display_field_file_window              = { "Display Field File Window" };

     static constexpr std::string_view field_file_window                      = { "Field File Window" };
     static constexpr std::string_view custom_paths_window                    = { "Custom Paths Window" };

     static constexpr std::string_view move_conflicts_only_tooltip            = {
          "Move Only Conflicts: It takes any tiles with conflicting source locations and it moves all but one to new locations that are "
                     "detected as empty. It also changes the depth to 4bpp. "
     };

     static constexpr std::string_view explore_tooltip = {
          "Opens the file explorer window in the currently assigned directory. On Windows, this opens File Explorer; on Linux, it should "
          "open a similar file browser."
     };

     static constexpr std::string_view input_mim_tooltip = {
          "Mim is the storage of the field texture in FF8. So this selection means your using the original low res textures."
     };
     static constexpr std::string_view input_deswizzle_tooltip = {
          "Load a directory of deswizzled textures. Including `.map` files if found. They may be upscaled."
     };
     static constexpr std::string_view input_swizzle_tooltip = {
          "Load a directory of swizzled textures. Including `.map` files if found. They may be upscaled."
     };

     static constexpr std::string_view compact_flatten_warning = {
          "The compact and flatten functions modify the `.map` file. For these changes to take effect in the game, the user must load "
          "the updated `.map` file into the field's (.fi.fl.fs) files."
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

     static constexpr std::string_view compact_map_order_ffnx         = { "Map Order 2" };
     static constexpr std::string_view compact_map_order_ffnx2        = { "Map Order 2: FFNX" };
     static constexpr std::string_view compact_map_order_ffnx_tooltip = {
          "FFNX Map Order: order tiles in rows of (number_of_tiles / 16). Changes all the tiles' deapth to 4bpp."
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
     static constexpr std::string_view language_generic_tool_tip    = { "Generic this is for matching files without lang code in them." };
     static constexpr std::string_view save_swizzle_import_tool_tip = {
          "Save swizzled image wtih new `.map`. Including imported texture."
     };


     static constexpr std::string_view swizzle_tooltip = {
          "Swizzle mode displays the tiles in their original positions as defined in the `.mim` file, or in their current swizzled "
          "positions "
          "if "
          "they have been rearranged."
     };

     static constexpr std::string_view deswizzle_tooltip = {
          "Deswizzle mode displays tiles in the positions where they would draw in game. It's a best effort attempt to emulate the way the "
          "game draws the image."
     };


     static constexpr std::string_view disable_blending_tooltip = {
          "Use Disable blending to turn off the effect that emulates PSX-style blending for tiles with semi-transparent parts, such as "
          "lights or colored glass."
     };
     static constexpr std::string_view draw_palette_texture_tooltip = {
          "Draw Palette Texture: The .mim file uses palettes to draw tiles. 4-bit palettes have 16 colors, 8-bit palettes support up to "
          "256, and 16-bit tiles don’t use palettes. This setting shows the raw palettes in a grid."
     };
     static constexpr auto tile_values =
       std::array{ hex,         source,       destination,     z,   bpp, palette, texture_page, layer_id, blend_mode,
                   blend_other, animation_id, animation_frame, draw };


     static constexpr std::string_view selected_path         = { "{selected_path}" };
     static constexpr std::string_view ff8_path              = { "{ff8_path}" };
     static constexpr std::string_view current_path          = { "{current_path}" };
     static constexpr std::string_view selected_path_tooltip = { "The user chooses a path to output or read data from." };
     static constexpr std::string_view ff8_path_tooltip      = { "The root directory where Final Fantasy VIII is installed." };
     static constexpr std::string_view current_path_tooltip  = { "The current working directory." };
     gui_labels()                                            = delete;
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_GUI_LABELS_HPP
