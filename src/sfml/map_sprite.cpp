#include "map_sprite.hpp"
#include "append_inserter.hpp"
#include "format_imgui_text.hpp"
#include "future_operations.hpp"
#include "map_operation.hpp"
#include "safedir.hpp"
#include "save_image_pbo.hpp"
#include <bit>
#include <open_viii/graphics/Png.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <utility>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;

bool map_sprite::empty() const
{
     return m_map_group.maps.const_back().visit_tiles([](const auto &tiles) { return std::empty(tiles); });
}

map_sprite::colors_type
  map_sprite::get_colors(const open_viii::graphics::background::Mim &mim, open_viii::graphics::BPPT bpp, std::uint8_t palette)
{
     return mim.get_colors<open_viii::graphics::Color32RGBA>(bpp, palette, false);
}

std::size_t
  map_sprite::get_texture_pos(const open_viii::graphics::BPPT bpp, const std::uint8_t palette, const std::uint8_t texture_page) const
{
     if (!m_filters.upscale.enabled())
     {
          if (bpp.bpp4())
          {
               return palette;
          }
          if (bpp.bpp8())
          {
               return palette + std::size(Mim::palette_selections());
          }
          return BPP16_INDEX;// 16bpp doesn't have palettes.
     }
     const size_t j = static_cast<size_t>(texture_page) * MAX_PALETTES + palette;

     if (m_texture->at(j).getSize().y == 0)
     {
          return START_OF_NO_PALETTE_INDEX + texture_page;
     }
     return j;
}

const sf::Texture *
  map_sprite::get_texture(const open_viii::graphics::BPPT bpp, const std::uint8_t palette, const std::uint8_t texture_page) const
{
     const size_t index = get_texture_pos(bpp, palette, texture_page);
     const size_t size  = m_texture->size();
     if (index >= size)
     {
          spdlog::debug("Increase texture array size. it is too small! index {} >= size {}", index, size);
          spdlog::debug("bpp: {}, palette: {}, texture page: {}", static_cast<int>(bpp), palette, texture_page);
     }
     return &m_texture->at(index);
}
const sf::Texture *map_sprite::get_texture(const ::PupuID &pupu) const
{
     const auto &values = m_all_unique_values_and_strings.pupu().values();
     auto        it     = std::ranges::find(values, pupu);
     if (it != values.end())
     {
          auto i = static_cast<std::size_t>(std::distance(values.begin(), it));
          if (i >= MAX_TEXTURES)
          {
               spdlog::error("{}:{} - Index out of range {} / {}", __FILE__, __LINE__, i, MAX_TEXTURES);
               return nullptr;
          }
          return &m_texture->at(i);
     }
     return nullptr;
}

std::shared_ptr<std::array<sf::Texture, map_sprite::MAX_TEXTURES>> map_sprite::load_textures_internal()
{
     std::vector<std::future<std::future<void>>> future_of_futures{};
     auto        ret   = std::make_shared<std::array<sf::Texture, MAX_TEXTURES>>(std::array<sf::Texture, MAX_TEXTURES>{});
     const auto &range = m_all_unique_values_and_strings.bpp().values();
     if (!m_filters.deswizzle.enabled())
     {
          if (!std::empty(range))
          {
               for (const auto &bpp : range)
               {
                    if (bpp.bpp24())
                    {
                         continue;
                    }
                    const auto &map = m_all_unique_values_and_strings.palette();
                    if (map.contains(bpp))
                    {

                         for (const auto &palette : map.at(bpp).values())
                         {
                              if (!m_filters.upscale.enabled())
                              {
                                   future_of_futures.push_back(load_mim_textures(ret, bpp, palette));
                              }
                              else
                              {
                                   for (const auto &texture_page : m_all_unique_values_and_strings.texture_page_id().values())
                                   {
                                        future_of_futures.push_back(load_upscale_textures(ret, texture_page, palette));
                                   }
                              }
                         }
                    }
               }
          }
          if (m_filters.upscale.enabled())
          {
               for (const auto &texture_page : m_all_unique_values_and_strings.texture_page_id().values())
               {
                    future_of_futures.push_back(load_upscale_textures(ret, texture_page));
               }
          }
     }
     else
     {

          std::ranges::for_each(m_all_unique_values_and_strings.pupu().values(), [&, pos = size_t{}](const ::PupuID &pupu) mutable {
               future_of_futures.push_back(load_deswizzle_textures(ret, pupu, pos));
               ++pos;
          });
     }
     auto fofh = FutureOfFutureConsumer{ std::move(future_of_futures) };
     fofh.consume_now();
     return ret;
}
void map_sprite::consume_futures(std::vector<std::future<std::future<void>>> &future_of_futures)
{
     std::vector<std::future<void>> futures{};
     futures.reserve(std::ranges::size(future_of_futures));
     std::ranges::transform(
       future_of_futures, std::back_inserter(futures), [](std::future<std::future<void>> &future_of_future) -> std::future<void> {
            if (future_of_future.valid())
            {
                 future_of_future.wait();
                 return future_of_future.get();
            }
            return {};
       });
     consume_futures(futures);
}
void map_sprite::consume_futures(std::vector<std::future<void>> &futures)
{
     std::ranges::for_each(futures, [](std::future<void> &future) {
          if (future.valid())
          {
               future.get();
          }
     });
}
std::shared_ptr<std::array<sf::Texture, map_sprite::MAX_TEXTURES>> map_sprite::load_textures()
{
     std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> ret = load_textures_internal();
     while (std::ranges::all_of(*ret, [](const sf::Texture &texture) {
          auto size = texture.getSize();
          return size.x == 0 || size.y == 0;
     }))
     {
          if (m_filters.upscale.enabled())
          {
               m_filters.upscale.disable();
               ret = load_textures_internal();
          }
          else if (m_filters.deswizzle.enabled())
          {
               m_filters.deswizzle.disable();
               ret = load_textures_internal();
          }
          else
          {
               break;
          }
     }
     return ret;
}

