#include "map_sprite.hpp"

#include "append_inserter.hpp"
#include "imgui_format_text.hpp"
#include <utility>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;
bool
  map_sprite::empty() const
{
  return m_map.visit_tiles([](auto &&tiles) { return std::empty(tiles); });
}

Mim
  map_sprite::get_mim() const
{
  if (m_field != nullptr)
  {
    auto lang_name =
      fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Mim::EXT);
    return { m_field->get_entry_data({ std::string_view(lang_name), Mim::EXT }),
      m_field->get_base_name() };
  }
  return {};
}

Map
  map_sprite::get_map(std::string *out_path, bool sort_remove, bool shift) const
{
  if (m_field != nullptr)
  {
    auto lang_name =
      fmt::format("_{}{}", open_viii::LangCommon::to_string(m_coo), Map::EXT);
    return Map{ m_mim.mim_type(),
      m_field->get_entry_data(
        { std::string_view(lang_name), Map::EXT }, out_path),
      sort_remove,
      shift };
  }
  return {};
}

map_sprite::colors_type
  map_sprite::get_colors(open_viii::graphics::BPPT bpp,
    std::uint8_t                                   palette) const
{
  return m_mim.get_colors<open_viii::graphics::Color32RGBA>(
    bpp, palette, false);
}

std::size_t
  map_sprite::get_texture_pos(const open_viii::graphics::BPPT bpp,
    const std::uint8_t                                        palette,
    const std::uint8_t texture_page) const
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
    return MAX_TEXTURES - 1;// 16bpp doesn't have palettes.
  }
  const size_t j = texture_page * 16 + palette;
  if (m_texture->at(j).getSize().y == 0)
  {
    return MAX_TEXTURES - 13U + texture_page;
  }
  return j;
}

const sf::Texture *
  map_sprite::get_texture(const open_viii::graphics::BPPT bpp,
    const std::uint8_t                                    palette,
    const std::uint8_t                                    texture_page) const
{
  return &m_texture->at(get_texture_pos(bpp, palette, texture_page));
}

std::shared_ptr<std::array<sf::Texture, map_sprite::MAX_TEXTURES>>
  map_sprite::get_textures() const
{

  auto ret = std::make_shared<std::array<sf::Texture, MAX_TEXTURES>>(
    std::array<sf::Texture, MAX_TEXTURES>{});
  const auto &range = m_all_unique_values_and_strings.bpp().values();
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
    if (m_filters.upscale.enabled())
    {
      find_upscale_path(ret);
    }
  }
  size_t i = {};
  for (const auto &texture : *ret)
  {
    const auto size = texture.getSize();
    if (size.x != 0)
    {
      fmt::print("\ttex: {:3} - ({:4}, {:4})\n", i++, size.x, size.y);
    }
    else
    {
      ++i;
    }
  }
  return ret;
}
void
  map_sprite::load_mim_textures(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
    open_viii::graphics::BPPT                               bpp,
    std::uint8_t                                            palette) const
{// std::cout << bpp << '\t' << +palette << '\t' << '\t';
  if (m_mim.get_width(bpp) != 0U)
  {
    size_t pos = get_texture_pos(bpp, palette, 0);
    m_futures.emplace_back(std::async(
      std::launch::async,
      [this](sf::Texture *texture, BPPT bppt, uint8_t pal)
      {
        const auto colors = get_colors(bppt, pal);
        // std::lock_guard<std::mutex> lock(mutex_texture);
        texture->create(m_mim.get_width(bppt), m_mim.get_height());
        texture->setSmooth(false);
        texture->update(reinterpret_cast<const sf::Uint8 *>(colors.data()));
      },
      &(ret->at(pos)),
      bpp,
      palette));
  }
}
void
  map_sprite::find_upscale_path(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret) const
{
  for (const auto &texture_page :
    m_all_unique_values_and_strings.texture_page_id().values())
  {
    const auto  &root           = m_filters.upscale.value();
    const auto   paths          = m_upscales.get_file_paths(root, texture_page);
    const size_t i              = MAX_TEXTURES - 13 + texture_page;
    auto         filtered_paths = paths
                          | std::views::filter(
                            [](const std::filesystem::path &path)
                            {
                              return std::filesystem::exists(path)
                                     && !std::filesystem::is_directory(path);
                            });
    if (filtered_paths.begin() != filtered_paths.end())
    {
      const auto &path = *(filtered_paths.begin());
      fmt::print("{}\n", path.string());
      ret->at(i).loadFromFile(path.string());
    }
  }
}
void
  map_sprite::find_upscale_path(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
    std::uint8_t                                            palette) const
{
  for (const auto &texture_page :
    m_all_unique_values_and_strings.texture_page_id().values())
  {
    const auto  &root  = m_filters.upscale.value();
    const auto   paths = m_upscales.get_file_paths(root, texture_page, palette);
    const size_t i     = texture_page * 16 + palette;
    auto         filtered_paths = paths
                          | std::views::filter(
                            [](const std::filesystem::path &path)
                            {
                              return std::filesystem::exists(path)
                                     && !std::filesystem::is_directory(path);
                            });

    if (filtered_paths.begin() != filtered_paths.end())
    {
      const auto &path = *(filtered_paths.begin());
      fmt::print("{}\n", path.string());
      ret->at(i).loadFromFile(path.string());
    }
  }
}
void
  map_sprite::wait_for_futures() const
{
  std::ranges::for_each(m_futures, [](auto &&f) { f.wait(); });
  m_futures.clear();
}

