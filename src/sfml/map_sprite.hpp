//
// Created by pcvii on 9/7/2021.
//

#ifndef FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#define FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#include "filter.hpp"
#include "grid.hpp"
#include "MapHistory.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "square.hpp"
#include "tile_sizes.hpp"
#include "unique_values.hpp"
#include "upscales.hpp"
#include <cppcoro/generator.hpp>
#include <cppcoro/task.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <future>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
// #include <stacktrace>
#include <utility>
#define USE_THREADS
static inline std::string str_to_lower(std::string input)
{
     std::string output{};
     output.reserve(std::size(input) + 1);
     std::ranges::transform(
       input, std::back_inserter(output), [](char character) -> char { return static_cast<char>(::tolower(character)); });
     return output;
}
struct map_sprite final
  : public sf::Drawable
  , public sf::Transformable
{
     template<typename funcT>
     auto const_visit_tiles(funcT &&p_function) const
     {
          return m_maps.const_back().visit_tiles(std::forward<decltype(p_function)>(p_function));
     }
     [[nodiscard]] const sf::Texture *get_texture(open_viii::graphics::BPPT bpp, std::uint8_t palette, std::uint8_t texture_page) const;
     [[nodiscard]] const sf::Texture *get_texture(const ::PupuID &pupu) const;

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

                         if (std::cmp_greater(std::ranges::ssize(m_maps.pupu()), distance))
                         {
                              auto pupu_it = m_maps.pupu().cbegin();
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

     [[nodiscard]] sf::Vector2u get_tile_texture_size(const sf::Texture *texture) const;

     sf::Vector2u               get_tile_draw_size() const;

     template<open_viii::graphics::background::is_tile tileT, typename T>
     static void format_tile_text(const tileT &tile, T &&format_function)
     {
          const auto raw_hex = to_hex(tile);
          std::invoke(format_function, "Hex", std::string_view(raw_hex.data(), raw_hex.size()));
          std::invoke(
            format_function,
            "Source",
            fmt::format(
              "({}, {}) ({}, {})",
              tile.source_rectangle().x(),
              tile.source_rectangle().y(),
              tile.source_rectangle().width(),
              tile.source_rectangle().height()));
          std::invoke(
            format_function,
            "Output",
            fmt::format(
              "({}, {}) ({}, {})",
              tile.output_rectangle().x(),
              tile.output_rectangle().y(),
              tile.output_rectangle().width(),
              tile.output_rectangle().height()));
          std::invoke(format_function, "Z", tile.z());
          std::invoke(format_function, "Depth", static_cast<int>(tile.depth()));
          std::invoke(format_function, "Palette ID", tile.palette_id());
          std::invoke(format_function, "Texture ID", tile.texture_id());
          std::invoke(format_function, "Layer ID", tile.layer_id());
          std::invoke(format_function, "Blend Mode", static_cast<std::uint16_t>(tile.blend_mode()));
          std::invoke(format_function, "Blend Other", tile.blend());
          std::invoke(format_function, "Animation ID", tile.animation_id());
          std::invoke(format_function, "Animation State", tile.animation_state());
          std::invoke(format_function, "Draw", tile.draw());
     }

     void update_render_texture(const sf::Texture *p_texture, open_viii::graphics::background::Map map, const tile_sizes tile_size);

     void compact_map_order();

   public:
     using color_type  = open_viii::graphics::Color32RGBA;
     using colors_type = std::vector<color_type>;
     map_sprite()      = default;
     map_sprite(
       std::shared_ptr<open_viii::archive::FIFLFS<false>> field,
       open_viii::LangT                                   coo,
       bool                                               draw_swizzle,
       ff_8::filters                                      in_filters,
       bool                                               force_disable_blends)
       : m_draw_swizzle(draw_swizzle)
       , m_disable_blends(force_disable_blends)
       , m_filters(std::move(in_filters))
       , m_field(std::move(field))
       , m_coo(coo)
       , m_upscales(get_upscales())
       , m_mim(get_mim())
       , m_maps(get_map(&m_map_path, true, m_using_coo))
       , m_all_unique_values_and_strings(get_all_unique_values_and_strings())
       , m_canvas(get_canvas())
       , m_texture(load_textures())
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

     void redo()
     {
          (void)m_maps.redo();
          update_render_texture();
     }

     void undo_all()
     {
          m_maps.undo_all();
          update_render_texture();
     }

     void redo_all()
     {
          m_maps.redo_all();
          update_render_texture();
     }

     bool undo_enabled()
     {
          return m_maps.undo_enabled();
     }

     bool redo_enabled()
     {
          return m_maps.redo_enabled();
     }
     bool history_remove_duplicate()
     {
          return m_maps.remove_duplicate();
     }

     auto get_map_scale() const
     {
          return m_scale;
     }

     [[nodiscard]] const all_unique_values_and_strings &uniques() const;

     map_sprite        update(std::shared_ptr<open_viii::archive::FIFLFS<false>> field, open_viii::LangT coo, bool draw_swizzle) const;

     void              enable_draw_swizzle();
     void              disable_draw_swizzle();
     void              enable_disable_blends();
     void              disable_disable_blends();
     std::string       map_filename() const;
     bool              fail() const;
     std::uint32_t     width() const;
     std::uint32_t     height() const;
     void              save(const std::filesystem::path &path) const;
     void              map_save(const std::filesystem::path &dest_path) const;
     void              test_map(const std::filesystem::path &saved_path) const;
     map_sprite        with_coo(open_viii::LangT coo) const;
     map_sprite        with_field(std::shared_ptr<open_viii::archive::FIFLFS<false>> field) const;
     map_sprite        with_filters(ff_8::filters filters) const;
     void              draw(sf::RenderTarget &target, sf::RenderStates states) const final;
     const map_sprite &toggle_grid(bool enable, bool enable_texture_page_grid) const;

     void              enable_square(sf::Vector2u position);
     void              disable_square() const;
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
     bool                 empty() const;
     const ff_8::filters &filter() const;
     ff_8::filters       &filter();
     void                 update_render_texture(bool reload_textures = false);
     void                 update_position(const sf::Vector2i &pixel_pos, const uint8_t &texture_page, const sf::Vector2i &down_pixel_pos);
     [[nodiscard]] std::vector<std::size_t> find_intersecting(
       const open_viii::graphics::background::Map &map,
       const sf::Vector2i                         &pixel_pos,
       const std::uint8_t                         &texture_page,
       bool                                        skip_filters = false) const
     {
          return map.visit_tiles([this, &pixel_pos, &texture_page, &skip_filters](const auto &tiles) {
               return find_intersecting(tiles, pixel_pos, texture_page, skip_filters);
          });
     }
     template<std::ranges::range tilesT>
     [[nodiscard]] std::vector<std::size_t> find_intersecting(
       const tilesT       &tiles,
       const sf::Vector2i &pixel_pos,
       const std::uint8_t &texture_page,
       bool                skip_filters = false,
       bool                find_all     = false) const
     {
          std::vector<std::size_t>                             out          = {};
          static constexpr std::vector<std::size_t>::size_type new_capacity = { 30 };
          out.reserve(new_capacity);
          auto filtered_tiles =
            tiles | std::views::filter([this, &skip_filters, &texture_page, &pixel_pos](const auto &tile) -> bool {
                 static constexpr auto in_bounds = [](auto input, auto low, auto high) {
                      return std::cmp_greater_equal(input, low) && std::cmp_less(input, high);
                 };
                 if (!skip_filters && fail_filter(tile))
                 {
                      return false;
                 }
                 if (m_draw_swizzle)
                 {
                      if (std::cmp_equal(tile.texture_id(), texture_page))
                      {
                           static constexpr int max_texture_page_dim = 256;
                           if (in_bounds(
                                 pixel_pos.x % max_texture_page_dim, tile.source_x(), tile.source_x() + static_cast<int>(TILE_SIZE)))
                           {
                                if (in_bounds(
                                      pixel_pos.y % max_texture_page_dim, tile.source_y(), tile.source_y() + static_cast<int>(TILE_SIZE)))
                                {
                                     return true;
                                }
                           }
                      }
                 }
                 else if (in_bounds(pixel_pos.x, tile.x(), tile.x() + static_cast<int>(TILE_SIZE)))
                 {
                      if (in_bounds(pixel_pos.y, tile.y(), tile.y() + static_cast<int>(TILE_SIZE)))
                      {
                           return true;
                      }
                 }
                 return false;
            });
          const auto get_indices = [&](auto &&range) {
               std::transform(std::begin(range), std::end(range), std::back_inserter(out), [&tiles](const auto &tile) {
                    const auto *const start = tiles.data();
                    const auto *const curr  = &tile;
                    format_tile_text(tile, [](std::string_view name, const auto &value) { spdlog::info("tile {}: {}", name, value); });
                    return static_cast<std::size_t>(std::distance(start, curr));
               });
          };
          if (m_draw_swizzle && !find_all)
          {
               // If palette and bpp are overlapping it causes problems.
               //  This prevents you selecting more than one at a time.
               //  min depth/bpp was chosen because lower bpp can be greater src x.
               const auto min_depth   = (std::ranges::min_element)(filtered_tiles, {}, [](const auto &tile) { return tile.depth(); });
               // min palette well, lower bpp tend to be a lower palette id I think.
               const auto min_palette = (std::ranges::min_element)(filtered_tiles, {}, [](const auto &tile) { return tile.palette_id(); });
               auto       filtered_tiles_with_depth_and_palette =
                 filtered_tiles | std::views::filter([&](const auto &tile) -> bool {
                      return min_depth->depth() == tile.depth() && min_palette->palette_id() == tile.palette_id();
                 });
               get_indices(filtered_tiles_with_depth_and_palette);
          }
          else
          {
               get_indices(filtered_tiles);
          }

          //    auto currentStacktrace = std::stacktrace::current();
          //    for (const auto &entry : currentStacktrace)
          //    {
          //      spdlog::info(
          //        "{}:{} - {}",
          //        entry.source_file(),
          //        entry.source_line(),
          //        entry.description());
          //    }
          //    spdlog::info("Found {:3} intersecting tiles", out.size());
          //    for (const auto &i : out)
          //    {
          //      spdlog::info("Tile index: {:4} ", i);
          //    }
          return out;
     }

     std::size_t         row_empties(std::uint8_t tile_y, std::uint8_t texture_page, bool move_from_row = false);
     sf::Sprite          save_intersecting(const sf::Vector2i &pixel_pos, const std::uint8_t &texture_page);
     std::uint8_t        max_x_for_saved() const;
     void                compact_rows();
     void                compact_all();
     void                flatten_bpp();
     void                flatten_palette();
     void                save_new_textures(const std::filesystem::path &path);
     cppcoro::task<void>                gen_new_textures(const std::filesystem::path path);
     void                save_pupu_textures(const std::filesystem::path &path);
     cppcoro::task<void> gen_pupu_textures(const std::filesystem::path path);
     void                save_modified_map(const std::filesystem::path &path) const;
     void                load_map(const std::filesystem::path &dest_path);
     std::string         get_base_name() const;

   private:
     static constexpr auto                              default_filter_lambda = [](auto &&) { return true; };
     static constexpr auto                              filter_invalid        = open_viii::graphics::background::Map::filter_invalid();
     square                                             m_square = { sf::Vector2u{}, sf::Vector2u{ TILE_SIZE, TILE_SIZE }, sf::Color::Red };


     bool                                               m_draw_swizzle                  = { false };
     bool                                               m_disable_texture_page_shift    = { false };
     bool                                               m_disable_blends                = { false };
     ff_8::filters                                      m_filters                       = {};
     std::shared_ptr<open_viii::archive::FIFLFS<false>> m_field                         = {};
     open_viii::LangT                                   m_coo                           = {};
     ::upscales                                         m_upscales                      = {};
     open_viii::graphics::background::Mim               m_mim                           = {};
     std::string                                        m_map_path                      = {};
     bool                                               m_using_coo                     = {};
     bool                                               m_using_imported_texture        = {};
     const sf::Texture                                 *m_imported_texture              = { nullptr };
     std::uint16_t                                      m_imported_tile_size            = {};
     open_viii::graphics::background::Map               m_imported_tile_map             = {};
     open_viii::graphics::background::Map               m_imported_tile_map_front       = {};
     ff_8::MapHistory                                   m_maps                          = {};
     all_unique_values_and_strings                      m_all_unique_values_and_strings = {};
     open_viii::graphics::Rectangle<std::uint32_t>      m_canvas                        = {};
     static constexpr std::uint8_t                      TILE_SIZE                       = 16U;
     static constexpr std::uint8_t                      MAX_TEXTURE_PAGES               = 14U;
     static constexpr std::uint8_t                      MAX_PALETTES                    = 16U;
     static constexpr std::uint8_t                      BPP_COMBOS                      = 2U;
     static constexpr std::uint16_t                     START_OF_NO_PALETTE_INDEX       = MAX_PALETTES * MAX_TEXTURE_PAGES;
     static constexpr std::uint16_t                     BPP16_INDEX                     = MAX_PALETTES * BPP_COMBOS + 1;
     static constexpr auto                              MAX_TEXTURES =
       (std::max)(static_cast<std::uint16_t>(START_OF_NO_PALETTE_INDEX + MAX_TEXTURE_PAGES), static_cast<std::uint16_t>(BPP16_INDEX + 1U));
     // todo ecenter3 shows different images for remaster and 2013. Fix?

     std::vector<std::future<void>>                         m_futures                = {};
     std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> m_texture                = {};
     std::shared_ptr<sf::RenderTexture>                     m_render_texture         = {};
     std::shared_ptr<sf::RenderTexture>                     m_drag_sprite_texture    = {};
     grid                                                   m_grid                   = {};
     grid                                                   m_texture_page_grid      = {};
     std::vector<std::size_t>                               m_saved_indices          = {};
     std::vector<std::size_t>                               m_saved_imported_indices = {};
     std::uint32_t                                          m_scale                  = { 1 };
     grid                                                   get_grid() const;
     grid                                                   get_texture_page_grid() const;
     all_unique_values_and_strings                          get_all_unique_values_and_strings() const;
     [[nodiscard]] open_viii::graphics::background::Mim     get_mim() const;
     open_viii::graphics::background::Map                   get_map(std::string *out_path, bool shift, bool &coo) const;
     [[nodiscard]] colors_type                              get_colors(open_viii::graphics::BPPT bpp, std::uint8_t palette) const;
     [[nodiscard]] std::size_t get_texture_pos(open_viii::graphics::BPPT bpp, std::uint8_t palette, std::uint8_t texture_page) const;
     [[nodiscard]] open_viii::graphics::Rectangle<std::uint32_t> get_canvas() const;
     void                                                        resize_render_texture();
     void                                                        init_render_texture();

     [[nodiscard]] std::array<sf::Vertex, 4U>                    get_triangle_strip(
                          const sf::Vector2u &draw_x,
                          const sf::Vector2u &texture_size,
                          std::integral auto  texture_x,
                          std::integral auto  texture_y,
                          std::integral auto  dest_x,
                          std::integral auto  dest_y) const;

     [[nodiscard]] std::array<sf::Vertex, 4U> get_triangle_strip(
       const sf::Vector2u                                  &draw_size,
       const sf::Vector2u                                  &texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile) const;

     static const sf::BlendMode &GetBlendSubtract();
     [[nodiscard]] bool          local_draw(sf::RenderTarget &target, sf::RenderStates states) const;
     template<open_viii::graphics::background::is_tile tileT>
     bool fail_filter(const tileT &tile) const
     {
          // m_filters.bpp.value() != 16_bpp
          return !std::invoke(m_filters, tile);
     }
     void wait_for_futures();
     auto duel_visitor(auto &&lambda) const
     {
          return m_maps.front().visit_tiles([this, &lambda](auto const &tiles_const) {
               return m_maps.back().visit_tiles([&lambda, &tiles_const](const auto &tiles) {
                    return std::invoke(std::forward<decltype(lambda)>(lambda), tiles_const, tiles);
               });
          });
     }
     auto duel_visitor(auto &&lambda)
     {
          return m_maps.front().visit_tiles([this, &lambda](auto const &tiles_const) {
               return m_maps.back().visit_tiles([&lambda, &tiles_const](auto &&tiles) {
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

     void find_upscale_path(std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret, uint8_t palette);
     void find_upscale_path(std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret);
     void find_deswizzle_path(std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret);
     void load_mim_textures(std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>> &ret, open_viii::graphics::BPPT bpp, uint8_t palette);
     template<typename F, typename... T>
     void spawn_thread(F &&function, T &&...arguments)
     {
#ifdef USE_THREADS
          m_futures.emplace_back(std::async(std::launch::async, std::forward<F>(function), std::forward<T>(arguments)...));
#undef USE_THREADS
#else
          std::invoke(std::forward<F>(function), std::forward<T>(arguments)...);
#endif
     }
     template<typename key_lambdaT, typename weight_lambdaT>
     [[maybe_unused]] void compact_generic(key_lambdaT &&key_lambda, weight_lambdaT &&weight_lambda, int passes = 2)
     {
          m_maps.copy_back().visit_tiles([&key_lambda, &weight_lambda, &passes, this](auto &&tiles) {
               for (int pass = passes; pass != 0; --pass)// at least 2 passes needed as things might get shifted to
                                                         // other texture pages and then the keys are less valuable.
               {
                    auto         pointers   = this->generate_map(tiles, key_lambda, [](auto &&tile) { return &tile; });
                    std::uint8_t col        = {};
                    std::uint8_t row        = {};
                    std::uint8_t page       = {};
                    std::size_t  row_weight = {};
                    for (auto &[key, tps] : pointers)
                    {
                         const auto weight = weight_lambda(key, tps);

                         if (
                           std::cmp_greater_equal(col, TILE_SIZE) || std::cmp_greater_equal(row_weight, TILE_SIZE)
                           || std::cmp_greater(row_weight + weight, TILE_SIZE))
                         {
                              ++row;
                              col        = {};
                              row_weight = {};
                         }

                         if (std::cmp_greater_equal(row, TILE_SIZE))
                         {
                              ++page;
                              row = {};
                         }

                         using tileT = std::remove_cvref_t<typename std::remove_cvref_t<decltype(tiles)>::value_type>;
                         for (tileT *const tp : tps)
                         {
                              *tp = tp->with_source_xy(
                                        static_cast<decltype(tileT{}.source_x())>(col * TILE_SIZE),
                                        static_cast<decltype(tileT{}.source_y())>(row * TILE_SIZE))
                                      .with_texture_id(static_cast<decltype(tileT{}.texture_id())>(page));
                         }

                         row_weight += weight;
                         ++col;
                    }
               }
          });
          update_render_texture();
     }

     std::shared_ptr<sf::RenderTexture> save_texture(std::uint32_t width, std::uint32_t height) const;
     uint32_t                           get_max_texture_height() const;
     void async_save(const std::filesystem::path &out_path, const std::shared_ptr<sf::RenderTexture> &out_texture);


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


     template<typename tilesT, typename key_lambdaT, typename value_lambdaT, typename filterT = decltype(default_filter_lambda)>
     auto generate_map(tilesT &&tiles, key_lambdaT &&key_lambda, value_lambdaT &&value_lambda, filterT &&filter = {}) const
     {
          using tileT  = std::remove_cvref_t<typename std::remove_cvref_t<tilesT>::value_type>;
          using keyT   = decltype(key_lambda(tileT{}));
          using valueT = decltype(value_lambda(tileT{}));
          std::map<keyT, std::vector<valueT>> map{};
          auto                                filtered_tiles = tiles | std::views::filter(filter);
          std::ranges::for_each(filtered_tiles, [&map, &key_lambda, &value_lambda](auto &&tile) {
               if (!filter_invalid(tile))
               {
                    return;
               }
               valueT value = value_lambda(tile);
               keyT   key   = key_lambda(tile);
               if (map.contains(key))
               {
                    map.at(key).push_back(value);
               }
               else
               {
                    map.emplace(key, std::vector<valueT>{ value });
               }
          });
          return map;
     }

     std::filesystem::path save_path_coo(
       fmt::format_string<std::string_view, std::string_view, uint8_t> pattern,
       const std::filesystem::path                                    &path,
       const std::string_view                                         &field_name,
       uint8_t                                                         texture_page) const;
     std::filesystem::path save_path_coo(
       fmt::format_string<std::string_view, std::string_view, uint8_t, uint8_t> pattern,
       const std::filesystem::path                                             &path,
       const std::string_view                                                  &field_name,
       uint8_t                                                                  texture_page,
       uint8_t                                                                  palette) const;
     std::filesystem::path save_path_coo(
       fmt::format_string<std::string_view, std::string_view, PupuID> pattern,
       const std::filesystem::path                                   &path,
       const std::string_view                                        &field_name,
       PupuID                                                         pupu) const;
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
     bool check_if_one_palette(const uint8_t &texture_page) const;
     auto get_conflicting_palettes() const
     {
          // todo rewrite this.
          return m_maps.back().visit_tiles([this](const auto &tiles) {
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
          if (m_field)
          {
               return { m_field->get_base_name(), m_coo };
          }
          return {};
     }
     template<open_viii::graphics::background::is_tile T>
          requires(std::is_standard_layout_v<T>)
     static constexpr auto to_hex(const T &tile)
     {
          constexpr auto to_hex_operation = [](const std::uint8_t input_byte, const auto operation) -> char {
               constexpr std::uint8_t number_of_values_in_nibble = 16U;
               constexpr char         threshold_of_A_to_F        = 10;
               char const             half_transformed_char      = static_cast<char>(operation(input_byte, number_of_values_in_nibble));
               return static_cast<char>(
                 (half_transformed_char < threshold_of_A_to_F ? half_transformed_char + '0'
                                                              : half_transformed_char - threshold_of_A_to_F + 'A'));
          };
          const auto                           raw_bytes = std::bit_cast<std::array<std::uint8_t, sizeof(T)>>(tile);
          std::array<char, sizeof(T) * 2U + 1> raw_hex{};
          raw_hex.back() = 0;
          auto rhi       = raw_hex.begin();
          for (const std::uint8_t current_byte : raw_bytes)
          {
               *rhi = to_hex_operation(current_byte, std::divides{});
               std::advance(rhi, 1);
               *rhi = to_hex_operation(current_byte, std::modulus{});
               std::advance(rhi, 1);
          }
          return raw_hex;
     }
     size_t                     size_of_map() const;
     static bool                save_png_image(const sf::Image &image, const std::string &filename);
     bool                       draw_imported(sf::RenderTarget &target, sf::RenderStates states) const;
     std::array<sf::Vertex, 4U> get_triangle_strip_for_imported(
       const sf::Vector2u                                  &draw_size,
       const sf::Vector2u                                  &texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile) const;
     sf::Vector2u         get_tile_texture_size_for_import() const;
     static sf::BlendMode set_blend_mode(const open_viii::graphics::background::BlendModeT &blend_mode, std::array<sf::Vertex, 4U> &quad);
     std::shared_ptr<std::array<sf::Texture, map_sprite::MAX_TEXTURES>> load_textures();
     void                                                               reset_render_texture();
     std::shared_ptr<std::array<sf::Texture, MAX_TEXTURES>>             load_textures_internal();
};
#endif// FIELD_MAP_EDITOR_MAP_SPRITE_HPP
