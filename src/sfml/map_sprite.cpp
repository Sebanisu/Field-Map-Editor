
#include "map_sprite.hpp"
#include "append_inserter.hpp"
#include "format_imgui_text.hpp"
#include "safedir.hpp"
#include "save_image_pbo.hpp"
#include <bit>
#include <open_viii/graphics/Png.hpp>
#include <spdlog/spdlog.h>
#include <utility>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;

bool map_sprite::empty() const
{
     return m_maps.const_back().visit_tiles([](const auto &tiles) { return std::empty(tiles); });
}

Mim map_sprite::get_mim() const
{
     if (m_field)
     {
          auto lang_name      = fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Mim::EXT);
          auto long_lang_name = fmt::format(
            "{}_{}{}", m_field->get_base_name(), open_viii::LangCommon::to_string(m_coo), open_viii::graphics::background::Mim::EXT);
          auto long_name = fmt::format("{}{}", m_field->get_base_name(), open_viii::graphics::background::Mim::EXT);
          return { m_field->get_entry_data(
                     { std::string_view(long_lang_name), std::string_view(long_name), std::string_view(lang_name), Mim::EXT }),
                   str_to_lower(m_field->get_base_name()) };
     }
     return {};
}

Map map_sprite::get_map(std::string *out_path, bool shift, bool &coo) const
{
     if (m_field)
     {
          std::size_t out_path_pos   = {};
          auto        lang_name      = fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Map::EXT);
          auto        long_lang_name = fmt::format(
            "{}_{}{}", m_field->get_base_name(), open_viii::LangCommon::to_string(m_coo), open_viii::graphics::background::Map::EXT);
          auto long_name = fmt::format("{}{}", m_field->get_base_name(), open_viii::graphics::background::Map::EXT);
          auto map       = Map{ m_mim.mim_type(),
                          m_field->get_entry_data(
                            { std::string_view(long_lang_name), std::string_view(long_name), std::string_view(lang_name), Map::EXT },
                            out_path,
                            &out_path_pos),
                          shift };
          coo            = out_path_pos == 0U;
          return map;
     }
     return {};
}