std::future<std::future<void>> map_sprite::load_mim_textures(
  std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
  open_viii::graphics::BPPT                               bpp,
  std::uint8_t                                            palette)
{
     if (!m_map_group.mim)
     {
          return {};
     }
     if (m_map_group.mim->get_width(bpp) == 0U)
     {
          return {};
     }
     else
     {
          std::size_t const pos = get_texture_pos(bpp, palette, 0);
          return { std::async(
            std::launch::async,
            [bpp, palette](const ff_8::map_group::SharedMim &mim, sf::Texture *const texture) -> std::future<void> {
                 return { std::async(
                   std::launch::deferred,
                   future_operations::LoadColorsIntoTexture{
                     texture, get_colors(*mim, bpp, palette), sf::Vector2u{ mim->get_width(bpp), mim->get_height() } }) };
            },
            m_map_group.mim,
            &(ret->at(pos))) };
     }
}
std::future<std::future<void>> map_sprite::load_deswizzle_textures(
  std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
  const ::PupuID                                          pupu,
  const size_t                                            pos) const
{
     if (pos >= MAX_TEXTURES)
     {
          spdlog::error("{}:{} - Index out of range {} / {}", __FILE__, __LINE__, pos, MAX_TEXTURES);
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{ &(ret->at(pos)), generate_deswizzle_paths(pupu) }) };
}
std::future<std::future<void>> map_sprite::load_upscale_textures(SharedTextures &ret, std::uint8_t texture_page, std::uint8_t palette)
{
     const std::size_t pos = std::size_t{ texture_page } * MAX_PALETTES + palette;
     if (pos >= MAX_TEXTURES)
     {
          spdlog::error("{}:{} - Index out of range {} / {}", __FILE__, __LINE__, pos, MAX_TEXTURES);
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{
         &(ret->at(pos)), m_upscales.generate_upscale_paths(m_filters.upscale.value(), texture_page, palette) }) };
}

std::future<std::future<void>> map_sprite::load_upscale_textures(SharedTextures &ret, std::uint8_t texture_page)
{
     const std::size_t pos = START_OF_NO_PALETTE_INDEX + texture_page;
     if (pos >= MAX_TEXTURES)
     {
          spdlog::error("{}:{} - Index out of range {} / {}", __FILE__, __LINE__, pos, MAX_TEXTURES);
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{
         &(ret->at(pos)), m_upscales.generate_upscale_paths(m_filters.upscale.value(), texture_page) }) };
}

void set_color(std::array<sf::Vertex, 4U> &vertices, const sf::Color &color)
{
     std::ranges::transform(vertices, vertices.begin(), [&color](sf::Vertex vertex) {
          vertex.color = color;
          return vertex;
     });
}
enum struct texture_page_width : std::uint16_t
{
     bit_4  = 256U,
     bit_8  = bit_4 >> 1U,
     bit_16 = bit_4 >> 2U,
};
static constexpr std::underlying_type_t<texture_page_width> operator+(texture_page_width input) noexcept
{
     return static_cast<std::underlying_type_t<texture_page_width>>(input);
}
std::uint8_t map_sprite::max_x_for_saved() const
{
     return m_map_group.maps.const_back().visit_tiles([this](const auto &tiles) {
          auto       transform_range = m_saved_indices | std::views::transform([this, &tiles](std::size_t tile_index) -> std::uint16_t {
                                      auto &tile = tiles[tile_index];
                                      if (m_draw_swizzle)
                                      {
                                           if (tile.depth().bpp4())
                                           {
                                                return +texture_page_width::bit_4;
                                           }
                                           if (tile.depth().bpp16())
                                           {
                                                return +texture_page_width::bit_16;
                                           }
                                           return +texture_page_width::bit_8;
                                      }
                                      return TILE_SIZE;// todo invalid?
                                 });

          const auto found_min       = std::ranges::min_element(transform_range);
          if (found_min != std::ranges::end(transform_range))
          {
               return static_cast<std::uint8_t>(*found_min - TILE_SIZE);
          }
          return static_cast<std::uint8_t>((std::numeric_limits<std::uint8_t>::max)() - TILE_SIZE);
     });
}


void map_sprite::compact_rows()
{
     ff_8::compact_rows(m_map_group.maps.copy_back());
     update_render_texture();
}
void map_sprite::compact_all()
{
     ff_8::compact_all(m_map_group.maps.copy_back());
     update_render_texture();
}
void map_sprite::flatten_bpp()
{
     ff_8::flatten_bpp(m_map_group.maps.copy_back());
     update_render_texture();
}
void map_sprite::flatten_palette()
{
     ff_8::flatten_palette(m_map_group.maps.copy_back());
     update_render_texture();
}

void map_sprite::update_position(const sf::Vector2i &pixel_pos, const uint8_t &texture_page, const sf::Vector2i &down_pixel_pos)
{
     if (m_saved_indices.empty() && m_saved_imported_indices.empty())
     {
          return;
     }
     Map       &current_map = m_map_group.maps.copy_back();
     const auto update_tile_positions =
       [this, &texture_page, &pixel_pos, &down_pixel_pos](const auto &map, auto &&tiles, const std::vector<std::size_t> &indices) {
            for (auto i : indices)
            {
                 auto &tile = tiles[i];
                 if (m_draw_swizzle)
                 {
                      if (auto intersecting = find_intersecting(m_imported_tile_map, pixel_pos, texture_page, true); !intersecting.empty())
                      {
                           // this might not be good enough as two 4 bpp tiles fit in the
                           // same location as 8 bpp. and two 8 bpp fit in space for 16
                           // bpp but this should catch obvious problems.

                           // in the end it is safer to keep all 8bpp tiles aligned left
                           // and all 4bpp aligned right. For each Texture page.
                           // 16bpp are rare, but they should be left of 8bpp.
                           spdlog::info(
                             "There is at least {} tile(s) at this location. Choose an empty "
                             "location!",// at least because I am filtering by depth and
                                         // palette
                             intersecting.size());
                           return;
                      }
                      if (auto intersecting = find_intersecting(map, pixel_pos, texture_page, true); !intersecting.empty())
                      {
                           // this might not be good enough as two 4 bpp tiles fit in the
                           // same location as 8 bpp. and two 8 bpp fit in space for 16
                           // bpp but this should catch obvious problems.

                           // in the end it is safer to keep all 8bpp tiles aligned left
                           // and all 4bpp aligned right. For each Texture page.
                           // 16bpp are rare, but they should be left of 8bpp.
                           spdlog::info(
                             "There is at least {} tile(s) at this location. Choose an empty "
                             "location!",// at least because I am filtering by depth and
                                         // palette
                             intersecting.size());
                           return;
                      }

                      const std::int32_t texture_page_width = 256;
                      const std::int32_t x_offset = (down_pixel_pos.x % texture_page_width) - static_cast<std::int32_t>(tile.source_x());
                      const std::int32_t y_offset = down_pixel_pos.y - static_cast<std::int32_t>(tile.source_y());
                      tile                        = tile
                               .with_source_xy(
                                 static_cast<std::uint8_t>((((pixel_pos.x % texture_page_width) - x_offset))),
                                 static_cast<std::uint8_t>(((pixel_pos.y - y_offset))))
                               .with_texture_id(texture_page);
                 }
                 else
                 {
                      const std::int32_t x_offset = down_pixel_pos.x - tile.x();
                      const std::int32_t y_offset = down_pixel_pos.y - tile.y();
                      tile =
                        tile.with_xy(static_cast<std::int16_t>(pixel_pos.x - x_offset), static_cast<std::int16_t>(pixel_pos.y - y_offset));
                 }
            }
       };
     current_map.visit_tiles([&](auto &&tiles) { update_tile_positions(current_map, tiles, m_saved_indices); });
     if (!m_draw_swizzle)
     {
          m_imported_tile_map.visit_tiles(
            [this, &update_tile_positions](auto &&tiles) { update_tile_positions(m_imported_tile_map, tiles, m_saved_imported_indices); });
     }
     std::ignore = history_remove_duplicate();
     m_saved_indices.clear();
     m_saved_imported_indices.clear();
     update_render_texture();
}