[[nodiscard]] std::array<sf::Vertex, 4U>
  map_sprite::get_triangle_strip(const sf::Vector2u &draw_size,
    const sf::Vector2u                              &texture_size,
    auto                                             source_x,
    auto                                             source_y,
    auto                                             x,
    auto                                             y) const
{
  constexpr static auto tile_size = 16U;
  auto i = static_cast<std::uint32_t>(x / static_cast<decltype(x)>(tile_size));
  auto j = static_cast<std::uint32_t>(y / static_cast<decltype(y)>(tile_size));
  std::uint32_t tu    = (source_x) / static_cast<decltype(source_x)>(tile_size);
  std::uint32_t tv    = source_y / static_cast<decltype(source_y)>(tile_size);
  const auto    tovec = [](auto &&x, auto &&y) {
    return sf::Vector2f{ static_cast<float>(x), static_cast<float>(y) };
  };
  const auto tovert = [&tovec](auto &&x, auto &&y, auto &&texx, auto &&texy) {
    return sf::Vertex{ tovec(x, y), tovec(texx, texy) };
  };
  return std::array{ tovert((i + 1) * draw_size.x,
                       j * draw_size.y,
                       (tu + 1) * texture_size.x,
                       tv * texture_size.y),
    tovert(i * draw_size.x,
      j * draw_size.y,
      tu * texture_size.x,
      tv * texture_size.y),
    tovert((i + 1) * draw_size.x,
      (j + 1) * draw_size.y,
      (tu + 1) * texture_size.x,
      (tv + 1) * texture_size.y),
    tovert(i * draw_size.x,
      (j + 1) * draw_size.y,
      tu * texture_size.x,
      (tv + 1) * texture_size.y) };
}

[[nodiscard]] std::array<sf::Vertex, 4U>
  map_sprite::get_triangle_strip(const sf::Vector2u &draw_size,
    const sf::Vector2u                              &texture_size,
    const auto                                      &tile_const,
    auto                                           &&tile) const
{
  using namespace open_viii::graphics::literals;
  using tile_type    = std::decay_t<decltype(tile)>;
  auto       src_tpw = tile_type::texture_page_width(tile_const.depth());
  const auto x       = [this, &tile_const, &src_tpw]() -> std::uint32_t
  {
    if (m_filters.upscale.enabled())
    {
      return 0;
    }
    return tile_const.texture_id() * src_tpw;
  }();
  const auto src_x = tile_const.source_x() + x;
  const auto src_y = tile_const.source_y();

  const auto dst_x = [this, &tile]()
  {
    if (m_draw_swizzle)
    {
      using tile_type = std::decay_t<decltype(tile)>;
      return static_cast<uint32_t>(
        tile.source_x()
        + tile.texture_id() * tile_type::texture_page_width(4_bpp));
    }
    return static_cast<uint32_t>(tile.x());
  }();
  const auto dst_y = [this, &tile]()
  {
    if (m_draw_swizzle)
    {
      return static_cast<uint32_t>(tile.source_y());
    }
    return static_cast<uint32_t>(tile.y());
  }();
  return get_triangle_strip(
    draw_size, texture_size, src_x, src_y, dst_x, dst_y);
}

