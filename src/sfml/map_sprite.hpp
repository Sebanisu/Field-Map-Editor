//
// Created by pcvii on 9/7/2021.
//

#ifndef FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#define FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#include "filter.hpp"
#include "grid.hpp"
#include "map_group.hpp"
#include "map_operation.hpp"
#include "MapHistory.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "settings_backup.hpp"
#include "square.hpp"
#include "tile_sizes.hpp"
#include "unique_values.hpp"
#include "upscales.hpp"


#include <cstdint>
#include <fmt/format.h>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
// #include <stacktrace>
#include <utility>
struct map_sprite final
  : public sf::Drawable
  , public sf::Transformable
{
   public:
     static constexpr auto          filter_invalid            = open_viii::graphics::background::Map::filter_invalid();
     static constexpr std::uint8_t  TILE_SIZE                 = 16U;
     static constexpr std::uint8_t  MAX_TEXTURE_PAGES         = 14U;
     static constexpr std::uint8_t  MAX_PALETTES              = 16U;
     static constexpr std::uint8_t  BPP_COMBOS                = 2U;
     static constexpr std::uint16_t START_OF_NO_PALETTE_INDEX = MAX_PALETTES * MAX_TEXTURE_PAGES;
     static constexpr std::uint16_t BPP16_INDEX               = MAX_PALETTES * BPP_COMBOS + 1;
     static constexpr auto          MAX_TEXTURES =
       (std::max)(static_cast<std::uint16_t>(START_OF_NO_PALETTE_INDEX + MAX_TEXTURE_PAGES), static_cast<std::uint16_t>(BPP16_INDEX + 1U));

     using BPPT           = open_viii::graphics::BPPT;
     using SharedField    = std::shared_ptr<open_viii::archive::FIFLFS<false>>;
     using Map            = open_viii::graphics::background::Map;
     using Mim            = open_viii::graphics::background::Mim;
     using color_type     = open_viii::graphics::Color32RGBA;
     using colors_type    = std::vector<color_type>;
     using SharedTextures = std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>>;
     using BlendModeT     = open_viii::graphics::background::BlendModeT;
     using Rectangle      = open_viii::graphics::Rectangle<std::uint32_t>;
     using iRectangle     = open_viii::graphics::Rectangle<std::int32_t>;

   private:
     ff_8::map_group                               m_map_group = {};
     square                                        m_square    = { sf::Vector2u{}, sf::Vector2u{ TILE_SIZE, TILE_SIZE }, sf::Color::Red };
     bool                                          m_draw_swizzle                  = { false };
     bool                                          m_disable_texture_page_shift    = { false };
     bool                                          m_disable_blends                = { false };
     ff_8::filters                                 m_filters                       = {};
     ::upscales                                    m_upscales                      = {};
     bool                                          m_using_imported_texture        = {};
     const sf::Texture                            *m_imported_texture              = { nullptr };
     std::uint16_t                                 m_imported_tile_size            = {};
     Map                                           m_imported_tile_map             = {};
     Map                                           m_imported_tile_map_front       = {};
     all_unique_values_and_strings                 m_all_unique_values_and_strings = {};
     open_viii::graphics::Rectangle<std::uint32_t> m_canvas                        = {};
     SharedTextures                                m_texture                       = {};
     std::shared_ptr<sf::RenderTexture>            m_render_texture                = {};
     std::shared_ptr<sf::RenderTexture>            m_drag_sprite_texture           = {};
     grid                                          m_grid                          = {};
     grid                                          m_texture_page_grid             = {};
     std::vector<std::size_t>                      m_saved_indices                 = {};
     std::vector<std::size_t>                      m_saved_imported_indices        = {};
     std::uint32_t                                 m_scale                         = { 1 };

   public:
     map_sprite() = default;
     map_sprite(ff_8::map_group map_group, bool draw_swizzle, ff_8::filters in_filters, bool force_disable_blends);
     std::uint32_t                                      get_map_scale() const;
     [[nodiscard]] const sf::Texture                   *get_texture(BPPT bpp, std::uint8_t palette, std::uint8_t texture_page) const;
     [[nodiscard]] const sf::Texture                   *get_texture(const ::PupuID &pupu) const;
     [[nodiscard]] sf::Vector2u                         get_tile_texture_size(const sf::Texture *texture) const;
     sf::Vector2u                                       get_tile_draw_size() const;
     std::shared_ptr<sf::RenderTexture>                 save_texture(std::uint32_t width, std::uint32_t height) const;
     uint32_t                                           get_max_texture_height() const;
     [[nodiscard]] bool                                 local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
     std::string                                        get_base_name() const;
     void                                               save_modified_map(const std::filesystem::path &path) const;
     [[nodiscard]] const all_unique_values_and_strings &uniques() const;
     std::string                                        map_filename() const;
     bool                                               fail() const;
     std::uint32_t                                      width() const;
     std::uint32_t                                      height() const;
     void                                               save(const std::filesystem::path &path) const;
     void                                               map_save(const std::filesystem::path &dest_path) const;
     void                                               test_map(const std::filesystem::path &saved_path) const;
     map_sprite                                         with_coo(open_viii::LangT coo) const;
     map_sprite                                         with_field(SharedField field, open_viii::LangT coo) const;
     map_sprite                                         with_filters(ff_8::filters filters) const;
     const map_sprite                                  &toggle_grid(bool enable, bool enable_texture_page_grid) const;
     void                                               disable_square() const;
     bool                                               empty() const;
     const ff_8::filters                               &filter() const;
     std::uint8_t                                       max_x_for_saved() const;
     map_sprite                                         update(ff_8::map_group map_group, bool draw_swizzle) const;
     grid                                               get_grid() const;
     grid                                               get_texture_page_grid() const;
     all_unique_values_and_strings                      get_all_unique_values_and_strings() const;
     sf::Vector2u                                       get_tile_texture_size_for_import() const;
     [[nodiscard]] colors_type                          get_colors(BPPT bpp, std::uint8_t palette) const;
     [[nodiscard]] Rectangle                            get_canvas() const;
     bool                                               check_if_one_palette(const uint8_t &texture_page) const;
     size_t                                             size_of_map() const;
     void                                               draw(sf::RenderTarget &target, sf::RenderStates states) const final;
     void                                               enable_draw_swizzle();
     void                                               disable_draw_swizzle();
     void                                               enable_disable_blends();
     void                                               disable_disable_blends();
     void                                               enable_square(sf::Vector2u position);
     void                                               compact_map_order();
     void                                               undo();
     void                                               redo();
     void                                               undo_all();
     void                                               redo_all();
     bool                                               undo_enabled();
     bool                                               redo_enabled();
     bool                                               history_remove_duplicate();
     ff_8::filters                                     &filter();
     void                                               update_render_texture(bool reload_textures = false);
     void                                               compact_rows();
     void                                               compact_all();
     void                                               flatten_bpp();
     void                                               flatten_palette();
     static sf::BlendMode                               set_blend_mode(const BlendModeT &blend_mode, std::array<sf::Vertex, 4U> &quad);
     SharedTextures                                     load_textures();
     void                                               reset_render_texture();
     SharedTextures                                     load_textures_internal();
     static colors_type                                 get_colors(const Mim &mim, BPPT bpp, uint8_t palette);
     void                                               save_new_textures(const std::filesystem::path &path);
     void                                               gen_new_textures(const std::filesystem::path path);
     void                                               save_pupu_textures(const std::filesystem::path &path);
     void                                               load_map(const std::filesystem::path &dest_path);
     void                                               resize_render_texture();
     void                                               init_render_texture();
     static const sf::BlendMode                        &GetBlendSubtract();
     void                                               find_upscale_path(SharedTextures &ret, uint8_t palette);
     void                                               find_upscale_path(SharedTextures &ret);
     void                                               find_deswizzle_path(SharedTextures &ret);
     static void                                        load_mim_textures(Mim mim, sf::Texture *texture, BPPT bppt, uint8_t pal);
     void                                               load_mim_textures(SharedTextures &ret, BPPT bpp, uint8_t palette);
     static void                                        async_save(const sf::Texture &out_texture, const std::filesystem::path &out_path);
     static bool                                        save_png_image(const sf::Image &image, const std::filesystem::path &filename);
     bool                                               draw_imported(sf::RenderTarget &target, sf::RenderStates states) const;
     static std::string                                 str_to_lower(std::string input);
     sf::Sprite                                         save_intersecting(const sf::Vector2i &pixel_pos, const std::uint8_t &texture_page);
     [[nodiscard]] std::size_t                          get_texture_pos(BPPT bpp, std::uint8_t palette, std::uint8_t texture_page) const;
     void        update_render_texture(const sf::Texture *p_texture, Map map, const tile_sizes tile_size);
     void        update_position(const sf::Vector2i &pixel_pos, const uint8_t &texture_page, const sf::Vector2i &down_pixel_pos);

     static void gen_pupu_textures(
       const std::filesystem::path        path,
       const std::string                  field_name,
       settings_backup                    settings,
       const std::vector<PupuID>          unique_pupu_ids,
       std::optional<open_viii::LangT>    coo,
       std::shared_ptr<sf::RenderTexture> out_texture);
     static std::filesystem::path save_path_coo(
       fmt::format_string<std::string_view, std::string_view, uint8_t> pattern,
       const std::filesystem::path                                    &path,
       const std::string_view                                         &field_name,
       const uint8_t                                                   texture_page,
       const open_viii::LangT                                          coo);
     static std::filesystem::path save_path_coo(
       fmt::format_string<std::string_view, std::string_view, uint8_t, uint8_t> pattern,
       const std::filesystem::path                                             &path,
       const std::string_view                                                  &field_name,
       const uint8_t                                                            texture_page,
       const uint8_t                                                            palette,
       const open_viii::LangT                                                   coo);
     static std::filesystem::path save_path_coo(
       fmt::format_string<std::string_view, std::string_view, PupuID> pattern,
       const std::filesystem::path                                   &path,
       const std::string_view                                        &field_name,
       const PupuID                                                   pupu,
       const open_viii::LangT                                         coo);
     static std::filesystem::path save_path(
       fmt::format_string<std::string_view, uint8_t> pattern,
       const std::filesystem::path                  &path,
       const std::string_view                       &field_name,
       uint8_t                                       texture_page);
     static std::filesystem::path save_path(
       fmt::format_string<std::string_view, uint8_t, uint8_t> pattern,
       const std::filesystem::path                           &path,
       const std::string_view                                &field_name,
       uint8_t                                                texture_page,
       uint8_t                                                palette);
     static std::filesystem::path save_path(
       fmt::format_string<std::string_view, PupuID> pattern,
       const std::filesystem::path                 &path,
       const std::string_view                      &field_name,
       PupuID                                       pupu);

     [[nodiscard]] std::vector<std::size_t> find_intersecting(
       const open_viii::graphics::background::Map &map,
       const sf::Vector2i                         &pixel_pos,
       const std::uint8_t                         &texture_page,
       bool                                        skip_filters = false,
       bool                                        find_all     = false) const;
     template<std::ranges::range tilesT>
     [[nodiscard]] std::vector<std::size_t> find_intersecting(
       const tilesT       &tiles,
       const sf::Vector2i &pixel_pos,
       const std::uint8_t &texture_page,
       bool                skip_filters = false,
       bool                find_all     = false) const
     {
          if (m_draw_swizzle)
          {
               return ff_8::find_intersecting_swizzle(tiles, m_filters, pixel_pos, texture_page, skip_filters, find_all);
          }
          return ff_8::find_intersecting_deswizzle(tiles, m_filters, pixel_pos, skip_filters, find_all);
     }
     template<typename funcT>
     auto const_visit_tiles(funcT &&p_function) const
     {
          return m_map_group.maps.const_back().visit_tiles(std::forward<decltype(p_function)>(p_function));
     }

     template<open_viii::graphics::background::is_tile tileT>
     [[nodiscard]] const sf::Texture *get_texture(const tileT &tile) const
     {

          if (!m_filters.deswizzle.enabled())
          {
               return get_texture(tile.depth(), tile.palette_id(), tile.texture_id());
          }
          else
          {
               // pupu_ids
               return const_visit_tiles([&tile, this](const auto &tiles) -> const sf::Texture * {
                    if (tiles.empty())
                    {
                         return nullptr;
                    }
                    using TileT1 = std::remove_cvref<decltype(tiles.front())>;
                    using TileT2 = std::remove_cvref<decltype(tile)>;
                    if constexpr (std::is_same_v<TileT1, TileT2>)
                    {
                         const auto found_iterator = std::ranges::find_if(tiles, [&tile](const auto &l_tile) { return l_tile == tile; });
                         const auto distance       = std::ranges::distance(tiles.begin(), found_iterator);

                         if (std::cmp_greater(std::ranges::ssize(m_map_group.maps.pupu()), distance))
                         {
                              auto pupu_it = m_map_group.maps.pupu().cbegin();
                              std::ranges::advance(pupu_it, distance);
                              return get_texture(*pupu_it);
                         }
                         return static_cast<const sf::Texture *>(nullptr);
                    }
                    else
                    {
                         return static_cast<const sf::Texture *>(nullptr);
                    }
               });
          }
     }


     template<open_viii::graphics::background::is_tile tile_type>
     void enable_square(const tile_type &tile)
     {
          using namespace open_viii::graphics::literals;
          auto       src_tpw = tile_type::texture_page_width(tile.depth());
          const auto x       = [&tile, &src_tpw]() -> std::uint32_t { return tile.texture_id() * src_tpw; }();
          const auto src_x   = [&tile, &x, this]() -> std::uint32_t {
               if (!m_filters.deswizzle.enabled())
               {
                    return static_cast<std::uint32_t>(tile.x());
               }
               return tile.source_x() + x;
          }();
          const auto src_y = [&tile, this]() -> std::uint32_t {
               if (!m_filters.deswizzle.enabled())
               {
                    return static_cast<std::uint32_t>(tile.y());
               }
               return tile.source_y();
          }();
          enable_square(sf::Vector2u(src_x, src_y));
     }

     auto duel_visitor(auto &&lambda) const
     {
          return m_map_group.maps.front().visit_tiles([this, &lambda](auto const &tiles_const) {
               return m_map_group.maps.back().visit_tiles([&lambda, &tiles_const](const auto &tiles) {
                    return std::invoke(std::forward<decltype(lambda)>(lambda), tiles_const, tiles);
               });
          });
     }
     auto duel_visitor(auto &&lambda)
     {
          return m_map_group.maps.front().visit_tiles([this, &lambda](auto const &tiles_const) {
               return m_map_group.maps.back().visit_tiles([&lambda, &tiles_const](auto &&tiles) {
                    return std::invoke(std::forward<decltype(lambda)>(lambda), tiles_const, std::forward<decltype(tiles)>(tiles));
               });
          });
     }
     void for_all_tiles(auto const &tiles_const, auto &&tiles, auto &&lambda, bool skip_invalid, bool regular_order) const
     {
          using namespace open_viii::graphics::background;
          // todo move pupu generation to constructor
          std::vector<PupuID> pupu_ids = {};
          pupu_ids.reserve(std::size(tiles_const));
          std::ranges::transform(tiles_const, std::back_inserter(pupu_ids), UniquifyPupu{});
          assert(std::size(tiles_const) == std::size(tiles));
          // assert(std::size(tiles_const) == std::size(m_pupu_ids));
          const auto process = [&skip_invalid,
                                &lambda](auto tiles_const_begin, const auto tiles_const_end, auto tiles_begin, auto pupu_ids_begin) {
               for (; /*t != te &&*/ tiles_const_begin != tiles_const_end; (void)++tiles_const_begin, ++tiles_begin, ++pupu_ids_begin)
               {
                    const is_tile auto &tile_const = *tiles_const_begin;
                    if (skip_invalid && !filter_invalid(tile_const))
                    {
                         continue;
                    }
                    is_tile auto &tile       = *tiles_begin;
                    const PupuID &pupu_const = *pupu_ids_begin;
                    lambda(tile_const, tile, pupu_const);
               }
          };
          if (!regular_order)
          {
               process(std::crbegin(tiles_const), std::crend(tiles_const), std::rbegin(tiles), std::rbegin(pupu_ids));
          }
          else
          {
               process(std::cbegin(tiles_const), std::cend(tiles_const), std::begin(tiles), std::begin(pupu_ids));
          }
     }
     void for_all_tiles(auto &&lambda, bool skip_invalid = true, bool regular_order = false) const
     {
          duel_visitor([&lambda, &skip_invalid, &regular_order, this](auto const &tiles_const, auto &&tiles) {
               if (std::ranges::size(tiles_const) != std::ranges::size(tiles))
               {
                    spdlog::warn("{} != {}", std::ranges::size(tiles_const), std::ranges::size(tiles));
               }
               this->for_all_tiles(
                 tiles_const, std::forward<decltype(tiles)>(tiles), std::forward<decltype(lambda)>(lambda), skip_invalid, regular_order);
          });
     }


     template<typename TilesT>
     auto find_conflicting_tiles(const TilesT &tiles) const
     {
          using TileT                                             = std::ranges::range_value_t<std::remove_cvref_t<TilesT>>;
          using TextureIdT                                        = ff_8::tile_operations::TextureIdT<TileT>;
          using PaletteIdT                                        = ff_8::tile_operations::PaletteIdT<TileT>;
          static constexpr auto                         SourceX   = ff_8::tile_operations::SourceX{};
          static constexpr auto                         SourceY   = ff_8::tile_operations::SourceY{};
          static constexpr auto                         TextureId = ff_8::tile_operations::TextureId{};
          static constexpr auto                         PaletteId = ff_8::tile_operations::PaletteId{};
          std::map<TextureIdT, std::vector<PaletteIdT>> conflicts;

          // Process each pair of matching tiles and insert their palette IDs into the conflicts map
          for (const auto &first : tiles)
          {
               for (const auto &second : tiles | std::views::drop(1))
               {
                    if (
                      &first != &second && PaletteId(first) != PaletteId(second) && TextureId(first) == TextureId(second)
                      && SourceX(first) == SourceX(second) && SourceY(first) == SourceY(second))
                    {
                         conflicts[TextureId(first)].push_back(PaletteId(first));
                         conflicts[TextureId(second)].push_back(PaletteId(second));
                    }
               }
          }

          // Sort and remove duplicates from the conflict sets
          for (auto &pair : conflicts)
          {
               std::vector<PaletteIdT> &vector = pair.second;
               std::ranges::sort(vector);
               auto last = std::ranges::unique(vector);
               vector.erase(last.begin(), last.end());
          }

          return conflicts;
     }


     auto get_conflicting_palettes() const
     {
          // todo rewrite this.
          return m_map_group.maps.back().visit_tiles([this](const auto &tiles) {
               //        using tileT =
               //          std::remove_cvref_t<typename
               //          std::remove_cvref_t<decltype(tiles)>::value_type>;

               //          auto map_xy_palette = generate_map(
               //            tiles,
               //            [](const auto &tile) { return std::make_tuple(tile.source_x() / tile.width(), tile.source_y() / tile.height());
               //            },
               //            [](const auto &tile) { return tile.palette_id(); },
               //            [&texture_page](const auto &tile) { return std::cmp_equal(texture_page, tile.texture_id()); });
               //          std::vector<uint8_t>     conflict_palettes{};
               //          std::vector<std::string> conflict_xy{};
               //          for (auto &kvp : map_xy_palette)
               //          {
               //               // const auto& xy = kvp.first;
               //               std::vector<std::uint8_t> &palette_vector = kvp.second;
               //               std::ranges::sort(palette_vector);
               //               auto [first, last] = std::ranges::unique(palette_vector);
               //               palette_vector.erase(first, last);
               //               if (palette_vector.size() <= 1U)
               //               {
               //                    // map_xy_palette.erase(xy);
               //               }
               //               else
               //               {
               //                    conflict_xy.emplace_back(fmt::format("({},{})", std::get<0>(kvp.first), std::get<1>(kvp.first)));
               //                    conflict_palettes.insert(conflict_palettes.end(), palette_vector.begin(), palette_vector.end());
               //               }
               //          }

               //          if (!conflict_xy.empty())
               //          {
               //               spdlog::info("Conflicting Palettes:");
               //               for (const auto &cxy : conflict_xy)
               //               {
               //                    spdlog::info("conflict xy: {}", cxy);
               //               }
               //               std::ranges::sort(conflict_palettes);
               //               auto [first, last] = std::ranges::unique(conflict_palettes);
               //               conflict_palettes.erase(first, last);
               //               for (auto p : conflict_palettes)
               //               {
               //                    spdlog::info("conflict palette: {}", p);
               //               }
               //          }
               //          return conflict_palettes;
               return find_conflicting_tiles(tiles);
          });
     }
     ::upscales get_upscales()
     {
          if (m_map_group.field)
          {
               return { m_map_group.field->get_base_name(), m_map_group.opt_coo ? *m_map_group.opt_coo : open_viii::LangT::generic };
          }
          return {};
     }

     [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip_for_imported(
       const sf::Vector2u                                  &draw_size,
       const sf::Vector2u                                  &texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile) const
     {
          return get_triangle_strip(draw_size, texture_size, tile_const, tile, true);
     }
     [[nodiscard]] static sf::Vector2f to_Vector2f(sf::Vector2u in_vec)
     {
          return { static_cast<float>(in_vec.x), static_cast<float>(in_vec.y) };
     }
     [[nodiscard]] static sf::Vector2f to_Vector2f(sf::Vector2i in_vec)
     {
          return { static_cast<float>(in_vec.x), static_cast<float>(in_vec.y) };
     }
     [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
       const sf::Vector2u                                  &draw_size,
       const sf::Vector2u                                  &texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile,
       bool                                                 imported = false) const
     {
          const auto src = [this, &tile_const, &imported]() {
               if (imported)
               {
                    return to_Vector2f(ff_8::get_triangle_strip_source_imported(tile_const));
               }
               if (m_filters.upscale.enabled())
               {
                    return to_Vector2f(ff_8::get_triangle_strip_source_upscale(tile_const));
               }
               if (m_filters.deswizzle.enabled())
               {
                    return to_Vector2f(ff_8::get_triangle_strip_source_deswizzle(tile_const));
               }
               return to_Vector2f(ff_8::get_triangle_strip_source_default(tile_const));
          }();
          const auto dest = [this, &tile]() {
               if (m_draw_swizzle)
               {
                    if (m_disable_texture_page_shift)
                    {
                         return to_Vector2f(ff_8::get_triangle_strip_dest_swizzle_disable_shift(tile));
                    }
                    return to_Vector2f(ff_8::get_triangle_strip_dest_swizzle(tile));
               }
               return to_Vector2f(ff_8::get_triangle_strip_dest_default(tile));
          }();
          return ff_8::get_triangle_strip(to_Vector2f(draw_size), to_Vector2f(texture_size), src, dest);
     }
};
#endif// FIELD_MAP_EDITOR_MAP_SPRITE_HPP