sf::Sprite map_sprite::save_intersecting(const sf::Vector2i &pixel_pos, const std::uint8_t &texture_page)
{
     // Initialize an empty sprite object
     sf::Sprite sprite      = {};

     // Get the size of the sprite texture
     const auto sprite_size = m_drag_sprite_texture->getSize();
     spdlog::info("sprite_size: ({},{})", sprite_size.x, sprite_size.y);
     spdlog::info("m_scale: ({})", m_scale);

     // Set the texture rectangle of the sprite to cover the whole texture
     sprite.setTextureRect({ 0, 0, static_cast<int>(sprite_size.x), static_cast<int>(sprite_size.y) });

     // Static constant factor for offset when positioning the sprite
     static constexpr float one_and_half = 1.5F;

     // Set the sprite's position based on the provided pixel position, adjusted by TILE_SIZE
     sprite.setPosition(
       static_cast<float>(pixel_pos.x) - (TILE_SIZE * one_and_half), static_cast<float>(pixel_pos.y) - (TILE_SIZE * one_and_half));

     // Scale the sprite based on the m_scale factor
     sprite.setScale(1.0F / static_cast<float>(m_scale), 1.0F / static_cast<float>(m_scale));

     // Find intersecting tiles for the given position and texture page
     m_saved_indices = find_intersecting(m_map_group.maps.const_back(), pixel_pos, texture_page, false, true);

     spdlog::info("m_saved_indices count: {}", std::ranges::size(m_saved_indices));

     // If drawing is not swizzled, find intersecting imported tiles as well
     if (!m_draw_swizzle)
     {
          m_saved_imported_indices = find_intersecting(m_imported_tile_map, pixel_pos, texture_page, false, true);
     }

     // Clear the drag sprite texture with transparency
     m_drag_sprite_texture->clear(sf::Color::Transparent);

     // Lambda function to draw tiles based on the front tiles and tile data, and optionally imported tiles
     const auto draw_drag_texture =
       [this, &pixel_pos, &sprite_size](const auto &front_tiles, const auto &tiles, const std::uint16_t z, bool imported = false) {
            sf::RenderStates       states = {};

            // Set the transformation to adjust the sprite's position based on the scale and pixel position
            static constexpr float half   = 0.5F;
            states.transform.translate(sf::Vector2f(
              (static_cast<float>(-pixel_pos.x) * static_cast<float>(m_scale)) + (static_cast<float>(sprite_size.x) * half),
              (static_cast<float>(-pixel_pos.y) * static_cast<float>(m_scale)) + (static_cast<float>(sprite_size.x) * half)));

            // Loop through either saved imported indices or regular saved indices based on the flag
            for (const auto tile_index : imported ? m_saved_imported_indices : m_saved_indices)
            {
                 const auto &tile       = tiles[tile_index];
                 const auto &front_tile = front_tiles[tile_index];

                 // Skip drawing if the front tile's z-index does not match the current z layer
                 if (front_tile.z() != z)
                 {
                      continue;
                 }

                 // Set the texture for the tile, either imported or regular
                 states.texture =
                   imported ? m_imported_texture : get_texture(front_tile.depth(), front_tile.palette_id(), front_tile.texture_id());

                 // Skip if the texture is invalid (size is zero)
                 if (states.texture == nullptr || states.texture->getSize().y == 0 || states.texture->getSize().x == 0)
                 {
                      continue;
                 }

                 // Calculate draw size and texture size for the tile
                 const auto draw_size    = get_tile_draw_size();
                 const auto texture_size = imported ? get_tile_texture_size_for_import() : get_tile_texture_size(states.texture);

                 // Generate the quad for the tile using triangle strips
                 auto       quad         = imported ? get_triangle_strip_for_imported(draw_size, texture_size, front_tile, tile)
                                                    : get_triangle_strip(draw_size, texture_size, front_tile, tile);

                 // Set the blend mode for the sprite
                 states.blendMode        = sf::BlendAlpha;
                 if (!m_disable_blends)
                 {
                      states.blendMode = set_blend_mode(tile.blend_mode(), quad);
                 }

                 // Draw the tile to the drag sprite texture
                 m_drag_sprite_texture->draw(quad.data(), quad.size(), sf::TriangleStrip, states);
            }
       };

     // Iterate over all unique z values and draw the intersecting tiles at each z layer
     for (const std::uint16_t &z : m_all_unique_values_and_strings.z().values())
     {
          // Draw tiles for both the regular map and the imported map
          m_map_group.maps.front().visit_tiles([&](const auto &front_tiles) {
               m_map_group.maps.const_back().visit_tiles([&](const auto &tiles) { draw_drag_texture(front_tiles, tiles, z); });
          });
          m_imported_tile_map_front.visit_tiles([&](const auto &imported_front_tiles) {
               m_imported_tile_map.visit_tiles(
                 [&](const auto &imported_tiles) { draw_drag_texture(imported_front_tiles, imported_tiles, z, true); });
          });
     }

     // Display the drawn texture to make it visible
     m_drag_sprite_texture->display();

     // Set the sprite's texture to the one stored in the drag sprite texture
     sprite.setTexture(m_drag_sprite_texture->getTexture());

     // Update the render texture to reflect any changes
     update_render_texture();

     // Return the sprite
     return sprite;
}