void
  set_color(std::array<sf::Vertex, 4U> &vertices, const sf::Color &color)
{
  for (auto &vertex : vertices)
  {
    vertex.color = color;
  }
}

std::uint8_t
  map_sprite::max_x_for_saved() const
{
  static constexpr std::uint8_t tile_size = 16U;
  return m_map.visit_tiles(
    [this](const auto &tiles)
    {
      auto transform_range = m_saved_indicies
                             | std::views::transform(
                               [this, &tiles](std::size_t i)
                               {
                                 auto &tile = tiles[i];
                                 if (m_draw_swizzle)
                                 {
                                   if (tile.depth().bpp4())
                                   {
                                     return 256U;
                                   }
                                   if (tile.depth().bpp16())
                                   {
                                     return 64U;
                                   }
                                   return 128U;
                                 }
                                 return std::uint32_t{ tile_size };
                               });
      const auto it =
        std::min_element(transform_range.begin(), transform_range.end());
      return static_cast<std::uint8_t>((*it - tile_size) / tile_size);
    });
}
std::size_t
  map_sprite::row_empties(std::uint8_t tile_y,
    std::uint8_t                       texture_page,
    const bool                         move_from_row)
{
  static constexpr auto tile_size = 16U;
  return m_map.visit_tiles(
    [this, &tile_y, &texture_page, &move_from_row](auto &&tiles) -> std::size_t
    {
      std::vector<std::pair<std::uint8_t, std::int8_t>> values = {};
      auto                                              filtered_range =
        tiles
        | std::views::filter(
          [&tile_y, &texture_page](const auto &tile) -> bool
          {
            return std::cmp_equal((tile.source_y() / tile_size), tile_y)
                   && std::cmp_equal(texture_page, tile.texture_id());
          });
      std::transform(std::ranges::begin(filtered_range),
        std::ranges::end(filtered_range),
        std::back_inserter(values),
        [](const auto &tile)
        {
          return std::make_pair(
            static_cast<std::uint8_t>(1U << (tile.depth().raw() & 3U)),
            static_cast<std::uint8_t>(tile.source_x() / tile_size));
        });
      std::ranges::sort(values);
      const auto [first, last] = std::ranges::unique(values);
      values.erase(first, last);
      std::size_t total = tile_size
                          - std::reduce(values.begin(),
                            values.end(),
                            std::size_t{},
                            [](const std::size_t left, const auto &right_pair)
                            { return left + right_pair.first; });
      if (move_from_row)
      {
        total += 1U;
      }
      return total;
    });
}

