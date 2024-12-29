//
// Created by pcvii on 11/2/2021.
//

#ifndef FIELD_MAP_EDITOR_GUI_LABELS_HPP
#define FIELD_MAP_EDITOR_GUI_LABELS_HPP
#include <string_view>
struct gui_labels
{
     static constexpr const char      *window_title                 = { "Field-Map-Editor - SFML" };

     static constexpr std::string_view palette                      = { "Palette" };
     static constexpr std::string_view bpp                          = { "BPP" };
     static constexpr std::string_view flatten                      = { "Flatten" };
     static constexpr std::string_view all                          = { "All" };
     static constexpr std::string_view map_order                    = { "Map Order" };
     static constexpr std::string_view compact                      = { "Compact" };
     static constexpr std::string_view rows                         = { "Rows" };
     static constexpr std::string_view width                        = { "Width" };
     static constexpr std::string_view max_tiles                    = { "Max Tiles" };
     static constexpr std::string_view filters                      = { "Filters" };
     static constexpr std::string_view background                   = { "Background" };
     static constexpr std::string_view control_panel                = { "Control Panel" };
     static constexpr std::string_view upscale_path                 = { "Upscale Path" };
     static constexpr std::string_view deswizzle_path               = { "Deswizzle Path" };
     static constexpr std::string_view processing                   = { "Processing" };
     static constexpr std::string_view animation_frame              = { "Animation Frame" };
     static constexpr std::string_view z                            = { "Z" };
     static constexpr std::string_view blend_other                  = { "Blend Other" };
     static constexpr std::string_view blend_mode                   = { "Blend Mode" };
     static constexpr std::string_view animation_id                 = { "Animation ID" };
     static constexpr std::string_view texture_page                 = { "Texture Page" };
     static constexpr std::string_view layers                       = { "Layers" };
     static constexpr std::string_view pupu_id                      = { "Pupu ID" };
     static constexpr std::string_view set                          = { "Set" };
     static constexpr std::string_view disable                      = { "Disable" };
     static constexpr std::string_view enable                       = { "Enable" };
     static constexpr std::string_view language                     = { "Language" };
     static constexpr std::string_view draw                         = { "Draw" };
     static constexpr std::string_view draw_bit                     = { "draw-bit" };
     static constexpr std::string_view path                         = { "Path" };
     static constexpr std::string_view field                        = { "Field" };
     static constexpr std::string_view directory_created            = { "Directory Created" };
     static constexpr std::string_view directory_exists             = { "Directory Exists" };
     static constexpr std::string_view saving_textures              = { "Saving Textures" };
     static constexpr std::string_view saving                       = { "Saving" };
     static constexpr std::string_view file                         = { "file" };
     static constexpr std::string_view start                        = { "Start" };
     static constexpr std::string_view count_of_maps                = { "count of maps" };
     static constexpr std::string_view draw_grid                    = { "Draw Grid" };
     static constexpr std::string_view selected_image_preview       = { "Selected Image Preview" };
     static constexpr std::string_view load_image_file              = { "Load Image File..." };
     static constexpr std::string_view cancel                       = { "Cancel" };
     static constexpr std::string_view cancel_tool_tip              = { "Hide window and reset window state." };
     static constexpr std::string_view reset                        = { "Reset" };
     static constexpr std::string_view reset_tool_tip               = { "Reset window state" };
     static constexpr std::string_view save_swizzle                 = { "Save Swizzle" };
     static constexpr std::string_view swizzle                      = { "Swizzle" };
     static constexpr std::string_view disable_blending             = { "Disable Blending" };
     static constexpr std::string_view selected_tile_info           = { "Selected Tile Info" };
     static constexpr std::string_view hovered_tiles                = { "Hovered Tiles" };
     static constexpr std::string_view tile_size                    = { "Tile Size" };
     static constexpr std::string_view import_image                    = { "Import Image" };
     

       static constexpr std::string_view batch_deswizzle_title        = { "Batch saving deswizzle textures..." };
     static constexpr std::string_view batch_deswizzle_finish       = { "Finished the batch deswizzle..." };

     static constexpr std::string_view possible_tiles               = { "Possible Tiles" };
     static constexpr std::string_view generated_tiles              = { "Generated Tiles" };
     static constexpr std::string_view save_swizzle_import_tool_tip = {
          "Save swizzled image wtih new `.map`. Including imported texture."
     };



     static constexpr std::string_view swizzle_tooltip        = {
          "Swizzle displays the tiles in their original positions as defined in the `.mim` file, or in their current swizzled positions "
                 "if "
                 "they have been rearranged."
     };

     static constexpr std::string_view disable_blending_tooltip = {
          "Use Disable blending to turn off the effect that emulates PSX-style blending for tiles with semi-transparent parts, such as "
          "lights or colored glass."
     };

     gui_labels() = delete;
};
#endif// FIELD_MAP_EDITOR_GUI_LABELS_HPP