[[nodiscard]] bool map_sprite::local_draw(sf::RenderTarget &target, sf::RenderStates states) const
{
     bool drew = false;
     target.clear(sf::Color::Transparent);
     for (const auto &z : m_all_unique_values_and_strings.z().values())
     {
          for_all_tiles(
            [this, &states, &target, &z, &drew]([[maybe_unused]] const auto &tile_const, const auto &tile, const PupuID pupu_id) {
                 if (!m_saved_indices.empty())
                 {
                      // skip saved indices on redraw.
                      const auto current_index = m_map_group.maps.get_offset_from_back(tile);
                      const auto find_index    = std::ranges::find_if(
                        m_saved_indices, [&current_index](const auto search_index) { return std::cmp_equal(search_index, current_index); });
                      if (find_index != m_saved_indices.end())
                      {
                           return;
                      }
                 }
                 if (m_filters.pupu.enabled())
                 {
                      if (m_filters.pupu.value() != pupu_id)
                      {
                           return;
                      }
                 }
                 if (tile.z() != z)
                 {
                      return;
                 }
                 if (ff_8::tile_operations::fail_any_filters(m_filters, tile))
                 {
                      return;
                 }
                 if (!filter_invalid(tile_const))
                 {
                      return;
                 }
                 if (!m_filters.deswizzle.enabled())
                 {
                      states.texture = get_texture(tile_const.depth(), tile_const.palette_id(), tile_const.texture_id());
                 }
                 else
                 {
                      states.texture = get_texture(pupu_id);
                 }
                 if (states.texture == nullptr || states.texture->getSize().y == 0 || states.texture->getSize().x == 0)
                 {
                      return;
                 }
                 const auto draw_size    = get_tile_draw_size();
                 const auto texture_size = get_tile_texture_size(states.texture);
                 auto       quad         = get_triangle_strip(draw_size, texture_size, tile_const, tile);
                 states.blendMode        = sf::BlendAlpha;
                 if (!m_disable_blends)
                 {
                      states.blendMode = set_blend_mode(tile.blend_mode(), quad);
                 }
                 // apply the tileset texture

                 // std::lock_guard<std::mutex> lock(mutex_texture);
                 // spdlog::info("({}, {})\t", raw_texture_size.x, raw_texture_size.y);
                 // draw the vertex array
                 target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
                 drew = true;
            });
     }
     return drew;
}
sf::BlendMode map_sprite::set_blend_mode(const BlendModeT &blend_mode, std::array<sf::Vertex, 4U> &quad)
{
     if (blend_mode == BlendModeT::add)
     {
          return sf::BlendAdd;
     }
     else if (blend_mode == BlendModeT::half_add)
     {
          constexpr static uint8_t per_50 = (std::numeric_limits<uint8_t>::max)() / 2U;
          set_color(quad, { per_50, per_50, per_50, per_50 });// 50% alpha
          return sf::BlendAdd;
     }
     else if (blend_mode == BlendModeT::quarter_add)
     {
          constexpr static uint8_t per_25 = (std::numeric_limits<uint8_t>::max)() / 4U;
          set_color(quad, { per_25, per_25, per_25, per_25 });// 25% alpha
          return sf::BlendAdd;
     }
     else if (blend_mode == BlendModeT::subtract)
     {
          return get_blend_subtract();
     }
     return sf::BlendAlpha;
}
[[nodiscard]] bool map_sprite::draw_imported([[maybe_unused]] sf::RenderTarget &target, [[maybe_unused]] sf::RenderStates states) const
{
     if (
       !m_using_imported_texture || m_imported_texture == nullptr || m_imported_texture->getSize().x == 0
       || m_imported_texture->getSize().y == 0)
     {
          return false;
     }
     states.texture                = m_imported_texture;
     bool       drew               = false;
     const auto draw_imported_tile = [this, &drew, &states, &target](
                                       const std::integral auto                             current_index,
                                       const open_viii::graphics::background::is_tile auto &tile_const,
                                       const open_viii::graphics::background::is_tile auto &tile) {
          if (!m_saved_imported_indices.empty())
          {
               const auto find_index = std::ranges::find_if(m_saved_imported_indices, [&current_index](const auto search_index) {
                    return std::cmp_equal(search_index, current_index);
               });
               if (find_index != m_saved_imported_indices.end())
               {
                    return;
               }
          }
          if (ff_8::tile_operations::fail_any_filters(m_filters, tile))
          {
               return;
          }
          if (!filter_invalid(tile_const))
          {
               return;
          }
          const auto draw_size    = get_tile_draw_size();
          const auto texture_size = get_tile_texture_size_for_import();
          auto       quad         = get_triangle_strip_for_imported(draw_size, texture_size, tile_const, tile);
          states.blendMode        = sf::BlendAlpha;
          if (!m_disable_blends)
          {
               states.blendMode = set_blend_mode(tile.blend_mode(), quad);
          }
          // apply the tileset texture
          target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
          drew = true;
     };
     for (const auto &z_axis : m_all_unique_values_and_strings.z().values())
     {
          m_imported_tile_map_front.visit_tiles([&](const auto &unchanged_tiles) {
               m_imported_tile_map.visit_tiles([&](const auto &changed_tiles) {
                    auto       unchanged_begin = unchanged_tiles.cbegin();
                    const auto unchanged_end   = unchanged_tiles.cend();
                    auto       changed_begin   = changed_tiles.cbegin();
                    const auto changed_end     = changed_tiles.cend();
                    for (; unchanged_begin != unchanged_end && changed_begin != changed_end; ++unchanged_begin, ++changed_begin)
                    {
                         const auto &unchanged_tile = *unchanged_begin;
                         const auto &changed_tile   = *changed_begin;
                         if (changed_tile.z() != z_axis)
                         {
                              return;
                         }
                         const auto current_index = std::ranges::distance(&changed_tiles.front(), &changed_tile);
                         draw_imported_tile(current_index, unchanged_tile, changed_tile);
                    }
               });
          });
     }
     return drew;
}
sf::Vector2u map_sprite::get_tile_draw_size() const
{
     return sf::Vector2u{ TILE_SIZE * m_scale, TILE_SIZE * m_scale };
}
sf::Vector2u map_sprite::get_tile_texture_size_for_import() const
{
     return sf::Vector2u{ m_imported_tile_size, m_imported_tile_size };
}
sf::Vector2u map_sprite::get_tile_texture_size(const sf::Texture *texture) const
{
     const auto raw_texture_size = texture->getSize();
     if (m_filters.deswizzle.enabled())
     {
          const auto local_scale = raw_texture_size.y / m_canvas.height();
          return sf::Vector2u{ TILE_SIZE * local_scale, TILE_SIZE * local_scale };
     }
     const auto i = raw_texture_size.y / TILE_SIZE;
     return sf::Vector2u{ i, i };
}
const sf::BlendMode &map_sprite::get_blend_subtract()
{
     const static auto blend_subtract =
       sf::BlendMode{ sf::BlendMode::DstColor,// or One
                      sf::BlendMode::One,      sf::BlendMode::ReverseSubtract, sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha,
                      sf::BlendMode::Add };
     return blend_subtract;
}

map_sprite map_sprite::with_coo(const open_viii::LangT coo) const
{
     return { ff_8::map_group{ m_map_group.field, coo }, m_draw_swizzle, m_filters, m_disable_blends, false };
}

map_sprite map_sprite::with_field(map_sprite::SharedField field, const open_viii::LangT coo) const
{
     return { ff_8::map_group{ std::move(field), coo }, m_draw_swizzle, m_filters, m_disable_blends, false };
}

map_sprite map_sprite::with_filters(ff_8::filters filters) const
{
     return { m_map_group, m_draw_swizzle, std::move(filters), m_disable_blends, false };
}