void
  map_sprite::update_position(const sf::Vector2i &pixel_pos,
    const sf::Vector2i                           &tile_pos,
    const std::uint8_t                           &texture_page)
{
  if (m_saved_indicies.empty())
  {
    return;
  }
  m_map.visit_tiles(
    [this, &tile_pos, &texture_page, &pixel_pos](auto &&tiles)
    {
      static constexpr std::uint8_t tile_size = 16U;
      std::uint8_t                  max_x     = max_x_for_saved() * tile_size;
      if (m_draw_swizzle)
      {
        if (auto intersecting =
              find_intersecting(pixel_pos, tile_pos, texture_page, true);
            !intersecting.empty())
        {
          // this might not be good enough as two 4 bpp tiles fit in the
          // same location as 8 bpp. and two 8 bpp fit in space for 16 bpp.
          // but this should catch obvious problems.
          fmt::print(
            "There are {} tiles at this location. Choose an empty "
            "location!\n",
            intersecting.size());
          return;
        }
        const auto &tile     = tiles[m_saved_indicies.front()];
        bool        same_row = ((tile.source_y() / tile_size) == tile_pos.y)
                        && (texture_page == tile.texture_id());
        //        fmt::print("{} == {} && {} == {}\n",
        //          (tile.source_y() / tile_size),
        //          tile_pos.y,
        //          texture_page,
        //          tile.texture_id());
        const auto empty_count = row_empties(
          static_cast<std::uint8_t>(tile_pos.y), texture_page, same_row);
        fmt::print("Empty cells in row = {}\n", empty_count);
        if (empty_count == 0)
        {
          return;
        }
      }
      for (auto i : m_saved_indicies)
      {
        auto &tile = tiles[i];
        if (m_draw_swizzle)
        {
          if (auto intersecting =
                find_intersecting(pixel_pos, tile_pos, texture_page, true);
              !intersecting.empty())
          {
            if (std::ranges::any_of(intersecting,
                  [&tile, &tiles](const auto &j)
                  {
                    const auto &other_tile = tiles[j];
                    return (tile.depth() != other_tile.depth())
                           || (tile.palette_id() != other_tile.palette_id());
                  }))
            {
              // this second pass is to make sure tiles you are moving aren't
              // conflicting with different bpp or palette.
              continue;
            }
          }
          tile =
            tile
              .with_source_xy(
                (std::min)(
                  static_cast<std::uint8_t>(tile_pos.x * tile_size), max_x),
                static_cast<std::uint8_t>(tile_pos.y * tile_size))
              .with_texture_id(texture_page);
        }
        else
        {
          tile = tile.with_xy(
            static_cast<std::int16_t>((pixel_pos.x / tile_size) * tile_size),
            static_cast<std::int16_t>((pixel_pos.y / tile_size) * tile_size));
        }
      }
    });
  update_render_texture();
}

sf::Sprite
  map_sprite::save_intersecting(const sf::Vector2i &pixel_pos,
    const sf::Vector2i                             &tile_pos,
    const std::uint8_t                             &texture_page)
{
  static constexpr auto tile_size       = 16;
  static constexpr auto tile_size_float = 16.F;
  sf::Sprite            sprite          = {};
  sprite.setTexture(m_render_texture->getTexture());
  sprite.setTextureRect({ (pixel_pos.x / tile_size) * tile_size,
    tile_pos.y * tile_size,
    tile_size,
    tile_size });
  sprite.setPosition(
    static_cast<float>(pixel_pos.x / tile_size) * tile_size_float,
    tile_pos.y * tile_size_float);
  m_saved_indicies = find_intersecting(pixel_pos, tile_pos, texture_page);
  return sprite;
}

std::vector<size_t>
  map_sprite::find_intersecting(const sf::Vector2i &pixel_pos,
    const sf::Vector2i                             &tile_pos,
    const std::uint8_t                             &texture_page,
    const bool                                      skip_filters)
{
  return m_map.visit_tiles(
    [this, &tile_pos, &texture_page, &pixel_pos, &skip_filters](auto &&tiles)
    {
      std::vector<std::size_t> out = {};
      auto                     filtered_tiles =
        tiles
        | std::views::filter(
          [this, &skip_filters, &tile_pos, &texture_page, &pixel_pos](
            const auto &tile) -> bool
          {
            static constexpr auto in_bounds = [](auto i, auto low, auto high) {
              return std::cmp_greater_equal(i, low)
                     && std::cmp_less_equal(i, high);
            };
            static constexpr auto tile_size = 16U;
            if (!skip_filters && fail_filter(tile))
            {
              return false;
            }
            if (m_draw_swizzle)
            {
              if (std::cmp_equal(tile_pos.x, tile.source_x() / tile_size))
              {
                if (std::cmp_equal(tile_pos.y, tile.source_y() / tile_size))
                {
                  if (std::cmp_equal(tile.texture_id(), texture_page))
                  {
                    return true;
                  }
                }
              }
            }
            else if (in_bounds(pixel_pos.x, tile.x(), tile.x() + tile_size))
            {
              if (in_bounds(pixel_pos.y, tile.y(), tile.y() + tile_size))
              {
                return true;
              }
            }
            return false;
          });
      std::transform(std::begin(filtered_tiles),
        std::end(filtered_tiles),
        std::back_inserter(out),
        [&tiles](const auto &tile)
        {
          const auto *const start = tiles.data();
          const auto *const curr  = &tile;
          std::cout << tile << std::endl;
          return static_cast<std::size_t>(std::distance(start, curr));
        });
      fmt::print("\n\tFound {:3}\n", out.size());
      for (const auto &i : out)
      {
        fmt::print("{:4} ", i);
      }
      puts("\n");
      return out;
    });
}

