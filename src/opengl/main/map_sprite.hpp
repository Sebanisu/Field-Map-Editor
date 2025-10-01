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
#include "path_search.hpp"
#include "RangeConsumer.hpp"
#include "settings_backup.hpp"
#include "tile_sizes.hpp"
#include "unique_values.hpp"
#include <algorithm>
#include <BatchRenderer.hpp>
#include <CompShader.hpp>
#include <cstdint>
#include <DistanceBuffer.hpp>
#include <fmt/format.h>
#include <FrameBuffer.hpp>
#include <HistogramBuffer.hpp>
#include <OrthographicCamera.hpp>
#include <PaletteBuffer.hpp>
#include <set>
#include <Shader.hpp>
#include <Texture.hpp>
#include <utility>

namespace fme
{


struct PupuOpEntry
{
     ff_8::PupuID         pupu;
     int                  color_index;
     glengine::SubTexture main_texture;
     glengine::SubTexture mask_texture;
     std::uint32_t        count;
     float                distance;
};

enum class DrawError
{
     None,
     NoTexture,
     ZeroSizedTexture,
     FilteredOut,
     PupuIdZero,
     Other
};

inline std::strong_ordering operator<=>(
  DrawError a,
  DrawError b)
{
     return std::to_underlying(a) <=> std::to_underlying(b);
}
}// namespace fme

template<>
struct fmt::formatter<fme::DrawError> : fmt::formatter<std::string_view>
{// parse is inherited from formatter<string_view>.
     template<typename FormatContext>
     constexpr auto format(
       fme::DrawError in_draw_error,
       FormatContext &ctx) const
     {
          using namespace std::string_view_literals;
          std::string_view name = {};
          switch (in_draw_error)
          {
               case fme::DrawError::None:
                    name = "NoTexture";
                    break;
               case fme::DrawError::NoTexture:
                    name = "NoTexture";
                    break;
               case fme::DrawError::ZeroSizedTexture:
                    name = "ZeroSizedTexture";
                    break;
               case fme::DrawError::FilteredOut:
                    name = "FilteredOut";
                    break;
               case fme::DrawError::PupuIdZero:
                    name = "PupuIdZero";
                    break;
               case fme::DrawError::Other:
                    name = "Other";
                    break;
          }
          return fmt::formatter<std::string_view>::format(name, ctx);
     }
};

namespace fme
{
// Optional: For more descriptive errors
struct DrawFailure
{
     DrawError   error   = DrawError::None;
     std::string message = "";


     DrawFailure(DrawError in_error)
       : error(in_error)
     {
     }

     DrawFailure(std::string in_message)
       : error(DrawError::Other)
       , message(std::move(in_message))
     {
     }

     DrawFailure(
       DrawError   in_error,
       std::string in_message)
       : error(in_error)
       , message(std::move(in_message))
     {
     }

     auto operator<=>(const DrawFailure &other) const = default;
};
}// namespace fme

template<>
struct fmt::formatter<fme::DrawFailure>
{
     // parse is default; no custom format parsing needed
     template<typename ParseContext>
     constexpr auto parse(ParseContext &ctx)
     {
          return ctx.begin();
     }

     template<typename FormatContext>
     auto format(
       fme::DrawFailure df,
       FormatContext   &ctx) const
     {
          using enum fme::BackgroundSettings;


          bool       first      = true;
          const auto write_part = [&](const auto &part)
          {
               if (first)
               {
                    fmt::format_to(ctx.out(), "{} : ", part);
                    first = false;
               }
               else
               {
                    fmt::format_to(ctx.out(), "\"{}\"", part);
               }
          };

          if (df.message.empty())
          {
               first = false;
               write_part(df.error);
          }
          else
          {
               write_part(df.error);
               write_part(df.message);
          }
          return ctx.out();
     }
};