void map_sprite::enable_draw_swizzle()
{
     m_draw_swizzle = true;
     init_render_texture();
     m_grid = get_grid();
}

void map_sprite::disable_draw_swizzle()
{
     m_draw_swizzle = false;
     init_render_texture();
     m_grid = get_grid();
}

void map_sprite::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
     // apply the transform
     states.transform *= getTransform();
     states.texture    = &m_render_texture->getTexture();
     auto texture_size = m_render_texture->getSize();
     auto draw_size    = sf::Vector2u(width(), height());
     auto quad         = ff_8::get_triangle_strip(to_Vector2f(draw_size), to_Vector2f(texture_size), {}, {});
     // draw the vertex array
     target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
     // draw grid
     target.draw(m_grid, states);
     // draw square
     target.draw(m_square, states);
     // draw texture_page_grid
     if (m_draw_swizzle)
     {
          target.draw(m_texture_page_grid, states);
     }
}
void map_sprite::update_render_texture(bool reload_textures)
{
     if (reload_textures)
     {
          m_texture = load_textures();
          reset_render_texture();
     }
     if (!fail())
     {
          (void)local_draw(*m_render_texture, sf::RenderStates::Default);
          (void)draw_imported(*m_render_texture, sf::RenderStates::Default);
          m_render_texture->display();
          m_render_texture->setSmooth(false);
          m_render_texture->setRepeated(false);
          m_render_texture->generateMipmap();
     }
}
void map_sprite::save(const std::filesystem::path &path) const
{
     if (fail())
     {
          return;
     }
     std::future<sf::Image> task = save_image_pbo(m_render_texture->getTexture());
     task.wait();
     const sf::Image image = task.get();
     if (!image.saveToFile(path.string()))
     {
          spdlog::warn("Failed to save file: {}", path.string());
     }
}
bool map_sprite::fail() const
{
     using namespace open_viii::graphics::literals;
     if (!m_render_texture)
     {
          if (once)
          {
               spdlog::warn("{}", "m_render_texture is null");
               once = false;
          }
          return true;
     }
     if (!m_texture)
     {
          if (once)
          {
               spdlog::warn("{}", "m_texture is null");
               once = false;
          }
          return true;
     }
     if (!m_map_group.mim)
     {
          return false;
     }
     if (m_map_group.mim->get_width(4_bpp, false) == 0)
     {
          if (once)
          {
               spdlog::warn("{}", "m_mim width is 0");
               once = false;
          }
          return true;
     }
     if (empty())
     {
          if (once)
          {
               spdlog::warn("{}", "m_map is empty");
               once = false;
          }
          return true;
     }
     once = true;
     return false;
}
void map_sprite::map_save(const std::filesystem::path &dest_path) const
{
     ff_8::map_group::OptCoo coo  = m_map_group.opt_coo;
     const auto              map  = ff_8::load_map(m_map_group.field, coo, m_map_group.mim, nullptr, false);
     const auto              path = dest_path.string();

     open_viii::tools::write_buffer(
       [&map](std::ostream &os) {
            map.visit_tiles([&os](auto &&tiles) {
                 for (const auto &tile : tiles)
                 {
                      //          std::array<char, sizeof(tile)> data{};
                      //          std::memcpy(data.data(), &tile, sizeof(tile));
                      const auto data = std::bit_cast<std::array<char, sizeof(tile)>>(tile);
                      os.write(data.data(), data.size());
                 }
            });
       },
       path,
       "");

     test_map(dest_path);
}
std::string map_sprite::map_filename() const
{
     return std::filesystem::path(m_map_group.map_path).filename().string();
}

const map_sprite &map_sprite::toggle_grid(bool enable, bool enable_texture_page_grid) const
{
     if (enable)
     {
          m_grid.enable();
     }
     else
     {
          m_grid.disable();
     }

     if (enable_texture_page_grid)
     {
          // spdlog::info("enabled: {}",m_texture_page_grid.count());
          m_texture_page_grid.enable();
     }
     else
     {
          m_texture_page_grid.disable();
     }
     return *this;
}
void map_sprite::resize_render_texture()
{
     if (!fail())
     {
          auto       filtered_textures = *m_texture | std::views::filter([](const auto &texture) {
               const auto &size = texture.getSize();
               return size.x != 0 && size.y != 0;
          });
          const auto check_size        = [this]() {
               const auto max_size = sf::Texture::getMaximumSize();
               while (width() * m_scale > max_size || height() * m_scale > max_size)
               {
                    m_scale >>= 1U;
                    if (m_scale <= 1U)
                    {
                         m_scale = 1U;
                         break;
                    }
               }
          };
          if (filtered_textures.begin() != filtered_textures.end())
          {
               const auto max_texture = (std::ranges::max)(filtered_textures, {}, [](const auto &texture) { return texture.getSize().y; });
               const auto y           = max_texture.getSize().y;
               static constexpr unsigned int mim_texture_height = 256U;
               m_scale                                          = y / mim_texture_height;
               if (m_filters.deswizzle.enabled())
               {
                    m_scale = y / m_canvas.height();
               }
          }
          else
          {
               m_scale = 1U;
          }
          if (const std::uint16_t tmp_scale = m_imported_tile_size / map_sprite::TILE_SIZE;
              m_using_imported_texture && std::cmp_less(m_scale, tmp_scale))
          {
               m_scale = tmp_scale;
          }
          check_size();
          spdlog::debug("Render Texture- scale:{}, size:({}, {})", m_scale, width() * m_scale, height() * m_scale);
          m_render_texture->create(width() * m_scale, height() * m_scale);
          m_drag_sprite_texture->create(TILE_SIZE * m_scale * 3, TILE_SIZE * m_scale * 3);
     }
}
void map_sprite::init_render_texture()
{
     reset_render_texture();
     update_render_texture();
}
void map_sprite::reset_render_texture()
{
     if (!m_render_texture || !m_drag_sprite_texture)
     {
          m_render_texture      = std::make_shared<sf::RenderTexture>();
          m_drag_sprite_texture = std::make_shared<sf::RenderTexture>();
     }
     resize_render_texture();
}


open_viii::graphics::Rectangle<std::uint32_t> map_sprite::get_canvas() const
{
     return static_cast<open_viii::graphics::Rectangle<std::uint32_t>>(m_map_group.maps.const_back().canvas());
}

std::uint32_t map_sprite::width() const
{
     if (m_draw_swizzle)
     {
          if (m_map_group.mim)
          {
               using namespace open_viii::graphics::literals;
               return m_map_group.mim->get_width(4_bpp);
          }
     }
     return m_canvas.width();
}