auto
  map_sprite::duel_visitor(auto &&lambda) const
{
  return m_map_const.visit_tiles(
    [this, &lambda](auto const &tiles_const)
    {
      return m_map.visit_tiles([&lambda, &tiles_const](auto &&tiles)
        { return lambda(tiles_const, std::forward<decltype(tiles)>(tiles)); });
    });
}

void
  map_sprite::for_all_tiles(auto const &tiles_const,
    auto                              &&tiles,
    auto                              &&lambda) const
{
  assert(std::size(tiles_const) == std::size(tiles));
  auto       tc  = std::crbegin(tiles_const);
  const auto tce = std::crend(tiles_const);
  auto       t   = std::rbegin(tiles);
  // const auto te  = std::rend(tiles);
  for (; /*t != te &&*/ tc != tce; (void)++tc, ++t)
  {
    const is_tile auto &tile_const = *tc;
    is_tile auto       &tile       = *t;
    lambda(tile_const, tile);
  }
}

void
  map_sprite::for_all_tiles(auto &&lambda) const
{
  duel_visitor(
    [&lambda, this](auto const &tiles_const, auto &&tiles)
    {
      for_all_tiles(tiles_const,
        std::forward<decltype(tiles)>(tiles),
        std::forward<decltype(lambda)>(lambda));
    });
}

void
  map_sprite::local_draw(sf::RenderTarget &target,
    sf::RenderStates                       states) const
{
  wait_for_futures();
  target.clear(sf::Color::Transparent);
  for (const auto &z : m_all_unique_values_and_strings.z().values())
  {
    for_all_tiles(
      [this, &states, &target, &z](
        [[maybe_unused]] const auto &tile_const, const auto &tile)
      {
        if (tile.z() != z)
        {
          return;
        }
        if (fail_filter(tile))
        {
          return;
        }
        states.texture =
          get_texture(tile.depth(), tile.palette_id(), tile.texture_id());
        const auto raw_texture_size = states.texture->getSize();
        const auto i                = raw_texture_size.y / 16U;
        const auto texture_size     = sf::Vector2u{ i, i };
        const auto draw_size = sf::Vector2u{ 16U * m_scale, 16U * m_scale };
        auto       quad =
          get_triangle_strip(draw_size, texture_size, tile_const, tile);
        states.blendMode = sf::BlendAlpha;
        if (tile.blend_mode() == BlendModeT::add)
        {
          states.blendMode = sf::BlendAdd;
        }
        else if (tile.blend_mode() == BlendModeT::half_add)
        {
          states.blendMode = sf::BlendAdd;
          constexpr static std::uint8_t per50 =
            (std::numeric_limits<std::uint8_t>::max)() / 2U;
          set_color(quad, { per50, per50, per50, per50 });// 50% alpha
        }
        else if (tile.blend_mode() == BlendModeT::quarter_add)
        {
          states.blendMode = sf::BlendAdd;
          constexpr static std::uint8_t per25 =
            (std::numeric_limits<std::uint8_t>::max)() / 4U;
          set_color(quad, { per25, per25, per25, per25 });// 25% alpha
        }
        else if (tile.blend_mode() == BlendModeT::subtract)
        {
          states.blendMode = GetBlendSubtract();
          // states.blendMode = sf::BlendMultiply;
        }
        // apply the tileset texture

        // std::lock_guard<std::mutex> lock(mutex_texture);

        // draw the vertex array
        target.draw(quad.data(), quad.size(), sf::TriangleStrip, states);
      });
  }
}

