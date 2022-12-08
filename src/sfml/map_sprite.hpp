//
// Created by pcvii on 9/7/2021.
//

#ifndef FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#define FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#include "filter.hpp"
#include "grid.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "unique_values.hpp"
#include "upscales.hpp"
#include <cppcoro/generator.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <future>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <utility>

static inline std::string str_to_lower(std::string &&input)
{
  std::string output{};
  output.reserve(std::size(input) + 1);
  std::ranges::transform(input, std::back_inserter(output), [](char c) -> char {
    return static_cast<char>(::tolower(c));
  });
  return output;
}
struct map_sprite final
  : public sf::Drawable
  , public sf::Transformable
{

  auto const_visit_tiles(auto &&p_function) const
  {
    return m_maps.const_back().visit_tiles(
      std::forward<decltype(p_function)>(p_function));
  }
  [[nodiscard]] const sf::Texture *get_texture(
    open_viii::graphics::BPPT bpp,
    std::uint8_t              palette,
    std::uint8_t              texture_page) const;
  [[nodiscard]] const sf::Texture *get_texture(const ::PupuID &pupu) const;
  [[nodiscard]] const sf::Texture *
    get_texture(const open_viii::graphics::background::is_tile auto &tile) const
  {
    return get_texture(tile.depth(), tile.palette_id(), tile.texture_id());
  }

  [[nodiscard]] sf::Vector2u
               get_tile_texture_size(const sf::Texture *texture) const;

  sf::Vector2u get_tile_draw_size() const;

  template<open_viii::graphics::background::is_tile this_type, typename T>
  static void format_tile_text(const this_type &tile, T &&format_function)
  {
    const auto raw_hex = to_hex(tile);
    format_function("Hex", std::string_view(raw_hex.data(), raw_hex.size()));
    format_function(
      "Source",
      fmt::format(
        "({}, {}) ({}, {})",
        tile.source_rectangle().x(),
        tile.source_rectangle().y(),
        tile.source_rectangle().width(),
        tile.source_rectangle().height()));
    format_function(
      "Output",
      fmt::format(
        "({}, {}) ({}, {})",
        tile.output_rectangle().x(),
        tile.output_rectangle().y(),
        tile.output_rectangle().width(),
        tile.output_rectangle().height()));
    format_function("Z", tile.z());
    format_function("Depth", static_cast<int>(tile.depth()));
    format_function("Palette ID", tile.palette_id());
    format_function("Texture ID", tile.texture_id());
    format_function("Layer ID", tile.layer_id());
    format_function(
      "Blend Mode", static_cast<std::uint16_t>(tile.blend_mode()));
    format_function("Blend Other", tile.blend());
    format_function("Animation ID", tile.animation_id());
    format_function("Animation State", tile.animation_state());
    format_function("Draw", tile.draw());
  }

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
  void undo()
  {
    (void)m_maps.undo();
    update_render_texture();
  }
  [[nodiscard]] const all_unique_values_and_strings &uniques() const;

  map_sprite                                         update(
                                            std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
                                            open_viii::LangT                                   coo,
                                            bool                                               draw_swizzle) const;

  void enable_draw_swizzle() const;
  void disable_draw_swizzle() const;
  void enable_disable_blends() const
  {
    m_disable_blends = true;
    init_render_texture();
  }
  void disable_disable_blends() const
  {
    m_disable_blends = false;
    init_render_texture();
  }

  std::string   map_filename() const;
  bool          fail() const;
  std::uint32_t width() const;
  std::uint32_t height() const;
  void          save(const std::filesystem::path &path) const;
  void          map_save(const std::filesystem::path &dest_path) const;
  void          test_map(const std::filesystem::path &saved_path) const;
  map_sprite    with_coo(open_viii::LangT coo) const;
  map_sprite
    with_field(std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const;
  map_sprite with_filters(::filters filters) const;
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
  const map_sprite        &
    toggle_grid(bool enable, bool enable_texture_page_grid) const;
  bool     empty() const;
  filters &filter() const;
  void     update_render_texture(bool reload_textures = false) const;
  void     update_position(
        const sf::Vector2i &pixel_pos,
        const sf::Vector2i &tile_pos,
        const std::uint8_t &texture_page);
  std::vector<std::size_t> find_intersecting(
    const sf::Vector2i &pixel_pos,
    const sf::Vector2i &tile_pos,
    const std::uint8_t &texture_page,
    bool                skip_filters = false);
  std::size_t row_empties(
    std::uint8_t tile_y,
    std::uint8_t texture_page,
    bool         move_from_row = false);
  sf::Sprite save_intersecting(
    const sf::Vector2i &pixel_pos,
    const sf::Vector2i &tile_pos,
    const std::uint8_t &texture_page);
  std::uint8_t max_x_for_saved() const;
  void         compact() const;
  void         compact2() const;
  void         flatten_bpp() const;
  void         flatten_palette() const;
  void         save_new_textures(const std::filesystem::path &path) const;
  cppcoro::generator<bool>
       gen_new_textures(const std::filesystem::path path) const;
  void save_pupu_textures(const std::filesystem::path &path) const;
  cppcoro::generator<bool>
              gen_pupu_textures(const std::filesystem::path path) const;
  void        save_modified_map(const std::filesystem::path &path) const;
  void        load_map(const std::filesystem::path &dest_path) const;
  std::string get_base_name() const;

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
    open_viii::graphics::background::Map &copy_back() const
    {
      m_history.push_back(true);
      return m_maps.emplace_back(m_maps.back());
    }
    const open_viii::graphics::background::Map &copy_back_to_front() const
    {
      m_maps.insert(m_maps.begin(), m_maps.back());
      m_history.push_back(false);
      return front();
    }
    const open_viii::graphics::background::Map &copy_front() const
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
    [[nodiscard]] open_viii::graphics::background::Map &back() const noexcept
    {
      return m_maps.back();
    }
    [[nodiscard]] const open_viii::graphics::background::Map &
      const_back() const noexcept
    {
      return m_maps.back();
    }
    bool undo() const
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
    open_viii::graphics::background::Map &pop_back() const
    {
      if (m_maps.size() > 2U)
      {
        m_maps.pop_back();
      }
      return m_maps.back();
    }
    const open_viii::graphics::background::Map &pop_front() const
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

  mutable bool    m_draw_swizzle                                  = { false };
  mutable bool    m_disable_texture_page_shift                    = { false };
  mutable bool    m_disable_blends                                = { false };
  mutable filters m_filters                                       = {};
  std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field      = {};
  open_viii::LangT                                   m_coo        = {};
  ::upscales                                         m_upscales   = {};
  open_viii::graphics::background::Mim               m_mim        = {};
  mutable std::string                                m_map_path   = {};
  bool                                               m_using_coo  = {};
  maps                                               m_maps       = {};
  all_unique_values_and_strings m_all_unique_values_and_strings   = {};
  open_viii::graphics::Rectangle<std::uint32_t> m_canvas          = {};
  static constexpr std::uint8_t                 TILE_SIZE         = 16U;
  static constexpr std::uint8_t                 MAX_TEXTURE_PAGES = 14U;
  static constexpr std::uint8_t                 MAX_PALETTES      = 16U;
  static constexpr std::uint8_t                 BPP_COMBOS        = 2U;
  static constexpr std::uint16_t                START_OF_NO_PALETTE_INDEX =
    MAX_PALETTES * MAX_TEXTURE_PAGES;
  static constexpr std::uint16_t BPP16_INDEX  = MAX_PALETTES * BPP_COMBOS + 1;
  static constexpr auto          MAX_TEXTURES = (std::max)(
    static_cast<std::uint16_t>(START_OF_NO_PALETTE_INDEX + MAX_TEXTURE_PAGES),
    static_cast<std::uint16_t>(BPP16_INDEX + 1U));
  // todo ecenter3 shows different images for remaster and 2013. Fix?

  mutable std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> m_texture = {};
  mutable std::shared_ptr<sf::RenderTexture> m_render_texture              = {};
  mutable grid                               m_grid                        = {};
  grid                                       m_texture_page_grid           = {};
  std::vector<std::size_t>                   m_saved_indicies              = {};
  mutable std::uint32_t                      m_scale      = { 1 };
  grid                                       get_grid() const;
  grid                                       get_texture_page_grid() const;
  all_unique_values_and_strings get_all_unique_values_and_strings() const;
  [[nodiscard]] open_viii::graphics::background::Mim get_mim() const;
  open_viii::graphics::background::Map
    get_map(std::string *out_path, bool shift, bool &coo) const;
  [[nodiscard]] colors_type
    get_colors(open_viii::graphics::BPPT bpp, std::uint8_t palette) const;
  [[nodiscard]] std::size_t get_texture_pos(
    open_viii::graphics::BPPT bpp,
    std::uint8_t              palette,
    std::uint8_t              texture_page) const;
  [[nodiscard]] std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>>
    get_textures() const;
  [[nodiscard]] open_viii::graphics::Rectangle<std::uint32_t>
                                           get_canvas() const;
  void                                     resize_render_texture() const;
  void                                     init_render_texture() const;

  [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
    const sf::Vector2u &draw_size,
    const sf::Vector2u &texture_size,
    std::integral auto  source_x,
    std::integral auto  source_y,
    std::integral auto  x,
    std::integral auto  y) const;

  [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
    const sf::Vector2u                                  &draw_size,
    const sf::Vector2u                                  &texture_size,
    const open_viii::graphics::background::is_tile auto &tile_const,
    open_viii::graphics::background::is_tile auto      &&tile) const;

  static const sf::BlendMode &GetBlendSubtract();
  [[nodiscard]] bool
       local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
  bool fail_filter(auto &tile) const;
  void wait_for_futures() const;
  auto duel_visitor(auto &&lambda) const;
  void for_all_tiles(
    auto const &tiles_const,
    auto      &&tiles,
    auto      &&lambda,
    bool        skip_invalid,
    bool        regular_order) const;
  void for_all_tiles(
    auto &&lambda,
    bool   skip_invalid  = true,
    bool   regular_order = false) const;
  void find_upscale_path(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
    uint8_t                                                 palette) const;
  void find_upscale_path(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret) const;
  void find_deswizzle_path(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret) const;
  void load_mim_textures(
    std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret,
    open_viii::graphics::BPPT                               bpp,
    uint8_t                                                 palette) const;
  template<typename F, typename... T>
  void spawn_thread(F &&f, T &&...t) const;
  template<typename key_lambdaT, typename weight_lambdaT>
  [[maybe_unused]] void compact_generic(
    key_lambdaT    &&key_lambda,
    weight_lambdaT &&weight_lambda,
    int              passes = 2) const;

  std::shared_ptr<sf::RenderTexture>
           save_texture(std::uint32_t width, std::uint32_t height) const;
  uint32_t get_max_texture_height() const;
  void     async_save(
        const std::filesystem::path              &out_path,
        const std::shared_ptr<sf::RenderTexture> &out_texture) const;
  template<
    typename tilesT,
    typename key_lambdaT,
    typename value_lambdaT,
    typename filterT = decltype(default_filter_lambda)>
  auto generate_map(
    tilesT        &&tiles,
    key_lambdaT   &&key_lambda,
    value_lambdaT &&value_lambda,
    filterT       &&filter = {}) const
  {
    using tileT =
      std::remove_cvref_t<typename std::remove_cvref_t<tilesT>::value_type>;
    using keyT   = decltype(key_lambda(tileT{}));
    using valueT = decltype(value_lambda(tileT{}));
    std::map<keyT, std::vector<valueT>> r{};
    auto filtered_tiles = tiles | std::views::filter(filter);
    std::ranges::for_each(
      filtered_tiles, [&r, &key_lambda, &value_lambda](auto &&tile) {
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

  std::filesystem::path save_path_coo(
    fmt::format_string<std::string_view, std::string_view, uint8_t> pattern,
    const std::filesystem::path                                    &path,
    const std::string_view                                         &field_name,
    uint8_t texture_page) const;
  std::filesystem::path save_path_coo(
    fmt::format_string<std::string_view, std::string_view, uint8_t, uint8_t>
                                 pattern,
    const std::filesystem::path &path,
    const std::string_view      &field_name,
    uint8_t                      texture_page,
    uint8_t                      palette) const;
  std::filesystem::path save_path_coo(
    fmt::format_string<std::string_view, std::string_view, PupuID> pattern,
    const std::filesystem::path                                   &path,
    const std::string_view                                        &field_name,
    PupuID                                                         pupu) const;
  std::filesystem::path save_path(
    fmt::format_string<std::string_view, uint8_t> pattern,
    const std::filesystem::path                  &path,
    const std::string_view                       &field_name,
    uint8_t                                       texture_page) const;
  std::filesystem::path save_path(
    fmt::format_string<std::string_view, uint8_t, uint8_t> pattern,
    const std::filesystem::path                           &path,
    const std::string_view                                &field_name,
    uint8_t                                                texture_page,
    uint8_t                                                palette) const;
  std::filesystem::path save_path(
    fmt::format_string<std::string_view, PupuID> pattern,
    const std::filesystem::path                 &path,
    const std::string_view                      &field_name,
    PupuID                                       pupu) const;
  bool check_if_one_palette(const uint8_t &texture_page) const;
  std::vector<uint8_t>
             get_conflicting_palettes(const uint8_t &texture_page) const;
  ::upscales get_upscales()
  {
    if (m_field)
    {
      return { m_field->get_base_name(), m_coo };
    }
    return {};
  }
  template<open_viii::graphics::background::is_tile T>
  static auto to_hex(const T &test)
  {
    std::array<std::uint8_t, sizeof(T)>  raw;
    std::array<char, sizeof(T) * 2U + 1> raw_hex;
    raw_hex.back() = 0;
    std::memcpy(raw.data(), &test, sizeof(T));

    std::size_t rhi{};
    for (const std::uint8_t r : raw)
    {
      char right     = r % 16U < 10 ? r % 16U + '0' : r % 16U - 10 + 'A';
      char left      = r / 16U < 10 ? r / 16U + '0' : r / 16U - 10 + 'A';
      raw_hex[rhi++] = left;
      raw_hex[rhi++] = right;
    }
    return raw_hex;
  }
  size_t size_of_map() const;
  bool
    save_png_image(const sf::Image &image, const std::string &filename) const;
};
#endif// FIELD_MAP_EDITOR_MAP_SPRITE_HPP
