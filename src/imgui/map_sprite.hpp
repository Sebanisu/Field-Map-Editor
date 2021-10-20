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
struct map_sprite
  : public sf::Drawable
  , public sf::Transformable
{
public:
  using color_type  = open_viii::graphics::Color32RGBA;
  using colors_type = std::vector<color_type>;
  map_sprite()      = default;
  map_sprite(std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
    open_viii::LangT                                            coo,
    bool                                                        draw_swizzle,
    filters                                                     in_filters)
    : m_draw_swizzle(draw_swizzle)
    , m_filters(std::move(in_filters))
    , m_field(std::move(field))
    , m_coo(coo)
    , m_upscales(m_field->get_base_name(), m_coo)
    , m_mim(get_mim())
    , m_map_const(get_map(&m_map_path))
    , m_map(m_map_const)
    , m_all_unique_values_and_strings(get_all_unique_values_and_strings())
    , m_canvas(get_canvas())
    , m_texture(get_textures())
    , m_grid(get_grid())
    , m_texture_page_grid(get_texture_page_grid())
    , m_render_texture(std::make_shared<sf::RenderTexture>())
  {
    init_render_texture();
  }
  [[nodiscard]] const all_unique_values_and_strings &
    uniques() const;

  map_sprite
    update(std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
      open_viii::LangT                                        coo,
      bool draw_swizzle) const;

  void
    enable_draw_swizzle() const;
  void
    disable_draw_swizzle() const;
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
  map_sprite
    with_coo(open_viii::LangT coo) const;
  map_sprite
    with_field(std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const;
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
    update_position(const sf::Vector2i &pixel_pos,
      const sf::Vector2i               &tile_pos,
      const std::uint8_t               &texture_page);
  std::vector<std::size_t>
    find_intersecting(const sf::Vector2i &pixel_pos,
      const sf::Vector2i                 &tile_pos,
      const std::uint8_t                 &texture_page,
      bool                                skip_filters = false);
  std::size_t
    row_empties(std::uint8_t tile_y,
      std::uint8_t           texture_page,
      bool                   move_from_row = false);
  sf::Sprite
    save_intersecting(const sf::Vector2i &pixel_pos,
      const sf::Vector2i                 &tile_pos,
      const std::uint8_t                 &texture_page);
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

private:
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
  open_viii::graphics::background::Map               m_map_const = {};
  mutable open_viii::graphics::background::Map       m_map       = {};
  all_unique_values_and_strings m_all_unique_values_and_strings  = {};
  open_viii::graphics::Rectangle<std::uint32_t> m_canvas         = {};
  static constexpr std::size_t                  MAX_TEXTURES =
    16 * 14;// 14*16 for texture page / palette combos. 16*2+1 for palette
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
    get_map(std::string *out_path = nullptr, bool shift = true) const;
  [[nodiscard]] colors_type
    get_colors(open_viii::graphics::BPPT bpp, std::uint8_t palette) const;
  [[nodiscard]] std::size_t
    get_texture_pos(open_viii::graphics::BPPT bpp,
      std::uint8_t                            palette,
      std::uint8_t                            texture_page) const;
  [[nodiscard]] const sf::Texture *
    get_texture(open_viii::graphics::BPPT bpp,
      std::uint8_t                        palette,
      std::uint8_t                        texture_page) const;
  [[nodiscard]] std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>>
    get_textures() const;
  [[nodiscard]] open_viii::graphics::Rectangle<std::uint32_t>
    get_canvas() const;
  void
    resize_render_texture() const;
  void
    init_render_texture() const;

  [[nodiscard]] std::array<sf::Vertex, 4U>
    get_triangle_strip(const sf::Vector2u &draw_size,
      const sf::Vector2u                  &texture_size,
      auto                                 source_x,
      auto                                 source_y,
      auto                                 x,
      auto                                 y) const;

  [[nodiscard]] std::array<sf::Vertex, 4U>
    get_triangle_strip(const sf::Vector2u &draw_size,
      const sf::Vector2u                  &texture_size,
      const auto                          &tile_const,
      auto                               &&tile) const;

  static const sf::BlendMode &
    GetBlendSubtract();
  [[nodiscard]] bool
    local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
  bool
                        fail_filter(auto &tile) const;
  static constexpr auto default_filter_lambda = [](auto &&) { return true; };
  void
    wait_for_futures() const;
  auto
    duel_visitor(auto &&lambda) const;
  void
    for_all_tiles(auto const &tiles_const, auto &&tiles, auto &&lambda) const;
  void
    for_all_tiles(auto &&lambda) const;
  void
    find_upscale_path(
      std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
      uint8_t                                                 palette) const;
  void
    find_upscale_path(
      std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret) const;
  void
    load_mim_textures(
      std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
      open_viii::graphics::BPPT                               bpp,
      uint8_t                                                 palette) const;
  template<typename key_lambdaT, typename weight_lambdaT>
  [[maybe_unused]] void
    compact_generic(key_lambdaT &&key_lambda,
      weight_lambdaT            &&weight_lambda,
      int                         passes = 2) const;

  template<const char *pattern>
  void
    save_specific(const std::filesystem::path &path,
      const std::string                       &field_name,
      std::uint32_t                            width,
      std::uint32_t                            height,
      std::optional<std::uint8_t>              texture_page = std::nullopt,
      std::optional<open_viii::graphics::BPPT> bpp          = std::nullopt,
      std::optional<std::uint8_t>              palette      = std::nullopt,
      std::optional<PupuID>                    pupu = std::nullopt) const;
  uint32_t
    get_max_texture_height() const;
};
#endif// MYPROJECT_MAP_SPRITE_HPP