const sf::BlendMode &
  map_sprite::GetBlendSubtract()
{
  const static auto BlendSubtract =
    sf::BlendMode{ sf::BlendMode::DstColor,// or One
      sf::BlendMode::One,
      sf::BlendMode::ReverseSubtract,
      sf::BlendMode::One,
      sf::BlendMode::OneMinusSrcAlpha,
      sf::BlendMode::Add };
  return BlendSubtract;
}

map_sprite
  map_sprite::with_coo(const open_viii::LangT coo) const
{
  return { m_field, coo, m_draw_swizzle, m_filters };
}

map_sprite
  map_sprite::with_field(
    std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const
{
  return { std::move(field), m_coo, m_draw_swizzle, m_filters };
}

void
  map_sprite::enable_draw_swizzle() const
{
  m_draw_swizzle = true;
  init_render_texture();
  m_grid = get_grid();
}

void
  map_sprite::disable_draw_swizzle() const
{
  m_draw_swizzle = false;
  init_render_texture();
  m_grid = get_grid();
}

void
  map_sprite::draw(sf::RenderTarget &target, sf::RenderStates states) const
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
  // draw texture_page_grid
  if (m_draw_swizzle)
  {
    target.draw(m_texture_page_grid, states);
  }
}
void
  map_sprite::update_render_texture(bool reload_textures) const
{
  if (reload_textures)
  {
    m_texture = get_textures();


    resize_render_texture();
  }
  if (!fail())
  {
    local_draw(*m_render_texture, sf::RenderStates::Default);
    m_render_texture->display();
  }
}
void
  map_sprite::save(const std::filesystem::path &path) const
{
  if (fail())
  {
    return;
  }
  const auto image = m_render_texture->getTexture().copyToImage();
  if (!image.saveToFile(path.string()))
  {
    std::cerr << "failure to save file:" << path << std::endl;
  }
}
bool
  map_sprite::fail() const
{
  static bool once = true;
  using namespace open_viii::graphics::literals;
  if (!m_render_texture)
  {
    if (once)
    {
      std::cout << "m_render_texture is null" << std::endl;
      once = false;
    }
    return true;
  }
  if (!m_texture)
  {
    if (once)
    {
      std::cout << "m_texture is null" << std::endl;
      once = false;
    }
    return true;
  }
  if (m_mim.get_width(4_bpp, false) == 0)
  {
    if (once)
    {
      std::cout << "m_mim width is 0" << std::endl;
      once = false;
    }
    return true;
  }
  if (empty())
  {
    if (once)
    {
      std::cout << "m_map is empty" << std::endl;
      once = false;
    }
    return true;
  }
  once = true;
  return false;
}
void
  map_sprite::map_save(const std::filesystem::path &dest_path) const
{
  const auto map  = get_map(nullptr, false, false);
  const auto path = dest_path.string();

  open_viii::tools::write_buffer(
    [&map](std::ostream &os)
    {
      map.visit_tiles(
        [&os](auto &&tiles)
        {
          for (const auto &tile : tiles)
          {
            //          std::array<char, sizeof(tile)> data{};
            //          std::memcpy(data.data(), &tile, sizeof(tile));
            const auto data =
              std::bit_cast<std::array<char, sizeof(tile)>>(tile);
            os.write(data.data(), data.size());
          }
        });
    },
    path,
    "");
}
std::string
  map_sprite::map_filename()
{
  return std::filesystem::path(m_map_path).filename().string();
}

const map_sprite &
  map_sprite::toggle_grid(bool enable, bool enable_texture_page_grid) const
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
    // std::cout << "enabled: " << m_texture_page_grid.count() << '\n';
    m_texture_page_grid.enable();
  }
  else
  {
    m_texture_page_grid.disable();
  }
  return *this;
}
void
  map_sprite::resize_render_texture() const
{
  if (!fail())
  {
    auto filtered_textures = *m_texture
                             | std::views::filter(
                               [](const auto &texture)
                               {
                                 const auto &size = texture.getSize();
                                 return size.x != 0 && size.y != 0;
                               });
    if (filtered_textures.begin() != filtered_textures.end())
    {
      const auto y = filtered_textures.begin()->getSize().y;
      m_scale      = (y - (y % 256U)) / 256U;
      fmt::print("{}\t", m_scale);
      m_render_texture->create(width() * m_scale, height() * m_scale);
    }
    else
    {
      m_scale = 1;
      m_render_texture->create(width(), height());
    }
  }
}
void
  map_sprite::init_render_texture() const
{
  if (!m_render_texture)
  {
    m_render_texture = std::make_shared<sf::RenderTexture>();
  }
  resize_render_texture();
  update_render_texture();
}


