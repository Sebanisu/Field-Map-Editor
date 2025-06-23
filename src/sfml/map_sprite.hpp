//
// Created by pcvii on 9/7/2021.
//

#ifndef FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#define FIELD_MAP_EDITOR_MAP_SPRITE_HPP
#include "filter.hpp"
#include "gui/key_value_data.hpp"
#include "gui/Selections.hpp"
#include "map_group.hpp"
#include "map_operation.hpp"
#include "MapHistory.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "RangeConsumer.hpp"
#include "settings_backup.hpp"
#include "tile_sizes.hpp"
#include "unique_values.hpp"
#include "upscales.hpp"
#include <algorithm>
#include <BatchRenderer.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <FrameBuffer.hpp>
#include <OrthographicCamera.hpp>
#include <Shader.hpp>
#include <Texture.hpp>
#include <utility>

namespace fme
{
struct [[nodiscard]] map_sprite// final
//   : public sf::Drawable
//   , public sf::Transformable
{
   public:
     static constexpr std::uint8_t  TILE_SIZE                 = 16U;
     static constexpr std::uint8_t  MAX_TEXTURE_PAGES         = 14U;
     static constexpr std::uint8_t  MAX_PALETTES              = 16U;
     static constexpr std::uint8_t  BPP_COMBOS                = 2U;
     static constexpr std::uint16_t START_OF_NO_PALETTE_INDEX = MAX_PALETTES * MAX_TEXTURE_PAGES;
     static constexpr std::uint16_t BPP16_INDEX               = MAX_PALETTES * BPP_COMBOS + 1;
     static constexpr auto          MAX_TEXTURES =
       (std::max)(static_cast<std::uint16_t>(START_OF_NO_PALETTE_INDEX + MAX_TEXTURE_PAGES), static_cast<std::uint16_t>(BPP16_INDEX + 1U));

     using BPPT           = open_viii::graphics::BPPT;
     using WeakField      = std::weak_ptr<open_viii::archive::FIFLFS<false>>;
     using Map            = open_viii::graphics::background::Map;
     using Mim            = open_viii::graphics::background::Mim;
     using color_type     = open_viii::graphics::Color32RGBA;
     using colors_type    = std::vector<color_type>;
     using SharedTextures = std::shared_ptr<std::array<glengine::Texture, MAX_TEXTURES>>;
     using BlendModeT     = open_viii::graphics::background::BlendModeT;
     using Rectangle      = open_viii::graphics::Rectangle<std::uint32_t>;
     using iRectangle     = open_viii::graphics::Rectangle<std::int32_t>;

   private:
     SharedTextures m_texture = std::make_shared<std::array<glengine::Texture, MAX_TEXTURES>>();
     mutable FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>> m_future_of_future_consumer  = {};
     mutable FutureConsumer<std::vector<std::future<void>>>                      m_future_consumer            = {};
     ff_8::map_group                                                             m_map_group                  = {};
     // TODO do I need square?
     //  square                                        m_square    = { glm::uvec2{}, glm::uvec2{ TILE_SIZE, TILE_SIZE }, sf::Color::Red };
     bool                                                                        m_draw_swizzle               = { false };
     bool                                                                        m_disable_texture_page_shift = { false };
     bool                                                                        m_disable_blends             = { false };
     mutable ff_8::filters                         m_filters                 = { false };// default false should be override by gui to true.
     std::weak_ptr<Selections>                     m_selections              = {};
     ::upscales                                    m_upscales                = { m_selections };
     bool                                          m_using_imported_texture  = {};
     const glengine::Texture                      *m_imported_texture        = { nullptr };
     std::uint16_t                                 m_imported_tile_size      = {};
     Map                                           m_imported_tile_map       = {};
     Map                                           m_imported_tile_map_front = {};
     all_unique_values_and_strings                 m_all_unique_values_and_strings = {};
     open_viii::graphics::Rectangle<std::uint32_t> m_canvas                        = {};

     glengine::BatchRenderer                       m_batch_renderer                = { 1000 };
     glengine::BatchRenderer                       m_batch_renderer_red_integer    = { 1,
                                                                                       { std::filesystem::current_path() / "res" / "shader"
                                                                                         / "red_integer.shader" } };
     glengine::FrameBuffer                         out_texture                     = {};
     mutable glengine::FrameBuffer                 m_render_framebuffer            = {};
     mutable glengine::FrameBuffer                 m_drag_sprite_framebuffer       = {};
     std::vector<std::size_t>                      m_saved_indices                 = {};
     std::vector<std::size_t>                      m_saved_imported_indices        = {};
     mutable std::uint32_t                         m_scale                         = { 1 };
     mutable bool                                  once                            = { true };
     mutable glengine::OrthographicCamera          m_fixed_render_camera           = {};

     static constexpr auto                         s_default_color                 = glm::vec4{ 1.F };// RGBA = (1, 1, 1, 1)
     static constexpr auto                         s_half_color                    = s_default_color / 2.F;// RGBA = (0.5, 0.5, 0.5, 0.5)
     static constexpr auto                         s_quarter_color                 = s_half_color / 2.F;// RGBA = (0.25, 0.25, 0.25, 0.25)
     mutable glm::vec4                             m_uniform_color                 = s_default_color;

   public:
     map_sprite() = default;
     map_sprite(
       ff_8::map_group           map_group,
       bool                      draw_swizzle,
       ff_8::filters             in_filters,
       bool                      force_disable_blends,
       bool                      require_coo,
       std::weak_ptr<Selections> selections);


     [[nodiscard]] std::string                  appends_prefix_base_name(std::string_view title) const;

     [[nodiscard]] std::uint32_t                get_map_scale() const;
     [[nodiscard]] const glengine::FrameBuffer &get_render_texture() const
     {
          return m_render_framebuffer;
     }
     [[nodiscard]] const glengine::Texture             *get_texture(BPPT bpp, std::uint8_t palette, std::uint8_t texture_page) const;
     [[nodiscard]] const glengine::Texture             *get_texture(const ff_8::PupuID &pupu) const;
     [[nodiscard]] glm::uvec2                           get_tile_texture_size(const glengine::Texture *const texture) const;
     [[nodiscard]] glm::uvec2                           get_tile_draw_size() const;
     [[nodiscard]] bool                                 generate_texture(const glengine::FrameBuffer &texture) const;
     [[nodiscard]] std::uint32_t                        get_max_texture_height() const;
     [[nodiscard]] bool                                 local_draw(const glengine::BatchRenderer &, const glengine::Shader &) const;
     [[nodiscard]] bool                                 draw_imported(const glengine::FrameBuffer &) const;
     [[nodiscard]] std::string                          get_base_name() const;
     [[nodiscard]] const all_unique_values_and_strings &uniques() const;
     [[nodiscard]] const std::vector<ff_8::PupuID>     &working_unique_pupu() const;
     [[nodiscard]] const std::vector<ff_8::PupuID>     &original_unique_pupu() const;
     [[nodiscard]] const std::vector<ff_8::PupuID>     &original_pupu() const;
     [[nodiscard]] const std::vector<ff_8::PupuID>     &working_pupu() const;
     [[nodiscard]] const ff_8::source_tile_conflicts   &original_conflicts() const;
     [[nodiscard]] const ff_8::source_tile_conflicts   &working_conflicts() const;
     [[nodiscard]] const ff_8::MapHistory::nst_map     &working_similar_counts() const;
     [[nodiscard]] const ff_8::MapHistory::nsat_map    &working_animation_counts() const;
     [[nodiscard]] std::string                          map_filename() const;
     [[nodiscard]] bool                                 fail() const;
     [[nodiscard]] std::uint32_t                        width() const;
     [[nodiscard]] std::uint32_t                        height() const;
     [[nodiscard]] map_sprite                           with_coo(open_viii::LangT coo) const;
     [[nodiscard]] map_sprite                           with_field(WeakField field, open_viii::LangT coo) const;
     [[nodiscard]] map_sprite                           with_filters(ff_8::filters filters) const;
     [[nodiscard]] bool                                 empty() const;
     [[nodiscard]] const ff_8::filters                 &filter() const;
     [[nodiscard]] std::uint8_t                         max_x_for_saved() const;
     [[nodiscard]] map_sprite                           update(ff_8::map_group map_group, bool draw_swizzle) const;
     [[nodiscard]] all_unique_values_and_strings        get_all_unique_values_and_strings() const;
     [[nodiscard]] glm::uvec2                           get_tile_texture_size_for_import() const;
     [[nodiscard]] Rectangle                            get_canvas() const;
     [[nodiscard]] bool                                 undo_enabled() const;
     [[nodiscard]] bool                                 redo_enabled() const;
     [[nodiscard]] bool                                 history_remove_duplicate();
     [[nodiscard]] ff_8::filters                       &filter();
     //[[nodiscard]] static sf::BlendMode                 set_blend_mode(const BlendModeT &blend_mode, std::array<sf::Vertex, 4U> &quad);
     [[nodiscard]] bool                                 fallback_textures() const;
     void                                               queue_texture_loading() const;
     [[nodiscard]] static colors_type                   get_colors(const Mim &mim, BPPT bpp, uint8_t palette);
     // [[nodiscard]] static const sf::BlendMode          &get_blend_subtract();
     [[nodiscard]] static std::future<std::future<void>>
                                      async_save(const glengine::Texture &out_texture, const std::filesystem::path &out_path);

     [[nodiscard]] bool               using_coo() const;
     [[nodiscard]] static std::string str_to_lower(std::string input);
     template<typename T>
          requires(std::same_as<std::remove_cvref_t<T>, std::string_view>)
     [[nodiscard]] static std::string str_to_lower(T input)
     {
          return str_to_lower(std::string{ input });
     }
     //[[nodiscard]] sf::Sprite                     save_intersecting(const glm::ivec2 &pixel_pos, const std::uint8_t &texture_page);
     [[nodiscard]] std::size_t                    get_texture_pos(BPPT bpp, std::uint8_t palette, std::uint8_t texture_page) const;
     [[nodiscard]] std::vector<std::future<void>> save_swizzle_textures(const std::string &keyed_string, const std::string &selected_path);
     [[nodiscard]] std::vector<std::future<void>>
       save_combined_swizzle_texture(const std::string &keyed_string, const std::string &selected_path);

     [[nodiscard]] std::vector<std::future<void>> save_pupu_textures(const std::string &keyed_string, const std::string &selected_path);
     [[nodiscard]] std::future<std::future<void>> load_upscale_textures(std::uint8_t texture_page) const;
     [[nodiscard]] std::future<std::future<void>> load_deswizzle_textures(const ff_8::PupuID pupu, const size_t pos) const;
     [[nodiscard]] std::future<std::future<void>> load_mim_textures(BPPT bpp, uint8_t palette) const;
     [[nodiscard]] std::future<std::future<void>> load_upscale_textures(std::uint8_t texture_page, std::uint8_t palette) const;

     void                                         save_modified_map(const std::filesystem::path &path) const;
     void                                         save(const std::filesystem::path &path) const;
     void                                         map_save(const std::filesystem::path &dest_path) const;
     void                                         test_map(const std::filesystem::path &saved_path) const;
     void                                         set_uniforms(const glengine::FrameBuffer &fbo, const glengine::Shader &shader) const;
     // void        disable_square() const;
     //  void        draw(sf::RenderTarget &target, sf::RenderStates states) const final;
     void                                         enable_draw_swizzle();
     void                                         disable_draw_swizzle();
     void                                         enable_disable_blends();
     void                                         disable_disable_blends();
     // void        enable_square(glm::uvec2 position);
     void                                         compact_move_conflicts_only();
     void                                         compact_map_order();
     void                                         compact_map_order_ffnx();
     void                                         first_to_working_and_original();
     void                                         undo();
     void                                         redo();
     void                                         undo_all();
     void                                         redo_all();
     void                                         update_render_texture(bool reload_textures = false) const;
     void                                         compact_rows();
     void                                         compact_all();
     void                                         flatten_bpp();
     void                                         flatten_palette();
     void                                         load_map(const std::filesystem::path &dest_path);
     void                                         resize_render_texture() const;
     void        update_render_texture(const glengine::Texture *p_texture, Map map, const tile_sizes tile_size);
     static void consume_futures(std::vector<std::future<void>> &futures);
     static void consume_futures(std::vector<std::future<std::future<void>>> &future_of_futures);
     void        update_position(const glm::ivec2 &pixel_pos, const uint8_t &texture_page, const glm::ivec2 &down_pixel_pos);
     bool        consume_one_future() const;
     void        consume_now() const;

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
       fmt::format_string<std::string_view, std::string_view, ff_8::PupuID> pattern,
       const std::filesystem::path                                         &path,
       const std::string_view                                              &field_name,
       const ff_8::PupuID                                                   pupu,
       const open_viii::LangT                                               coo);
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
       fmt::format_string<std::string_view, ff_8::PupuID> pattern,
       const std::filesystem::path                       &path,
       const std::string_view                            &field_name,
       ff_8::PupuID                                       pupu);

     [[nodiscard]] std::vector<std::size_t> find_intersecting(
       const open_viii::graphics::background::Map &map,
       const glm::ivec2                           &pixel_pos,
       const std::uint8_t                         &texture_page,
       bool                                        skip_filters = false,
       bool                                        find_all     = false) const;
     template<std::ranges::range tilesT>
     [[nodiscard]] std::vector<std::size_t> find_intersecting(
       const tilesT       &tiles,
       const glm::ivec2   &pixel_pos,
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
     auto const_visit_working_tiles(funcT &&p_function) const
     {
          return m_map_group.maps.const_working().visit_tiles(std::forward<decltype(p_function)>(p_function));
     }
     template<typename funcT>
     auto const_visit_original_tiles(funcT &&p_function) const
     {
          return m_map_group.maps.original().visit_tiles(std::forward<decltype(p_function)>(p_function));
     }
     template<typename funcT>
     auto const_visit_tiles_both(funcT &&p_function) const
     {
          return m_map_group.maps.const_working().visit_tiles([&](const auto &back) {
               return m_map_group.maps.original().visit_tiles([&](const auto &front) { return p_function(back, front); });
          });
     }

     template<open_viii::graphics::background::is_tile tileT>
     [[nodiscard]] const glengine::Texture *get_texture(const tileT &tile) const
     {

          if (!m_filters.deswizzle.enabled())
          {
               return get_texture(tile.depth(), tile.palette_id(), tile.texture_id());
          }
          else
          {
               // pupu_ids
               return const_visit_original_tiles([&tile, this](const auto &tiles) -> const glengine::Texture * {
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

                         if (std::cmp_greater(std::ranges::ssize(m_map_group.maps.original_pupu()), distance))
                         {
                              auto pupu_it = m_map_group.maps.original_pupu().cbegin();
                              std::ranges::advance(pupu_it, distance);
                              return get_texture(*pupu_it);
                         }
                         return static_cast<const glengine::Texture *>(nullptr);
                    }
                    else
                    {
                         return static_cast<const glengine::Texture *>(nullptr);
                    }
               });
          }
     }


     // template<open_viii::graphics::background::is_tile tile_type>
     // void enable_square(const tile_type &tile)
     // {
     //      using namespace open_viii::graphics::literals;
     //      auto       src_tpw = tile_type::texture_page_width(tile.depth());
     //      const auto x       = [&tile, &src_tpw]() -> std::uint32_t { return tile.texture_id() * src_tpw; }();
     //      const auto src_x   = [&tile, &x, this]() -> std::uint32_t {
     //           if (!m_filters.deswizzle.enabled())
     //           {
     //                return static_cast<std::uint32_t>(tile.x());
     //           }
     //           return tile.source_x() + x;
     //      }();
     //      const auto src_y = [&tile, this]() -> std::uint32_t {
     //           if (!m_filters.deswizzle.enabled())
     //           {
     //                return static_cast<std::uint32_t>(tile.y());
     //           }
     //           return tile.source_y();
     //      }();
     //      enable_square(glm::uvec2(src_x, src_y));
     // }

     auto duel_visitor(auto &&lambda) const
     {
          return m_map_group.maps.original().visit_tiles([this, &lambda](auto const &tiles_const) {
               return m_map_group.maps.working().visit_tiles([&lambda, &tiles_const](const auto &tiles) {
                    return std::invoke(std::forward<decltype(lambda)>(lambda), tiles_const, tiles);
               });
          });
     }
     auto duel_visitor(auto &&lambda)
     {
          return m_map_group.maps.original().visit_tiles([this, &lambda](auto const &tiles_const) {
               return m_map_group.maps.working().visit_tiles([&lambda, &tiles_const](auto &&tiles) {
                    return std::invoke(std::forward<decltype(lambda)>(lambda), tiles_const, std::forward<decltype(tiles)>(tiles));
               });
          });
     }
     void for_all_tiles(auto const &tiles_const, auto &&tiles, auto &&lambda, bool skip_invalid, bool regular_order) const
     {
          using namespace open_viii::graphics::background;
          m_map_group.maps.refresh_original_all();
          const auto &pupu_ids = m_map_group.maps.original_pupu();
          namespace v          = std::ranges::views;
          namespace r          = std::ranges;
          auto zipped_range    = v::zip(tiles_const, tiles, pupu_ids)
                              | v::filter([&](const auto &current) { return !skip_invalid || std::apply(Map::filter_invalid(), current); });
          if (!regular_order)
          {
               for (decltype(auto) current : zipped_range | v::reverse)
               {
                    // spdlog::info(" {} -- {} -- {} ", std::get<0>(current), std::get<1>(current), std::get<2>(current));
                    std::apply(lambda, std::forward<decltype(current)>(current));
               }
          }
          else
          {
               for (decltype(auto) current : zipped_range)
               {
                    // spdlog::info(" {} -- {} -- {} ", std::get<0>(current), std::get<1>(current), std::get<2>(current));
                    std::apply(lambda, std::forward<decltype(current)>(current));
               }
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

     [[nodiscard]] std::string current_undo_description() const;

     [[nodiscard]] std::string current_redo_description() const;

     [[nodiscard]] auto        undo_history() const
     {
          return m_map_group.maps.undo_history();
     }

     [[nodiscard]] auto redo_history() const
     {
          return m_map_group.maps.redo_history();
     }


     [[nodiscard]] auto get_conflicting_palettes() const
     {
          return m_map_group.maps.working().visit_tiles([this](const auto &tiles) { return find_conflicting_tiles(tiles); });
     }
     [[nodiscard]] ::upscales get_upscales()
     {
          const auto field = m_map_group.field.lock();
          if (!field)
          {
               return { m_selections };
          }
          return { std::string{ field->get_base_name() }, m_map_group.opt_coo, m_selections };
     }

     static glm::vec2 to_vec2(const glm::ivec2 &v)
     {
          return glm::vec2(v);
     }
     static glm::vec2 to_vec2(const glm::uvec2 &v)
     {
          return glm::vec2(v);
     }

     [[nodiscard]] ff_8::QuadStrip get_triangle_strip_for_imported(
       const glm::uvec2                                    &source_tile_size,
       const glm::uvec2                                    &destination_tile_size,
       const glm::uvec2                                    &source_texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile) const
     {
          return get_triangle_strip(source_tile_size, destination_tile_size, source_texture_size, tile_const, tile, true);
     }
     [[nodiscard]] ff_8::QuadStrip get_triangle_strip(
       const glm::uvec2                                    &source_tile_size,
       const glm::uvec2                                    &destination_tile_size,
       const glm::uvec2                                    &source_texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile,
       bool                                                 imported = false) const
     {
          const auto src = [this, &tile_const, &imported]() {
               if (imported)
               {
                    return to_vec2(ff_8::get_triangle_strip_source_imported(tile_const));
               }
               if (m_filters.upscale.enabled())
               {
                    return to_vec2(ff_8::get_triangle_strip_source_upscale(tile_const));
               }
               if (m_filters.deswizzle.enabled())
               {
                    return to_vec2(ff_8::get_triangle_strip_source_deswizzle(tile_const));
               }
               return to_vec2(ff_8::get_triangle_strip_source_default(tile_const));
          }();
          const auto dest = [this, &tile]() {
               if (m_draw_swizzle)
               {
                    if (m_disable_texture_page_shift)
                    {
                         return to_vec2(ff_8::get_triangle_strip_dest_swizzle_disable_shift(tile));
                    }
                    return to_vec2(ff_8::get_triangle_strip_dest_swizzle(tile));
               }
               return to_vec2(ff_8::get_triangle_strip_dest_default(tile));
          }();
          return ff_8::get_triangle_strip(
            to_vec2(source_tile_size), to_vec2(destination_tile_size), to_vec2(source_texture_size), src, dest);
     }

     [[nodiscard]] std::vector<std::filesystem::path> generate_deswizzle_paths(const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_paths(const std::string &ext) const;

     [[nodiscard]] std::vector<std::filesystem::path>
       generate_deswizzle_paths(const std::filesystem::path &path, const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_swizzle_paths(const std::filesystem::path &path, const std::string &ext) const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_deswizzle_map_paths(const std::string &ext = ".map") const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_map_paths(const std::string &ext = ".map") const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_deswizzle_map_paths(const std::filesystem::path &path, const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_swizzle_map_paths(const std::filesystem::path &path, const std::string &ext) const;

     [[nodiscard]] std::vector<std::filesystem::path>
       generate_deswizzle_paths(const ff_8::PupuID pupu, const std::string &ext = ".png") const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_swizzle_paths(const std::uint8_t texture_page, std::uint8_t palette, const std::string &ext = ".png") const;
     [[nodiscard]] std::vector<std::filesystem::path>
                        generate_swizzle_paths(const std::uint8_t texture_page, const std::string &ext = ".png") const;


     [[nodiscard]] bool has_map_path(
       const std::filesystem::path &filter_path,
       const std::string           &ext                      = ".map",
       const std::string           &secondary_output_pattern = "") const;
     [[nodiscard]] bool has_deswizzle_path(const ff_8::PupuID pupu, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(const std::uint8_t texture_page, std::uint8_t palette, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(const std::uint8_t texture_page, const std::string &ext = ".png") const;

     [[nodiscard]] bool has_deswizzle_path(const std::filesystem::path &filter_path, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(const std::filesystem::path &filter_path, const std::string &ext = ".png") const;
     [[nodiscard]] bool
       has_deswizzle_path(const std::filesystem::path &filter_path, const ff_8::PupuID pupu, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(
       const std::filesystem::path &filter_path,
       const std::uint8_t           texture_page,
       std::uint8_t                 palette,
       const std::string           &ext = ".png") const;
     bool
       has_swizzle_path(const std::filesystem::path &filter_path, const std::uint8_t texture_page, const std::string &ext = ".png") const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_paths(
       const std::string    &filter_path,
       const key_value_data &cpm,
       const std::string    &output_pattern           = "",
       const std::string    &secondary_output_pattern = "") const;
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_MAP_SPRITE_HPP
