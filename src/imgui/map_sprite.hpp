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
#include <fmt/format.h>
#include <future>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <utility>

struct map_sprite
  : public sf::Drawable
  , public sf::Transformable
{


public:
  map_sprite() = default;
  map_sprite(std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
    open_viii::LangT                                            coo,
    bool                                                        draw_swizzle,
    filters                                                     in_filters)
    : m_draw_swizzle(draw_swizzle), m_filters(in_filters),
      m_field(std::move(field)), m_coo(coo), m_mim(get_mim()),
      m_map(get_map(&m_map_path)), m_canvas(get_canvas()),
      m_unique_layers(get_unique_layers()),
      m_unique_texture_pages(get_unique_texture_pages()),
      m_unique_animation_ids(get_unique_animation_ids()),
      m_unique_animation_frames(get_unique_animation_frames()),
      m_blend_modes(get_blend_modes()), m_unique_z_axis(get_unique_z_axis()),
      m_unique_layers_str(get_strings(m_unique_layers)),
      m_unique_texture_pages_str(get_strings(m_unique_texture_pages)),
      m_unique_animation_ids_str(get_strings(m_unique_animation_ids)),
      m_unique_animation_frames_str(get_strings(m_unique_animation_frames)),
      m_blend_modes_str(get_strings(m_blend_modes)),
      m_bpp_and_palette(get_bpp_and_palette()), m_texture(get_textures()),
      m_grid(get_grid()), m_texture_page_grid(get_texture_page_grid()),
      m_render_texture(std::make_shared<sf::RenderTexture>())
  {
    init_render_texture();
  }
  template<typename T>
  static std::vector<std::string> get_strings(const std::vector<T> &data);
  template<typename T, typename U>
  static std::map<T, std::vector<std::string>> get_strings(
    const std::map<T, std::vector<U>> &data);
  static std::vector<const char *> get_c_str(
    const std::vector<std::string> &data)
  {
    std::vector<const char *> vector;
    vector.reserve(std::size(data));
    std::ranges::transform(data,
      std::back_inserter(vector),
      [](const std::string &t) { return t.c_str(); });
    return vector;
  }
  template<typename T>
  static std::map<T, std::vector<const char *>> get_c_str(
    const std::map<T, std::vector<std::string>> &data)
  {
    std::map<T, std::vector<const char *>> map = {};
    for (const auto &[key, vector] : data) {
      map.emplace(key, get_c_str(vector));
    }
    return map;
  }
  map_sprite update(std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
    open_viii::LangT                                                   coo,
    bool draw_swizzle) const

  {
    return { std::move(field), coo, draw_swizzle, m_filters };
  }

  void          enable_draw_swizzle() const;
  void          disable_draw_swizzle() const;
  std::string   map_filename();
  bool          fail() const;
  std::uint32_t width() const;
  std::uint32_t height() const;
  void          save(const std::filesystem::path &path) const;
  void          map_save(const std::filesystem::path &dest_path) const;
  map_sprite    with_coo(open_viii::LangT coo) const;
  map_sprite    with_field(
       std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const;
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
  const map_sprite &toggle_grid(bool enable,
    bool                             enable_texture_page_grid) const;
  bool              empty() const;
  auto             &filter() const { return m_filters; }
  const auto       &blend_modes() const { return m_blend_modes; }
  const auto       &blend_modes_str() const { return m_blend_modes_str; }
  const auto       &layer_ids() const { return m_unique_layers; }
  const auto       &layer_ids_str() const { return m_unique_layers_str; }
  const auto       &texture_pages() const { return m_unique_texture_pages; }
  const auto &texture_pages_str() const { return m_unique_texture_pages_str; }

  const auto &animation_ids() const { return m_unique_animation_ids; }
  const auto &animation_ids_str() const { return m_unique_animation_ids_str; }

  const auto &animation_frames() const
  {
    const auto value = m_filters.animation_id.value();
    if (m_unique_animation_frames.contains(value)) {
      return m_unique_animation_frames.at(value);
    }
    static std::vector<std::uint8_t> empty{};
    return empty;
  }
  const auto &animation_frames_str() const
  {
    const auto value = m_filters.animation_id.value();
    if (m_unique_animation_frames_str.contains(value)) {
      return m_unique_animation_frames_str.at(value);
    }
    static std::vector<std::string> empty{};
    return empty;
  }
  void update_render_texture() const;

private:
  mutable std::vector<std::future<void>>             m_futures      = {};
  mutable bool                                       m_draw_swizzle = { false };
  mutable filters                                    m_filters      = {};
  std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field        = {};
  open_viii::LangT                                   m_coo          = {};
  open_viii::graphics::background::Mim               m_mim          = {};
  mutable std::string                                m_map_path     = {};
  open_viii::graphics::background::Map               m_map          = {};
  open_viii::graphics::Rectangle<std::uint32_t>      m_canvas       = {};
  std::vector<std::uint8_t>                          m_unique_layers = {};
  std::vector<std::uint8_t> m_unique_texture_pages                   = {};
  std::vector<std::uint8_t> m_unique_animation_ids                   = {};
  std::map<std::uint8_t, std::vector<std::uint8_t>>
    m_unique_animation_frames                                              = {};
  std::vector<open_viii::graphics::background::BlendModeT> m_blend_modes   = {};
  std::vector<std::uint16_t>                               m_unique_z_axis = {};

  std::vector<std::string> m_unique_layers_str                             = {};
  std::vector<std::string> m_unique_texture_pages_str                      = {};
  std::vector<std::string> m_unique_animation_ids_str                      = {};
  std::map<std::uint8_t, std::vector<std::string>>
                                         m_unique_animation_frames_str = {};
  std::vector<std::string>               m_blend_modes_str             = {};
  std::vector<std::string>               m_unique_z_axis_str           = {};

  std::vector<open_viii::graphics::BPPT> m_bpps                        = {};
  std::map<open_viii::graphics::BPPT, std::vector<std::uint8_t>>
                           m_palettes = {};
  std::vector<std::string> m_bpps_str = {};
  std::map<open_viii::graphics::BPPT, std::vector<std::string>>
    m_palettes_str = {};
  std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
                               m_bpp_and_palette = {};
  static constexpr std::size_t MAX_TEXTURES =
    16 * 13;// 13*16 for texture page / palette combos. 16*2+1 for palette bpp
            // combos.
  mutable std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> m_texture = {};
  mutable std::shared_ptr<sf::RenderTexture> m_render_texture              = {};
  mutable grid                               m_grid                        = {};
  grid                                       m_texture_page_grid           = {};

  grid                                       get_grid() const;
  grid                                       get_texture_page_grid() const;


  using color_type  = open_viii::graphics::Color32RGBA;
  using colors_type = std::vector<color_type>;
  [[nodiscard]] open_viii::graphics::background::Mim get_mim() const;
  open_viii::graphics::background::Map get_map(std::string *out_path = nullptr,
    bool                                                    sort_remove = true,
    bool                                                    shift = true) const;
  [[nodiscard]] colors_type            get_colors(open_viii::graphics::BPPT bpp,
               std::uint8_t                                                 palette,
               bool draw_palette = false) const;
  [[nodiscard]] static constexpr std::size_t
    get_texture_pos(open_viii::graphics::BPPT bpp, std::uint8_t palette);
  [[nodiscard]] const sf::Texture *get_texture(open_viii::graphics::BPPT bpp,
    std::uint8_t palette) const;
  [[nodiscard]] std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>>
                                           get_textures() const;
  [[nodiscard]] std::vector<std::uint16_t> get_unique_z_axis() const;
  [[nodiscard]] std::vector<std::uint8_t>  get_unique_layers() const;
  [[nodiscard]] std::vector<open_viii::graphics::background::BlendModeT>
    get_blend_modes() const;
  [[nodiscard]] std::vector<std::pair<open_viii::graphics::BPPT, std::uint8_t>>
    get_bpp_and_palette() const;

  [[nodiscard]] open_viii::graphics::Rectangle<std::uint32_t>
                                           get_canvas() const;
  void                                     resize_render_texture() const;
  void                                     init_render_texture() const;

  [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
    const sf::Vector2u &new_tileSize,
    const sf::Vector2u &raw_tileSize,
    auto                source_x,
    auto                source_y,
    auto                x,
    auto                y) const;

  [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
    const sf::Vector2u &new_tileSize,
    const sf::Vector2u &raw_tileSize,
    auto              &&tile) const;


  static const sf::BlendMode &GetBlendSubtract();
  void local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
  bool fail_filter(auto &tile) const;
  std::vector<std::uint8_t> get_unique_animation_ids() const;
  static constexpr auto default_filter_lambda = [](auto &&) { return true; };
  template<typename T,
    typename lambdaT,
    typename sortT   = std::less<>,
    typename filterT = decltype(default_filter_lambda)>
  std::vector<T>            get_unique_from_tiles(lambdaT &&lambda,
               sortT                                      &&sort = {},
               filterT                                                   &&= {}) const;
  std::vector<std::uint8_t> get_unique_texture_pages() const;
  std::map<std::uint8_t, std::vector<std::uint8_t>>
       get_unique_animation_frames() const;
  void wait_for_futures() const;
};
#endif// MYPROJECT_MAP_SPRITE_HPP