open_viii::graphics::Rectangle<std::uint32_t>
  map_sprite::get_canvas() const
{
  return static_cast<open_viii::graphics::Rectangle<std::uint32_t>>(
    m_map.canvas());
}

std::uint32_t
  map_sprite::width() const
{
  if (m_draw_swizzle)
  {
    using namespace open_viii::graphics::literals;
    return m_mim.get_width(4_bpp);
  }
  return m_canvas.width();
}

std::uint32_t
  map_sprite::height() const
{
  if (m_draw_swizzle)
  {
    return m_mim.get_height();
  }
  return m_canvas.height();
}
grid
  map_sprite::get_grid() const
{
  return { { 16U, 16U }, { width(), height() } };
}
grid
  map_sprite::get_texture_page_grid() const
{
  using namespace open_viii::graphics::literals;
  return { { (1U << static_cast<unsigned int>((8 - ((4_bpp).raw() & 3U)))),
             256U },
    { m_mim.get_width(4_bpp), m_mim.get_height() },
    sf::Color::Yellow };
}

bool
  map_sprite::fail_filter(auto &tile) const
{
  using namespace open_viii::graphics::literals;
#if 1
  const auto test = []<typename T>(
                      const ::filter<T> &in_filter, const auto &value) -> bool
  { return (in_filter && value != in_filter); };
  return (m_filters.bpp.value() != 16_bpp
           && test(m_filters.palette, tile.palette_id()))
         || test(m_filters.bpp, tile.depth())
         || test(m_filters.blend_mode, tile.blend_mode())
         || test(m_filters.blend_other, tile.blend())
         || test(m_filters.animation_id, tile.animation_id())
         || test(m_filters.animation_frame, tile.animation_state())
         || test(m_filters.layer_id, tile.layer_id())
         || test(m_filters.texture_page_id, tile.texture_id())
         || test(m_filters.z, tile.z());
#else
  if (m_filters.palette.enabled() && m_filters.bpp.value() != 16_bpp)
  {
    if (tile.palette_id() != m_filters.palette.value())
    {
      return true;
    }
  }
  if (m_filters.bpp.enabled())
  {
    if (tile.depth() != m_filters.bpp.value())
    {
      return true;
    }
  }
  if (m_filters.blend_mode.enabled())
  {
    if (tile.blend_mode() != m_filters.blend_mode.value())
    {
      return true;
    }
  }
  if (m_filters.blend_other.enabled())
  {
    if (tile.blend() != m_filters.blend_other.value())
    {
      return true;
    }
  }
  if (m_filters.animation_id.enabled())
  {
    if (tile.animation_id() != m_filters.animation_id.value())
    {
      return true;
    }
  }
  if (m_filters.animation_frame.enabled())
  {
    if (tile.animation_state() != m_filters.animation_frame.value())
    {
      return true;
    }
  }
  if (m_filters.layer_id.enabled())
  {
    if (tile.layer_id() != m_filters.layer_id.value())
    {
      return true;
    }
  }
  if (m_filters.texture_page_id.enabled())
  {
    if (tile.texture_id() != m_filters.texture_page_id.value())
    {
      return true;
    }
  }
  return false;
#endif
}

all_unique_values_and_strings
  map_sprite::get_all_unique_values_and_strings()
{
  return m_map.visit_tiles(
    [](const auto &tiles) { return all_unique_values_and_strings(tiles); });
}

filters &
  map_sprite::filter() const
{
  return m_filters;
}
map_sprite
  map_sprite::update(std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
    open_viii::LangT                                                    coo,
    bool draw_swizzle) const
{
  return { std::move(field), coo, draw_swizzle, m_filters };
}
const all_unique_values_and_strings &
  map_sprite::uniques() const
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