namespace fme
{
struct [[nodiscard]] map_sprite// final
//   : public sf::Drawable
//   , public sf::Transformable
{
   public:
     static constexpr std::uint8_t  TILE_SIZE         = 16U;
     static constexpr std::uint8_t  MAX_TEXTURE_PAGES = 14U;
     static constexpr std::uint8_t  MAX_PALETTES      = 16U;
     static constexpr std::uint8_t  BPP_COMBOS        = 2U;
     static constexpr std::uint16_t START_OF_NO_PALETTE_INDEX
       = MAX_PALETTES * MAX_TEXTURE_PAGES;
     static constexpr std::uint16_t BPP16_INDEX = MAX_PALETTES * BPP_COMBOS + 1;
     static constexpr auto          MAX_TEXTURES
       = (std::max)(static_cast<std::uint16_t>(
                      START_OF_NO_PALETTE_INDEX + MAX_TEXTURE_PAGES),
                    static_cast<std::uint16_t>(BPP16_INDEX + 1U));

     using BPPT        = open_viii::graphics::BPPT;
     using WeakField   = std::weak_ptr<open_viii::archive::FIFLFS<false>>;
     using Map         = open_viii::graphics::background::Map;
     using Mim         = open_viii::graphics::background::Mim;
     using color_type  = open_viii::graphics::Color32RGBA;
     using colors_type = std::vector<color_type>;
     using BlendModeT  = open_viii::graphics::background::BlendModeT;
     using Rectangle   = open_viii::graphics::Rectangle<std::uint32_t>;
     using iRectangle  = open_viii::graphics::Rectangle<std::int32_t>;

   private:
     std::shared_ptr<std::array<glengine::Texture, MAX_TEXTURES>> m_texture
       = std::make_shared<std::array<glengine::Texture, MAX_TEXTURES>>();
     std::shared_ptr<std::map<std::string, glengine::Texture>>
       m_full_filename_textures
       = std::make_shared<std::map<std::string, glengine::Texture>>();
     mutable std::map<std::string, std::string> m_full_filename_to_mask_name
       = {};
     mutable std::map<std::string, std::optional<glengine::FrameBuffer>>
                                                m_cache_framebuffer = {};
     mutable std::map<std::string, std::string> m_cache_framebuffer_tooltips
       = {};
     mutable std::map<std::string, std::vector<ff_8::PupuID>>
                                         m_cache_framebuffer_pupuids = {};
     ff_8::map_group                     m_map_group                 = {};
     map_sprite_settings                 m_settings                  = {};

     mutable std::unique_ptr<map_sprite> m_child_map_sprite = { nullptr };
     mutable std::map<std::string, std::optional<glengine::FrameBuffer>>
                           m_child_textures_map = {};
     mutable ff_8::filters m_filters
       = { false };// default false should be override by gui to true.
     std::weak_ptr<Selections>           m_selections             = {};
     bool                                m_using_imported_texture = {};
     const glengine::Texture            *m_imported_texture       = { nullptr };
     std::uint16_t                       m_imported_tile_size     = {};
     Map                                 m_imported_tile_map      = {};
     Map                                 m_imported_tile_map_front       = {};
     ff_8::all_unique_values_and_strings m_all_unique_values_and_strings = {};
     open_viii::graphics::Rectangle<std::uint32_t> m_canvas              = {};

     glengine::BatchRenderer                       m_batch_renderer = { 1000 };
     // glengine::BatchRenderer m_batch_renderer_red_integer    = { 1,
     //                                                                                    { std::filesystem::current_path() / "res" /
     //                                                                                    "shader"
     //                                                                                      / "red_integer.shader" } };
     std::expected<
       glengine::CompShader,
       std::string>
       m_mask_comp_shader = [&]() -> std::expected<
                                    glengine::CompShader,
                                    std::string>
     {
          try
          {
               return glengine::CompShader(
                 std::filesystem::current_path() / "res" / "shader"
                 / "mask.comp");
          }
          catch (const std::filesystem::filesystem_error &e)
          {
               return std::unexpected(
                 std::string("Filesystem error: ") + e.what());
          }
          catch (const std::exception &e)
          {
               return std::unexpected(std::string("Error: ") + e.what());
          }
     }();

     std::expected<
       glengine::CompShader,
       std::string>
       m_mask_count_comp_shader = [&]() -> std::expected<
                                          glengine::CompShader,
                                          std::string>
     {
          try
          {
               return glengine::CompShader(
                 std::filesystem::current_path() / "res" / "shader"
                 / "mask_count.comp");
          }
          catch (const std::filesystem::filesystem_error &e)
          {
               return std::unexpected(
                 std::string("Filesystem error: ") + e.what());
          }
          catch (const std::exception &e)
          {
               return std::unexpected(std::string("Error: ") + e.what());
          }
     }();


     mutable std::shared_ptr<glengine::FrameBuffer> m_render_framebuffer = {};

     std::vector<std::size_t>             m_saved_imported_indices       = {};
     mutable bool                         once                  = { true };
     mutable glengine::OrthographicCamera m_fixed_render_camera = {};

     static constexpr auto                s_default_color
       = glm::vec4{ 1.F, 1.F, 1.F, 1.F };// white, fully opaque
     static constexpr auto s_half_color
       = glm::vec4{ 0.5F, 0.5F, 0.5F, 1.F };// dimmed, fully opaque
     static constexpr auto s_quarter_color
       = glm::vec4{ 0.25F, 0.25F, 0.25F, 1.F };// very dim, fully opaque

     mutable glm::vec4 m_uniform_color = s_default_color;


     mutable FutureConsumer<std::vector<std::future<void>>> m_future_consumer
       = {};
     mutable FutureOfFutureConsumer<std::vector<std::future<std::future<void>>>>
                                   m_future_of_future_consumer = {};

     [[nodiscard]] settings_backup get_backup_settings(const bool);

     void                          purge_empty_full_filename_texture() const;
     [[nodiscard]] bool check_all_masks_exists_full_filename_texture() const;
     void               load_child_map_sprite_full_filename_texture() const;
     void
       generate_combined_textures_for_child_map_sprite_full_filename_texture()
         const;
     void post_op_full_filename_texture() const;
     std::tuple<
       glengine::PaletteBuffer,
       glengine::HistogramBuffer,
       glengine::DistanceBuffer>
       initialize_buffers(const std::vector<glm::vec4> &palette) const;

     std::pair<
       std::vector<PupuOpEntry>,
       std::vector<std::string>>
       collect_post_op_entries(
         std::tuple<
           glengine::PaletteBuffer,
           glengine::HistogramBuffer,
           glengine::DistanceBuffer> &buffers) const;

     void process_post_op_entries(
       const std::vector<PupuOpEntry> &multi_pupu_post_op,
       glengine::PaletteBuffer        &pb) const;
     void cleanup_full_filename_textures(
       const std::vector<std::string> &remove_queue) const;

   public:
     map_sprite() = default;
     map_sprite(
       ff_8::map_group                        map_group,
       map_sprite_settings                    settings,
       ff_8::filters                          in_filters,
       std::weak_ptr<Selections>              selections,
       std::shared_ptr<glengine::FrameBuffer> framebuffer
       = std::make_shared<glengine::FrameBuffer>());
     explicit operator ff_8::path_search() const;

     [[nodiscard]] std::optional<open_viii::LangT> get_opt_coo() const;

     [[nodiscard]] std::string
       appends_prefix_base_name(std::string_view title) const;

     [[nodiscard]] const glengine::FrameBuffer &get_framebuffer() const
     {
          assert(m_render_framebuffer && "frame buffer is null ptr");
          return *m_render_framebuffer;
     }
     [[nodiscard]] const std::array<
       glengine::Texture,
       MAX_TEXTURES> &
       get_textures() const;
     [[nodiscard]] const std::map<
       std::string,
       glengine::Texture> &
       get_full_filename_textures();
     [[nodiscard]] const std::map<
       std::string,
       std::optional<glengine::FrameBuffer>>                                      &
       child_textures_map() const;
     [[nodiscard]] const glengine::Texture *get_texture(
       BPPT         bpp,
       std::uint8_t palette,
       std::uint8_t texture_page) const;
     [[nodiscard]] glengine::Texture *
       get_texture_mutable(const ff_8::PupuID &pupu) const;
     [[nodiscard]] const glengine::Texture *
       get_texture(const ff_8::PupuID &pupu) const;
     [[nodiscard]] glm::uvec2
       get_tile_texture_size(const glengine::Texture *const texture) const;
     std::expected<
       void,
       std::set<DrawFailure>>
       generate_texture(const glengine::FrameBuffer &texture) const;
     [[nodiscard]] std::uint32_t get_max_texture_height() const;
     std::expected<
       void,
       std::set<DrawFailure>>
       local_draw(
         const glengine::FrameBuffer   &target_framebuffer,
         const glengine::BatchRenderer &target_renderer) const;
     [[nodiscard]] bool draw_imported(const glengine::FrameBuffer &) const;
     [[nodiscard]] std::string get_base_name() const;
     [[nodiscard]] const ff_8::all_unique_values_and_strings &uniques() const;
     [[nodiscard]] const std::vector<ff_8::PupuID> &working_unique_pupu() const;
     [[nodiscard]] std::vector<std::tuple<
       glm::vec4,
       ff_8::PupuID>>
       working_unique_color_pupu() const;
     [[nodiscard]] const std::vector<ff_8::PupuID> &
       original_unique_pupu() const;
     [[nodiscard]] const std::vector<ff_8::PupuID> &original_pupu() const;
     [[nodiscard]] const std::vector<ff_8::PupuID> &working_pupu() const;
     [[nodiscard]] const ff_8::source_tile_conflicts &
       original_conflicts() const;
     [[nodiscard]] const ff_8::source_tile_conflicts &working_conflicts() const;
     [[nodiscard]] const ff_8::MapHistory::nst_map &
       working_similar_counts() const;
     [[nodiscard]] const ff_8::MapHistory::nsat_map                           &
       working_animation_counts() const;
     [[nodiscard]] std::string   map_filename() const;
     [[nodiscard]] bool          fail() const;
     [[nodiscard]] std::uint32_t width() const;
     [[nodiscard]] std::uint32_t height() const;
     [[nodiscard]] map_sprite    with_coo(open_viii::LangT coo) const;
     [[nodiscard]] map_sprite    with_field(
          WeakField        field,
          open_viii::LangT coo) const;
     [[nodiscard]] map_sprite with_filters(ff_8::filters filters) const;
     [[nodiscard]] bool       empty() const;
     [[nodiscard]] const ff_8::filters &filter() const;
     [[nodiscard]] map_sprite           update(
                 ff_8::map_group map_group,
                 bool            draw_swizzle) const;
     [[nodiscard]] ff_8::all_unique_values_and_strings
                                      get_all_unique_values_and_strings() const;
     [[nodiscard]] glm::uvec2         get_tile_texture_size_for_import() const;
     [[nodiscard]] Rectangle          get_canvas() const;
     [[nodiscard]] bool               undo_enabled() const;
     [[nodiscard]] bool               redo_enabled() const;
     [[nodiscard]] ff_8::filters     &filter();
     //[[nodiscard]] static sf::BlendMode                 set_blend_mode(const
     // BlendModeT &blend_mode, std::array<sf::Vertex, 4U> &quad);
     [[nodiscard]] bool               fallback_textures() const;
     void                             queue_texture_loading() const;
     [[nodiscard]] static colors_type get_colors(
       const Mim &mim,
       BPPT       bpp,
       uint8_t    palette);
     // [[nodiscard]] static const sf::BlendMode          &get_blend_subtract();
     [[nodiscard]] static std::future<std::future<void>> async_save(
       const glengine::Texture     &out_texture,
       const std::filesystem::path &out_path);
     void                             process_full_filename_textures() const;
     [[nodiscard]] bool               using_coo() const;
     [[nodiscard]] static std::string str_to_lower(std::string input);
     template<typename T>
          requires(std::same_as<
                   std::remove_cvref_t<T>,
                   std::string_view>)
     [[nodiscard]] static std::string str_to_lower(T input)
     {
          return str_to_lower(std::string{ input });
     }
     //[[nodiscard]] sf::Sprite                     save_intersecting(const
     // glm::ivec2 &pixel_pos, const std::uint8_t &texture_page);
     [[nodiscard]] std::size_t get_texture_pos(
       BPPT         bpp,
       std::uint8_t palette,
       std::uint8_t texture_page) const;
     [[nodiscard]] std::vector<std::future<void>> save_swizzle_textures(
       const std::string           &keyed_string,
       const std::filesystem::path &selected_path);
     [[nodiscard]] std::vector<std::future<void>>
       save_swizzle_as_one_image_textures(
         const std::string           &keyed_string,
         const std::filesystem::path &selected_path);
     [[nodiscard]] std::vector<std::future<void>> save_csv(
       const std::string           &keyed_string,
       const std::filesystem::path &selected_path);
     [[nodiscard]] std::vector<std::future<void>> save_deswizzle_textures(
       const std::string           &keyed_string,
       const std::filesystem::path &selected_path);
     void save_deswizzle_generate_toml(
       const std::string           &keyed_string,
       const std::filesystem::path &selected_path);
     void save_deswizzle_generate_toml(
       const std::string                 &keyed_string,
       const std::filesystem::path       &selected_path,
       toml::table                       &coo_table,
       const std::shared_ptr<Selections> &selections);
     void cache_pupuids(
       const std::string &,
       const ff_8::filters &) const;
     [[nodiscard]] const std::map<
       std::string,
       std::string> &
       get_deswizzle_combined_textures_tooltips();
     [[nodiscard]] const std::map<
       std::string,
       std::vector<ff_8::PupuID>> &
       get_deswizzle_combined_textures_pupuids();
     [[nodiscard]] std::expected<
       std::map<
         std::string,
         std::optional<glengine::FrameBuffer>> *,
       std::string>
       get_deswizzle_combined_textures(
         const int  scale      = {},
         const bool force_load = false);
     [[nodiscard]] std::string generate_deswizzle_combined_tool_tip(
       const toml::table *file_table) const;
     [[nodiscard]] std::vector<ff_8::PupuID>
       generate_deswizzle_combined_pupu_id(const toml::table *file_table) const;
     [[nodiscard]] open_viii::LangT
       get_used_coo(const fme::FailOverLevels max_failover) const;
     [[nodiscard]] toml::table *get_deswizzle_combined_coo_table(
       open_viii::LangT   *out_used_coo = nullptr,
       fme::FailOverLevels failover     = fme::FailOverLevels::Loaded) const;
     [[nodiscard]] toml::table *
       get_deswizzle_combined_toml_table(const std::string &) const;
     [[nodiscard]] std::vector<std::string> toml_filenames() const;
     [[nodiscard]] std::string              get_recommended_prefix();
     [[nodiscard]] toml::table *rename_deswizzle_combined_toml_table(
       const std::string &,
       const std::string &);
     [[nodiscard]] std::size_t
       remove_deswizzle_combined_toml_table(const std::string &);
     [[nodiscard]] toml::table    *
       add_deswizzle_combined_toml_table(const std::string &);
     void refresh_tooltip(const std::string &);
     void apply_multi_pupu_filter_deswizzle_combined_toml_table(
       const std::string                                  &file_name_key,
       const ff_8::filter_old<ff_8::FilterTag::MultiPupu> &new_filter);
     [[nodiscard]] toml::table *add_combine_deswizzle_combined_toml_table(
       const std::vector<std::string> &,
       const std::string &);
     void copy_deswizzle_combined_toml_table(
       const std::vector<std::string> &,
       std::move_only_function<std::string(void)>);
     [[nodiscard]] std::vector<std::future<void>>
       save_deswizzle_full_filename_textures(
         const std::string           &keyed_string,
         const std::filesystem::path &selected_path);
     [[nodiscard]] std::future<std::future<void>> load_swizzle_textures(
       std::uint8_t texture_page,
       std::uint8_t palette) const;
     [[nodiscard]] std::future<std::future<void>>
       load_swizzle_textures(std::uint8_t texture_page) const;
     [[nodiscard]] std::future<std::future<void>>
       load_swizzle_as_one_image_textures(
         std::optional<std::uint8_t> palette = std::nullopt) const;
     [[nodiscard]] std::future<std::future<void>> load_deswizzle_textures(
       const ff_8::PupuID pupu,
       const size_t       pos) const;
     [[nodiscard]] std::future<std::future<void>>
       load_full_filename_textures(const std::string filename) const;
     [[nodiscard]] std::future<std::future<void>> load_mim_textures(
       BPPT    bpp,
       uint8_t palette) const;

     void save_modified_map(const std::filesystem::path &path) const;
     void save(const std::filesystem::path &path) const;
     void save_map(const std::filesystem::path &dest_path) const;
     void test_map(const std::filesystem::path &saved_path) const;
     void set_uniforms(
       const glengine::FrameBuffer &fbo,
       const glengine::Shader      &shader) const;
     // void        disable_square() const;
     //  void        draw(sf::RenderTarget &target, sf::RenderStates states)
     //  const final;
     void enable_draw_swizzle();
     void disable_draw_swizzle();
     void enable_disable_blends();
     void disable_disable_blends();
     // void        enable_square(glm::uvec2 position);
     void first_to_working_and_original(const bool skip_update = false);
     void begin_multi_frame_working(std::string description);
     void end_multi_frame_working(std::string description = {});
     void undo();
     void redo();
     void undo_all();
     void redo_all();
     void clear_toml_cached_framebuffers() const;
     void update_render_texture(bool reload_textures = false) const;
     void compact_rows();
     void compact_all();
     void compact_move_conflicts_only();
     void compact_map_order();
     void compact_map_order_ffnx();
     void flatten_bpp();
     void flatten_palette();
     void compact_rows_original(const bool skip_update);
     void compact_all_original(const bool skip_update);
     void compact_move_conflicts_only_original(const bool skip_update);
     void compact_map_order_original(const bool skip_update);
     void compact_map_order_ffnx_original(const bool skip_update);
     void flatten_bpp_original(const bool skip_update);
     void flatten_palette_original(const bool skip_update);
     void toggle_filter_compact_on_load_original(
       const std::optional<bool> state = std::nullopt);
     void toggle_filter_flatten_on_load_original(
       const std::optional<bool> state = std::nullopt);
     void load_map(
       const std::filesystem::path &dest_path,
       const bool                   skip_update = false);
     void resize_render_texture() const;
     void update_render_texture(
       const glengine::Texture *p_texture,
       Map                      map,
       const tile_sizes         tile_size);
     static void consume_futures(std::vector<std::future<void>> &futures);
     static void consume_futures(
       std::vector<std::future<std::future<void>>> &future_of_futures);
     void update_position(
       const glm::ivec2               &pixel_pos,
       const glm::ivec2               &down_pixel_pos,
       const std::vector<std::size_t> &saved_indices);
     bool                         consume_one_future() const;
     void                         consume_now(const bool update = true) const;
     bool                         all_futures_done() const;

     static std::filesystem::path save_path_coo(
       fmt::format_string<
         std::string_view,
         std::string_view,
         uint8_t>                   pattern,
       const std::filesystem::path &path,
       const std::string_view      &field_name,
       const uint8_t                texture_page,
       const open_viii::LangT       coo);
     static std::filesystem::path save_path_coo(
       fmt::format_string<
         std::string_view,
         std::string_view,
         uint8_t,
         uint8_t>                   pattern,
       const std::filesystem::path &path,
       const std::string_view      &field_name,
       const uint8_t                texture_page,
       const uint8_t                palette,
       const open_viii::LangT       coo);
     static std::filesystem::path save_path_coo(
       fmt::format_string<
         std::string_view,
         std::string_view,
         ff_8::PupuID>              pattern,
       const std::filesystem::path &path,
       const std::string_view      &field_name,
       const ff_8::PupuID           pupu,
       const open_viii::LangT       coo);
     static std::filesystem::path save_path(
       fmt::format_string<
         std::string_view,
         uint8_t>                   pattern,
       const std::filesystem::path &path,
       const std::string_view      &field_name,
       uint8_t                      texture_page);
     static std::filesystem::path save_path(
       fmt::format_string<
         std::string_view,
         uint8_t,
         uint8_t>                   pattern,
       const std::filesystem::path &path,
       const std::string_view      &field_name,
       uint8_t                      texture_page,
       uint8_t                      palette);
     static std::filesystem::path save_path(
       fmt::format_string<
         std::string_view,
         ff_8::PupuID>              pattern,
       const std::filesystem::path &path,
       const std::string_view      &field_name,
       ff_8::PupuID                 pupu);

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
          if (m_settings.draw_swizzle)
          {
               return ff_8::find_intersecting_swizzle(
                 tiles,
                 m_filters,
                 pixel_pos,
                 texture_page,
                 skip_filters,
                 find_all);
          }
          return ff_8::find_intersecting_deswizzle(
            tiles, m_filters, pixel_pos, skip_filters, find_all);
     }
     template<typename funcT>
     auto const_visit_working_tiles(funcT &&p_function) const
     {
          return m_map_group.maps.const_working().visit_tiles(
            std::forward<decltype(p_function)>(p_function));
     }
     template<typename funcT>
     auto const_visit_original_tiles(funcT &&p_function) const
     {
          return m_map_group.maps.original().visit_tiles(
            std::forward<decltype(p_function)>(p_function));
     }
     template<typename funcT>
     auto const_visit_tiles_both(funcT &&p_function) const
     {
          return m_map_group.maps.const_working().visit_tiles(
            [&](const auto &back)
            {
                 return m_map_group.maps.original().visit_tiles(
                   [&](const auto &front) { return p_function(back, front); });
            });
     }

