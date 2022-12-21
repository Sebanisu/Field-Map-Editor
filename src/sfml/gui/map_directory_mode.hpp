//
// Created by pcvii on 12/21/2022.
//

#ifndef FIELD_MAP_EDITOR_MAP_DIRECTORY_MODE_HPP
#define FIELD_MAP_EDITOR_MAP_DIRECTORY_MODE_HPP
enum struct map_directory_mode
{
  ff8_install_directory,
  save_swizzle_textures,
  save_deswizzle_textures,
  load_swizzle_textures,
  load_deswizzle_textures,
  batch_save_deswizzle_textures,
  batch_load_deswizzle_textures,
  batch_save_swizzle_textures,
  batch_embed_map_files,
  custom_upscale_directory,
};
#endif// FIELD_MAP_EDITOR_MAP_DIRECTORY_MODE_HPP