std::uint32_t map_sprite::height() const
{
     if (m_draw_swizzle)
     {
          if (m_map_group.mim)
          {
               return m_map_group.mim->get_height();
          }
     }
     return m_canvas.height();
}
grid map_sprite::get_grid() const
{
     return { { TILE_SIZE, TILE_SIZE }, { width(), height() } };
}
grid map_sprite::get_texture_page_grid() const
{
     using namespace open_viii::graphics::literals;
     if (!m_map_group.mim)
     {
          return {};
     }
     const unsigned int texture_page_width = 256U;
     const unsigned int grid_spacing       = (1U << ((8U - (open_viii::graphics::BPPT::BPP4_CONST().raw() & 3U))));
     //(1U << (open_viii::graphics::BPPT::CLP_VALUE - ((4_bpp).raw() & open_viii::graphics::BPPT::RAW24_VALUE)))
     return { { grid_spacing, texture_page_width },
              { m_map_group.mim->get_width(4_bpp), m_map_group.mim->get_height() },
              sf::Color::Yellow };
}

all_unique_values_and_strings map_sprite::get_all_unique_values_and_strings() const
{
     return m_map_group.maps.const_back().visit_tiles([](const auto &tiles) { return all_unique_values_and_strings(tiles); });
}

const ff_8::filters &map_sprite::filter() const
{
     return m_filters;
}
ff_8::filters &map_sprite::filter()
{
     return m_filters;
}
map_sprite map_sprite::update(ff_8::map_group map_group, bool draw_swizzle) const
{
     return { std::move(map_group), draw_swizzle, m_filters, m_disable_blends, false };
}
const all_unique_values_and_strings &map_sprite::uniques() const
{
     return m_all_unique_values_and_strings;
}

// template<typename... T>
// requires(sizeof...(T) == 6U) bool map_sprite::draw_drop_downs()
//{
//   static constexpr std::array factor   = { "Zero",
//     "One",
//     "SrcColor",
//     "OneMinusSrcColor",
//     "DstColor",
//     "OneMinusDstColor",
//     "SrcAlpha",
//     "OneMinusSrcAlpha",
//     "DstAlpha",
//     "OneMinusDstAlpha" };
//
//   static constexpr std::array equation = {
//     "Add", "Subtract", "ReverseSubtract"//, "Min", "Max"
//   };
//
//   static constexpr std::array names = {
//     "colorSourceFactor",
//     "colorDestinationFactor",
//     "colorBlendEquation",
//     "alphaSourceFactor",
//     "alphaDestinationFactor",
//     "alphaBlendEquation",
//   };
//
//   static std::array<int, std::ranges::size(names)> values = {};
//   auto name  = std::ranges::begin(names);
//   auto value = std::ranges::begin(values);
//   static_assert(sizeof...(T) == std::ranges::size(names));
//   const auto result = std::ranges::any_of(
//     std::array{ ([](const char *const local_name, int &local_value) -> bool {
//       using Real_Factor   = sf::BlendMode::Factor;
//       using Real_Equation = sf::BlendMode::Equation;
//       if constexpr (std::is_same_v<T, Real_Factor>) {
//         return ImGui::Combo(local_name,
//           &local_value,
//           std::ranges::data(factor),
//           static_cast<int>(std::ranges::ssize(factor)));
//       } else if constexpr (std::is_same_v<T, Real_Equation>) {
//         return ImGui::Combo(local_name,
//           &local_value,
//           std::ranges::data(equation),
//           static_cast<int>(std::ranges::ssize(equation)));
//       } else {
//         return false;
//       }
//     }(*(name++), *(value++)))... },
//     std::identity());
//   if (result) {
//     value              = std::ranges::begin(values);
//     GetBlendSubtract() = sf::BlendMode{ (static_cast<T>(*(value++)))... };
//   }
//   return result;
// }
std::vector<std::future<std::future<void>>> map_sprite::save_swizzle_textures(const std::filesystem::path &path)
{
     // assert(std::filesystem::path.is_directory(path));
     const std::string                 field_name                       = { get_base_name() };
     static constexpr std::string_view pattern_texture_page             = { "{}_{}.png" };
     static constexpr std::string_view pattern_texture_page_palette     = { "{}_{}_{}.png" };
     static constexpr std::string_view pattern_coo_texture_page         = { "{}_{}_{}.png" };
     static constexpr std::string_view pattern_coo_texture_page_palette = { "{}_{}_{}_{}.png" };

     const auto                        unique_values                    = get_all_unique_values_and_strings();
     const auto                       &unique_texture_page_ids          = unique_values.texture_page_id().values();
     const auto                       &unique_bpp                       = unique_values.bpp().values();
     settings_backup                   settings(m_filters, m_draw_swizzle, m_disable_texture_page_shift, m_disable_blends, m_scale);
     settings.filters                         = ff_8::filters{};
     settings.filters.value().upscale         = settings.filters.backup().upscale;
     settings.filters.value().deswizzle       = settings.filters.backup().deswizzle;
     settings.draw_swizzle                    = true;
     settings.disable_texture_page_shift      = true;
     settings.disable_blends                  = true;
     uint32_t                      height     = get_max_texture_height();
     constexpr static unsigned int mim_height = { 256U };
     settings.scale                           = height / mim_height;
     if (settings.filters.value().deswizzle.enabled())
     {
          settings.scale = height / m_canvas.height();
          height         = settings.scale.value() * mim_height;
     }
     if (settings.scale == 0U)
     {
          settings.scale = 1U;
     }

     if (unique_bpp.size() == 1U && unique_values.palette().at(unique_bpp.front()).values().size() <= 1U)
     {
          return {};
     }
     using map_type                                                          = std::remove_cvref_t<decltype(get_conflicting_palettes())>;
     using mapped_type                                                       = typename map_type::mapped_type;
     const map_type                              conflicting_palettes_map    = get_conflicting_palettes();
     std::vector<std::future<std::future<void>>> future_of_futures           = {};
     const unsigned int                          max_number_of_texture_pages = 13U;
     future_of_futures.reserve(max_number_of_texture_pages);
     sf::RenderTexture out_texture{};
     out_texture.create(height, height);
     for (const auto &texture_page : unique_texture_page_ids)
     {
          settings.filters.value().texture_page_id.update(texture_page).enable();
          const bool contains_conflicts = conflicting_palettes_map.contains(texture_page);
          if (contains_conflicts)
          {
               const mapped_type &conflicting_palettes = conflicting_palettes_map.at(texture_page);
               for (const auto &bpp : unique_bpp)
               {
                    const auto &unique_palette = unique_values.palette().at(bpp).values();
                    auto        filter_palette = unique_palette | std::views::filter([&conflicting_palettes](const std::uint8_t &palette) {
                                               return std::ranges::any_of(
                                                 conflicting_palettes, [&palette](const std::uint8_t &other) { return palette == other; });
                                          });
                    for (const auto &palette : filter_palette)
                    {
                         settings.filters.value().palette.update(palette).enable();
                         settings.filters.value().bpp.update(bpp).enable();
                         if (!generate_texture(&out_texture))
                         {
                              continue;
                         }
                         auto out_path = [&]() -> std::filesystem::path {
                              if (m_map_group.opt_coo)
                              {
                                   return save_path_coo(
                                     pattern_coo_texture_page_palette, path, field_name, texture_page, palette, *m_map_group.opt_coo);
                              }
                              return save_path(pattern_texture_page_palette, path, field_name, texture_page, palette);
                         }();
                         future_of_futures.push_back(async_save(out_texture.getTexture(), out_path));
                    }
               }
          }

          settings.filters.value().palette.disable();
          settings.filters.value().bpp.disable();
          if (!generate_texture(&out_texture))
          {
               continue;
          }
          auto out_path = m_map_group.opt_coo
                            ? save_path_coo(pattern_coo_texture_page, path, field_name, texture_page, *m_map_group.opt_coo)
                            : save_path(pattern_texture_page, path, field_name, texture_page);
          future_of_futures.push_back(async_save(out_texture.getTexture(), out_path));
     }
     return future_of_futures;
     // consume_futures(future_of_futures);
}