     template<open_viii::graphics::background::is_tile tileT>
     [[nodiscard]] const glengine::Texture *get_texture(const tileT &tile) const
     {

          if (
            !m_filters.deswizzle.enabled()
            && !m_filters.full_filename.enabled())
          {
               return get_texture(
                 tile.depth(), tile.palette_id(), tile.texture_id());
          }
          else
          {
               // pupu_ids
               return const_visit_original_tiles(
                 [&tile, this](const auto &tiles) -> const glengine::Texture *
                 {
                      if (tiles.empty())
                      {
                           return nullptr;
                      }
                      using TileT1 = std::remove_cvref<decltype(tiles.front())>;
                      using TileT2 = std::remove_cvref<decltype(tile)>;
                      if constexpr (std::is_same_v<TileT1, TileT2>)
                      {
                           const auto found_iterator = std::ranges::find_if(
                             tiles,
                             [&tile](const auto &l_tile)
                             { return l_tile == tile; });
                           const auto distance = std::ranges::distance(
                             tiles.begin(), found_iterator);

                           if (std::cmp_greater(
                                 std::ranges::ssize(
                                   m_map_group.maps.original_pupu()),
                                 distance))
                           {
                                auto pupu_it
                                  = m_map_group.maps.original_pupu().cbegin();
                                std::ranges::advance(pupu_it, distance);
                                return get_texture(*pupu_it);
                           }
                           return static_cast<const glengine::Texture *>(
                             nullptr);
                      }
                      else
                      {
                           return static_cast<const glengine::Texture *>(
                             nullptr);
                      }
                 });
          }
     }


