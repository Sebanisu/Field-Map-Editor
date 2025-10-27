#include "map_textures.hpp"

using BPPT = open_viii::graphics::BPPT;
namespace fme
{

map_textures::map_textures(std::weak_ptr<Selections> selections)
  : m_selections(selections)
{
}


void map_textures::queue_texture_loading(
  [[maybe_unused]] const ff_8::TextureFilters &filters)
{
}
std::future<void> map_textures::load_mim_textures(
  [[maybe_unused]] const BPPT    bpp,
  [[maybe_unused]] const uint8_t palette)
{
}
// ... other load/save methods
const glengine::Texture *map_textures::get_texture(
  [[maybe_unused]] const BPPT    bpp,
  [[maybe_unused]] const uint8_t palette,
  [[maybe_unused]] const uint8_t texture_page) const
{
}
void map_textures::consume_futures([[maybe_unused]] const bool update) {}
bool map_textures::all_futures_done() const
{
     return {};
}
uint32_t map_textures::get_max_texture_height() const
{
     return {};
}

// Helper functions like get_texture_pos()
}// namespace fme