map_sprite::colors_type map_sprite::get_colors(open_viii::graphics::BPPT bpp, std::uint8_t palette) const
{
     return m_mim.get_colors<open_viii::graphics::Color32RGBA>(bpp, palette, false);
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
     auto        ret   = std::make_shared<std::array<sf::Texture, MAX_TEXTURES>>(std::array<sf::Texture, MAX_TEXTURES>{});
     const auto &range = m_all_unique_values_and_strings.bpp().values();
     if (!m_filters.deswizzle.enabled())
     {
          if (!std::empty(range))
          {
               for (const auto &bpp : range)
               {
                    const auto &map = m_all_unique_values_and_strings.palette();
                    if (map.contains(bpp))
                    {

                         for (const auto &palette : map.at(bpp).values())
                         {
                              if (bpp.bpp24())
                              {
                                   continue;
                              }
                              if (!m_filters.upscale.enabled())
                              {
                                   load_mim_textures(ret, bpp, palette);
                              }
                              else
                              {
                                   find_upscale_path(ret, palette);
                              }
                         }
                    }
               }
          }
          if (m_filters.upscale.enabled())
          {
               find_upscale_path(ret);
          }
     }
     else
     {
          find_deswizzle_path(ret);
     }
     wait_for_futures();
     return ret;
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
void map_sprite::load_mim_textures(
  std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
  open_viii::graphics::BPPT                               bpp,
  std::uint8_t                                            palette)
{
     if (m_mim.get_width(bpp) != 0U)
     {
          std::size_t const pos  = get_texture_pos(bpp, palette, 0);
          const auto        task = [this](sf::Texture *texture, BPPT bppt, uint8_t pal) {
               const auto colors = get_colors(bppt, pal);
               // std::lock_guard<std::mutex> lock(mutex_texture);
               texture->create(m_mim.get_width(bppt), m_mim.get_height());
               texture->update(colors.front().data());
               texture->setSmooth(false);
               texture->setRepeated(false);
               texture->generateMipmap();
          };
          spawn_thread(task, &(ret->at(pos)), bpp, palette);
     }
}
void map_sprite::find_upscale_path(std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret)
{
     for (const auto &texture_page : m_all_unique_values_and_strings.texture_page_id().values())
     {
          const size_t texture_index                 = START_OF_NO_PALETTE_INDEX + texture_page;
          const auto   texture_file_exists_then_load = [this, texture_page](sf::Texture *texture) -> void {
               const auto &root           = m_filters.upscale.value();
               const auto  paths          = m_upscales.get_file_paths(root, texture_page);
               auto        filtered_paths = paths | std::views::filter([](safedir path) { return path.is_exists() && !path.is_dir(); });
               if (filtered_paths.begin() != filtered_paths.end())
               {
                    const auto &path = *(filtered_paths.begin());
                    spdlog::info("upscale path: {}", path.string());
                    texture->loadFromFile(path.string());
                    texture->setSmooth(false);
                    texture->setRepeated(false);
                    texture->generateMipmap();
               }
          };
          if (texture_index >= MAX_TEXTURES)
          {
               spdlog::error("{}:{} - Index out of range {} / {}", __FILE__, __LINE__, texture_index, MAX_TEXTURES);
               return;
          }
          spawn_thread(texture_file_exists_then_load, &(ret->at(texture_index)));
     }
}

void map_sprite::find_deswizzle_path(std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret)
{
     auto field_name = get_base_name();
     std::ranges::for_each(
       m_all_unique_values_and_strings.pupu().values(), [&ret, &field_name, this, i = size_t{}](const ::PupuID &pupu) mutable {
            static constexpr auto pattern_pupu     = std::string_view("{}_{}.png");
            static constexpr auto pattern_coo_pupu = std::string_view("{}_{}_{}.png");
            std::filesystem::path in_path{};
            if (m_using_coo)
            {
                 in_path = save_path_coo(pattern_coo_pupu, m_filters.deswizzle.value(), field_name, pupu);
            }
            else
            {
                 in_path = save_path(pattern_pupu, m_filters.deswizzle.value(), field_name, pupu);
            }
            const auto check_if_file_exists_and_load_it = [in_path](sf::Texture *texture) -> void {
                 if (safedir(in_path).is_exists())
                 {
                      spdlog::info("texture path: \"{}\"", in_path.string());
                      texture->loadFromFile(in_path.string());
                      texture->setSmooth(false);
                      texture->setRepeated(false);
                      texture->generateMipmap();
                 }
            };
            if (i >= MAX_TEXTURES)
            {
                 spdlog::error("{}:{} - Index out of range {} / {}", __FILE__, __LINE__, i, MAX_TEXTURES);
                 return;
            }
            spawn_thread(check_if_file_exists_and_load_it, &(ret->at(i++)));
       });
     /*for (const auto& texture_page :
         m_all_unique_values_and_strings.texture_page_id().values())
     {
         const size_t i = MAX_TEXTURES - MAX_TEXTURE_PAGES + texture_page;
         const auto   fn = [this, texture_page](sf::Texture* texture) -> void
         {
             const auto& root = m_filters.upscale.value();
             const auto  paths = m_upscales.get_file_paths(root, texture_page);
             auto        filtered_paths = paths
                 | std::views::filter(
                     [](const std::filesystem::path& path)
                     {
                         return std::filesystem::exists(path)
                             && !std::filesystem::is_directory(path);
                     });
             if (filtered_paths.begin() != filtered_paths.end())
             {
                 const auto& path = *(filtered_paths.begin());
                 spdlog::info("{}", path.string());
                 texture->loadFromFile(path.string());
                 texture->setSmooth(false);
                 texture->setRepeated(false);
                 texture->generateMipmap();
             }
         };
         spawn_thread(fn, &(ret->at(i)));
     }*/
}
void map_sprite::find_upscale_path(std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret, std::uint8_t palette)
{
     for (const auto &texture_page : m_all_unique_values_and_strings.texture_page_id().values())
     {
          const size_t i  = size_t{ texture_page } * MAX_PALETTES + palette;
          auto const   fn = [texture_page, palette, this](sf::Texture *const texture) {
               const auto &root           = m_filters.upscale.value();
               const auto  paths          = m_upscales.get_file_paths(root, texture_page, palette);
               auto        filtered_paths = paths | std::views::filter([](safedir path) { return path.is_exists() && !path.is_dir(); });

               if (filtered_paths.begin() != filtered_paths.end())
               {
                    const auto &path = *(filtered_paths.begin());
                    spdlog::info("texture path: \"{}\"", path.string());
                    texture->loadFromFile(path.string());
                    texture->setSmooth(false);
                    texture->setRepeated(false);
                    texture->generateMipmap();
               }
          };
          spawn_thread(fn, &(ret->at(i)));
     }
}
void map_sprite::wait_for_futures()
{
     std::ranges::for_each(m_futures, [](auto &future) { future.wait(); });
     m_futures.clear();
}

[[nodiscard]] std::array<sf::Vertex, 4U> map_sprite::get_triangle_strip(
  const sf::Vector2u &draw_size,
  const sf::Vector2u &texture_size,
  std::integral auto  source_x,
  std::integral auto  source_y,
  std::integral auto  dest_x,
  std::integral auto  dest_y) const
{
     const sf::Vector2f     draw_size_f     = { static_cast<float>(draw_size.x), static_cast<float>(draw_size.y) };
     const sf::Vector2f     texture_size_f  = { static_cast<float>(texture_size.x), static_cast<float>(texture_size.y) };
     constexpr static float tile_size_f     = static_cast<float>(TILE_SIZE);
     auto                   scaled_dest_x   = static_cast<float>(dest_x) / tile_size_f;
     auto                   scaled_dest_y   = static_cast<float>(dest_y) / tile_size_f;
     float                  scaled_source_x = static_cast<float>(source_x) / tile_size_f;
     float                  scaled_source_y = static_cast<float>(source_y) / tile_size_f;
     const auto tovec  = [](auto &&in_x, auto &&in_y) { return sf::Vector2f{ static_cast<float>(in_x), static_cast<float>(in_y) }; };
     const auto tovert = [&tovec](auto &&draw_x, auto &&draw_y, auto &&texture_x, auto &&texture_y) {
          return sf::Vertex{ tovec(draw_x, draw_y), tovec(texture_x, texture_y) };
     };
     return std::array{ tovert(
                          (scaled_dest_x + 1) * draw_size_f.x,
                          scaled_dest_y * draw_size_f.y,
                          (scaled_source_x + 1) * texture_size_f.x,
                          scaled_source_y * texture_size_f.y),
                        tovert(
                          scaled_dest_x * draw_size_f.x,
                          scaled_dest_y * draw_size_f.y,
                          scaled_source_x * texture_size_f.x,
                          scaled_source_y * texture_size_f.y),
                        tovert(
                          (scaled_dest_x + 1) * draw_size_f.x,
                          (scaled_dest_y + 1) * draw_size_f.y,
                          (scaled_source_x + 1) * texture_size_f.x,
                          (scaled_source_y + 1) * texture_size_f.y),
                        tovert(
                          scaled_dest_x * draw_size_f.x,
                          (scaled_dest_y + 1) * draw_size_f.y,
                          scaled_source_x * texture_size_f.x,
                          (scaled_source_y + 1) * texture_size_f.y) };
}
[[nodiscard]] std::array<sf::Vertex, 4U> map_sprite::get_triangle_strip_for_imported(
  const sf::Vector2u                                  &draw_size,
  const sf::Vector2u                                  &texture_size,
  const open_viii::graphics::background::is_tile auto &tile_const,
  open_viii::graphics::background::is_tile auto      &&tile) const
{
     using namespace open_viii::graphics::literals;
     using tile_type  = std::remove_cvref_t<decltype(tile)>;
     //  auto       src_tpw = tile_type::texture_page_width(tile_const.depth());
     //  const auto x       = [this, &tile_const, &src_tpw]() -> std::uint32_t {
     //    if (m_filters.upscale.enabled())
     //    {
     //      return 0;
     //    }
     //    return tile_const.texture_id() * src_tpw;
     //  }();
     const auto src_x = [&tile_const]() -> std::uint32_t { return static_cast<std::uint32_t>(tile_const.x()); }();
     const auto src_y = [&tile_const]() -> std::uint32_t { return static_cast<std::uint32_t>(tile_const.y()); }();

     const auto dst_x = [this, &tile]() {
          if (m_draw_swizzle)
          {
               if (!m_disable_texture_page_shift)
               {
                    return static_cast<uint32_t>(tile.source_x() + tile.texture_id() * tile_type::texture_page_width(4_bpp));
               }
               return static_cast<uint32_t>(tile.source_x());
          }
          return static_cast<uint32_t>(tile.x());
     }();
     const auto dst_y = [this, &tile]() {
          if (m_draw_swizzle)
          {
               return static_cast<uint32_t>(tile.source_y());
          }
          return static_cast<uint32_t>(tile.y());
     }();
     return get_triangle_strip(draw_size, texture_size, src_x, src_y, dst_x, dst_y);
}
[[nodiscard]] std::array<sf::Vertex, 4U> map_sprite::get_triangle_strip(
  const sf::Vector2u                                  &draw_size,
  const sf::Vector2u                                  &texture_size,
  const open_viii::graphics::background::is_tile auto &tile_const,
  open_viii::graphics::background::is_tile auto      &&tile) const
{
     using namespace open_viii::graphics::literals;
     using tile_type    = std::remove_cvref_t<decltype(tile)>;
     auto       src_tpw = tile_type::texture_page_width(tile_const.depth());
     const auto x       = [this, &tile_const, &src_tpw]() -> std::uint32_t {
          if (m_filters.upscale.enabled())
          {
               return 0;
          }
          return tile_const.texture_id() * src_tpw;
     }();
     const auto src_x = [&tile_const, &x, this]() -> std::uint32_t {
          if (m_filters.deswizzle.enabled())
          {
               return static_cast<std::uint32_t>(tile_const.x());
          }
          return tile_const.source_x() + x;
     }();
     const auto src_y = [&tile_const, this]() -> std::uint32_t {
          if (m_filters.deswizzle.enabled())
          {
               return static_cast<std::uint32_t>(tile_const.y());
          }
          return tile_const.source_y();
     }();

     const auto dst_x = [this, &tile]() {
          if (m_draw_swizzle)
          {
               if (!m_disable_texture_page_shift)
               {
                    return static_cast<uint32_t>(tile.source_x() + tile.texture_id() * tile_type::texture_page_width(4_bpp));
               }
               return static_cast<uint32_t>(tile.source_x());
          }
          return static_cast<uint32_t>(tile.x());
     }();
     const auto dst_y = [this, &tile]() {
          if (m_draw_swizzle)
          {
               return static_cast<uint32_t>(tile.source_y());
          }
          return static_cast<uint32_t>(tile.y());
     }();
     return get_triangle_strip(draw_size, texture_size, src_x, src_y, dst_x, dst_y);
}

void set_color(std::array<sf::Vertex, 4U> &vertices, const sf::Color &color)
{
     for (auto &vertex : vertices)
     {
          vertex.color = color;
     }
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
     return m_maps.const_back().visit_tiles([this](const auto &tiles) {
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
     compact_generic(
       [](const auto &tile) {
            return std::make_tuple(
              tile.texture_id(),
              tile.source_y(),
              static_cast<std::uint8_t>(3U - (tile.depth().raw() & 3U)),
              tile.source_x(),
              tile.palette_id());
       },
       [](const auto &key, const auto &) { return static_cast<std::uint8_t>(1U << (3U - std::get<2>(key))); });
}


void map_sprite::compact_all()
{
     compact_generic(
       [](const auto &tile) {
            return std::make_tuple(
              static_cast<std::uint8_t>(3U - (tile.depth().raw() & 3U)),
              tile.texture_id(),
              tile.source_y(),
              tile.source_x(),
              tile.palette_id());
       },
       [](const auto &key, const auto &) { return static_cast<std::uint8_t>(1U << (3U - std::get<0>(key))); });
}

void map_sprite::flatten_bpp()
{
     m_maps.copy_back().visit_tiles([](auto &tiles) {
          std::ranges::transform(tiles, tiles.begin(), [](const auto tile) {
               if (filter_invalid(tile))
               {
                    return tile.with_depth(4_bpp);
               }
               return tile;
          });
     });
}
void map_sprite::flatten_palette()
{
     m_maps.copy_back().visit_tiles([](auto &tiles) {
          std::ranges::transform(tiles, tiles.begin(), [](const auto tile) {
               if (filter_invalid(tile))
               {
                    return tile.with_palette_id(0);
               }
               return tile;
          });
     });
}

std::size_t map_sprite::row_empties(std::uint8_t tile_y, std::uint8_t texture_page, const bool move_from_row)
{
     return m_maps.const_back().visit_tiles([&tile_y, &texture_page, &move_from_row](const auto &tiles) -> std::size_t {
          std::vector<std::pair<std::uint8_t, std::int8_t>> values = {};
          auto                                              filtered_range =
            tiles | std::views::filter([&tile_y, &texture_page](const auto &tile) -> bool {
                 return std::cmp_equal((tile.source_y() / TILE_SIZE), tile_y) && std::cmp_equal(texture_page, tile.texture_id());
            });
          std::transform(
            std::ranges::begin(filtered_range), std::ranges::end(filtered_range), std::back_inserter(values), [](const auto &tile) {
                 return std::make_pair(
                   static_cast<std::uint8_t>(1U << (tile.depth().raw() & 3U)), static_cast<std::uint8_t>(tile.source_x() / TILE_SIZE));
            });
          std::ranges::sort(values);
          const auto [first, last] = std::ranges::unique(values);
          values.erase(first, last);
          auto        transform_values = values | std::views::transform([](const auto &pair) -> std::size_t { return pair.first; });
          std::size_t total            = TILE_SIZE - std::reduce(transform_values.begin(), transform_values.end(), std::size_t{});
          if (move_from_row)
          {
               total += 1U;
          }
          return total;
     });
}
void map_sprite::update_position(const sf::Vector2i &pixel_pos, const uint8_t &texture_page, const sf::Vector2i &down_pixel_pos)
{
     if (m_saved_indices.empty() && m_saved_imported_indices.empty())
     {
          return;
     }
     Map       &current_map = m_maps.copy_back();
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
                             "location!",// at least because i am filtering by depth and
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
                             "location!",// at least because i am filtering by depth and
                                         // palette
                             intersecting.size());
                           return;
                      }
                      const std::int32_t x_offset = (down_pixel_pos.x % 256) - static_cast<int>(tile.source_x());
                      const std::int32_t y_offset = down_pixel_pos.y - static_cast<int>(tile.source_y());
                      tile                        = tile
                               .with_source_xy(
                                 static_cast<std::uint8_t>((((pixel_pos.x % 256) - x_offset))),
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
     history_remove_duplicate();
     m_saved_indices.clear();
     m_saved_imported_indices.clear();
     update_render_texture();
}

sf::Sprite map_sprite::save_intersecting(const sf::Vector2i &pixel_pos, const std::uint8_t &texture_page)
{
     // static constexpr auto tile_size_float = static_cast<float>(TILE_SIZE);
     sf::Sprite sprite      = {};
     const auto sprite_size = m_drag_sprite_texture->getSize();
     spdlog::info("sprite_size: ({},{})", sprite_size.x, sprite_size.y);
     spdlog::info("m_scale: ({})", m_scale);
     sprite.setTextureRect({ 0, 0, static_cast<int>(sprite_size.x), static_cast<int>(sprite_size.y) });
     static constexpr float one_and_half = 1.5F;
     sprite.setPosition(
       static_cast<float>(pixel_pos.x) - (TILE_SIZE * one_and_half), static_cast<float>(pixel_pos.y) - (TILE_SIZE * one_and_half));
     sprite.setScale(1.0F / static_cast<float>(m_scale), 1.0F / static_cast<float>(m_scale));
     //  sprite.setScale(
     //    1.F / static_cast<float>(m_scale), 1.F / static_cast<float>(m_scale));
     m_saved_indices = find_intersecting(m_maps.const_back(), pixel_pos, texture_page);
     if (!m_draw_swizzle)
     {
          m_saved_imported_indices = find_intersecting(m_imported_tile_map, pixel_pos, texture_page);
     }
     m_drag_sprite_texture->clear(sf::Color::Transparent);
     const auto draw_drag_texture =
       [this, &pixel_pos, &sprite_size](const auto &front_tiles, const auto &tiles, const std::uint16_t z, bool imported = false) {
            sf::RenderStates states = {};
            //        const auto render_texture_size = m_render_texture->getSize()
            //        / m_scale;
            states.transform.translate(sf::Vector2f(
              (static_cast<float>(-pixel_pos.x) * static_cast<float>(m_scale)) + (static_cast<float>(sprite_size.x) * 0.5F),
              (static_cast<float>(-pixel_pos.y) * static_cast<float>(m_scale)) + (static_cast<float>(sprite_size.x) * 0.5F)));
            for (const auto tile_index : imported ? m_saved_imported_indices : m_saved_indices)
            {
                 const auto &tile       = tiles[tile_index];
                 const auto &front_tile = front_tiles[tile_index];
                 if (front_tile.z() != z)
                 {
                      continue;
                 }
                 states.texture = imported ? m_imported_texture : get_texture(tile.depth(), tile.palette_id(), tile.texture_id());
                 if (states.texture == nullptr || states.texture->getSize().y == 0 || states.texture->getSize().x == 0)
                 {
                      continue;
                 }
                 const auto draw_size    = get_tile_draw_size();
                 const auto texture_size = imported ? get_tile_texture_size_for_import() : get_tile_texture_size(states.texture);
                 auto       quad         = imported ? get_triangle_strip_for_imported(draw_size, texture_size, front_tile, tile)
                                                    : get_triangle_strip(draw_size, texture_size, front_tile, tile);
                 states.blendMode        = sf::BlendAlpha;
                 if (!m_disable_blends)
                 {
                      states.blendMode = set_blend_mode(tile.blend_mode(), quad);
                 }

                 m_drag_sprite_texture->draw(quad.data(), quad.size(), sf::TriangleStrip, states);
            }
       };

     for (const std::uint16_t &z : m_all_unique_values_and_strings.z().values())
     {
          m_maps.front().visit_tiles([&](const auto &front_tiles) {
               m_maps.const_back().visit_tiles([&](const auto &tiles) { draw_drag_texture(front_tiles, tiles, z); });
          });
          m_imported_tile_map_front.visit_tiles([&](const auto &imported_front_tiles) {
               m_imported_tile_map.visit_tiles(
                 [&](const auto &imported_tiles) { draw_drag_texture(imported_front_tiles, imported_tiles, z, true); });
          });
     }
     m_drag_sprite_texture->display();
     sprite.setTexture(m_drag_sprite_texture->getTexture());
     update_render_texture();
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
                      const auto current_index = m_maps.get_offset_from_back(tile);
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
                 if (fail_filter(tile))
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
          constexpr static uint8_t per50 = (std::numeric_limits<uint8_t>::max)() / 2U;
          set_color(quad, { per50, per50, per50, per50 });// 50% alpha
          return sf::BlendAdd;
     }
     else if (blend_mode == BlendModeT::quarter_add)
     {
          constexpr static uint8_t per25 = (std::numeric_limits<uint8_t>::max)() / 4U;
          set_color(quad, { per25, per25, per25, per25 });// 25% alpha
          return sf::BlendAdd;
     }
     else if (blend_mode == BlendModeT::subtract)
     {
          return GetBlendSubtract();
     }
     return sf::BlendAlpha;
}
[[nodiscard]] bool map_sprite::draw_imported([[maybe_unused]] sf::RenderTarget &target, [[maybe_unused]] sf::RenderStates states) const
{
     if (!m_using_imported_texture || m_imported_texture == nullptr)
     {
          return false;
     }
     bool       drew               = false;
     // target.clear(sf::Color::Transparent);
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
          if (fail_filter(tile))
          {
               return;
          }
          if (!filter_invalid(tile_const))
          {
               return;
          }
          states.texture = m_imported_texture;
          //        if (!m_filters.deswizzle.enabled())
          //        {
          //          states.texture = get_texture(
          //            tile_const.depth(),
          //            tile_const.palette_id(),
          //            tile_const.texture_id());
          //        }
          //        else
          //        {
          //          states.texture = get_texture(pupu_id);
          //        }
          if (states.texture == nullptr || states.texture->getSize().y == 0 || states.texture->getSize().x == 0)
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

          // std::lock_guard<std::mutex> lock(mutex_texture);
          // spdlog::info("({}, {})\t", raw_texture_size.x,
          // raw_texture_size.y); draw the vertex array
          target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
          drew = true;
     };
     for (const auto &z : m_all_unique_values_and_strings.z().values())
     {
          m_imported_tile_map_front.visit_tiles([&](const auto &front_tiles) {
               m_imported_tile_map.visit_tiles([&](const auto &tiles) {
                    auto fb = front_tiles.cbegin();
                    auto fe = front_tiles.cend();
                    auto bb = tiles.cbegin();
                    auto be = tiles.cend();
                    for (; fb != fe && bb != be; ++fb, ++bb)
                    {
                         const auto &tile_const = *fb;
                         const auto &tile       = *bb;
                         if (tile.z() != z)
                         {
                              return;
                         }
                         const auto current_index = std::ranges::distance(&tiles.front(), &tile);
                         draw_imported_tile(current_index, tile_const, tile);
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
const sf::BlendMode &map_sprite::GetBlendSubtract()
{
     const static auto BlendSubtract =
       sf::BlendMode{ sf::BlendMode::DstColor,// or One
                      sf::BlendMode::One,      sf::BlendMode::ReverseSubtract, sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha,
                      sf::BlendMode::Add };
     return BlendSubtract;
}

map_sprite map_sprite::with_coo(const open_viii::LangT coo) const
{
     return { m_field, coo, m_draw_swizzle, m_filters, m_disable_blends };
}

map_sprite map_sprite::with_field(std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const
{
     return { std::move(field), m_coo, m_draw_swizzle, m_filters, m_disable_blends };
}

map_sprite map_sprite::with_filters(ff_8::filters filters) const
{
     return { m_field, m_coo, m_draw_swizzle, std::move(filters), m_disable_blends };
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
     auto quad         = get_triangle_strip(draw_size, texture_size, 0, 0, 0, 0);
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
     auto future = save_image_pbo(m_render_texture->getTexture());
     future.wait();
     const auto image = future.get();
     if (!image.saveToFile(path.string()))
     {
          spdlog::warn("Failed to save file: {}", path.string());
     }
}
bool map_sprite::fail() const
{
     static bool once = true;
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
     if (m_mim.get_width(4_bpp, false) == 0)
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
     bool       coo  = false;
     const auto map  = get_map(nullptr, false, coo);
     const auto path = dest_path.string();

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
}
std::string map_sprite::map_filename() const
{
     return std::filesystem::path(m_map_path).filename().string();
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
               m_scale                = y / 256U;
               if (m_filters.deswizzle.enabled())
               {
                    m_scale = y / m_canvas.height();
               }
          }
          else
          {
               m_scale = 1U;
          }
          if (m_using_imported_texture && m_scale < m_imported_tile_size / 16U)
          {
               m_scale = m_imported_tile_size / 16U;
          }
          check_size();
          spdlog::info("Render Texture- scale:{}, size:({}, {})", m_scale, width() * m_scale, height() * m_scale);
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
     return static_cast<open_viii::graphics::Rectangle<std::uint32_t>>(m_maps.const_back().canvas());
}

std::uint32_t map_sprite::width() const
{
     if (m_draw_swizzle)
     {
          using namespace open_viii::graphics::literals;
          return m_mim.get_width(4_bpp);
     }
     return m_canvas.width();
}

std::uint32_t map_sprite::height() const
{
     if (m_draw_swizzle)
     {
          return m_mim.get_height();
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
     return { { (1U << (8U - ((4_bpp).raw() & 3U))), 256U }, { m_mim.get_width(4_bpp), m_mim.get_height() }, sf::Color::Yellow };
}

all_unique_values_and_strings map_sprite::get_all_unique_values_and_strings() const
{
     return m_maps.const_back().visit_tiles([](const auto &tiles) { return all_unique_values_and_strings(tiles); });
}

const ff_8::filters &map_sprite::filter() const
{
     return m_filters;
}
ff_8::filters &map_sprite::filter()
{
     return m_filters;
}
map_sprite map_sprite::update(std::shared_ptr<open_viii::archive::FIFLFS<false>> field, open_viii::LangT coo, bool draw_swizzle) const
{
     return { std::move(field), coo, draw_swizzle, m_filters, m_disable_blends };
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
//
// Each image is a texture page use the scale of the largest
// source_texture_height/256. The texture is square. or you could look at it as
// 16x16 tiles. Though I'd rather keep the same dimensions as the original
// image. If you have overlapping palettes you can use texture page and palette.
// Though for successful drawing this shouldn't really be usable. Unless
// demaster fixes the palette filtering this really isn't 100% helpful though it
// does keep the tiles from being lost.
//
// Add option to Menu Save "Save new textures".
// Pop up a directory picker dialog box. So They can choose a directory to save
// the images too. Pre populated with the 2 letter prefix\fieldname. If path
// doesn't exist create it. create blank textures same dims as existing textures
// height in a square. draw all tiles to that texture that exist with only alpha
// blending no special sauce. With the matching texture page and or palette.
// Save to png with {fieldname}_{texture_page}_{palette}.png or
// {fieldname}_{texture_page}.png Do this till all the textures are created. Add
// detection for overlapping palettes and make sure to place them into other
// files. map - save changes #48 - this should be done at the same time
// especially when map has changed.
template<typename T>
struct setting_backup
{
   private:
     T                         m_backup;
     std::reference_wrapper<T> m_value;

   public:
     setting_backup()                                  = delete;
     setting_backup(const setting_backup &)            = default;
     setting_backup &operator=(const setting_backup &) = default;
     setting_backup(setting_backup &&)                 = delete;
     setting_backup &operator=(setting_backup &&)      = delete;

     explicit setting_backup(T &in_value)
       : m_backup(in_value)
       , m_value(in_value)
     {
     }

     ~setting_backup() noexcept(noexcept(m_value.get() = m_backup))
     {
          m_value.get() = m_backup;
     }

     [[nodiscard]] const T &backup() const
     {
          return m_backup;
     }

     [[nodiscard]] const T &value() const
     {
          return m_value.get();
     }

     [[nodiscard]] T &value()
     {
          return m_value.get();
     }

     auto &operator=(const T &in_value)
     {
          m_value.get() = in_value;
          return *this;
     }

     [[nodiscard]] bool operator==(const T &in_value) const
     {
          return m_value == in_value;
     }
};
struct settings_backup
{
   public:
     setting_backup<ff_8::filters> filters;
     setting_backup<bool>          draw_swizzle;
     setting_backup<bool>          disable_texture_page_shift;
     setting_backup<bool>          disable_blends;
     setting_backup<uint32_t>      scale;

     settings_backup(
       ff_8::filters &in_filters,
       bool          &in_draw_swizzle,
       bool          &in_disable_texture_page_shift,
       bool          &in_disable_blends,
       std::uint32_t &in_scale)
       : filters{ in_filters }
       , draw_swizzle{ in_draw_swizzle }
       , disable_texture_page_shift{ in_disable_texture_page_shift }
       , disable_blends{ in_disable_blends }
       , scale{ in_scale }
     {
     }
};
bool map_sprite::check_if_one_palette(const std::uint8_t &texture_page) const
{
     return m_maps.const_back().visit_tiles([&texture_page](const auto &tiles) -> bool {
          auto filtered_tiles = tiles | std::views::filter([&texture_page](const auto &tile) -> bool {
                                     return std::cmp_equal(tile.texture_id(), texture_page);
                                });
          auto [min, max]     = std::ranges::minmax_element(filtered_tiles, {}, [](const auto &tile) { return tile.palette_id(); });
          if (min != filtered_tiles.end() && max != filtered_tiles.end())
          {
               spdlog::info("Unmatched Palettes {}\t{}", min->palette_id(), max->palette_id());
               return !std::cmp_equal(min->palette_id(), max->palette_id());
          }
          return false;
     });
}
void map_sprite::save_new_textures(const std::filesystem::path &path)
{
     auto task = gen_new_textures(path);
     while (!task.is_ready())
     {
     }
}

cppcoro::task<void> map_sprite::gen_new_textures(const std::filesystem::path path)
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
          co_return;
     }
     using map_type                          = std::remove_cvref_t<decltype(get_conflicting_palettes())>;
     using mapped_type                       = typename map_type::mapped_type;
     const map_type conflicting_palettes_map = get_conflicting_palettes();
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
                         auto out_path = [&]() -> std::filesystem::path {
                              if (m_using_coo)
                              {
                                   return save_path_coo(pattern_coo_texture_page_palette, path, field_name, texture_page, palette);
                              }
                              return save_path(pattern_texture_page_palette, path, field_name, texture_page, palette);
                         }();

                         auto out_texture = save_texture(height, height);

                         async_save(out_path, out_texture);
                         co_await cppcoro::suspend_always{};
                    }
               }
          }

          settings.filters.value().palette.disable();
          settings.filters.value().bpp.disable();
          auto out_path    = m_using_coo ? save_path_coo(pattern_coo_texture_page, path, field_name, texture_page)
                                         : save_path(pattern_texture_page, path, field_name, texture_page);

          auto out_texture = save_texture(height, height);

          async_save(out_path, out_texture);
          co_await cppcoro::suspend_always{};
     }

     for (std::future<void> &future : m_futures)
     {
          while (true)
          {
               std::future_status status = future.wait_for(std::chrono::seconds(0));
               if (status == std::future_status::ready)
               {
                    // Do something with the result
                    co_await cppcoro::suspend_always{};
                    break;
               }
               else if (status == std::future_status::timeout)
               {
                    // Future not yet ready
               }
               else if (status == std::future_status::deferred)
               {
                    // Deferred future, not yet started
               }
               co_await cppcoro::suspend_always{};
          }
     }
     m_futures.clear();
}
std::string map_sprite::get_base_name() const
{
     return str_to_lower(m_field->get_base_name());
}

void map_sprite::save_pupu_textures(const std::filesystem::path &path)
{
     auto task = gen_pupu_textures(path);
     while (!task.is_ready())
     {
     }
}

cppcoro::task<void> map_sprite::gen_pupu_textures(const std::filesystem::path path)
{
     // assert(std::filesystem::path.is_directory(path));
     const std::string                 field_name       = { str_to_lower(m_field->get_base_name()) };
     static constexpr std::string_view pattern_pupu     = { "{}_{}.png" };
     static constexpr std::string_view pattern_coo_pupu = { "{}_{}_{}.png" };

     const auto                       &unique_pupu_ids  = m_all_unique_values_and_strings.pupu().values();
     settings_backup                   settings(m_filters, m_draw_swizzle, m_disable_texture_page_shift, m_disable_blends, m_scale);
     settings.filters                         = ff_8::filters{};
     settings.filters.value().upscale         = settings.filters.backup().upscale;
     settings.draw_swizzle                    = false;
     settings.disable_texture_page_shift      = true;
     settings.disable_blends                  = true;
     // todo maybe draw with blends enabled to transparent black or white.
     uint32_t                      tex_height = get_max_texture_height();
     static constexpr unsigned int mim_height = { 256U };
     settings.scale                           = tex_height / mim_height;
     const auto canvas                        = m_maps.const_back().canvas() * static_cast<int>(m_scale);
     if (settings.scale == 0U)
     {
          settings.scale = 1U;
     }
     for (const PupuID &pupu : unique_pupu_ids)
     {
          settings.filters.value().pupu.update(pupu).enable();
          std::filesystem::path out_path =
            m_using_coo ? save_path_coo(pattern_coo_pupu, path, field_name, pupu) : save_path(pattern_pupu, path, field_name, pupu);
          co_await cppcoro::suspend_always{};
          std::shared_ptr<sf::RenderTexture> out_texture =
            save_texture(static_cast<std::uint32_t>(canvas.width()), static_cast<std::uint32_t>(canvas.height()));
          co_await cppcoro::suspend_always{};
          async_save(out_path, out_texture);
          co_await cppcoro::suspend_always{};
     }
     for (std::future<void> &future : m_futures)
     {
          co_await cppcoro::suspend_always{};
          future.wait();
          co_await cppcoro::suspend_always{};
     }
     m_futures.clear();
}
void map_sprite::async_save(const std::filesystem::path &out_path, const std::shared_ptr<sf::RenderTexture> &out_texture)
{
     if (out_texture)
     {
          // trying packaged task to, so we don't wait for files to save.
          const auto task = [=, image = out_texture->getTexture().copyToImage()]() {
               std::error_code error_code{};
               std::filesystem::create_directories(out_path.parent_path(), error_code);
               if (error_code)
               {
                    spdlog::error(
                      "error {}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), out_path.string());
                    error_code.clear();
               }
               if (image.getSize().x == 0 || image.getSize().y == 0 || image.getPixelsPtr() == nullptr)
               {
                    spdlog::error(
                      "error {}:{} Invalid image: \"{}\" - ({},{})",
                      __FILE__,
                      __LINE__,
                      out_path.string(),
                      image.getSize().x,
                      image.getSize().y);
                    return;
               }
               using namespace std::chrono_literals;
               const std::string     filename         = out_path.string();
               std::size_t           count            = { 0U };
               static constexpr auto error_delay_time = 1000ms;
               for (;;)
               {
                    if (!save_png_image(image, filename))
                    {
                         spdlog::error("Looping on fail {:>2} times", ++count);
                         std::this_thread::sleep_for(error_delay_time);
                    }
                    else
                    {
                         spdlog::info("Saved \"{}\"", filename);
                         break;
                    }
               }
          };

#if 0
    auto package =
      std::packaged_task<void(std::filesystem::path, sf::Image)>{ task };

    std::thread{ std::move(package),
                 out_path,
                 out_texture->getTexture().copyToImage() }
      .detach();
#else
          spawn_thread(task);
#endif
     }
}
bool map_sprite::save_png_image(const sf::Image &image, const std::string &filename)
{
#if 0
  return image.saveToFile(filename);
#else
     return open_viii::graphics::Png::save(image.getPixelsPtr(), image.getSize().x, image.getSize().y, filename, {}, {}).has_value();
#endif
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
          tex_height = 256U;
     }
     if (m_using_imported_texture && m_imported_tile_size / 16 * 256U > tex_height)
     {
          tex_height = m_imported_tile_size / 16 * 256U;
     }
     return tex_height;
}
std::filesystem::path map_sprite::save_path_coo(
  fmt::format_string<std::string_view, std::string_view, uint8_t> pattern,
  const std::filesystem::path                                    &path,
  const std::string_view                                         &field_name,
  uint8_t                                                         texture_page) const
{
     return path
            / fmt::vformat(
              fmt::string_view(pattern), fmt::make_format_args(field_name, open_viii::LangCommon::to_string(m_coo), texture_page));
}
std::filesystem::path map_sprite::save_path_coo(
  fmt::format_string<std::string_view, std::string_view, uint8_t, uint8_t> pattern,
  const std::filesystem::path                                             &path,
  const std::string_view                                                  &field_name,
  uint8_t                                                                  texture_page,
  uint8_t                                                                  palette) const
{
     return path
            / fmt::vformat(
              fmt::string_view(pattern), fmt::make_format_args(field_name, open_viii::LangCommon::to_string(m_coo), texture_page, palette));
}
std::filesystem::path map_sprite::save_path_coo(
  fmt::format_string<std::string_view, std::string_view, PupuID> pattern,
  const std::filesystem::path                                   &path,
  const std::string_view                                        &field_name,
  PupuID                                                         pupu) const
{
     return path
            / fmt::vformat(fmt::string_view(pattern), fmt::make_format_args(field_name, open_viii::LangCommon::to_string(m_coo), pupu));
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
// std::filesystem::path
//   map_sprite::save_path(fmt::string_view pattern,
//     const std::filesystem::path         &path,
//     const std::string                   &field_name,
//     std::optional<std::uint8_t>          texture_page,
//     std::optional<std::uint8_t>          palette,
//     std::optional<PupuID>                pupu) const
//{
//   // todo put language code in filename. because of remaster multilanguage
//   // maps.
//   std::string filename = {};
//   if (texture_page.has_value())
//   {
//     if (palette.has_value())
//     {
//       filename =
//         fmt::format(fmt::runtime(pattern), field_name, *texture_page,
//         *palette);
//     }
//     else
//     {
//       filename = fmt::format(fmt::runtime(pattern), field_name,
//       *texture_page);
//     }
//   }
//   else if (pupu.has_value())
//   {
//     filename = fmt::format(fmt::runtime(pattern), field_name, *pupu);
//   }
//   return path / filename;
// }

std::shared_ptr<sf::RenderTexture> map_sprite::save_texture(std::uint32_t width, std::uint32_t height) const
{
     auto texture = std::make_shared<sf::RenderTexture>();
     texture->create(width, height);
     if (local_draw(*texture, sf::RenderStates::Default))
     {
          (void)draw_imported(*texture, sf::RenderStates::Default);
          texture->display();
          texture->setSmooth(false);
          texture->setRepeated(false);
          texture->generateMipmap();
          return texture;
     }
     return nullptr;
}
void map_sprite::load_map(const std::filesystem::path &src_path)
{
     const auto path = src_path.string();
     open_viii::tools::read_from_file(
       [this](std::istream &os) {
            (void)m_maps.copy_back();
            m_maps.front().visit_tiles([this, &os](const auto &const_tiles) {
                 using TileT   = std::remove_cvref_t<decltype(const_tiles.front())>;
                 m_maps.back() = open_viii::graphics::background::Map(
                   [&os]() -> std::variant<
                             open_viii::graphics::background::Tile1,
                             open_viii::graphics::background::Tile2,
                             open_viii::graphics::background::Tile3,
                             std::monostate> {
                        TileT      tile{};
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
            m_maps.back().shift(m_maps.front().offset().abs());
            (void)m_maps.copy_back_to_front();
       },
       path);
     update_render_texture();
}
void map_sprite::test_map(const std::filesystem::path &saved_path) const
{
     const auto raw_map   = Map{ m_mim.mim_type(), m_field->get_entry_data({ saved_path.filename().string() }), false };
     auto       saved_map = Map{ m_mim.mim_type(), open_viii::tools::read_entire_file(saved_path), false };

     raw_map.visit_tiles([&](const auto &raw_tiles) {
          saved_map.visit_tiles([&](const auto &saved_tiles) {
               if constexpr (std::is_same_v<decltype(raw_tiles), decltype(saved_tiles)>)
               {
                    using pair_type = decltype(std::make_pair(&raw_tiles.front(), &saved_tiles.front()));
                    std::vector<pair_type> pairs{};
                    std::ranges::transform(
                      raw_tiles, saved_tiles, std::back_inserter(pairs), [](const auto &raw_tile, const auto &saved_tile) {
                           if (raw_tile != saved_tile)
                           {
                                return std::make_pair(&raw_tile, &saved_tile);
                           }
                           return pair_type{ nullptr, nullptr };
                      });
                    pairs.erase(std::remove(pairs.begin(), pairs.end(), pair_type{ nullptr, nullptr }), pairs.end());
                    std::ranges::for_each(pairs, [](const pair_type &pair) {
                         format_tile_text(
                           *pair.first, [](std::string_view name, const auto &value) { spdlog::info("tile {}: {}", name, value); });
                         format_tile_text(
                           *pair.second, [](std::string_view name, const auto &value) { spdlog::info("tile {}: {}", name, value); });
                    });
               }
          });
     });
}
void map_sprite::save_modified_map(const std::filesystem::path &dest_path) const
{
     const auto path = dest_path.string();
     spdlog::info("Saving modified map: {}", path);
     open_viii::tools::write_buffer(
       [this](std::ostream &os) {
            bool       used_imports   = false;
            bool       wrote_end_tile = false;
            const auto append         = [this, &os](auto t) {
                 // shift to original offset
                 if (filter_invalid(t))
                 {
                      t = t.shift_xy(m_maps.const_back().offset());
                 }
                 // save tile
                 const auto data = std::bit_cast<std::array<char, sizeof(t)>>(t);
                 os.write(data.data(), data.size());
            };
            const auto append_imported_tiles = [this, &used_imports, &append]() {
                 if (m_using_imported_texture && !used_imports)
                 {
                      used_imports = true;
                      m_imported_tile_map.visit_tiles([&append](const auto &import_tiles) {
                           spdlog::info("Saving imported tiles {} count", std::ranges::size(import_tiles));
                           for (const auto &import_tile : import_tiles)
                           {
                                if (filter_invalid(import_tile))
                                {
                                     append(import_tile);
                                }
                           }
                      });
                 }
            };
            append_imported_tiles();
            for_all_tiles(
              [&append, &wrote_end_tile](const auto &tile_const, const auto &tile, const auto &) {
                   bool const end_const = filter_invalid(tile_const);
                   bool const end_other = filter_invalid(tile);
                   if (end_const || end_other)// should be last tile.
                   {
                        // spdlog::info("About to save the last tile.");
                        //  write imported tiles first.
                        //  write from tiles const
                        if (end_other)
                        {
                             append(tile);
                        }
                        else
                        {
                             append(tile_const);
                        }
                        wrote_end_tile = true;
                        return;
                   }
                   append(tile);
                   // write from tiles.
              },
              false,
              true);
            // write imported tiles first.
            //      append_imported_tiles();
            //      if (!wrote_end_tile)
            //      {
            //        const_visit_tiles([&append, &wrote_end_tile](const auto
            //        &tiles)
            //        {
            //          spdlog::info("Generating the last tile. (shouldn't
            //          happen)"); using tile_t =
            //          std::remove_cvref_t<decltype(tiles.front())>; tile_t tile{};
            //          append(tile.with_xy(0x7FFFU, 0U));
            //          wrote_end_tile = true;
            //        });
            //      }
       },
       path,
       "");
     test_map(dest_path);
}
std::size_t map_sprite::size_of_map() const
{
     return m_maps.back().visit_tiles([](const auto &tiles) {
          using tile_type = typename std::remove_cvref_t<decltype(tiles)>::value_type;
          return std::ranges::size(tiles) * sizeof(tile_type);
     });
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
     auto &map = m_maps.copy_back();
     map.visit_tiles([](auto &&tiles) {
          auto filtered_tiles            = tiles | std::views::filter(ff_8::tile_operations::InvalidTile{});
          using tile_t                   = std::remove_cvref_t<std::ranges::range_value_t<decltype(tiles)>>;
          const auto WithDepth_operation = ff_8::tile_operations::WithDepth<tile_t>{ open_viii::graphics::BPPT::BPP4_CONST() };
          for (std::size_t tile_index = {}; tile_t & tile : filtered_tiles)
          {
               const auto texture_page    = static_cast<ff_8::tile_operations::TextureIdT<tile_t>>(tile_index / 256);
               const auto file_tile_index = tile_index % 256;
               const auto source_x        = static_cast<ff_8::tile_operations::SourceXT<tile_t>>((file_tile_index % TILE_SIZE) * TILE_SIZE);
               const auto source_y        = static_cast<ff_8::tile_operations::SourceYT<tile_t>>((file_tile_index / TILE_SIZE) * TILE_SIZE);
               ++tile_index;
               const auto WithTextureId_operation = ff_8::tile_operations::WithTextureId<tile_t>{ texture_page };
               const auto WithSourceXY_operation  = ff_8::tile_operations::WithSourceXY<tile_t>{ { source_x, source_y } };
               tile                               = tile | WithDepth_operation | WithSourceXY_operation | WithTextureId_operation;
          }
     });
     update_render_texture();
}