std::string map_sprite::get_base_name() const
{
     if (m_map_group.field)
     {
          return str_to_lower(m_map_group.field->get_base_name());
     }
     return {};
}

std::vector<std::future<std::future<void>>> map_sprite::save_pupu_textures(const std::filesystem::path &path)
{
     auto settings    = settings_backup{ m_filters, m_draw_swizzle, m_disable_texture_page_shift, m_disable_blends, m_scale };
     settings.filters = ff_8::filters{};
     settings.filters.value().upscale         = settings.filters.backup().upscale;
     settings.draw_swizzle                    = false;
     settings.disable_texture_page_shift      = true;
     settings.disable_blends                  = true;
     // todo maybe draw with blends enabled to transparent black or white.
     static constexpr unsigned int mim_height = { 256U };
     settings.scale                           = get_max_texture_height() / mim_height;
     if (settings.scale == 0U)
     {
          settings.scale = 1U;
     }

     if (!m_map_group.field)
     {
          return {};
     }

     const std::string                field_name      = std::string{ str_to_lower(m_map_group.field->get_base_name()) };
     const std::vector<PupuID>       &unique_pupu_ids = m_all_unique_values_and_strings.pupu().values();
     std::optional<open_viii::LangT> &coo             = m_map_group.opt_coo;

     assert(safedir(path).is_dir());
     static constexpr std::string_view           pattern_pupu                = { "{}_{}.png" };
     static constexpr std::string_view           pattern_coo_pupu            = { "{}_{}_{}.png" };
     const unsigned int                          max_number_of_texture_pages = 13U;
     std::vector<std::future<std::future<void>>> future_of_futures           = {};
     future_of_futures.reserve(max_number_of_texture_pages);

     sf::RenderTexture out_texture{};
     iRectangle const  canvas = m_map_group.maps.const_back().canvas() * static_cast<int>(m_scale);
     out_texture.create(static_cast<std::uint32_t>(canvas.width()), static_cast<std::uint32_t>(canvas.height()));
     for (const PupuID &pupu : unique_pupu_ids)
     {
          settings.filters.value().pupu.update(pupu).enable();
          if (!generate_texture(&out_texture))
          {
               continue;
          }
          std::filesystem::path const out_path =
            coo ? save_path_coo(pattern_coo_pupu, path, field_name, pupu, *coo) : save_path(pattern_pupu, path, field_name, pupu);
          future_of_futures.push_back(async_save(out_texture.getTexture(), out_path));
     }
     return future_of_futures;
     // consume_futures(future_of_futures);
}

[[nodiscard]] std::future<std::future<void>> map_sprite::async_save(const sf::Texture &out_texture, const std::filesystem::path &out_path)
{
     return { std::async(
       std::launch::deferred,
       [out_path](std::future<sf::Image> image_task) -> std::future<void> {
            return std::async(std::launch::async, future_operations::save_image_to_path{ out_path, image_task.get() });
       },
       save_image_pbo(out_texture)) };
}
uint32_t map_sprite::get_max_texture_height() const
{
     auto     transform_range = (*m_texture) | std::views::transform([](const sf::Texture &texture) { return texture.getSize().y; });
     auto     max_height_it   = std::ranges::max_element(transform_range);
     uint32_t tex_height      = {};
     if (max_height_it != std::ranges::end(transform_range))
     {
          tex_height = *max_height_it;
     }
     else
     {
          static constexpr std::uint16_t mim_texture_height = 256U;
          tex_height                                        = mim_texture_height;
     }
     if (const auto tex_height_scale = static_cast<std::uint16_t>(static_cast<std::uint16_t>(m_imported_tile_size >> 4U) << 8U);
         m_using_imported_texture && std::cmp_greater(tex_height_scale, tex_height))
     {
          tex_height = tex_height_scale;
     }

     return tex_height;
}
std::filesystem::path map_sprite::save_path_coo(
  fmt::format_string<std::string_view, std::string_view, uint8_t> pattern,
  const std::filesystem::path                                    &path,
  const std::string_view                                         &field_name,
  const uint8_t                                                   texture_page,
  const open_viii::LangT                                          coo)
{
     return path
            / fmt::vformat(
              fmt::string_view(pattern), fmt::make_format_args(field_name, open_viii::LangCommon::to_string(coo), texture_page));
}
std::filesystem::path map_sprite::save_path_coo(
  fmt::format_string<std::string_view, std::string_view, uint8_t, uint8_t> pattern,
  const std::filesystem::path                                             &path,
  const std::string_view                                                  &field_name,
  const uint8_t                                                            texture_page,
  const uint8_t                                                            palette,
  const open_viii::LangT                                                   coo)
{
     return path
            / fmt::vformat(
              fmt::string_view(pattern), fmt::make_format_args(field_name, open_viii::LangCommon::to_string(coo), texture_page, palette));
}
std::filesystem::path map_sprite::save_path_coo(
  fmt::format_string<std::string_view, std::string_view, PupuID> pattern,
  const std::filesystem::path                                   &path,
  const std::string_view                                        &field_name,
  const PupuID                                                   pupu,
  const open_viii::LangT                                         coo)
{
     return path / fmt::vformat(fmt::string_view(pattern), fmt::make_format_args(field_name, open_viii::LangCommon::to_string(coo), pupu));
}
std::filesystem::path map_sprite::save_path(
  fmt::format_string<std::string_view, std::uint8_t> pattern,
  const std::filesystem::path                       &path,
  const std::string_view                            &field_name,
  const std::uint8_t                                 texture_page)
{
     return path / fmt::vformat(fmt::string_view(pattern), fmt::make_format_args(field_name, texture_page));
}
std::filesystem::path map_sprite::save_path(
  fmt::format_string<std::string_view, std::uint8_t, std::uint8_t> pattern,
  const std::filesystem::path                                     &path,
  const std::string_view                                          &field_name,
  std::uint8_t                                                     texture_page,
  std::uint8_t                                                     palette)
{
     return path / fmt::vformat(fmt::string_view(pattern), fmt::make_format_args(field_name, texture_page, palette));
}
std::filesystem::path map_sprite::save_path(
  fmt::format_string<std::string_view, PupuID> pattern,
  const std::filesystem::path                 &path,
  const std::string_view                      &field_name,
  PupuID                                       pupu)
{
     return path / fmt::vformat(fmt::string_view(pattern), fmt::make_format_args(field_name, pupu));
}

