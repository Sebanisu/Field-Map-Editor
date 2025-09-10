#include "Image.hpp"
glengine::Image::Image(
  std::filesystem::path in_path,
  bool                  in_flip)
  : path(std::move(in_path))
  , png_data([&] {
       stbi_set_flip_vertically_on_load(in_flip ? 1 : 0);
       return stbi_load(path.string().c_str(), &width, &height, &channels, 4);
  }())
{
}