     // template<open_viii::graphics::background::is_tile tile_type>
     // void enable_square(const tile_type &tile)
     // {
     //      using namespace open_viii::graphics::literals;
     //      auto       src_tpw = tile_type::texture_page_width(tile.depth());
     //      const auto x       = [&tile, &src_tpw]() -> std::uint32_t { return
     //      tile.texture_id() * src_tpw; }(); const auto src_x   = [&tile, &x,
     //      this]() -> std::uint32_t {
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
          return m_map_group.maps.original().visit_tiles(
            [this, &lambda](auto const &tiles_const)
            {
                 return m_map_group.maps.working().visit_tiles(
                   [&lambda, &tiles_const](const auto &tiles)
                   {
                        return std::invoke(
                          std::forward<decltype(lambda)>(lambda),
                          tiles_const,
                          tiles);
                   });
            });
     }
     auto duel_visitor(auto &&lambda)
     {
          return m_map_group.maps.original().visit_tiles(
            [this, &lambda](auto const &tiles_const)
            {
                 return m_map_group.maps.working().visit_tiles(
                   [&lambda, &tiles_const](auto &&tiles)
                   {
                        return std::invoke(
                          std::forward<decltype(lambda)>(lambda),
                          tiles_const,
                          std::forward<decltype(tiles)>(tiles));
                   });
            });
     }
     void for_all_tiles(
       auto const &tiles_const,
       auto      &&tiles,
       auto      &&lambda,
       bool        skip_invalid,
       bool        regular_order) const
     {
          using namespace open_viii::graphics::background;
          m_map_group.maps.refresh_original_all();
          auto pupu_ids = m_map_group.maps.original_pupu();
          pupu_ids.push_back({});
          namespace v = std::ranges::views;
          namespace r = std::ranges;
          const bool same_size
            = (tiles_const.size() == tiles.size() && tiles.size() == pupu_ids.size());
          if (!same_size)
          {
               spdlog::error(
                 "tiles_const: {}, tiles: {}, pupu_ids: {}, same size: {}",
                 tiles_const.size(),
                 tiles.size(),
                 pupu_ids.size(),
                 same_size);
          }
          auto zipped_range
            = v::zip(tiles_const, tiles, pupu_ids)
              | v::filter(
                [&](const auto &current)
                {
                     return !skip_invalid
                            || std::apply(Map::filter_invalid(), current);
                });
          if (!regular_order)
          {
               for (decltype(auto) current : zipped_range | v::reverse)
               {
                    // spdlog::info(" {} -- {} -- {} ", std::get<0>(current),
                    // std::get<1>(current), std::get<2>(current));
                    std::apply(
                      lambda, std::forward<decltype(current)>(current));
               }
          }
          else
          {
               for (decltype(auto) current : zipped_range)
               {
                    // spdlog::info(" {} -- {} -- {} ", std::get<0>(current),
                    // std::get<1>(current), std::get<2>(current));
                    std::apply(
                      lambda, std::forward<decltype(current)>(current));
               }
          }
     }
     void for_all_tiles(
       auto &&lambda,
       bool   skip_invalid  = true,
       bool   regular_order = false) const
     {
          duel_visitor(
            [&lambda, &skip_invalid, &regular_order, this](
              auto const &tiles_const, auto &&tiles)
            {
                 this->for_all_tiles(
                   tiles_const,
                   std::forward<decltype(tiles)>(tiles),
                   std::forward<decltype(lambda)>(lambda),
                   skip_invalid,
                   regular_order);
            });
     }


     template<typename TilesT>
     auto find_conflicting_tiles(const TilesT &tiles) const
     {
          using TileT = std::ranges::range_value_t<std::remove_cvref_t<TilesT>>;
          using TextureIdT = ff_8::tile_operations::TextureIdT<TileT>;
          using PaletteIdT = ff_8::tile_operations::PaletteIdT<TileT>;
          static constexpr auto SourceX   = ff_8::tile_operations::SourceX{};
          static constexpr auto SourceY   = ff_8::tile_operations::SourceY{};
          static constexpr auto TextureId = ff_8::tile_operations::TextureId{};
          static constexpr auto PaletteId = ff_8::tile_operations::PaletteId{};
          std::map<TextureIdT, std::vector<PaletteIdT>> conflicts;

          // Process each pair of matching tiles and insert their palette IDs
          // into the conflicts map
          for (const auto &first : tiles)
          {
               for (const auto &second : tiles | std::views::drop(1))
               {
                    if (
                      &first != &second && PaletteId(first) != PaletteId(second)
                      && TextureId(first) == TextureId(second)
                      && SourceX(first) == SourceX(second)
                      && SourceY(first) == SourceY(second))
                    {
                         conflicts[TextureId(first)].push_back(
                           PaletteId(first));
                         conflicts[TextureId(second)].push_back(
                           PaletteId(second));
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
          return m_map_group.maps.working().visit_tiles(
            [this](const auto &tiles)
            { return find_conflicting_tiles(tiles); });
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
       const glm::uvec2 &source_tile_size,
       const glm::uvec2 &destination_tile_size,
       const glm::uvec2 &source_texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile) const
     {
          return get_triangle_strip(
            source_tile_size,
            destination_tile_size,
            source_texture_size,
            tile_const,
            tile,
            true);
     }
     [[nodiscard]] ff_8::QuadStrip get_triangle_strip(
       const glm::uvec2 &source_tile_size,
       const glm::uvec2 &destination_tile_size,
       const glm::uvec2 &source_texture_size,
       const open_viii::graphics::background::is_tile auto &tile_const,
       open_viii::graphics::background::is_tile auto      &&tile,
       bool imported = false) const
     {
          const auto src = [this, &tile_const, &imported]()
          {
               if (imported)
               {
                    return to_vec2(
                      ff_8::source_coords_for_imported(tile_const));
               }
               if (m_filters.swizzle_as_one_image.enabled())
               {
                    // Calculate UVs for single texture
                    // Source X adjusted by texture_page * texture_page_width
                    // Base this on dest_coords_for_swizzle
                    return to_vec2(
                      ff_8::source_coords_for_single_swizzle(tile_const));
               }
               if (m_filters.swizzle.enabled())
               {
                    return to_vec2(ff_8::source_coords_for_swizzle(tile_const));
               }
               if (
                 m_filters.deswizzle.enabled()
                 || m_filters.full_filename.enabled())
               {
                    return to_vec2(
                      ff_8::source_coords_for_deswizzle(tile_const));
               }
               return to_vec2(ff_8::source_coords_for_default(tile_const));
          }();
          const auto dest = [this, &tile]()
          {
               if (m_settings.draw_swizzle)
               {
                    if (m_settings.disable_texture_page_shift)
                    {
                         return to_vec2(
                           ff_8::dest_coords_for_swizzle_disable_shift(tile));
                    }
                    return to_vec2(ff_8::dest_coords_for_swizzle(tile));
               }
               return to_vec2(ff_8::dest_coords_for_default(tile));
          }();
          return ff_8::get_triangle_strip(
            to_vec2(source_tile_size),
            to_vec2(destination_tile_size),
            to_vec2(source_texture_size),
            src,
            dest);
     }
};


std::move_only_function<std::vector<std::filesystem::path>()>
  generate_swizzle_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    std::uint8_t                      texture_page);

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_swizzle_as_one_image_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    std::optional<std::uint8_t>       palette = std::nullopt);

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_swizzle_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    std::uint8_t                      texture_page,
    std::uint8_t                      palette);

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_deswizzle_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    const ff_8::PupuID                pupu_id);

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_full_filename_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    const std::string                &filename);

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_map_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite);

}// namespace fme
#endif// FIELD_MAP_EDITOR_MAP_SPRITE_HPP
