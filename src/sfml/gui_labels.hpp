//
// Created by pcvii on 11/2/2021.
//

#ifndef FIELD_MAP_EDITOR_GUI_LABELS_HPP
#define FIELD_MAP_EDITOR_GUI_LABELS_HPP
#include <string_view>
struct gui_labels
{
  static constexpr const char      *window_title      = { "Field-Map-Editor - SFML" };

  static constexpr std::string_view palette           = { "Palette" };
  static constexpr std::string_view bpp               = { "BPP" };
  static constexpr std::string_view flatten           = { "Flatten" };
  static constexpr std::string_view all               = { "All" };
  static constexpr std::string_view compact           = { "Compact" };
  static constexpr std::string_view rows              = { "Rows" };
  static constexpr std::string_view width             = { "Width" };
  static constexpr std::string_view max_tiles         = { "Max Tiles" };
  static constexpr std::string_view filters           = { "Filters" };
  static constexpr std::string_view background        = { "Background" };
  static constexpr std::string_view control_panel     = { "Control Panel" };
  static constexpr std::string_view upscale_path      = { "Upscale Path" };
  static constexpr std::string_view deswizzle_path    = { "Deswizzle Path" };
  static constexpr std::string_view processing        = { "Processing" };
  static constexpr std::string_view animation_frame   = { "Animation Frame" };
  static constexpr std::string_view z                 = { "Z" };
  static constexpr std::string_view blend_other       = { "Blend Other" };
  static constexpr std::string_view blend_mode        = { "Blend Mode" };
  static constexpr std::string_view animation_id      = { "Animation ID" };
  static constexpr std::string_view texture_page      = { "Texture Page" };
  static constexpr std::string_view layers            = { "Layers" };
  static constexpr std::string_view pupu_id           = { "Pupu ID" };
  static constexpr std::string_view set               = { "Set" };
  static constexpr std::string_view disable           = { "Disable" };
  static constexpr std::string_view enable            = { "Enable" };
  static constexpr std::string_view language          = { "Language" };
  static constexpr std::string_view draw              = { "Draw" };
  static constexpr std::string_view draw_bit          = { "Draw-bit" };
  static constexpr std::string_view path              = { "Path" };
  static constexpr std::string_view field             = { "Field" };
  static constexpr std::string_view directory_created = { "Directory Created" };
  static constexpr std::string_view directory_exists  = { "Directory Exists" };
  static constexpr std::string_view saving_textures   = { "Saving Textures" };
  static constexpr std::string_view saving            = { "Saving" };
  static constexpr std::string_view file              = { "file" };
  static constexpr std::string_view start             = { "Start" };
  static constexpr std::string_view count_of_maps     = { "count of maps" };


  static constexpr std::string_view batch_deswizzle_title = {
    "Batch saving deswizzle textures..."
  };
  static constexpr std::string_view batch_deswizzle_finish = {
    "Finished the batch deswizzle..."
  };

  gui_labels() = delete;
};
#endif// FIELD_MAP_EDITOR_GUI_LABELS_HPP