bool map_sprite::generate_texture(sf::RenderTexture *texture) const
{
     if (texture == nullptr)
     {
          return false;
     }
     if (local_draw(*texture, sf::RenderStates::Default))
     {
          (void)draw_imported(*texture, sf::RenderStates::Default);
          texture->display();
          texture->setSmooth(false);
          texture->setRepeated(false);
          texture->generateMipmap();
          return true;
     }
     return false;
}
void map_sprite::load_map(const std::filesystem::path &src_path)
{
     const auto path = src_path.string();
     open_viii::tools::read_from_file(
       [this](std::istream &os) {
            (void)m_map_group.maps.copy_back();
            m_map_group.maps.front().visit_tiles([this, &os](const auto &const_tiles) {
                 using tile_t            = std::remove_cvref_t<decltype(const_tiles.front())>;
                 m_map_group.maps.back() = open_viii::graphics::background::Map(
                   [&os]() -> std::variant<
                             open_viii::graphics::background::Tile1,
                             open_viii::graphics::background::Tile2,
                             open_viii::graphics::background::Tile3,
                             std::monostate> {
                        tile_t     tile{};
                        const auto append = [&os](auto &t) -> bool {
                             // load tile
                             std::array<char, sizeof(t)> data = {};
                             if (!os.read(data.data(), data.size()))
                             {
                                  return false;
                             }
                             t = std::bit_cast<std::remove_cvref_t<decltype(t)>>(data);

                             return true;
                        };

                        if (append(tile))
                        {
                             // write from tiles.
                             return tile;
                        }
                        return std::monostate{};
                   });
            });
            //   shift to origin
            m_map_group.maps.back().shift(m_map_group.maps.front().offset().abs());
            (void)m_map_group.maps.copy_back_to_front();
       },
       path);
     update_render_texture();
}
void map_sprite::test_map(const std::filesystem::path &saved_path) const
{
     if (!m_map_group.mim)
     {
          return;
     }
     const MimType &type = m_map_group.mim->mim_type();
     std::ignore         = ff_8::test_if_map_same(saved_path, m_map_group.field, type);
}
void map_sprite::save_modified_map(const std::filesystem::path &dest_path) const
{
     if (m_using_imported_texture)
     {
          ff_8::save_modified_map(dest_path, m_map_group.maps.front(), m_map_group.maps.const_back(), &m_imported_tile_map);
     }
     else
     {
          ff_8::save_modified_map(dest_path, m_map_group.maps.front(), m_map_group.maps.const_back());
     }
     test_map(dest_path);
}
void map_sprite::update_render_texture(const sf::Texture *p_texture, open_viii::graphics::background::Map map, const tile_sizes tile_size)
{
     m_imported_texture        = p_texture;
     m_imported_tile_map       = std::move(map);
     m_imported_tile_map_front = m_imported_tile_map;
     m_imported_tile_size      = static_cast<uint16_t>(tile_size);
     m_using_imported_texture  = p_texture != nullptr;
     update_render_texture(true);
}
void map_sprite::enable_square(sf::Vector2u position)
{
     m_square = m_square.with_position(position);
     m_square.enable();
}
void map_sprite::disable_square() const
{
     m_square.disable();
}
void map_sprite::enable_disable_blends()
{
     m_disable_blends = true;
     init_render_texture();
}
void map_sprite::disable_disable_blends()
{
     m_disable_blends = false;
     init_render_texture();
}
void map_sprite::compact_map_order()
{
     ff_8::compact_map_order(m_map_group.maps.copy_back());
     update_render_texture();
}
std::string map_sprite::str_to_lower(std::string input)
{
     std::string output{};
     output.reserve(std::size(input) + 1);
     std::ranges::transform(
       input, std::back_inserter(output), [](char character) -> char { return static_cast<char>(::tolower(character)); });
     return output;
}
map_sprite::map_sprite(ff_8::map_group map_group, bool draw_swizzle, ff_8::filters in_filters, bool force_disable_blends, bool require_coo)
  : m_map_group(
      !require_coo || (map_group.opt_coo && map_group.opt_coo.value() != open_viii::LangT::generic) ? std::move(map_group)
                                                                                                    : ff_8::map_group{})
  , m_draw_swizzle(draw_swizzle)
  , m_disable_blends(force_disable_blends)
  , m_filters(std::move(in_filters))
  , m_upscales(get_upscales())
  , m_all_unique_values_and_strings(get_all_unique_values_and_strings())
  , m_canvas(get_canvas())
  , m_texture(load_textures())
  , m_render_texture(std::make_shared<sf::RenderTexture>())
  , m_grid(get_grid())
  , m_texture_page_grid(get_texture_page_grid())
{
     init_render_texture();
}
void map_sprite::undo()
{
     (void)m_map_group.maps.undo();
     update_render_texture();
}
void map_sprite::redo()
{
     (void)m_map_group.maps.redo();
     update_render_texture();
}
void map_sprite::undo_all()
{
     m_map_group.maps.undo_all();
     update_render_texture();
}
void map_sprite::redo_all()
{
     m_map_group.maps.redo_all();
     update_render_texture();
}
bool map_sprite::undo_enabled() const
{
     return m_map_group.maps.undo_enabled();
}
bool map_sprite::redo_enabled() const
{
     return m_map_group.maps.redo_enabled();
}
bool map_sprite::history_remove_duplicate()
{
     return m_map_group.maps.remove_duplicate();
}
std::uint32_t map_sprite::get_map_scale() const
{
     return m_scale;
}
std::vector<std::size_t> map_sprite::find_intersecting(
  const Map          &map,
  const sf::Vector2i &pixel_pos,
  const uint8_t      &texture_page,
  bool                skip_filters,
  bool                find_all) const
{
     if (m_draw_swizzle)
     {
          return ff_8::find_intersecting_swizzle(map, m_filters, pixel_pos, texture_page, skip_filters, find_all);
     }
     return ff_8::find_intersecting_deswizzle(map, m_filters, pixel_pos, skip_filters, find_all);
}
bool map_sprite::using_coo() const
{
     return m_map_group.opt_coo.operator bool();
}
