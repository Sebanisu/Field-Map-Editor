//
// Created by pcvii on 9/7/2021.
//

#ifndef MYPROJECT_MAP_SPRITE_HPP
#define MYPROJECT_MAP_SPRITE_HPP
#include "filter.hpp"
#include "grid.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "unique_values.hpp"
#include "upscales.hpp"
#include <cstdint>
#include <fmt/format.h>
#include <future>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <utility>

static inline std::string
  str_to_lower(std::string &&input)
{
  std::string output{};
  output.reserve(std::size(input) + 1);
  std::ranges::transform(
    input,
    std::back_inserter(output),
    [](char c) -> char { return static_cast<char>(::tolower(c)); });
  return output;
}
struct map_sprite final
  : public sf::Drawable
  , public sf::Transformable
{
public:
  using color_type  = open_viii::graphics::Color32RGBA;
  using colors_type = std::vector<color_type>;
  map_sprite()      = default;
  map_sprite(
    std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
    open_viii::LangT                                   coo,
    bool                                               draw_swizzle,
    filters                                            in_filters,
    bool                                               force_disable_blends)
    : m_draw_swizzle(draw_swizzle)
    , m_disable_blends(force_disable_blends)
    , m_filters(std::move(in_filters))
    , m_field(std::move(field))
    , m_coo(coo)
    , m_upscales(get_upscales())
    , m_mim(get_mim())
    , m_using_coo(false)
    , m_maps(get_map(&m_map_path, true, m_using_coo))
    , m_all_unique_values_and_strings(get_all_unique_values_and_strings())
    , m_canvas(get_canvas())
    , m_texture(get_textures())
    , m_render_texture(std::make_shared<sf::RenderTexture>())
    , m_grid(get_grid())
    , m_texture_page_grid(get_texture_page_grid())
  {
    init_render_texture();
  }
  void
    undo()
  {
    (void)m_maps.undo();
    update_render_texture();
  }
  [[nodiscard]] const all_unique_values_and_strings &
    uniques() const;

  map_sprite
    update(
      std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
      open_viii::LangT                                   coo,
      bool                                               draw_swizzle) const;

  void
    enable_draw_swizzle() const;
  void
    disable_draw_swizzle() const;
  void
    enable_disable_blends() const
  {
    m_disable_blends = true;
    init_render_texture();
  }
  void
    disable_disable_blends() const
  {
    m_disable_blends = false;
    init_render_texture();
  }

  std::string
    map_filename();
  bool
    fail() const;
  std::uint32_t
    width() const;
  std::uint32_t
    height() const;
  void
    save(const std::filesystem::path &path) const;
  void
    map_save(const std::filesystem::path &dest_path) const;
  void
    test_map(const std::filesystem::path &saved_path) const;
  map_sprite
    with_coo(open_viii::LangT coo) const;
  map_sprite
    with_field(std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const;
  map_sprite
    with_filters(::filters filters) const;
  void
    draw(sf::RenderTarget &target, sf::RenderStates states) const final;
  const map_sprite &
    toggle_grid(bool enable, bool enable_texture_page_grid) const;
  bool
    empty() const;
  filters &
    filter() const;
  void
    update_render_texture(bool reload_textures = false) const;
  void
    update_position(
      const sf::Vector2i &pixel_pos,
      const sf::Vector2i &tile_pos,
      const std::uint8_t &texture_page);
  std::vector<std::size_t>
    find_intersecting(
      const sf::Vector2i &pixel_pos,
      const sf::Vector2i &tile_pos,
      const std::uint8_t &texture_page,
      bool                skip_filters = false);
  std::size_t
    row_empties(
      std::uint8_t tile_y,
      std::uint8_t texture_page,
      bool         move_from_row = false);
  sf::Sprite
    save_intersecting(
      const sf::Vector2i &pixel_pos,
      const sf::Vector2i &tile_pos,
      const std::uint8_t &texture_page);
  std::uint8_t
    max_x_for_saved() const;
  void
    compact() const;
  void
    compact2() const;
  void
    flatten_bpp() const;
  void
    flatten_palette() const;
  void
    save_new_textures(const std::filesystem::path &path) const;
  void
    save_pupu_textures(const std::filesystem::path &path) const;
  void
    save_modified_map(const std::filesystem::path &path) const;
  void
    load_map(const std::filesystem::path &dest_path) const;
  std::string
    get_base_name() const;

private:
  static constexpr auto default_filter_lambda = [](auto &&) { return true; };
  static constexpr auto filter_invalid =
    open_viii::graphics::background::Map::filter_invalid();
  struct maps
  {
    maps() = default;
    explicit maps(open_viii::graphics::background::Map &&in_map)
      : m_maps{ std::move(in_map) }
    {
      copy_back();
    }
    open_viii::graphics::background::Map &
      copy_back() const
    {
      m_history.push_back(true);
      return m_maps.emplace_back(m_maps.back());
    }
    const open_viii::graphics::background::Map &
      copy_back_to_front() const
    {
      m_maps.insert(m_maps.begin(), m_maps.back());
      m_history.push_back(false);
      return front();
    }
    const open_viii::graphics::background::Map &
      copy_front() const
    {
      m_maps.insert(m_maps.begin(), m_maps.front());
      m_history.push_back(false);
      return m_maps.front();
    }
    [[nodiscard]] const open_viii::graphics::background::Map &
      front() const noexcept
    {
      return m_maps.front();
    }
    [[nodiscard]] open_viii::graphics::background::Map &
      mutable_front() const noexcept
    {
      return m_maps.front();
    }
    [[nodiscard]] open_viii::graphics::background::Map &
      back() const noexcept
    {
      return m_maps.back();
    }
    [[nodiscard]] const open_viii::graphics::background::Map &
      const_back() const noexcept
    {
      return m_maps.back();
    }
    bool
      undo() const
    {
      if (m_maps.size() <= 2U)
      {
        return false;
      }
      bool last = m_history.back();
      m_history.pop_back();
      if (last)
      {
        (void)pop_back();
        return true;
      }
      (void)pop_front();
      return true;
    }

  private:
    open_viii::graphics::background::Map &
      pop_back() const
    {
      if (m_maps.size() > 2U)
      {
        m_maps.pop_back();
      }
      return m_maps.back();
    }
    const open_viii::graphics::background::Map &
      pop_front() const
    {
      if (m_maps.size() > 2U)
      {
        m_maps.erase(m_maps.begin());
      }
      return m_maps.front();
    }
    mutable std::vector<open_viii::graphics::background::Map> m_maps{};
    mutable std::vector<bool>                                 m_history{};
  };
  mutable std::vector<std::future<void>> m_futures               = {};
  mutable bool                           m_draw_swizzle          = { false };
  mutable bool    m_disable_texture_page_shift                   = { false };
  mutable bool    m_disable_blends                               = { false };
  mutable filters m_filters                                      = {};
  std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field     = {};
  open_viii::LangT                                   m_coo       = {};
  ::upscales                                         m_upscales  = {};
  open_viii::graphics::background::Mim               m_mim       = {};
  mutable std::string                                m_map_path  = {};
  bool                                               m_using_coo = {};
  maps                                               m_maps      = {};
  all_unique_values_and_strings m_all_unique_values_and_strings  = {};
  open_viii::graphics::Rectangle<std::uint32_t> m_canvas         = {};
  static constexpr std::size_t                  MAX_TEXTURES =
    250U;// glgate1 had 238
         // 16 * 14;// 14*16 for texture page / palette combos. 16*2+1 for
         // palette
         // bpp combos.
  mutable std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> m_texture = {};
  mutable std::shared_ptr<sf::RenderTexture> m_render_texture              = {};
  mutable grid                               m_grid                        = {};
  grid                                       m_texture_page_grid           = {};
  std::vector<std::size_t>                   m_saved_indicies              = {};
  mutable std::uint32_t                      m_scale = { 1 };

  grid
    get_grid() const;
  grid
    get_texture_page_grid() const;
  all_unique_values_and_strings
    get_all_unique_values_and_strings() const;
  [[nodiscard]] open_viii::graphics::background::Mim
    get_mim() const;
  open_viii::graphics::background::Map
    get_map(std::string *out_path, bool shift, bool &coo) const;
  [[nodiscard]] colors_type
    get_colors(open_viii::graphics::BPPT bpp, std::uint8_t palette) const;
  [[nodiscard]] std::size_t
    get_texture_pos(
      open_viii::graphics::BPPT bpp,
      std::uint8_t              palette,
      std::uint8_t              texture_page) const;
  [[nodiscard]] const sf::Texture *
    get_texture(
      open_viii::graphics::BPPT bpp,
      std::uint8_t              palette,
      std::uint8_t              texture_page) const;
  [[nodiscard]] const sf::Texture *
    get_texture(const ::PupuID &pupu) const;
  [[nodiscard]] std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>>
    get_textures() const;
  [[nodiscard]] open_viii::graphics::Rectangle<std::uint32_t>
    get_canvas() const;
  void
    resize_render_texture() const;
  void
    init_render_texture() const;

  [[nodiscard]] std::array<sf::Vertex, 4U>
    get_triangle_strip(
      const sf::Vector2u &draw_size,
      const sf::Vector2u &texture_size,
      auto                source_x,
      auto                source_y,
      auto                x,
      auto                y) const;

  [[nodiscard]] std::array<sf::Vertex, 4U>
    get_triangle_strip(
      const sf::Vector2u &draw_size,
      const sf::Vector2u &texture_size,
      const auto         &tile_const,
      auto              &&tile) const;

  static const sf::BlendMode &
    GetBlendSubtract();
  [[nodiscard]] bool
    local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
  bool
    fail_filter(auto &tile) const;
  void
    wait_for_futures() const;
  auto
    duel_visitor(auto &&lambda) const;
  void
    for_all_tiles(
      auto const &tiles_const,
      auto      &&tiles,
      auto      &&lambda,
      bool        skip_invalid,
      bool        regular_order) const;
  void
    for_all_tiles(
      auto &&lambda,
      bool   skip_invalid  = true,
      bool   regular_order = false) const;
  void
    find_upscale_path(
      std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
      uint8_t                                                 palette) const;
  void
    find_upscale_path(
      std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret) const;
  void
    find_deswizzle_path(
      std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret) const;
  void
    load_mim_textures(
      std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
      open_viii::graphics::BPPT                               bpp,
      uint8_t                                                 palette) const;
  template<typename key_lambdaT, typename weight_lambdaT>
  [[maybe_unused]] void
    compact_generic(
      key_lambdaT    &&key_lambda,
      weight_lambdaT &&weight_lambda,
      int              passes = 2) const;

  std::shared_ptr<sf::RenderTexture>
    save_texture(std::uint32_t width, std::uint32_t height) const;
  uint32_t
    get_max_texture_height() const;
  void
    async_save(
      const std::filesystem::path              &out_path,
      const std::shared_ptr<sf::RenderTexture> &out_texture) const;
  template<
    typename tilesT,
    typename key_lambdaT,
    typename value_lambdaT,
    typename filterT = decltype(default_filter_lambda)>
  auto
    generate_map(
      tilesT        &&tiles,
      key_lambdaT   &&key_lambda,
      value_lambdaT &&value_lambda,
      filterT       &&filter = {}) const
  {
    using tileT  = std::decay_t<typename std::decay_t<tilesT>::value_type>;
    using keyT   = decltype(key_lambda(tileT{}));
    using valueT = decltype(value_lambda(tileT{}));
    std::map<keyT, std::vector<valueT>> r{};
    auto filtered_tiles = tiles | std::views::filter(filter);
    std::ranges::for_each(
      filtered_tiles,
      [&r, &key_lambda, &value_lambda](auto &&tile)
      {
        if (!filter_invalid(tile))
        {
          return;
        }
        valueT value = value_lambda(tile);
        keyT   key   = key_lambda(tile);
        if (r.contains(key))
        {
          r.at(key).push_back(value);
        }
        else
        {
          r.emplace(key, std::vector<valueT>{ value });
        }
      });
    return r;
  }

  std::filesystem::path
    save_path_coo(
      fmt::format_string<std::string_view, std::string_view, uint8_t> pattern,
      const std::filesystem::path                                    &path,
      const std::string_view &field_name,
      uint8_t                 texture_page) const;
  std::filesystem::path
    save_path_coo(
      fmt::format_string<std::string_view, std::string_view, uint8_t, uint8_t>
                                   pattern,
      const std::filesystem::path &path,
      const std::string_view      &field_name,
      uint8_t                      texture_page,
      uint8_t                      palette) const;
  std::filesystem::path
    save_path_coo(
      fmt::format_string<std::string_view, std::string_view, PupuID> pattern,
      const std::filesystem::path                                   &path,
      const std::string_view                                        &field_name,
      PupuID pupu) const;
  std::filesystem::path
    save_path(
      fmt::format_string<std::string_view, uint8_t> pattern,
      const std::filesystem::path                  &path,
      const std::string_view                       &field_name,
      uint8_t                                       texture_page) const;
  std::filesystem::path
    save_path(
      fmt::format_string<std::string_view, uint8_t, uint8_t> pattern,
      const std::filesystem::path                           &path,
      const std::string_view                                &field_name,
      uint8_t                                                texture_page,
      uint8_t                                                palette) const;
  std::filesystem::path
    save_path(
      fmt::format_string<std::string_view, PupuID> pattern,
      const std::filesystem::path                 &path,
      const std::string_view                      &field_name,
      PupuID                                       pupu) const;
  bool
    check_if_one_palette(const uint8_t &texture_page) const;
  std::vector<uint8_t>
    get_conflicting_palettes(const uint8_t &texture_page) const;
  ::upscales
    get_upscales()
  {
    if (m_field)
    {
      return { m_field->get_base_name(), m_coo };
    }
    return {};
  }
  size_t
    size_of_map() const;
};
#endif// MYPROJECT_MAP_SPRITE_HPP
