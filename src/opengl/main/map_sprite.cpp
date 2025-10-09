#include "map_sprite.hpp"
#include "append_inserter.hpp"
#include "format_imgui_text.hpp"
#include "future_operations.hpp"
#include "gui/gui_labels.hpp"
#include "map_operation.hpp"
#include "safedir.hpp"
#include "save_image_pbo.hpp"
#include "utilities.hpp"
#include <bit>
#include <BlendModeSettings.hpp>
#include <DistanceBuffer.hpp>
#include <expected>
#include <fmt/format.h>
#include <FrameBuffer.hpp>
#include <FrameBufferBackup.hpp>
#include <HistogramBuffer.hpp>
#include <open_viii/graphics/Png.hpp>
#include <PaletteBuffer.hpp>
#include <ranges>
#include <ScopeGuard.hpp>
#include <spdlog/spdlog.h>
#include <stb_image.h>
#include <SubTexture.hpp>
#include <utility>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;

namespace fme
{

map_sprite::map_sprite(
  ff_8::map_group                        map_group,
  map_sprite_settings                    settings,
  ff_8::filters                          in_filters,
  std::weak_ptr<Selections>              selections,
  std::shared_ptr<glengine::FrameBuffer> framebuffer)
  : m_map_group(
      !settings.require_coo
          || (map_group.opt_coo && map_group.opt_coo.value() != open_viii::LangT::generic)
        ? std::move(map_group)
        : ff_8::map_group{})
  , m_settings(settings)
  , m_filters(std::move(in_filters))
  , m_selections(selections)
  , m_all_unique_values_and_strings(get_all_unique_values_and_strings())
  , m_canvas(get_canvas())
  , m_render_framebuffer(std::move(framebuffer))
{
     if (m_map_group.field.expired())
     {
          return;
     }
     if (m_selections.expired())
     {
          return;
     }
     const auto ps = ff_8::path_search{
          .selections = m_selections.lock(),
          .opt_coo    = m_map_group.opt_coo,
          .field_name = get_base_name(),
          .filters_deswizzle_value_string
          = m_filters.value<ff_8::FilterTag::Deswizzle>().string(),
          .filters_full_filename_value_string
          = m_filters.value<ff_8::FilterTag::FullFileName>().string(),
          .filters_swizzle_value_string
          = m_filters.value<ff_8::FilterTag::Swizzle>().string(),
          .filters_swizzle_as_one_image_string
          = m_filters.value<ff_8::FilterTag::SwizzleAsOneImage>().string(),
          .filters_map_value_string
          = m_filters.value<ff_8::FilterTag::Map>().string()
          //.working_unique_pupu                = working_unique_pupu(),
          //.current_filenames                  = toml_filenames(),
          //.bpp_palette                        = uniques().palette(),
          //.texture_page_id                    = uniques().texture_page_id()
     };


     if (m_filters.enabled<ff_8::FilterTag::Map>())
     {
          if (const auto paths = ps.generate_map_paths(".map");
              !std::ranges::empty(paths))
          {
               load_map(paths.front(), true);// grab the first match.
          }
          else
          {
               //.map was not found.
               m_filters.disable<ff_8::FilterTag::Map>();
          }
     }
     toggle_filter_compact_on_load_original();
     toggle_filter_flatten_on_load_original();

     update_render_texture(true);
}


void map_sprite::toggle_filter_compact_on_load_original(
  const std::optional<bool> state)
{
     const bool skip_update = true;//! state.has_value();
     if (state.has_value())
     {
          if (*state)
          {
               m_filters.enable<ff_8::FilterTag::CompactOnLoadOriginal>();
          }
          else if (m_filters.enabled<ff_8::FilterTag::CompactOnLoadOriginal>())
          {
               m_filters.disable<ff_8::FilterTag::CompactOnLoadOriginal>();
               first_to_working_and_original(skip_update);
          }
     }


     if (m_filters.enabled<ff_8::FilterTag::CompactOnLoadOriginal>())
     {
          switch (m_filters.value<ff_8::FilterTag::CompactOnLoadOriginal>())
          {
               case compact_type::rows:
                    compact_rows_original(skip_update);
                    break;
               case compact_type::all:
                    compact_all_original(skip_update);
                    break;
               case compact_type::move_only_conflicts:
               {
                    compact_move_conflicts_only_original(skip_update);
                    break;
               }
               case compact_type::map_order:
                    compact_map_order_original(skip_update);
                    break;
               case compact_type::map_order_ffnx:
                    compact_map_order_ffnx_original(skip_update);
                    break;
          }
     }
}

void map_sprite::toggle_filter_flatten_on_load_original(
  const std::optional<bool> state)
{
     const bool skip_update = true;//! state.has_value();
     if (state.has_value())
     {
          if (*state)
          {
               m_filters.enable<ff_8::FilterTag::FlattenOnLoadOriginal>();
          }
          else if (m_filters.enabled<ff_8::FilterTag::FlattenOnLoadOriginal>())
          {
               m_filters.disable<ff_8::FilterTag::FlattenOnLoadOriginal>();
               first_to_working_and_original(skip_update);
          }
     }

     if (m_filters.enabled<ff_8::FilterTag::FlattenOnLoadOriginal>())
     {
          switch (m_filters.value<ff_8::FilterTag::FlattenOnLoadOriginal>())
          {
               case flatten_type::bpp:
                    // Only flatten BPP if compact type isn't using map
                    // order
                    if (
                 !m_filters.enabled<ff_8::FilterTag::CompactOnLoadOriginal>()
                 || (m_filters.value<ff_8::FilterTag::CompactOnLoadOriginal>() != compact_type::map_order && m_filters.value<ff_8::FilterTag::CompactOnLoadOriginal>() != compact_type::map_order_ffnx))
                    {
                         flatten_bpp_original(skip_update);
                    }
                    break;

               case flatten_type::palette:
                    flatten_palette_original(skip_update);
                    break;

               case flatten_type::both:
                    // Only flatten BPP if not using map order
                    if (
                 !m_filters.enabled<ff_8::FilterTag::CompactOnLoadOriginal>()
                 || (m_filters.value<ff_8::FilterTag::CompactOnLoadOriginal>() != compact_type::map_order && m_filters.value<ff_8::FilterTag::CompactOnLoadOriginal>() != compact_type::map_order_ffnx))
                    {
                         flatten_bpp_original(skip_update);
                    }
                    flatten_palette_original(skip_update);
                    break;
          }
     }
}

map_sprite::operator ff_8::path_search() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     return { .selections = std::move(selections),
              .opt_coo    = m_map_group.opt_coo,
              .field_name = get_base_name(),
              .filters_deswizzle_value_string
              = m_filters.value<ff_8::FilterTag::Deswizzle>().string(),
              .filters_full_filename_value_string
              = m_filters.value<ff_8::FilterTag::FullFileName>().string(),
              .filters_swizzle_value_string
              = m_filters.value<ff_8::FilterTag::Swizzle>().string(),
              .filters_swizzle_as_one_image_string
              = m_filters.value<ff_8::FilterTag::SwizzleAsOneImage>().string(),
              .filters_map_value_string
              = m_filters.value<ff_8::FilterTag::Map>().string(),
              .working_unique_pupu = working_unique_pupu(),
              .current_filenames   = toml_filenames(),
              .bpp_palette         = uniques().palette(),
              .texture_page_id     = uniques().texture_page_id() };
}


[[nodiscard]] std::optional<open_viii::LangT> map_sprite::get_opt_coo() const
{
     return m_map_group.opt_coo;
}


map_sprite map_sprite::with_coo(const open_viii::LangT coo) const
{

     return { ff_8::map_group{ m_map_group.field, coo }, m_settings, m_filters,
              m_selections, m_render_framebuffer };
}

map_sprite map_sprite::with_field(
  map_sprite::WeakField  field,
  const open_viii::LangT coo) const
{
     return { ff_8::map_group{ std::move(field), coo }, m_settings, m_filters,
              m_selections, m_render_framebuffer };
}

map_sprite map_sprite::with_filters(ff_8::filters filters) const
{
     return { m_map_group, m_settings, std::move(filters), m_selections,
              m_render_framebuffer };
}

bool map_sprite::empty() const
{
     return m_map_group.maps.const_working().visit_tiles(
       [](const auto &tiles) { return std::empty(tiles); });
}

map_sprite::colors_type map_sprite::get_colors(
  const open_viii::graphics::background::Mim &mim,
  open_viii::graphics::BPPT                   bpp,
  std::uint8_t                                palette)
{
     return mim.get_colors<open_viii::graphics::Color32RGBA>(
       bpp, palette, false);
}

std::size_t map_sprite::get_texture_pos(
  const open_viii::graphics::BPPT bpp,
  const std::uint8_t              palette,
  const std::uint8_t              texture_page) const
{
     if (!m_filters.enabled<ff_8::FilterTag::SwizzleAsOneImage>())
     {
          if (!m_filters.enabled<ff_8::FilterTag::Swizzle>())
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
          const size_t j
            = static_cast<size_t>(texture_page) * MAX_PALETTES + palette;

          if (m_texture->at(j).height() == 0)
          {
               return START_OF_NO_PALETTE_INDEX + texture_page;
          }
          return j;
     }
     const size_t j = static_cast<size_t>(palette) + 1U;
     if (m_texture->at(j).height() == 0)
     {
          return 0U;
     }
     return j;
}


[[nodiscard]] const std::array<
  glengine::Texture,
  map_sprite::MAX_TEXTURES> &
  map_sprite::get_textures() const
{
     return *m_texture;
}

[[nodiscard]] const std::map<
  std::string,
  glengine::Texture> &
  map_sprite::get_full_filename_textures()
{
     return *m_full_filename_textures;
}

const std::map<
  std::string,
  std::optional<glengine::FrameBuffer>> &
  map_sprite::child_textures_map() const
{
     return m_child_textures_map;
}

const glengine::Texture *map_sprite::get_texture(
  const open_viii::graphics::BPPT bpp,
  const std::uint8_t              palette,
  const std::uint8_t              texture_page) const
{
     const size_t index = get_texture_pos(bpp, palette, texture_page);
     const size_t size  = m_texture->size();
     if (index >= size)
     {
          spdlog::debug(
            "Increase texture array size. it is too small! index {} >= size {}",
            index,
            size);
          spdlog::debug(
            "bpp: {}, palette: {}, texture page: {}",
            static_cast<int>(bpp),
            palette,
            texture_page);
          return nullptr;
     }
     return &m_texture->at(index);
}

glengine::Texture *
  map_sprite::get_texture_mutable(const ff_8::PupuID &pupu) const
{
     const auto &values = original_unique_pupu();
     auto        it     = std::ranges::find(values, pupu);
     if (it != values.end())
     {
          auto i = static_cast<std::size_t>(std::distance(values.begin(), it));
          if (i >= MAX_TEXTURES)
          {
               spdlog::error(
                 "{}:{} - Index out of range {} / {}",
                 __FILE__,
                 __LINE__,
                 i,
                 MAX_TEXTURES);
               return nullptr;
          }
          return &m_texture->at(i);
     }
     return nullptr;
}

const glengine::Texture *map_sprite::get_texture(const ff_8::PupuID &pupu) const
{
     return (get_texture_mutable(pupu));
}

/**
 * @brief Loads textures for map sprites based on current filters and internal
 * data.
 *
 * This function handles multiple texture loading strategies depending on the
 * state of the `deswizzle` and `upscale` filters. It uses asynchronous
 * operations wrapped in nested futures to schedule texture loading without
 * blocking the main thread.
 *
 * @return A shared pointer to an array of loaded glengine::Texture objects of
 * size MAX_TEXTURES.
 */
void map_sprite::queue_texture_loading() const
{
     // Container to hold nested futures representing asynchronous texture load
     // operations
     std::vector<std::future<std::future<void>>> future_of_futures{};


     const auto fofh_consumer = glengine::ScopeGuard{
          [&]()
          {
               // Consume the collected nested futures
               // immediately to kick off the work

               if (m_settings.force_loading)
               {
                    spdlog::debug(
                      "Force_loading textures: {}",
                      std::ranges::size(future_of_futures));
                    m_future_of_future_consumer += std::move(future_of_futures);
                    consume_now();
               }
               else
               {
                    spdlog::debug(
                      "Queued textures: {}",
                      std::ranges::size(future_of_futures));
                    m_future_of_future_consumer += std::move(future_of_futures);
               }
          }
     };

     // Check if the deswizzle filter is enabled
     if (m_filters.enabled<ff_8::FilterTag::Deswizzle>())
     {
          // Deswizzling is enabled; load textures based on PupuIDs in order
          std::ranges::for_each(
            working_unique_pupu(),
            [&, pos = size_t{}](const ff_8::PupuID &pupu) mutable
            {
                 future_of_futures.push_back(
                   load_deswizzle_textures(pupu, pos));
                 ++pos;
            });
          return;
     }

     if (m_filters.enabled<ff_8::FilterTag::FullFileName>())
     {
          // m_child_map_sprite.reset();
          m_child_textures_map.clear();
          // Deswizzling is enabled; load textures based on PupuIDs in order
          const auto filenames = toml_filenames();
          // const auto &unique_pupu = working_unique_pupu();
          const auto operation = [&](const std::string &filename)
          {
               // Original
               future_of_futures.push_back(
                 load_full_filename_textures(filename));

               // Mask variant Colorful Mask
               const std::filesystem::path p(filename);
               auto                        mask_name = fmt::format(
                 "{}_mask{}", p.stem().string(), p.extension().string());
               future_of_futures.push_back(
                 load_full_filename_textures(mask_name));

               m_full_filename_to_mask_name[filename] = mask_name;

               // todo use pupu ids in the toml table for each entry instead of
               // using all the unique pupu ids for each file.
               //  // Pupu Mask variant White on Black Mask.
               //  for (const auto &pupu : unique_pupu)
               //  {
               //       auto pupu_name = fmt::format("{}_mask_{}{}",
               //       p.stem().string(), pupu, p.extension().string());
               //       future_of_futures.push_back(load_full_filename_textures(pupu_name));
               //  }
          };
          std::ranges::for_each(filenames, operation);
          spdlog::info(
            "{}:{}, Queued up m_full_filename_textures.size() = {}",
            __FILE__,
            __LINE__,
            std::ranges::size(*m_full_filename_textures));
          return;
     }


     // Check if palette data exists for the given BPP

     for (const auto &[bpp, palette_set] :
          m_all_unique_values_and_strings.palette())
     {
          if (bpp.bpp24())
          {
               continue;
          }
          for (const auto &palette : palette_set.values())
          {
               if (m_filters.enabled<ff_8::FilterTag::SwizzleAsOneImage>())
               {
                    future_of_futures.push_back(
                      load_swizzle_as_one_image_textures(palette));
               }
               else if (m_filters.enabled<ff_8::FilterTag::Swizzle>())
               {
                    // Schedule upscale texture loads for each texture page
                    for (const auto &texture_page :
                         m_all_unique_values_and_strings.texture_page_id()
                           .values())
                    {
                         future_of_futures.push_back(
                           load_swizzle_textures(texture_page, palette));
                    }
               }
               else
               {
                    // Schedule normal MIM texture load (no upscale)
                    future_of_futures.push_back(
                      load_mim_textures(bpp, palette));
               }
          }
     }


     // Additional upscale loading for non-palette based textures
     if (m_filters.enabled<ff_8::FilterTag::SwizzleAsOneImage>())
     {
          future_of_futures.push_back(load_swizzle_as_one_image_textures());
     }
     else if (m_filters.enabled<ff_8::FilterTag::Swizzle>())
     {
          for (const auto &texture_page :
               m_all_unique_values_and_strings.texture_page_id().values())
          {
               future_of_futures.push_back(load_swizzle_textures(texture_page));
          }
     }

     // Return the shared array of loaded textures
     return;
}

void map_sprite::consume_futures(
  std::vector<std::future<std::future<void>>> &future_of_futures)
{
     std::vector<std::future<void>> futures{};
     futures.reserve(std::ranges::size(future_of_futures));
     std::ranges::transform(
       future_of_futures,
       std::back_inserter(futures),
       [](std::future<std::future<void>> &future_of_future) -> std::future<void>
       {
            if (future_of_future.valid())
            {
                 future_of_future.wait();
                 return future_of_future.get();
            }
            return {};
       });
     consume_futures(futures);
}

void map_sprite::consume_futures(std::vector<std::future<void>> &futures)
{
     std::ranges::for_each(
       futures,
       [](std::future<void> &future)
       {
            if (future.valid())
            {
                 future.get();
            }
       });
}

bool map_sprite::fallback_textures() const
{
     if (std::ranges::all_of(
           *m_texture.get(),
           [](const glengine::Texture &texture)
           {
                const auto size = texture.get_size();
                // spdlog::info("{}", size);
                return size.x == 0 || size.y == 0;
           }))
     {
          if (m_filters.enabled<ff_8::FilterTag::Swizzle>())
          {
               m_filters.disable<ff_8::FilterTag::Swizzle>();
               queue_texture_loading();
               return true;
          }
          else if (m_filters.enabled<ff_8::FilterTag::Deswizzle>())
          {
               m_filters.disable<ff_8::FilterTag::Deswizzle>();
               queue_texture_loading();
               return true;
          }
          else if (m_filters.enabled<ff_8::FilterTag::SwizzleAsOneImage>())
          {
               m_filters.disable<ff_8::FilterTag::SwizzleAsOneImage>();
               queue_texture_loading();
               return true;
          }
          else if (m_filters.enabled<ff_8::FilterTag::FullFileName>())
          {
               m_filters.disable<ff_8::FilterTag::FullFileName>();
               queue_texture_loading();
               return true;
          }
     }
     return false;
}

std::future<std::future<void>> map_sprite::load_mim_textures(
  open_viii::graphics::BPPT bpp,
  std::uint8_t              palette) const
{
     if (!m_map_group.mim)
     {
          return {};
     }
     if (m_map_group.mim->get_width(bpp) == 0U)
     {
          return {};
     }
     else
     {
          std::size_t const pos = get_texture_pos(bpp, palette, 0);
          open_viii::graphics::background::Mim mim     = *m_map_group.mim;
          glengine::Texture *const             texture = &(m_texture->at(pos));
          spdlog::debug(
            "Attempting to queue texture loading from .mim file : bpp {}, "
            "palette {}",
            bpp,
            +palette);
          return { std::async(
            std::launch::async,
            [         =,
             weak_ptr = std::weak_ptr<void>(m_texture)]() -> std::future<void>
            {
                 spdlog::debug(
                   "Getting colors from .mim file : bpp {}, palette {}",
                   bpp,
                   +palette);
                 return { std::async(
                   std::launch::deferred,
                   future_operations::LoadColorsIntoTexture{
                     .weak_ptr = weak_ptr,
                     .texture  = texture,
                     .colors   = get_colors(mim, bpp, palette),
                     .size
                     = glm::uvec2{ mim.get_width(bpp), mim.get_height() } }) };
            }) };
     }
}

std::future<std::future<void>> map_sprite::load_deswizzle_textures(
  const ff_8::PupuID pupu,
  const size_t       pos) const
{
     if (pos >= MAX_TEXTURES)
     {
          spdlog::error(
            "{}:{} - Index out of range {} / {}",
            __FILE__,
            __LINE__,
            pos,
            MAX_TEXTURES);
          return {};
     }
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{
         .weak_ptr  = m_texture,
         .texture   = &(m_texture->at(pos)),
         .paths_get = fme::generate_deswizzle_paths(
           std::move(selections), *this, pupu) }) };
}

std::future<std::future<void>>
  map_sprite::load_full_filename_textures(const std::string filename) const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{
         .weak_ptr  = m_full_filename_textures,
         .texture   = &((*m_full_filename_textures)[filename]),
         .paths_get = fme::generate_full_filename_paths(
           std::move(selections), *this, filename) }) };
}

std::future<std::future<void>> map_sprite::load_swizzle_textures(
  std::uint8_t texture_page,
  std::uint8_t palette) const
{
     const std::size_t pos
       = std::size_t{ texture_page } * MAX_PALETTES + palette;
     if (pos >= MAX_TEXTURES)
     {
          spdlog::error(
            "{}:{} - Index out of range {} / {}",
            __FILE__,
            __LINE__,
            pos,
            MAX_TEXTURES);
          return {};
     }
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{
         .weak_ptr  = m_texture,
         .texture   = &(m_texture->at(pos)),
         .paths_get = fme::generate_swizzle_paths(
           std::move(selections), *this, texture_page, palette) }) };
}

std::future<std::future<void>>
  map_sprite::load_swizzle_textures(std::uint8_t texture_page) const
{
     const std::size_t pos = START_OF_NO_PALETTE_INDEX + texture_page;
     if (pos >= MAX_TEXTURES)
     {
          spdlog::error(
            "{}:{} - Index out of range {} / {}",
            __FILE__,
            __LINE__,
            pos,
            MAX_TEXTURES);
          return {};
     }
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{
         .weak_ptr  = m_texture,
         .texture   = &(m_texture->at(pos)),
         .paths_get = fme::generate_swizzle_paths(
           std::move(selections), *this, texture_page) }) };
}


std::future<std::future<void>> map_sprite::load_swizzle_as_one_image_textures(
  std::optional<std::uint8_t> palette) const
{
     const std::size_t pos = palette.has_value() ? palette.value() + 1U : 0U;
     if (pos >= MAX_TEXTURES)
     {
          spdlog::error(
            "{}:{} - Index out of range {} / {}",
            __FILE__,
            __LINE__,
            pos,
            MAX_TEXTURES);
          return {};
     }
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     return { std::async(
       std::launch::async,
       future_operations::GetImageFromFromFirstValidPathCreateFuture{
         .weak_ptr  = m_texture,
         .texture   = &(m_texture->at(pos)),
         .paths_get = fme::generate_swizzle_as_one_image_paths(
           std::move(selections), *this, palette) }) };
}

enum struct texture_page_width : std::uint16_t
{
     bit_4  = 256U,
     bit_8  = bit_4 >> 1U,
     bit_16 = bit_4 >> 2U,
};
static constexpr std::underlying_type_t<texture_page_width>
  operator+(texture_page_width input) noexcept
{
     return static_cast<std::underlying_type_t<texture_page_width>>(input);
}

void map_sprite::compact_rows()
{
     ff_8::compact_rows(m_map_group.maps.copy_working(
       fmt::format("{} {}", gui_labels::compact, gui_labels::rows)));
     update_render_texture();
}
void map_sprite::compact_all()
{
     ff_8::compact_all(m_map_group.maps.copy_working(
       fmt::format("{} {}", gui_labels::compact, gui_labels::all)));
     update_render_texture();
}
void map_sprite::compact_move_conflicts_only()
{
     const auto &conflicts = m_map_group.maps.original_conflicts();
     ff_8::compact_move_conflicts_only(
       m_map_group.maps.copy_working(
         fmt::format(
           "{} {}", gui_labels::compact, gui_labels::move_conflicts_only)),
       conflicts);
     update_render_texture();
}
void map_sprite::compact_map_order()
{
     ff_8::compact_map_order(m_map_group.maps.copy_working(
       fmt::format("{} {}", gui_labels::compact, gui_labels::map_order)));
     update_render_texture();
}
void map_sprite::compact_map_order_ffnx()
{
     spdlog::debug(
       "{} {}", gui_labels::compact, gui_labels::compact_map_order_ffnx2);
     ff_8::compact_map_order_ffnx(m_map_group.maps.copy_working(
       fmt::format(
         "{} {}", gui_labels::compact, gui_labels::compact_map_order_ffnx2)));
     update_render_texture();
}
void map_sprite::flatten_bpp()
{
     ff_8::flatten_bpp(m_map_group.maps.copy_working(
       fmt::format("{} {}", gui_labels::flatten, gui_labels::bpp)));
     update_render_texture();
}
void map_sprite::flatten_palette()
{
     ff_8::flatten_palette(m_map_group.maps.copy_working(
       fmt::format("{} {}", gui_labels::flatten, gui_labels::palette)));
     update_render_texture();
}


void map_sprite::compact_rows_original(const bool skip_update)
{
     ff_8::compact_rows(m_map_group.maps.copy_original(
       fmt::format("{} {}", gui_labels::compact, gui_labels::rows)));
     if (!skip_update)
     {
          update_render_texture();
     }
}
void map_sprite::compact_all_original(const bool skip_update)
{
     ff_8::compact_all(m_map_group.maps.copy_original(
       fmt::format("{} {}", gui_labels::compact, gui_labels::all)));
     if (!skip_update)
     {
          update_render_texture();
     }
}
void map_sprite::compact_move_conflicts_only_original(const bool skip_update)
{
     const auto &conflicts = m_map_group.maps.working_conflicts();
     ff_8::compact_move_conflicts_only(
       m_map_group.maps.copy_original(
         fmt::format(
           "{} {}", gui_labels::compact, gui_labels::move_conflicts_only)),
       conflicts);
     if (!skip_update)
     {
          update_render_texture();
     }
}
void map_sprite::compact_map_order_original(const bool skip_update)
{
     ff_8::compact_map_order(m_map_group.maps.copy_original(
       fmt::format("{} {}", gui_labels::compact, gui_labels::map_order)));
     if (!skip_update)
     {
          update_render_texture();
     }
}
void map_sprite::compact_map_order_ffnx_original(const bool skip_update)
{
     spdlog::debug(
       "{} {}", gui_labels::compact, gui_labels::compact_map_order_ffnx2);
     ff_8::compact_map_order_ffnx(m_map_group.maps.copy_original(
       fmt::format(
         "{} {}", gui_labels::compact, gui_labels::compact_map_order_ffnx2)));
     if (!skip_update)
     {
          update_render_texture();
     }
}
void map_sprite::flatten_bpp_original(const bool skip_update)
{
     ff_8::flatten_bpp(m_map_group.maps.copy_original(
       fmt::format("{} {}", gui_labels::flatten, gui_labels::bpp)));
     if (!skip_update)
     {
          update_render_texture();
     }
}
void map_sprite::flatten_palette_original(const bool skip_update)
{
     ff_8::flatten_palette(m_map_group.maps.copy_original(
       fmt::format("{} {}", gui_labels::flatten, gui_labels::palette)));
     if (!skip_update)
     {
          update_render_texture();
     }
}

void map_sprite::update_position(
  const glm::ivec2               &pixel_pos,
  const glm::ivec2               &down_pixel_pos,
  const std::vector<std::size_t> &saved_indices)
{
     if (saved_indices.empty() && m_saved_imported_indices.empty())
     {
          return;
     }
     const Map &current_original = m_map_group.maps.original();
     Map       &current_map      = m_map_group.maps.copy_working(
       fmt::format(
         "{} ({},{}) -> ({},{})",
         gui_labels::update_position,
         pixel_pos.x,
         pixel_pos.y,
         down_pixel_pos.x,
         down_pixel_pos.y));
     const auto update_tile_positions
       = [this, &pixel_pos, &down_pixel_pos](
           /*const auto &map, */ const auto &const_tiles,
           auto                            &&tiles,
           const std::vector<std::size_t>   &indices)
     {
          for (auto i : indices)
          {
               const auto &original_tile = const_tiles[i];
               auto       &working_tile  = tiles[i];
               if (m_settings.draw_swizzle)
               {
                    const std::int32_t texture_page_width = 256;
                    const std::int32_t x_offset
                      = (down_pixel_pos.x % texture_page_width)
                        - static_cast<std::int32_t>(working_tile.source_x());
                    const std::int32_t y_offset
                      = down_pixel_pos.y
                        - static_cast<std::int32_t>(working_tile.source_y());
                    const std::uint8_t x = static_cast<std::uint8_t>(
                      (((pixel_pos.x % texture_page_width) - x_offset)));
                    const std::uint8_t y
                      = static_cast<std::uint8_t>(((pixel_pos.y - y_offset)));
                    const std::uint8_t texture_page = static_cast<std::uint8_t>(
                      pixel_pos.x / texture_page_width);
                    using namespace open_viii::graphics;
                    const BPPT bppt = [&]()
                    {
                         if (x >= 128 || original_tile.depth().bpp4())
                         {
                              return 4_bpp;
                         }
                         if (x >= 64 || original_tile.depth().bpp8())
                         {
                              return 8_bpp;
                         }
                         return original_tile.depth();
                    }();
                    working_tile = working_tile.with_source_xy(x, y)
                                     .with_texture_id(texture_page)
                                     .with_depth(bppt);
               }
               else
               {
                    const std::int32_t x_offset
                      = down_pixel_pos.x - working_tile.x();
                    const std::int32_t y_offset
                      = down_pixel_pos.y - working_tile.y();
                    working_tile = working_tile.with_xy(
                      static_cast<std::int16_t>(pixel_pos.x - x_offset),
                      static_cast<std::int16_t>(pixel_pos.y - y_offset));
               }
          }
     };
     current_original.visit_tiles(
       [&](const auto &const_tiles)
       {
            current_map.visit_tiles(
              [&](auto &&tiles)
              {
                   update_tile_positions(
                     /*current_map,*/ const_tiles, tiles, saved_indices);
              });
       });
     // if (!m_draw_swizzle)
     // {
     //      m_imported_tile_map.visit_tiles(
     //        [this, &update_tile_positions](auto &&tiles) {
     //        update_tile_positions(m_imported_tile_map, tiles,
     //        m_saved_imported_indices);
     //        });
     // }
     // m_saved_indices.clear();
     m_saved_imported_indices.clear();
     update_render_texture();
}

[[nodiscard]] std::expected<
  void,
  std::set<DrawFailure>>
  map_sprite::local_draw(
    const glengine::FrameBuffer   &target_framebuffer,
    const glengine::BatchRenderer &target_renderer) const
{
     const auto &shader             = target_renderer.shader();
     const auto  reset_blend_at_end = glengine::ScopeGuard(
       [] { glengine::BlendModeSettings::default_blend(); });
     glengine::BlendModeSettings::default_blend();
     using open_viii::graphics::background::BlendModeT;
     [[maybe_unused]] BlendModeT last_blend_mode{ BlendModeT::none };
     m_uniform_color = s_default_color;
     shader.set_uniform("u_Tint", s_default_color);
     bool                  drew_any_tile   = { false };
     std::set<DrawFailure> failures        = {};
     const auto           &unique_pupu_ids = working_unique_pupu();
     std::uint16_t         z               = 0;
     const auto            draw_one_tile   = [&](
                                  [[maybe_unused]] const auto &tile_const,
                                  const auto                  &tile,
                                  const ff_8::PupuID           pupu_id)
     {
          if (tile.z() != z && !m_settings.draw_swizzle)
          {
               failures.emplace(DrawError::FilteredOut);
               return;
          }
          if (pupu_id.raw() == 0)
          {
               failures.emplace(
                 DrawError::PupuIdZero, "pupu_id.raw() returned 0");
               return;// continue to next tile
          }
          if (
            m_filters.enabled<ff_8::FilterTag::MultiPupu>()
            && std::ranges::all_of(
              m_filters.value<ff_8::FilterTag::MultiPupu>(),
              [&](const auto &test) -> bool { return test != pupu_id; }))
          {
               failures.emplace(DrawError::FilteredOut);
               return;
          }
          if (
            m_filters.enabled<ff_8::FilterTag::Pupu>()
            && m_filters.value<ff_8::FilterTag::Pupu>() != pupu_id)
          {
               failures.emplace(DrawError::FilteredOut);
               return;
          }

          if (ff_8::tile_operations::fail_any_filters(m_filters, tile))
          {
               failures.emplace(DrawError::FilteredOut);
               return;
          }
          const auto *texture = [&]()
          {
               if (
                 m_filters.enabled<ff_8::FilterTag::Deswizzle>()
                 || m_filters.enabled<ff_8::FilterTag::FullFileName>())
               {
                    return get_texture(pupu_id);
               }
               else
               {
                    return get_texture(
                      tile_const.depth(),
                      tile_const.palette_id(),
                      tile_const.texture_id());
               }
          }();


          if (!texture)
          {
               failures.emplace(
                 DrawError::NoTexture, "Texture not found for tile");
               return;
          }

          if (texture->width() == 0 || texture->height() == 0)
          {
               failures.emplace(
                 DrawError::ZeroSizedTexture,
                 fmt::format(
                   "Texture has size ({},{})", texture->width(),
                   texture->height()));
               return;
          }
          if (!m_settings.disable_blends && !m_settings.draw_swizzle)
          {
               auto blend_mode = tile.blend_mode();
               if (blend_mode != last_blend_mode)
               {
                    spdlog::debug("Blend mode: {}", blend_mode);
                    target_renderer.draw();// flush buffer.
                    last_blend_mode = blend_mode;
                    // if (s_blends.percent_blend_enabled())
                    // {
                    switch (blend_mode)
                    {
                         case open_viii::graphics::background::BlendModeT::
                           half_add:
                              m_uniform_color = s_half_color;
                              break;
                         case open_viii::graphics::background::BlendModeT::
                           quarter_add:
                              m_uniform_color = s_quarter_color;
                              break;
                         default:
                              m_uniform_color = s_default_color;
                              break;
                    }
                    shader.set_uniform("u_Tint", m_uniform_color);
                    // }
                    switch (blend_mode)
                    {
                         case open_viii::graphics::background::BlendModeT::
                           half_add:
                         case open_viii::graphics::background::BlendModeT::
                           quarter_add:
                         case open_viii::graphics::background::BlendModeT::add:
                         {
                              // s_blends.set_add_blend();
                              glengine::BlendModeSettings::add_blend();
                         }
                         break;
                         case open_viii::graphics::background::BlendModeT ::
                           subtract:
                         {
                              // s_blends.set_subtract_blend();
                              glengine::BlendModeSettings::subtract_blend();
                         }
                         break;
                         default:
                              glengine::BlendModeSettings::default_blend();
                    }
               }
          }

          const auto destination_tile_size
            = glm::uvec2{ TILE_SIZE, TILE_SIZE }
              * static_cast<std::uint32_t>(target_framebuffer.scale());
          const auto source_tile_size = get_tile_texture_size(texture);
          const auto source_texture_size
            = glm::uvec2{ texture->width(), texture->height() };
          const ff_8::QuadStrip quad = get_triangle_strip(
            source_tile_size,
            destination_tile_size,
            source_texture_size,
            tile_const,
            tile);

          //  Extract draw position (x, y) from quad[0] (top-left vertex)
          glm::vec3            draw_position = glm::vec3(quad.draw_pos, 0.F);

          // Create the SubTexture
          glengine::SubTexture subtexture(*texture, quad.uv_min, quad.uv_max);

          spdlog::trace(
            "Target framebuffer scale: {}", target_framebuffer.scale());
          spdlog::trace(
            "Destination tile size: ({}, {})",
            destination_tile_size.x,
            destination_tile_size.y);
          spdlog::trace(
            "Source tile size: ({}, {})",
            source_tile_size.x,
            source_tile_size.y);
          spdlog::trace(
            "Source texture size: ({}, {})",
            source_texture_size.x,
            source_texture_size.y);
          spdlog::trace(
            "Draw position: ({}, {}, {})",
            draw_position.x,
            draw_position.y,
            draw_position.z);
          spdlog::trace("UV min: ({}, {})", quad.uv_min.x, quad.uv_min.y);
          spdlog::trace("UV max: ({}, {})", quad.uv_max.x, quad.uv_max.y);
          const auto find_id = [&]()
          {
               if (const auto it = std::ranges::find(unique_pupu_ids, pupu_id);
                   it != std::ranges::end(unique_pupu_ids))
               {
                    return static_cast<int>(
                      it - std::ranges::begin(unique_pupu_ids));
               }
               return -1;
          };
          target_renderer.draw_quad(
            subtexture,
            draw_position,
            glm::vec2{
              static_cast<float>(TILE_SIZE * target_framebuffer.scale()) },
            static_cast<int>(m_map_group.maps.get_offset_from_working(tile)),
            static_cast<GLuint>(find_id()));
          drew_any_tile = true;
     };


     if (m_settings.draw_swizzle)
     {
          for_all_tiles(draw_one_tile);
     }
     for (const auto &in_z : m_all_unique_values_and_strings.z().values())
     {
          z = in_z;
          for_all_tiles(draw_one_tile);
     }
     if (drew_any_tile)
     {
          target_renderer.draw();
          target_renderer.on_render();
          return {};// success
     }
     return std::unexpected(failures);// failed to draw anything
}

[[nodiscard]] bool map_sprite::draw_imported(
  [[maybe_unused]] const glengine::FrameBuffer &target_framebuffer) const
{
     // todo fix imported
     return false;
     // using namespace open_viii::graphics::background;
     // namespace v = std::ranges::views;
     // namespace r = std::ranges;

     // if (
     //   !m_using_imported_texture || m_imported_texture == nullptr ||
     //   m_imported_texture->width() == 0 || m_imported_texture->height() == 0)
     // {
     //      return false;
     // }
     // m_imported_texture->bind();
     // const auto pop_unbind         = glengine::ScopeGuard{ [&]() {
     // m_imported_texture->unbind(); } }; bool       drew               =
     // false; const auto draw_imported_tile = [this, &drew,
     // &target_framebuffer](
     //                                   const std::integral auto
     //                                   current_index, const is_tile auto
     //                                   &tile_const, const is_tile auto &tile)
     //                                   {
     //      if (!m_saved_imported_indices.empty())
     //      {
     //           const auto find_index =
     //           std::ranges::find_if(m_saved_imported_indices,
     //           [&current_index](const auto search_index) {
     //                return std::cmp_equal(search_index, current_index);
     //           });
     //           if (find_index != m_saved_imported_indices.end())
     //           {
     //                return;
     //           }
     //      }
     //      if (ff_8::tile_operations::fail_any_filters(m_filters, tile))
     //      {
     //           return;
     //      }
     //      const auto source_tile_size      =
     //      get_tile_texture_size_for_import(); const auto
     //      destination_tile_size = glm::uvec2{ TILE_SIZE, TILE_SIZE } *
     //      static_cast<std::uint32_t>(target_framebuffer.scale()); const
     //      glm::uvec2 source_texture_size = { m_imported_texture->width(),
     //      m_imported_texture->height() }; ff_8::QuadStrip  quad =
     //        get_triangle_strip_for_imported(source_tile_size,
     //        destination_tile_size, source_texture_size, tile_const, tile);
     //      /// TODO fix blend mode
     //      //   states.blendMode        = sf::BlendAlpha;
     //      //   if (!m_disable_blends)
     //      //   {
     //      //        states.blendMode = set_blend_mode(tile.blend_mode(),
     //      quad);
     //      //   }
     //      // apply the tileset texture
     //      /// TODO fix drawing quad
     //      // target.draw(quad.data(), quad.size(), sf::TriangleStrip,
     //      states); drew = true;
     // };
     // m_imported_tile_map_front.visit_tiles([&](const auto &unchanged_tiles) {
     //      m_imported_tile_map.visit_tiles([&](const auto &changed_tiles) {
     //           for (const auto &z_axis :
     //           m_all_unique_values_and_strings.z().values())
     //           {
     //                const auto z_test = [&]([[maybe_unused]] const is_tile
     //                auto &tile_const, const is_tile auto &tile) {
     //                     return std::cmp_equal(z_axis, tile.z());
     //                };

     //                auto zipped_range = v::zip(unchanged_tiles,
     //                changed_tiles)
     //                                    | v::filter([&](const auto &current)
     //                                    { return
     //                                    std::apply(Map::filter_invalid(),
     //                                    current); }) | v::filter([&](const
     //                                    auto &current) { return
     //                                    std::apply(z_test, current); });
     //                for (decltype(auto) current : zipped_range)
     //                {
     //                     std::apply(
     //                       [&](const is_tile auto &unchanged_tile, const
     //                       is_tile auto &changed_tile) {
     //                            const auto current_index =
     //                            std::ranges::distance(&changed_tiles.front(),
     //                            &changed_tile);
     //                            draw_imported_tile(current_index,
     //                            unchanged_tile, changed_tile);
     //                       },
     //                       current);
     //                }
     //           }
     //      });
     // });
     // return drew;
}

glm::uvec2 map_sprite::get_tile_texture_size_for_import() const
{
     return glm::uvec2{ m_imported_tile_size, m_imported_tile_size };
}

glm::uvec2 map_sprite::get_tile_texture_size(
  const glengine::Texture *const texture) const
{
     const auto raw_texture_size = texture->get_size();
     if (
       m_filters.enabled<ff_8::FilterTag::Deswizzle>()
       || m_filters.enabled<ff_8::FilterTag::FullFileName>())
     {
          const auto local_scale
            = static_cast<std::uint32_t>(raw_texture_size.y)
              / m_canvas.height();
          return glm::uvec2{ TILE_SIZE * local_scale, TILE_SIZE * local_scale };
     }
     const auto i = static_cast<std::uint32_t>(raw_texture_size.y / TILE_SIZE);
     return glm::uvec2{ i, i };
}

void map_sprite::enable_draw_swizzle()
{
     m_settings.draw_swizzle = true;
     update_render_texture();
}

void map_sprite::disable_draw_swizzle()
{
     m_settings.draw_swizzle = false;
     update_render_texture();
}

void map_sprite::clear_toml_cached_framebuffers() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     const key_value_data        config_path_values = { .ext = ".toml" };
     const std::filesystem::path config_path = config_path_values.replace_tags(
       selections->get<ConfigKey::OutputTomlPattern>(), selections);
     auto config = Configuration(config_path);
     config.save(true);
     m_cache_framebuffer.clear();
     m_cache_framebuffer_tooltips.clear();
     m_cache_framebuffer_pupuids.clear();
}

void map_sprite::update_render_texture(const bool reload_textures) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     if (reload_textures)
     {
          // consume all the futures.
          consume_now(false);
          // reset the textures
          *m_texture = {};
          m_cache_framebuffer.clear();
          m_cache_framebuffer_tooltips.clear();
          m_cache_framebuffer_pupuids.clear();
          queue_texture_loading();


          if (selections->get<ConfigKey::ForceReloadingOfTextures>())
          {
               consume_now(false);
          }
     }
     else if (selections->get<ConfigKey::ForceRenderingOfMap>())
     {
          consume_now(false);
     }

     if (fail())
     {
          spdlog::warn("map in fail() state");
          return;
     }


     process_full_filename_textures();

     if (!all_futures_done())
     {
          return;
     }
     if (fallback_textures())
     {
          // see if no textures are loaded and fall
          // back to .mim if not.
          spdlog::debug("Loading Fallback Textures (.mim)");
          if (!m_settings.force_loading)
          {
               return;
          }
     }
     // don't resize render texture till we have something to draw?
     // if (std::ranges::any_of(*m_texture.get(), [](const auto &texture) {
     // return texture.width() > 0 && texture.height() > 0; })) all tasks
     // completed.


     resize_render_texture();
     assert(m_render_framebuffer && "m_render_framebuffer is nullptr");
     (void)generate_texture(*m_render_framebuffer);
}


void map_sprite::purge_empty_full_filename_texture() const
{
     spdlog::debug("Task 1: Purge empty textures (sync, quick)");
     std::erase_if(
       *m_full_filename_textures,
       [](auto &pair)
       {
            const auto &texture = pair.second;
            const auto  size    = texture.get_size();
            return size.x == 0 || size.y == 0;
       });
     spdlog::info(
       "{}:{}, Loaded and purged empty textures, "
       "m_full_filename_textures.size() = {}",
       __FILE__,
       __LINE__,
       std::ranges::size(*m_full_filename_textures));
};


bool map_sprite::check_all_masks_exists_full_filename_texture() const
{
     spdlog::debug("Task 2: Check if all masks exist (sync, quick)");
     return std::ranges::all_of(
       m_full_filename_to_mask_name,
       [&](const auto &pair)
       {
            const auto &[filename, maskname] = pair;
            return m_full_filename_textures->contains(filename)
                   && m_full_filename_textures->contains(maskname);
       });
}

void map_sprite::load_child_map_sprite_full_filename_texture() const
{
     if (!check_all_masks_exists_full_filename_texture())
     {
          spdlog::debug(
            "Task 3.1: Load child map_sprite with only the mim textures.");
          map_sprite_settings settings
            = { .draw_swizzle   = false,
                .disable_blends = true,
                .require_coo    = m_settings.require_coo,
                .force_loading  = true };
          m_child_map_sprite = std::make_unique<map_sprite>(
            m_map_group, settings, ff_8::filters{ false }, m_selections);
          m_child_map_sprite->consume_now(true);
          // return;// return early here as the texture loading is placed
          // in
          //  the queue.
     }
}

void map_sprite::
  generate_combined_textures_for_child_map_sprite_full_filename_texture() const
{
     if (!m_child_map_sprite)
     {
          spdlog::warn("m_child_map_sprite is nullptr");
          return;
     }
     spdlog::debug(
       "Task 3.2: Trigger child map_sprite call to "
       "get_deswizzle_combined_textures.");
     // Task 3.2: Preparation (mask generation if needed + init
     // shaders/buffers) This is the outer task, returning a future for the
     // follow-up (processing). Queue it as m_future_of_future_consumer for
     // deferred execution.
     // this generates framebuffers containing the masks on
     // GL_COLOR_ATTACHMENT1 with scale canvas_size/max_texture_size.

     // calculate scale using one of the existing textures

     const auto scale = [&]() -> glm::ivec2
     {
          if (m_full_filename_textures->empty())
               return glm::ivec2{ 1, 1 };

          const auto [min_it, max_it] = std::ranges::minmax_element(
            *m_full_filename_textures,
            [](const auto &apair, const auto &bpair)
            {
                 const auto &a  = std::get<1>(apair);
                 const auto &b  = std::get<1>(bpair);
                 const auto  sa = a.get_size();
                 const auto  sb = b.get_size();
                 return (sa.x * sa.y) < (sb.x * sb.y);
            });

          const auto max_size = std::get<1>(*max_it).get_size();
          assert(std::get<1>(*min_it).get_size() == max_size);

          const auto canvas = m_child_map_sprite->get_canvas();
          return glm::ivec2(canvas.width(), canvas.height()) / max_size;
     }();
     assert(scale.x == scale.y);
     assert(std::has_single_bit(static_cast<unsigned int>(scale.x)));

     const auto temp
       = m_child_map_sprite->get_deswizzle_combined_textures(scale.x, true);
     m_child_map_sprite->consume_now(true);
     if (!temp.has_value())
     {
          m_child_textures_map.clear();
          spdlog::error(
            "{}:{} "
            "m_child_map_sprite->get_deswizzle_combined_textures(scale.x, "
            "true) failed: {}",
            __FILE__,
            __LINE__,
            temp.error());
          // lets try again next frame.
          // m_child_map_sprite->update_render_texture(true);
          // m_future_consumer += std::async(
          //   std::launch::deferred, [this] { update_render_texture(); });
          // return;
     }
     // m_future_consumer += std::async(
     //   std::launch::deferred,
     if (!m_child_map_sprite)
     {
          return;
     }
     if (temp.has_value())
     {
          m_child_textures_map = std::move(*temp.value());
     }
     spdlog::debug(
       "Move child combined textures into m_child_textures_map: "
       "{}, futures_done: {}",
       m_child_textures_map.size(),
       m_child_map_sprite->all_futures_done());
     // get_deswizzle_combined_textures queues up texture
     // generation. so we're queueing up the post operation that
     // should run afterwards here.
     m_child_map_sprite.reset();


     // return;
}

std::tuple<
  glengine::PaletteBuffer,
  glengine::HistogramBuffer,
  glengine::DistanceBuffer>
  map_sprite::initialize_buffers(const std::vector<glm::vec4> &palette) const
{
     std::tuple<
       glengine::PaletteBuffer, glengine::HistogramBuffer,
       glengine::DistanceBuffer>
       ret{ glengine::PaletteBuffer{},
            glengine::HistogramBuffer{ std::ranges::size(palette) },
            glengine::DistanceBuffer{ std::ranges::size(palette) } };
     auto &[pb, hb, db] = ret;
     pb.initialize(palette);
     if (!pb.id())
          spdlog::critical("PaletteBuffer initialization failed, aborting");

     if (!hb.id())
          spdlog::critical("HistogramBuffer initialization failed, aborting");

     if (!db.id())
          spdlog::critical("DistanceBuffer initialization failed, aborting");

     return ret;
}


std::pair<
  std::vector<PupuOpEntry>,
  std::vector<std::string>>
  map_sprite::collect_post_op_entries(
    std::tuple<
      glengine::PaletteBuffer,
      glengine::HistogramBuffer,
      glengine::DistanceBuffer> &buffers) const
{
     const auto &unique_pupu = working_unique_pupu();
     const auto &palette     = m_map_group.maps.working_unique_pupu_color();
     std::pair<std::vector<PupuOpEntry>, std::vector<std::string>> ret = {};

     auto &[multi_pupu_post_op, remove_queue]                          = ret;
     auto &[pb, hb, db] = buffers;
     // Main loop over pairs
     for (const auto &[filename, maskname] : m_full_filename_to_mask_name)
     {

          auto get_mask_texture = [&](
                                    std::uint32_t      in_color_attachment_id,
                                    const std::string &in_maskname)
            -> std::expected<const glengine::SubTexture, std::string>
          {
               if (auto it = m_full_filename_textures->find(in_maskname);
                   it != m_full_filename_textures->end())
               {
                    spdlog::debug(
                      "{}:{} Mask chosen external mask png id: {}, "
                      "mask_filename: {}",
                      __FILE__, __LINE__,
                      static_cast<std::uint32_t>(it->second.id()), in_maskname);
                    return it->second;
               }

               if (m_child_textures_map.empty())
               {
                    return std::unexpected("opt_textures_map is empty");
               }

               auto map_it = m_child_textures_map.find(filename);
               if (map_it == m_child_textures_map.end())
               {
                    return std::unexpected("Filename not found: " + filename);
               }

               if (!map_it->second.has_value())
               {
                    return std::unexpected("FrameBuffer has no value");
               }

               spdlog::debug(
                 "{}:{} Mask chosen generated map id: {}, toml_filename: {}",
                 __FILE__, __LINE__,
                 static_cast<std::uint32_t>(
                   map_it->second->color_attachment_id(in_color_attachment_id)),
                 filename);

               return map_it->second->color_attachment_id(
                 in_color_attachment_id);
          };

          // Example call
          auto main_texture = get_mask_texture(0, filename);
          if (!main_texture)
          {
               spdlog::error("Mask lookup failed: {}", main_texture.error());
               remove_queue.push_back(filename);
               continue;
          }

          auto mask_texture = get_mask_texture(1, maskname);
          if (!mask_texture)
          {
               spdlog::error("Mask lookup failed: {}", mask_texture.error());
               remove_queue.push_back(filename);
               continue;
          }

          const toml::table *file_table
            = get_deswizzle_combined_toml_table(filename);
          ff_8::filter<ff_8::FilterTag::MultiPupu> multi_pupu
            = { ff_8::FilterSettings::All_Disabled };
          multi_pupu.reload(*file_table);
          if (!multi_pupu.enabled())
          {
               continue;
          }
          if (multi_pupu.value().empty())
          {
               continue;
          }

          // if (std::cmp_equal(value<ff_8::FilterTag::MultiPupu>().size(), 1))
          // {
          //      // Only one pupu.
          //      glengine::Texture *target_texture
          //        =
          //        get_texture_mutable(value<ff_8::FilterTag::MultiPupu>().front());
          //      if (!target_texture)
          //      {
          //           continue;
          //      }
          //      const auto size = main_texture.get_size();

          //      if (const auto t_size = target_texture->get_size();
          //          t_size.x != 0 and t_size.y != 0)
          //      {
          //           continue;// texture in use already.
          //      }
          //      *target_texture = glengine::Texture(size.x, size.y);

          //      mask_texture->bind_read_only(0);
          //      main_texture.bind_read_only(1);
          //      target_texture->bind_write_only(2);

          //      // Load and execute compute shader
          //      const auto pop_shader = m_mask_comp_shader->backup();
          //      m_mask_comp_shader->bind();
          //      m_mask_comp_shader->set_uniform(
          //        "chosenColor", glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
          //      m_mask_comp_shader->execute(
          //        static_cast<GLuint>(size.x),
          //        static_cast<GLuint>(size.y),
          //        GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
          //      remove_queue.push_back(filename);
          //      remove_queue.push_back(maskname);
          // }
          // else// more than one.
          {

               db.bind(3);
               pb.bind(2);
               hb.bind(1);
               mask_texture->bind_read_only(0);
               const auto size       = main_texture->get_size();
               const auto pop_shader = m_mask_count_comp_shader->backup();
               m_mask_count_comp_shader->bind();
               m_mask_count_comp_shader->set_uniform(
                 "numColors", static_cast<GLint>(std::ranges::size(palette)));
               m_mask_count_comp_shader->execute(
                 static_cast<GLuint>(size.x),
                 static_cast<GLuint>(size.y),
                 GL_SHADER_STORAGE_BARRIER_BIT);

               // todo use a std::async to defer this to later.
               // Because of warnings from gpu for doing it too fast.
               // And I'm not sure it'll fix it but it might.
               //(For now, kept sync; you can wrap read_back in another
               // future<void> here if needed)
               std::vector<GLuint> counts;
               hb.read_back(counts);
               hb.reset();
               std::vector<float> distances;
               db.read_back(distances);
               db.reset();

               spdlog::info("Maskname \"{}\" counts: ", maskname);
               for (const auto &[index, zip] :
                    std::views::zip(palette, unique_pupu, counts, distances)
                      | std::views::enumerate)
               {
                    const auto &[color, pupu, count, distance] = zip;
                    multi_pupu_post_op.push_back(
                      PupuOpEntry{ .pupu         = pupu,
                                   .color_index  = static_cast<int>(index),
                                   .main_texture = *main_texture,
                                   .mask_texture = *mask_texture,
                                   .count        = count,
                                   .distance     = distance });
                    if (count > 0)
                    {
                         spdlog::info(
                           "Index {:>3}, Color {}, Pupu {}, Count {:>6}, "
                           "Distance {}",
                           index,
                           fme::color{ color },
                           pupu,
                           count,
                           distance);
                    }
               }
               // I need both textures a index and count.
               // We need to take an index that doesn't have texture
               // or texture.get_size() == glm::ivec2(0,0) We to sort
               // the textures by the count for that index, largest to
               // smallest We can grab the color again using the
               // index. We use the color and the decided main_texture
               // and mask_texture and creat the output texture at
               // *m_texture[index];
               //  Then we remove the filename and maskname using the
               //  remove_queue. We'll need to using some kinda map of
               //  index to tuple. And start a new loop after this
               //  loop.

               remove_queue.push_back(filename);
               remove_queue.push_back(maskname);
          }
     }
     return ret;
}

void map_sprite::process_post_op_entries(
  const std::vector<PupuOpEntry> &multi_pupu_post_op,
  glengine::PaletteBuffer        &pb) const
{
     const auto &unique_pupu = working_unique_pupu();
     const auto &palette     = m_map_group.maps.working_unique_pupu_color();
     // Post-op loop
     for (auto &&current_unique_pupu : unique_pupu)
     {
          // Create a view over only matching items
          auto matching = multi_pupu_post_op
                          | std::views::filter(
                            [&](const PupuOpEntry &values)
                            { return values.pupu == current_unique_pupu; });

          // Find the element with the largest count among the matching
          // ones
          auto best_it = std::ranges::max_element(
            matching,
            [](const PupuOpEntry &a, const PupuOpEntry &b)
            { return a.count < b.count; });

          if (best_it == std::ranges::end(matching))
               continue;

          glengine::Texture *target_texture
            = get_texture_mutable(current_unique_pupu);
          if (!target_texture)
          {
               spdlog::error(
                 "{}:{} target_texture is nullptr", __FILE__, __LINE__);
               continue;
          }

          const auto size = best_it->main_texture.get_size();

          if (const auto t_size = target_texture->get_size();
              t_size.x != 0 && t_size.y != 0)
          {
               spdlog::error(
                 "{}:{} target_texture is in use.", __FILE__, __LINE__);
               continue;// texture already in use
          }

          *target_texture = glengine::Texture(size.x, size.y);

          best_it->mask_texture.bind_read_only(0);
          best_it->main_texture.bind_read_only(1);
          target_texture->bind_write_only(2);
          pb.bind(3);

          const auto pop_shader = m_mask_comp_shader->backup();
          m_mask_comp_shader->bind();
          m_mask_comp_shader->set_uniform("chosenIndex", best_it->color_index);
          m_mask_comp_shader->set_uniform(
            "numColors", static_cast<GLint>(std::ranges::size(palette)));
          m_mask_comp_shader->execute(
            static_cast<GLuint>(size.x),
            static_cast<GLuint>(size.y),
            GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
          target_texture->generate_mipmaps();

          spdlog::debug(
            "PupuOpEntry: pupu={}, color_index={}, "
            "main_tex_id={}, main_tex_size=({}x{}), "
            "mask_tex_id={}, mask_tex_size=({}x{}), count={}, "
            "distance={}",
            best_it->pupu, best_it->color_index,
            static_cast<std::uint32_t>(best_it->main_texture.id()),
            best_it->main_texture.get_size().x,
            best_it->main_texture.get_size().y,
            static_cast<std::uint32_t>(best_it->mask_texture.id()),
            best_it->mask_texture.get_size().x,
            best_it->mask_texture.get_size().y, best_it->count,
            best_it->distance);
     }
}

void map_sprite::cleanup_full_filename_textures(
  const std::vector<std::string> &remove_queue) const
{
     // cleanup
     for (const std::string &filename : remove_queue)
     {
          // m_full_filename_textures.erase(filename);
          m_full_filename_to_mask_name.erase(filename);
     }
}

void map_sprite::post_op_full_filename_texture() const
{
     spdlog::debug(
       "Task 4: Follow-up processing task (main loop + post-op + "
       "cleanup)");
     // This is the inner task, chained after preparation.

     auto buffers
       = initialize_buffers(m_map_group.maps.working_unique_pupu_color());
     auto &[pb, hb, db] = buffers;
     auto post_op_data  = collect_post_op_entries(buffers);
     auto &[multi_pupu_post_op, remove_queue] = post_op_data;
     process_post_op_entries(multi_pupu_post_op, pb);
     cleanup_full_filename_textures(remove_queue);
};

void map_sprite::process_full_filename_textures() const
{
     if (
       !all_futures_done()
       || !m_filters.enabled<ff_8::FilterTag::FullFileName>()
       || m_full_filename_textures->empty()
       || m_full_filename_to_mask_name.empty())
     {
          return;
     }

     if (!m_mask_comp_shader)
     {
          spdlog::critical(
            "CompShader m_mask_comp_shader {}", m_mask_comp_shader.error());
          m_full_filename_textures->clear();
          return;
     }
     if (!m_mask_count_comp_shader)
     {
          spdlog::critical(
            "CompShader m_mask_count_comp_shader {}",
            m_mask_count_comp_shader.error());
          m_full_filename_textures->clear();
          return;
     }
     purge_empty_full_filename_texture();

     // if (m_full_filename_textures->empty())
     // {
     //      return;
     // }
     load_child_map_sprite_full_filename_texture();
     generate_combined_textures_for_child_map_sprite_full_filename_texture();

     post_op_full_filename_texture();
}

void map_sprite::set_uniforms(
  const glengine::FrameBuffer &fbo,
  const glengine::Shader      &shader) const
{
     m_fixed_render_camera.set_projection(
       0.f,
       static_cast<float>(fbo.width()),
       0.f,
       static_cast<float>(fbo.height()));
     // if (m_offscreen_drawing || m_saving)
     // {
     shader.set_uniform(
       "u_MVP", m_fixed_render_camera.view_projection_matrix());
     // }
     // else if (m_preview)
     // {
     //      shader.set_uniform("u_MVP",
     //      m_imgui_viewport_window.preview_view_projection_matrix());
     // }
     // else
     // {
     //      shader.set_uniform("u_MVP",
     //      m_imgui_viewport_window.view_projection_matrix());
     // }
     shader.set_uniform("u_Grid", 0.F, 0.F);
     //    if (!s_draw_grid || m_offscreen_drawing || m_saving)
     //    {
     //      shader.set_uniform("u_Grid", 0.F, 0.F);
     //    }
     //    else
     //    {
     //      shader.set_uniform(
     //        "u_Grid", m_map_dims.scaled_tile_size());
     //    }
     shader.set_uniform("u_Tint", m_uniform_color);
}
void map_sprite::save([[maybe_unused]] const std::filesystem::path &path) const
{
     if (fail())
     {
          return;
     }
     consume_now();
     std::future<void> task
       = save_image_pbo(path, m_render_framebuffer->clone());
     task.wait();
}
bool map_sprite::fail() const
{
     using namespace open_viii::graphics::literals;
     // if (!m_render_texture)
     // {
     //      if (once)
     //      {
     //           spdlog::warn("{}", "m_render_texture is null");
     //           once = false;
     //      }
     //      return true;
     // }
     if (!m_texture)
     {
          if (once)
          {
               spdlog::warn("{}", "m_texture is null");
               once = false;
          }
          return true;
     }
     if (m_map_group.field.expired())
     {
          return true;
     }
     if (!m_map_group.mim)
     {
          return true;
     }
     if (m_map_group.mim->get_width(4_bpp, false) == 0)
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
void map_sprite::save_map(const std::filesystem::path &dest_path) const
{
     ff_8::map_group::OptCoo coo
       = m_map_group.opt_coo;// copy because coo is modified
     const auto map = ff_8::load_map(
       m_map_group.field, coo, m_map_group.mim, nullptr, false);
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
                        //          std::memcpy(data.data(), &tile,
                        //          sizeof(tile));
                        const auto data
                          = std::bit_cast<std::array<char, sizeof(tile)>>(tile);
                        os.write(data.data(), data.size());
                   }
              });
       },
       path,
       "");

     test_map(dest_path);
}
std::string map_sprite::map_filename() const
{
     return std::filesystem::path(m_map_group.map_path).filename().string();
}

void map_sprite::resize_render_texture() const
{
     if (fail())
     {
          return;
     }
     auto filtered_textures = *(m_texture.get())
                              | std::views::filter(
                                [](const auto &texture)
                                {
                                     const auto &size = texture.get_size();
                                     return size.x != 0 && size.y != 0;
                                });
     const auto check_size = [this]()
     {
          static const GLint max_size = []()
          {
               GLint return_val = {};
               glGetIntegerv(GL_MAX_TEXTURE_SIZE, &return_val);
               return return_val;
          }();

          while (std::cmp_greater(
                   static_cast<int>(width()) * m_render_framebuffer->scale(),
                   max_size)
                 || std::cmp_greater(
                   static_cast<int>(height()) * m_render_framebuffer->scale(),
                   max_size))
          {
               m_render_framebuffer->set_scale(
                 static_cast<std::int32_t>(
                   static_cast<std::uint32_t>(m_render_framebuffer->scale())
                   >> 1U));
               if (m_render_framebuffer->scale() <= 1)
               {
                    m_render_framebuffer->set_scale(1);
                    break;
               }
          }
     };
     if (filtered_textures.begin() != filtered_textures.end())
     {
          const auto max_height
            = (std::ranges::max)(filtered_textures
                                 | std::ranges::views::transform(
                                   [](const auto &texture)
                                   { return texture.height(); }));
          static constexpr std::int16_t mim_texture_height = 256U;

          if (
            m_filters.enabled<ff_8::FilterTag::Deswizzle>()
            || m_filters.enabled<ff_8::FilterTag::FullFileName>()) [[unlikely]]
          {
               m_render_framebuffer->set_scale(
                 max_height / static_cast<std::int32_t>(m_canvas.height()));
          }
          else [[likely]]
          {
               m_render_framebuffer->set_scale(max_height / mim_texture_height);
          }
     }
     else
     {
          m_render_framebuffer->set_scale(1U);
     }
     if (const std::uint16_t tmp_scale
         = m_imported_tile_size / map_sprite::TILE_SIZE;
         m_using_imported_texture
         && std::cmp_less(m_render_framebuffer->scale(), tmp_scale))
     {
          m_render_framebuffer->set_scale(tmp_scale);
     }
     check_size();
     auto spec = glengine::FrameBufferSpecification{
          .width  = static_cast<int>(width()) * m_render_framebuffer->scale(),
          .height = static_cast<int>(height()) * m_render_framebuffer->scale(),
          .scale  = m_render_framebuffer->scale()
     };
     if (
       m_render_framebuffer->width() != spec.width
       || m_render_framebuffer->height() != spec.height)
     {
          *m_render_framebuffer = glengine::FrameBuffer{ std::move(spec) };
     }
}

open_viii::graphics::Rectangle<std::uint32_t> map_sprite::get_canvas() const
{
     return static_cast<open_viii::graphics::Rectangle<std::uint32_t>>(
       m_map_group.maps.const_working().canvas());
}

std::uint32_t map_sprite::width() const
{
     if (m_settings.draw_swizzle)
     {
          if (m_map_group.mim)
          {
               using namespace open_viii::graphics::literals;
               return m_map_group.mim->get_width(4_bpp);
          }
     }
     return m_canvas.width();
}

std::uint32_t map_sprite::height() const
{
     if (m_settings.draw_swizzle)
     {
          if (m_map_group.mim)
          {
               return m_map_group.mim->get_height();
          }
     }
     return m_canvas.height();
}

ff_8::all_unique_values_and_strings
  map_sprite::get_all_unique_values_and_strings() const
{
     return m_map_group.maps.const_working().visit_tiles(
       [](const auto &tiles)
       { return ff_8::all_unique_values_and_strings(tiles); });
}

const ff_8::filters &map_sprite::filter() const
{
     return m_filters;
}
ff_8::filters &map_sprite::filter()
{
     return m_filters;
}
map_sprite map_sprite::update(
  ff_8::map_group map_group,
  bool            draw_swizzle) const
{
     auto settings         = m_settings;
     settings.draw_swizzle = draw_swizzle;
     return { std::move(map_group), settings, m_filters, m_selections };
}
const ff_8::all_unique_values_and_strings &map_sprite::uniques() const
{
     return m_all_unique_values_and_strings;
}


const std::vector<ff_8::PupuID> &map_sprite::working_pupu() const
{
     // side effect. we wait till pupu is needed than we refresh it.
     m_map_group.maps.refresh_working_all();
     return m_map_group.maps.working_pupu();
}
const std::vector<ff_8::PupuID> &map_sprite::original_pupu() const
{
     // side effect. we wait till pupu is needed than we refresh it.
     m_map_group.maps.refresh_original_all();
     return m_map_group.maps.original_pupu();
}

const std::vector<ff_8::PupuID> &map_sprite::working_unique_pupu() const
{
     // side effect. we wait till pupu is needed than we refresh it.
     m_map_group.maps.refresh_working_all();
     return m_map_group.maps.working_unique_pupu();
}

std::vector<std::tuple<
  glm::vec4,
  ff_8::PupuID>>
  map_sprite::working_unique_color_pupu() const
{
     // side effect. we wait till pupu is needed than we refresh it.
     m_map_group.maps.refresh_working_all();
     return std::views::zip(
              m_map_group.maps.working_unique_pupu_color(),
              m_map_group.maps.working_unique_pupu())
            | std::ranges::to<std::vector>();
}

const std::vector<ff_8::PupuID> &map_sprite::original_unique_pupu() const
{
     // side effect. we wait till pupu is needed than we refresh it.
     m_map_group.maps.refresh_original_all();
     return m_map_group.maps.original_unique_pupu();
}


const ff_8::source_tile_conflicts &map_sprite::original_conflicts() const
{
     // side effect. we wait till conflicts is needed than we refresh it.
     m_map_group.maps.refresh_original_all();
     if (all_futures_done())
     {
          return m_map_group.maps.original_conflicts();
     }
     static const ff_8::source_tile_conflicts blank{};
     return blank;
}

const ff_8::source_tile_conflicts &map_sprite::working_conflicts() const
{
     // side effect. we wait till conflicts is needed than we refresh it.
     m_map_group.maps.refresh_working_all();
     if (all_futures_done())
     {
          return m_map_group.maps.working_conflicts();
     }
     static const ff_8::source_tile_conflicts blank{};
     return blank;
}

const ff_8::MapHistory::nst_map &map_sprite::working_similar_counts() const
{
     m_map_group.maps.refresh_working_all();
     if (all_futures_done())
     {
          return m_map_group.maps.working_similar_counts();
     }
     static const ff_8::MapHistory::nst_map blank{};
     return blank;
}

const ff_8::MapHistory::nsat_map &map_sprite::working_animation_counts() const
{
     m_map_group.maps.refresh_working_all();
     if (all_futures_done())
     {
          return m_map_group.maps.working_animation_counts();
     }
     static const ff_8::MapHistory::nsat_map blank{};
     return blank;
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
//     std::array{ ([](const char *const local_name, int &local_value) ->
//     bool {
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
//     GetBlendSubtract() = sf::BlendMode{ (static_cast<T>(*(value++)))...
//     };
//   }
//   return result;
// }

/**
 * @brief Saves swizzled texture pages (and optionally palettes) to disk
 * asynchronously.
 *
 * Generates textures for each unique texture page (and conflicting
 * palettes if needed) and saves them as PNG files. If the texture has
 * palette conflicts (e.g., multiple palettes for the same texture page),
 * generates separate files for each conflicting palette.
 *
 * @param path The base directory path where the images will be saved.
 * @return A vector of futures representing the save operations, allowing
 * the caller to later wait or check for completion.
 *
 * @note Caller is responsible for consuming or waiting on the futures to
 * ensure save completion.
 */
[[nodiscard]] std::vector<std::future<void>> map_sprite::save_swizzle_textures(
  const std::string           &keyed_string,
  const std::filesystem::path &selected_path)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     consume_now();

     // Extract unique texture page IDs and BPP (bits per pixel) values
     // from the map.
     const auto  unique_values = get_all_unique_values_and_strings();
     const auto &unique_texture_page_ids
       = unique_values.texture_page_id().values();
     const auto     &unique_bpp = unique_values.bpp().values();

     // Backup and override current settings for exporting textures.
     settings_backup settings   = get_backup_settings(true);

     // Adjust scale based on texture height or deswizzle state.
     std::int32_t height = static_cast<std::int32_t>(get_max_texture_height());

     // If there’s only one bpp and at most one palette, nothing needs
     // saving.
     if (
       unique_bpp.size() == 1U
       && unique_values.palette().at(unique_bpp.front()).values().size() <= 1U)
     {
          return {};
     }

     // Prepare for gathering palette conflicts.
     using map_type = std::remove_cvref_t<decltype(get_conflicting_palettes())>;
     using mapped_type                       = typename map_type::mapped_type;
     const map_type conflicting_palettes_map = get_conflicting_palettes();

     // Prepare futures to track save operations.
     std::vector<std::future<void>> future_of_futures           = {};
     const unsigned int             max_number_of_texture_pages = 13U;
     future_of_futures.reserve(max_number_of_texture_pages);

     // Create an off-screen render texture to draw into.
     const auto specification
       = glengine::FrameBufferSpecification{ .width  = height,
                                             .height = height,
                                             .scale
                                             = m_render_framebuffer->scale() };

     // Loop over all unique texture pages.
     for (const auto &texture_page : unique_texture_page_ids)
     {
          settings.filters.value()
            .update<ff_8::FilterTag::TexturePageId>(texture_page)
            .enable();
          const bool contains_conflicts
            = conflicting_palettes_map.contains(texture_page);

          if (contains_conflicts)
          {
               // Handle palette conflicts: export each conflicting palette
               // individually.
               const mapped_type &conflicting_palettes
                 = conflicting_palettes_map.at(texture_page);
               for (const auto &bpp : unique_bpp)
               {
                    const auto &unique_palette
                      = unique_values.palette().at(bpp).values();

                    // Filter palettes that conflict with the current
                    // texture page.
                    auto filter_palette
                      = unique_palette
                        | std::views::filter(
                          [&conflicting_palettes](const std::uint8_t &palette)
                          {
                               return std::ranges::any_of(
                                 conflicting_palettes,
                                 [&palette](const std::uint8_t &other)
                                 { return palette == other; });
                          });

                    for (const auto &palette : filter_palette)
                    {
                         settings.filters.value()
                           .update<ff_8::FilterTag::Palette>(palette)
                           .enable();
                         settings.filters.value()
                           .update<ff_8::FilterTag::Bpp>(bpp)
                           .enable();

                         auto out_framebuffer
                           = glengine::FrameBuffer{ specification };
                         // Generate the texture.
                         if (generate_texture(out_framebuffer))
                         {

                              // Determine output path based on COO
                              // presence.
                              const key_value_data cpm
                                = { .field_name = get_base_name(),
                                    .ext        = ".png",
                                    .language_code
                                    = m_map_group.opt_coo.has_value()
                                          && m_map_group.opt_coo.value()
                                               != open_viii::LangT::generic
                                        ? m_map_group.opt_coo
                                        : std::nullopt,
                                    .palette      = palette,
                                    .texture_page = texture_page };
                              std::filesystem::path out_path = cpm.replace_tags(
                                keyed_string, selections, selected_path);

                              // Start async save and store the future.
                              future_of_futures.push_back(save_image_pbo(
                                std::move(out_path),
                                std::move(out_framebuffer)));
                         }
                    }
               }
          }

          // No conflicting palettes — save the texture page normally.
          settings.filters.value().disable<ff_8::FilterTag::Palette>();
          settings.filters.value().disable<ff_8::FilterTag::Bpp>();

          auto out_framebuffer = glengine::FrameBuffer{ specification };
          if (generate_texture(out_framebuffer))
          {

               // Determine output path based on COO presence.
               const key_value_data cpm
                 = { .field_name    = get_base_name(),
                     .ext           = ".png",
                     .language_code = m_map_group.opt_coo.has_value()
                                          && m_map_group.opt_coo.value()
                                               != open_viii::LangT::generic
                                        ? m_map_group.opt_coo
                                        : std::nullopt,
                     .texture_page  = texture_page };
               std::filesystem::path out_path
                 = cpm.replace_tags(keyed_string, selections, selected_path);
               future_of_futures.push_back(save_image_pbo(
                 std::move(out_path), std::move(out_framebuffer)));
          }
     }

     // Return the list of save operations to the caller.
     return future_of_futures;
     // consume_futures(future_of_futures); // Optionally wait here.
}


/**
 * @brief Saves swizzled texture pages (and optionally palettes) to disk
 * asynchronously.
 *
 * Generates textures for each unique texture page (and conflicting
 * palettes if needed) and saves them as PNG files. If the texture has
 * palette conflicts (e.g., multiple palettes for the same texture page),
 * generates separate files for each conflicting palette.
 *
 * @param path The base directory path where the images will be saved.
 * @return A vector of futures representing the save operations, allowing
 * the caller to later wait or check for completion.
 *
 * @note Caller is responsible for consuming or waiting on the futures to
 * ensure save completion.
 */
[[nodiscard]] std::vector<std::future<void>>
  map_sprite::save_swizzle_as_one_image_textures(
    const std::string           &keyed_string,
    const std::filesystem::path &selected_path)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     consume_now(true);
     // Extract unique texture page IDs and BPP (bits per pixel) values
     // from the map.
     const auto  unique_values = get_all_unique_values_and_strings();
     const auto &unique_texture_page_ids
       = unique_values.texture_page_id().values();
     const auto &unique_bpp         = unique_values.bpp().values();
     const auto max_texture_page_id = std::ranges::max(unique_texture_page_ids);
     const auto max_source_x
       = m_map_group.maps.working().visit_tiles(
           [&](const auto &tiles) -> std::int32_t
           {
                auto f_t_range
                  = tiles
                    | std::ranges::views::filter(
                      [&](const auto &tile)
                      { return tile.texture_id() == max_texture_page_id; })
                    | std::ranges::views::transform(
                      [](const auto &tile) { return tile.source_x(); });
                return static_cast<std::int32_t>(std::ranges::max(f_t_range));
           })
         + static_cast<std::int32_t>(TILE_SIZE);

     // Backup and override current backup for exporting textures.
     settings_backup backup                      = get_backup_settings(true);
     backup.settings->disable_texture_page_shift = false;
     if (auto detected_height
         = static_cast<std::int32_t>(get_max_texture_height());
         detected_height == 0)
     {
          return {};
     }
     const std::int32_t height = 256 * m_render_framebuffer->scale();
     const std::int32_t width
       = ((256 * static_cast<std::int32_t>(max_texture_page_id)) + max_source_x)
         * m_render_framebuffer->scale();

     // If there’s only one bpp and at most one palette, nothing needs
     // saving.
     if (
       unique_bpp.size() == 1U
       && unique_values.palette().at(unique_bpp.front()).values().size() <= 1U)
     {
          return {};
     }

     // Prepare for gathering palette conflicts.
     const auto conflicting_palettes_map = get_conflicting_palettes();
     auto       conflicting_palettes_flatten
       = conflicting_palettes_map
         | std::ranges::views::values     // Get the vectors (values of the map)
         | std::ranges::views::join       // Flatten the vectors into a single
                                          // range
         | std::ranges::to<std::vector>();// merge to vector;
     sort_and_remove_duplicates(conflicting_palettes_flatten);

     // Prepare futures to track save operations.
     std::vector<std::future<void>> future_of_futures           = {};
     const unsigned int             max_number_of_texture_pages = 13U;
     future_of_futures.reserve(max_number_of_texture_pages);

     // Create an off-screen render texture to draw into.
     const auto specification
       = glengine::FrameBufferSpecification{ .width  = width,
                                             .height = height,
                                             .scale
                                             = m_render_framebuffer->scale() };


     // Loop over all unique texture pages.

     if (!std::ranges::empty(conflicting_palettes_flatten))
     {
          // Handle palette conflicts: export each conflicting palette
          // individually.
          for (const auto &bpp : unique_bpp)
          {
               const auto &unique_palette
                 = unique_values.palette().at(bpp).values();

               // Filter palettes that conflict with the current texture
               // page.
               auto filter_palette
                 = unique_palette
                   | std::views::filter(
                     [&conflicting_palettes_flatten](
                       const std::uint8_t &palette)
                     {
                          return std::ranges::any_of(
                            conflicting_palettes_flatten,
                            [&palette](const std::uint8_t &other)
                            { return palette == other; });
                     });

               for (const auto &palette : filter_palette)
               {
                    backup.filters.value()
                      .update<ff_8::FilterTag::Palette>(palette)
                      .enable();
                    backup.filters.value()
                      .update<ff_8::FilterTag::Bpp>(bpp)
                      .enable();

                    // Generate the texture.
                    auto out_framebuffer
                      = glengine::FrameBuffer{ specification };
                    if (generate_texture(out_framebuffer))
                    {

                         // Determine output path based on COO presence.
                         const key_value_data cpm
                           = { .field_name = get_base_name(),
                               .ext        = ".png",
                               .language_code
                               = m_map_group.opt_coo.has_value()
                                     && m_map_group.opt_coo.value()
                                          != open_viii::LangT::generic
                                   ? m_map_group.opt_coo
                                   : std::nullopt,
                               .palette = palette };
                         std::filesystem::path out_path = cpm.replace_tags(
                           keyed_string, selections, selected_path);
                         // Start async save and store the future.
                         future_of_futures.push_back(save_image_pbo(
                           std::move(out_path), std::move(out_framebuffer)));
                    }
               }
          }
     }

     // No conflicting palettes — save the texture page normally.
     backup.filters.value().disable<ff_8::FilterTag::Palette>();
     backup.filters.value().disable<ff_8::FilterTag::Bpp>();

     auto out_framebuffer = glengine::FrameBuffer{ specification };
     if (generate_texture(out_framebuffer))
     {
          // Determine output path based on COO presence.
          const key_value_data cpm
            = { .field_name    = get_base_name(),
                .ext           = ".png",
                .language_code = m_map_group.opt_coo.has_value()
                                     && m_map_group.opt_coo.value()
                                          != open_viii::LangT::generic
                                   ? m_map_group.opt_coo
                                   : std::nullopt };
          std::filesystem::path out_path
            = cpm.replace_tags(keyed_string, selections, selected_path);

          // Start async save and store the future.
          future_of_futures.push_back(
            save_image_pbo(std::move(out_path), std::move(out_framebuffer)));
     }


     // Return the list of save operations to the caller.
     return future_of_futures;
     // consume_futures(future_of_futures); // Optionally wait here.
}

[[nodiscard]] std::vector<std::future<void>> map_sprite::save_csv(
  const std::string           &keyed_string,
  const std::filesystem::path &selected_path)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }

     const key_value_data cpm
       = { .field_name = get_base_name(),
           .ext        = ".csv",
           .language_code
           = m_map_group.opt_coo.has_value()
                 && m_map_group.opt_coo.value() != open_viii::LangT::generic
               ? m_map_group.opt_coo
               : std::nullopt };

     // Prepare futures to track save operations.
     std::vector<std::future<void>> future_of_futures = {};

     future_of_futures.push_back(
       std::async(
         std::launch::async,
         [map  = m_map_group.maps.const_working(),
          pupu = m_map_group.maps.working_pupu()
                 | std::views::transform([](const auto &pupu_id)
                                         { return pupu_id.raw(); })
                 | std::ranges::to<std::vector>(),
          path
          = cpm.replace_tags(keyed_string, selections, selected_path)]() mutable
         {
              map.unshift_from_origin();
              map.save_csv(path, pupu);
         }));
     return future_of_futures;
}


std::string map_sprite::get_base_name() const
{
     const auto field = m_map_group.field.lock();
     if (!field)
     {
          spdlog::error(
            "Failed to lock m_map_group.field: shared_ptr is expired.");
          return {};
     }
     return str_to_lower(field->get_base_name());
}

/**
 * @brief Saves unique "Pupu" textures for the map field to individual PNG
 * files.
 *
 * This function sets up temporary settings specifically for Pupu texture
 * rendering, generates textures using the current map data, and
 * asynchronously saves them to disk. Each texture is saved under a
 * specific file naming pattern based on the field name and Pupu ID.
 *
 * @param path Filesystem path where the textures should be saved. Must be
 * a directory.
 * @return A vector of futures, each wrapping a future task that will save
 * one texture. Caller should consume or wait on these to ensure saving
 * completes.
 */
[[nodiscard]] std::vector<std::future<void>>
  map_sprite::save_deswizzle_textures(
    const std::string           &keyed_string,
    const std::filesystem::path &selected_path)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     consume_now();
     // Backup current settings and adjust for saving Pupu textures
     auto       settings = get_backup_settings(false);

     // Acquire the field associated with this map group
     const auto field    = m_map_group.field.lock();
     if (!field)
     {
          spdlog::error(
            "Failed to lock m_map_group.field: shared_ptr is expired.");
          return {};// Field no longer exists, nothing to save
     }

     const std::string field_name
       = std::string{ str_to_lower(field->get_base_name()) };
     const std::vector<ff_8::PupuID> &unique_pupu_ids
       = working_unique_pupu();// Get list of unique Pupu IDs
     const unsigned int max_number_of_texture_pages
       = 13U;// Reserve space for futures
     std::vector<std::future<void>> future_of_futures = {};
     future_of_futures.reserve(max_number_of_texture_pages);

     // Setup an off-screen render texture
     iRectangle const canvas = m_map_group.maps.const_working().canvas()
                               * m_render_framebuffer->scale();
     const auto specification
       = glengine::FrameBufferSpecification{ .width  = canvas.width(),
                                             .height = canvas.height(),
                                             .scale
                                             = m_render_framebuffer->scale() };


     // Loop through each Pupu ID and generate/save textures
     for (const ff_8::PupuID &pupu : unique_pupu_ids)
     {
          settings.filters.value()
            .update<ff_8::FilterTag::Pupu>(pupu)
            .enable();// Enable this specific Pupu ID
          auto out_framebuffer = glengine::FrameBuffer{ specification };
          if (generate_texture(out_framebuffer))
          {
               const key_value_data cpm
                 = { .field_name    = get_base_name(),
                     .ext           = ".png",
                     .language_code = m_map_group.opt_coo.has_value()
                                          && m_map_group.opt_coo.value()
                                               != open_viii::LangT::generic
                                        ? m_map_group.opt_coo
                                        : std::nullopt,
                     .pupu_id       = pupu.raw() };
               std::filesystem::path out_path
                 = cpm.replace_tags(keyed_string, selections, selected_path);
               future_of_futures.push_back(save_image_pbo(
                 std::move(out_path), std::move(out_framebuffer)));
          }
     }

     return future_of_futures;
     // Note: Caller should consume_futures(future_of_futures) to wait for
     // saves to finish
}
void map_sprite::save_deswizzle_generate_toml(
  const std::string           &keyed_string,
  const std::filesystem::path &selected_path)
{
     consume_now();
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     open_viii::LangT used_coo  = {};
     toml::table     *coo_table = get_deswizzle_combined_coo_table(
       &used_coo, selections->get<ConfigKey::TOMLFailOverForEditor>());
     spdlog::info("used_coo: {} ", used_coo);
     if (!coo_table)
     {
          spdlog::error("Failed to create coo_table");
          return;
     }
     save_deswizzle_generate_toml(
       keyed_string, selected_path, *coo_table, selections);
}

void map_sprite::save_deswizzle_generate_toml(
  const std::string                 &keyed_string,
  const std::filesystem::path       &selected_path,
  toml::table                       &coo_table,
  const std::shared_ptr<Selections> &selections)
{
     // Backup current settings and adjust for saving Pupu textures
     auto       settings = get_backup_settings(false);

     // Acquire the field associated with this map group
     const auto field    = m_map_group.field.lock();
     if (!field)
     {
          spdlog::error(
            "Failed to lock m_map_group.field: shared_ptr is expired.");
          return;// Field no longer exists, nothing to save
     }

     const std::string field_name
       = std::string{ str_to_lower(field->get_base_name()) };
     const std::vector<ff_8::PupuID> &unique_pupu_ids
       = working_unique_pupu();// Get list of unique Pupu IDs
     // Setup an off-screen render texture
     iRectangle const canvas = m_map_group.maps.const_working().canvas()
                               * m_render_framebuffer->scale();
     const auto specification
       = glengine::FrameBufferSpecification{ .width  = canvas.width(),
                                             .height = canvas.height(),
                                             .scale
                                             = m_render_framebuffer->scale() };
     const auto coo
       = m_map_group.opt_coo.has_value()
             && m_map_group.opt_coo.value() != open_viii::LangT::generic
           ? m_map_group.opt_coo
           : std::nullopt;
     if (coo.has_value())
     {
          spdlog::info("coo: {} ", *coo);
     }
     if (m_map_group.opt_coo.has_value())
     {
          spdlog::info("m_map_group.opt_coo: {} ", *m_map_group.opt_coo);
     }

     toml::table ids_table = {};
     for (auto &&pupu : unique_pupu_ids)
     {
          const auto pupu_str = fmt::format("{:08X}", pupu.raw());
          ids_table.insert(pupu_str, pupu.raw());
     }
     coo_table.insert_or_assign("unique_pupu_ids", std::move(ids_table));

     // Loop through each Pupu ID and generate/save textures
     // for (auto &&[index, pupu_range] : enumerated)


     for (auto &&pupu : unique_pupu_ids)
     {
          auto &filters         = settings.filters.value();
          auto  updated_filters = std::forward_as_tuple(
            filters.update<ff_8::FilterTag::MultiPupu>(std::vector{ pupu })
              .enable());
          auto out_framebuffer = glengine::FrameBuffer{ specification };
          if (generate_texture(
                out_framebuffer))// todo make a if would pass filter with
                                 // out generating textures.
          {
               toml::table          file;
               const key_value_data cpm
                 = { .field_name    = field_name,
                     .ext           = ".png",
                     .language_code = coo,
                     .pupu_id       = static_cast<std::uint32_t>(pupu) };

               std::apply(
                 [&](auto &&...fns) { (fns.update(file), ...); },
                 updated_filters);
               std::filesystem::path out_path
                 = cpm.replace_tags(keyed_string, selections, selected_path);

               coo_table.insert_or_assign(
                 out_path.filename().string(), std::move(file));
          }
     }
}

[[nodiscard]] std::string map_sprite::get_recommended_prefix()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     const auto field = m_map_group.field.lock();
     if (!field)
     {
          spdlog::error(
            "Failed to lock m_map_group.field: shared_ptr is expired.");
          return {};// Field no longer exists, nothing to save
     }

     const std::string field_name
       = std::string{ str_to_lower(field->get_base_name()) };
     const auto coo
       = m_map_group.opt_coo.has_value()
             && m_map_group.opt_coo.value() != open_viii::LangT::generic
           ? m_map_group.opt_coo
           : std::nullopt;
     const key_value_data cpm
       = { .field_name = field_name, .ext = ".png", .language_code = coo };
     static const std::string pattern = "{field_name}{_{2_letter_lang}}";

     return cpm.replace_tags(pattern, selections, "{current_path}");
}
[[nodiscard]] settings_backup
  map_sprite::get_backup_settings(const bool draw_swizzle)
{
     auto backup = settings_backup{
          m_filters, m_settings
          //, m_render_framebuffer.mutable_scale()
     };
     backup.filters = ff_8::filters{ false };
     backup.filters.value().get<ff_8::FilterTag::Swizzle>()
       = backup.filters.backup().get<ff_8::FilterTag::Swizzle>();
     backup.filters.value().get<ff_8::FilterTag::SwizzleAsOneImage>()
       = backup.filters.backup().get<ff_8::FilterTag::SwizzleAsOneImage>();
     backup.filters.value().get<ff_8::FilterTag::Deswizzle>()
       = backup.filters.backup().get<ff_8::FilterTag::Deswizzle>();
     backup.filters.value().get<ff_8::FilterTag::FullFileName>()
       = backup.filters.backup().get<ff_8::FilterTag::FullFileName>();
     backup.filters.value().get<ff_8::FilterTag::Map>()
       = backup.filters.backup().get<ff_8::FilterTag::Map>();
     backup.settings->draw_swizzle = draw_swizzle;// No swizzling when saving
     backup.settings->disable_texture_page_shift
       = true;                              // Disable texture page shifts
     backup.settings->disable_blends = true;// Disable blending

     return backup;
}

[[nodiscard]] std::vector<std::future<void>>
  map_sprite::save_deswizzle_full_filename_textures(
    const std::string           &keyed_string,
    const std::filesystem::path &selected_path)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return {};
     }
     consume_now();
     // Backup current settings and adjust for saving Pupu textures
     auto       settings = get_backup_settings(false);

     // Acquire the field associated with this map group
     const auto field    = m_map_group.field.lock();
     if (!field)
     {
          spdlog::error(
            "Failed to lock m_map_group.field: shared_ptr is expired.");
          return {};// Field no longer exists, nothing to save
     }

     const std::string field_name
       = std::string{ str_to_lower(field->get_base_name()) };
     std::vector<std::future<void>> future_of_futures = {};

     // Setup an off-screen render texture
     iRectangle const canvas = m_map_group.maps.const_working().canvas()
                               * m_render_framebuffer->scale();
     const auto specification
       = glengine::FrameBufferSpecification{ .width  = canvas.width(),
                                             .height = canvas.height(),
                                             .scale
                                             = m_render_framebuffer->scale() };
     open_viii::LangT   coo = {};
     const toml::table *coo_table
       = get_deswizzle_combined_coo_table(&coo, fme::FailOverLevels::All);

     if (!coo_table)
     {
          return {};
     }
     for (auto &&[file_name, file_node] : *coo_table)
     {
          if (file_name == "unique_pupu_ids")
               continue;
          if (!file_node.is_table())
               continue;
          auto &filters = settings.filters.value();
          filters.reload(*file_node.as_table());

          auto out_framebuffer = glengine::FrameBuffer{ specification };
          if (generate_texture(out_framebuffer))
          {
               const key_value_data cpm
                 = { .field_name    = field_name,
                     .full_filename = std::string(file_name),
                     .language_code = coo == open_viii::LangT::generic
                                        ? std::optional<open_viii::LangT>(coo)
                                        : std::optional<open_viii::LangT>{} };

               std::filesystem::path out_path
                 = cpm.replace_tags(keyed_string, selections, selected_path);
               if (selections->get<ConfigKey::BatchGenerateColorfulMask>())
               {
                    std::filesystem::path mask_path
                      = out_path.parent_path() / (out_path.stem().string() + "_mask" + out_path.extension().string());
                    spdlog::debug(
                      "Queued image save: mask='{}'", mask_path.string());
                    const auto colors_to_pupu =
                      [&]() -> std::vector<std::tuple<glm::vec4, ff_8::PupuID>>
                    {
                         if (selections->get<
                               ConfigKey::BatchGenerateWhiteOnBlackMask>())
                         {
                              return working_unique_color_pupu();
                         }
                         return {};
                    };
                    future_of_futures.push_back(save_image_pbo(
                      std::move(mask_path),
                      out_framebuffer.clone(),
                      GL_COLOR_ATTACHMENT1,
                      colors_to_pupu()));
               }
               spdlog::debug("Queued image save: main='{}'", out_path.string());
               future_of_futures.push_back(save_image_pbo(
                 std::move(out_path), std::move(out_framebuffer)));
          }
     }
     return future_of_futures;
     // Note: Caller should consume_futures(future_of_futures) to wait for
     // saves to finish
}

[[nodiscard]] const std::map<
  std::string,
  std::string> &
  map_sprite::get_deswizzle_combined_textures_tooltips()
{
     return m_cache_framebuffer_tooltips;
}


[[nodiscard]] const std::map<
  std::string,
  std::vector<ff_8::PupuID>> &
  map_sprite::get_deswizzle_combined_textures_pupuids()
{
     return m_cache_framebuffer_pupuids;
}


[[nodiscard]] std::expected<
  std::map<
    std::string,
    std::optional<glengine::FrameBuffer>> *,
  std::string>
  map_sprite::get_deswizzle_combined_textures(
    const int  in_scale,
    const bool force_load)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          return std::unexpected(
            "Failed to lock m_selections: shared_ptr is expired.");
     }
     if (!all_futures_done())
     {
          return std::unexpected("Not all futures are done!");
     }
     if (std::ranges::all_of(
           *m_texture.get(),
           [](const glengine::Texture &texture)
           {
                const auto size = texture.get_size();
                // spdlog::info("{}", size);
                return size.x == 0 || size.y == 0;
           }))
     {
          return std::unexpected("Textures are not loaded yet!");
     }

     // Acquire the field associated with this map group
     const auto field = m_map_group.field.lock();
     if (!field)
     {
          return std::unexpected(
            "Failed to lock m_map_group.field: shared_ptr is expired.");
     }

     const std::string field_name
       = std::string{ str_to_lower(field->get_base_name()) };

     std::vector<std::future<void>> futures;
     const toml::table *coo_table = get_deswizzle_combined_coo_table(
       {}, selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
     {
          return std::unexpected("Failed to load coo_table.");
     }
     for (auto &&[file_name, file_node] : *coo_table)
     {
          if (file_name == "unique_pupu_ids")
               continue;
          if (!file_node.is_table())
               continue;
          auto out_file_name_str = std::string(file_name);
          if (m_cache_framebuffer.contains(out_file_name_str))
          {
               continue;
          }
          // Insert placeholder entry in the map *synchronously*
          auto [it, inserted]
            = m_cache_framebuffer.emplace(out_file_name_str, std::nullopt);
          if (!inserted)
               continue;

          const int scale = [&]()
          {
               if (
                 in_scale <= 0
                 || !std::has_single_bit(static_cast<unsigned int>(in_scale)))
               {
                    return m_render_framebuffer->scale();
               }
               return in_scale;
          }();
          const auto queue_framebuffer_load
            = [this, it, file_table = file_node.as_table(), scale]()
          {
               if (!m_texture)
               {
                    return;
               }
               // Setup an off-screen render texture
               iRectangle const canvas
                 = m_map_group.maps.const_working().canvas() * scale;
               const auto specification
                 = glengine::FrameBufferSpecification{ .width = canvas.width(),
                                                       .height
                                                       = canvas.height(),
                                                       .scale = scale };
               // Backup current settings and adjust for saving Pupu
               // textures
               auto  settings = get_backup_settings(false);
               auto &filters  = settings.filters.value();
               filters.reload(*file_table);

               // Generate
               m_cache_framebuffer_tooltips[it->first]
                 = generate_deswizzle_combined_tool_tip(file_table);

               cache_pupuids(it->first, filters);

               if (glengine::FrameBuffer fb(specification);
                   generate_texture(fb))
               {
                    it->second = std::move(fb);// replace empty optional
               }
               else
               {
                    it->second.reset();// stays empty if generation failed
               }
          };

          if (force_load || m_settings.force_loading)
          {
               queue_framebuffer_load();
          }
          else
          {
               futures.emplace_back(
                 std::async(std::launch::deferred, queue_framebuffer_load));
          }
     }
     if (!(force_load || m_settings.force_loading))
     {
          m_future_consumer += std::move(futures);
          spdlog::trace(
            "{}:{} Framebuffer generation queued up. Currently framebuffer "
            "placeholders empty.",
            __FILE__, __LINE__);
     }
     else
     {

          spdlog::debug(
            "{}:{} Framebuffer generation force loaded!", __FILE__, __LINE__);
     }
     return &m_cache_framebuffer;
}

void map_sprite::cache_pupuids(
  const std::string   &file_name_str,
  const ff_8::filters &filters) const
{
     if (
       filters.enabled<ff_8::FilterTag::MultiPupu>()
       && filters.enabled<ff_8::FilterTag::Pupu>())
     {
          m_cache_framebuffer_pupuids[file_name_str]
            = filters.value<ff_8::FilterTag::MultiPupu>();
          m_cache_framebuffer_pupuids[file_name_str].push_back(
            filters.value<ff_8::FilterTag::Pupu>());
     }
     else if (filters.enabled<ff_8::FilterTag::MultiPupu>())
     {
          m_cache_framebuffer_pupuids[file_name_str]
            = filters.value<ff_8::FilterTag::MultiPupu>();
     }
     else if (filters.enabled<ff_8::FilterTag::Pupu>())
     {
          m_cache_framebuffer_pupuids[file_name_str].push_back(
            filters.value<ff_8::FilterTag::Pupu>());
     }
}
// m_cache_framebuffer_pupuids


std::vector<ff_8::PupuID> map_sprite::generate_deswizzle_combined_pupu_id(
  const toml::table *file_table) const
{
     std::vector<ff_8::PupuID> result{};

     if (auto pupu_array = (*file_table)["filter_multi_pupu"].as_array();
         pupu_array)
     {
          for (auto &elem : *pupu_array)
          {
               if (auto val = elem.value<uint32_t>(); val)
               {
                    result.emplace_back(*val);
               }
          }
     }
     if (auto val = (*file_table)["filter_pupu"].value<uint32_t>(); val)
     {
          result.emplace_back(*val);
     }
     return result;
};

std::string map_sprite::generate_deswizzle_combined_tool_tip(
  const toml::table *file_table) const
{
     std::ostringstream ss{};
     ss << *file_table;

     if (auto pupu_array = (*file_table)["filter_multi_pupu"].as_array();
         pupu_array)
     {
          for (auto &elem : *pupu_array)
          {
               if (auto val = elem.value<uint32_t>(); val)
               {
                    ff_8::PupuID pupu_id{ *val };

                    ss << fmt::format(
                      "\n\nPupu ID: {:08X} - {}\n{}\n",
                      *val,
                      *val,
                      pupu_id.create_summary());
               }
          }
     }
     return ss.str();
};

open_viii::LangT
  map_sprite::get_used_coo(const fme::FailOverLevels max_failover) const
{
     const auto selections = m_selections.lock();
     if (!selections)
          return open_viii::LangT::generic;

     const auto field = m_map_group.field.lock();
     if (!field)
          return open_viii::LangT::generic;

     const std::string field_name = str_to_lower(field->get_base_name());

     const auto        coo_opt
       = m_map_group.opt_coo.has_value()
             && m_map_group.opt_coo.value() != open_viii::LangT::generic
           ? m_map_group.opt_coo
           : field->get_lang_from_fl_paths();

     const auto failover_sequence = std::to_array(
       { coo_opt.value_or(open_viii::LangT::generic), open_viii::LangT::generic,
         open_viii::LangT::en, open_viii::LangT::fr, open_viii::LangT::de,
         open_viii::LangT::it, open_viii::LangT::es, open_viii::LangT::jp });

     const key_value_data        config_path_values{ .ext = ".toml" };
     const std::filesystem::path config_path = config_path_values.replace_tags(
       selections->get<ConfigKey::OutputTomlPattern>(), selections);

     auto         config      = Configuration(config_path);
     toml::table &root_table  = config;

     toml::table *field_table = nullptr;
     if (auto it_base = root_table.find(field_name);
         it_base != root_table.end() && it_base->second.is_table())
     {
          field_table = it_base->second.as_table();
     }

     if (!field_table)
          return coo_opt.value_or(open_viii::LangT::generic);

     auto get_table_by_coo = [&](const open_viii::LangT lang) -> toml::table *
     {
          const std::string key
            = (lang != open_viii::LangT::generic)
                ? std::string(open_viii::LangCommon::to_string_3_char(lang))
                : "x";

          if (auto it_coo = field_table->find(key);
              it_coo != field_table->end() && it_coo->second.is_table())
               return it_coo->second.as_table();

          return nullptr;
     };

     for (const auto &[index, lang] : failover_sequence | std::views::enumerate)
     {
          auto *coo_table = get_table_by_coo(lang);

          if (
            coo_table
            && (max_failover == fme::FailOverLevels::Loaded || !coo_table->empty()))
               return lang;

          if (std::cmp_equal(index, std::to_underlying(max_failover)))
               break;
     }

     // mimic the "insert" branch of the original
     return coo_opt.value_or(open_viii::LangT::generic);
}

toml::table *map_sprite::get_deswizzle_combined_coo_table(
  open_viii::LangT *const   out_used_coo,
  const fme::FailOverLevels max_failover) const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections.");
          return nullptr;
     }
     const auto field = m_map_group.field.lock();
     if (!field)
     {
          spdlog::error("Failed to lock m_map_group.field.");
          return nullptr;
     }

     const std::string    field_name = str_to_lower(field->get_base_name());
     const key_value_data config_path_values = { .ext = ".toml" };
     const std::filesystem::path config_path = config_path_values.replace_tags(
       selections->get<ConfigKey::OutputTomlPattern>(), selections);
     auto         config     = Configuration(config_path);
     toml::table &root_table = config;

     const auto   coo_opt    = [&]()
     {
          if (m_map_group.opt_coo.has_value())
          {
               return m_map_group.opt_coo;
          }
          return field->get_lang_from_fl_paths();
     }();

     toml::table *field_table = nullptr;

     toml::table *coo_table   = nullptr;


     if (auto it_base = root_table.find(field_name);
         it_base != root_table.end() && it_base->second.is_table())
     {
          field_table = it_base->second.as_table();
     }
     else
     {
          auto &&[it, inserted] = root_table.insert(field_name, toml::table{});
          if (inserted)
          {
               field_table
                 = it->second.is_table() ? it->second.as_table() : nullptr;
          }
     }
     if (!field_table)
     {
          return nullptr;
     }
     const auto failover_sequence = std::to_array(
       { coo_opt.value_or(open_viii::LangT::generic), open_viii::LangT::generic,
         open_viii::LangT::en, open_viii::LangT::fr, open_viii::LangT::de,
         open_viii::LangT::it, open_viii::LangT::es, open_viii::LangT::jp });
     const auto get_key_str = [&](const open_viii::LangT lang)
     {
          if (lang != open_viii::LangT::generic)
          {
               return std::string(
                 open_viii::LangCommon::to_string_3_char(lang));
          }
          return std::string("x");
     };
     const auto get_table_by_coo = [&](const std::string &key) -> toml::table *
     {
          if (auto it_coo = field_table->find(key);
              it_coo != field_table->end() && it_coo->second.is_table())
          {
               spdlog::trace("{}:{}, found key!: {}", __FILE__, __LINE__, key);
               return it_coo->second.as_table();
          }
          spdlog::trace(
            "{}:{}, NO! did not find key: {}", __FILE__, __LINE__, key);
          return nullptr;
     };

     for (const auto &[index, lang] : failover_sequence | std::views::enumerate)
     {
          const std::string key = get_key_str(lang);
          coo_table             = get_table_by_coo(key);
          const bool has_valid_table
            = (coo_table != nullptr && !std::ranges::empty(*coo_table));
          const bool is_failover_match
            = std::cmp_equal(std::to_underlying(max_failover), index);
          const bool matches_opt_coo
            = (m_map_group.opt_coo.has_value() && m_map_group.opt_coo.value() == lang);

          if (has_valid_table || is_failover_match || matches_opt_coo)
          {
               if (out_used_coo)
               {
                    *out_used_coo = lang;
               }
               if (!coo_table)
               {
                    spdlog::trace("{}:{}, key:{}", __FILE__, __LINE__, key);
                    auto &&[it, inserted]
                      = field_table->insert(key, toml::table{});
                    if (inserted)
                    {
                         coo_table = it->second.is_table()
                                       ? it->second.as_table()
                                       : nullptr;
                    }
               }
               break;
          }
     }
     return coo_table;
}

[[nodiscard]] std::vector<std::string> map_sprite::toml_filenames() const
{
     std::vector<std::string> result{};
     const toml::table       *coo_table
       = get_deswizzle_combined_coo_table({}, fme::FailOverLevels::All);
     if (!coo_table)
     {
          return result;
     }
     result.reserve(coo_table->size());
     for (const auto &[key, value] : *coo_table)
     {
          if (!value.is_table())
          {
               continue;
          }
          if (auto key_str = key.str();
              key_str.size() > 4
              && key_str.ends_with(".png"))// todo case insensitive compare?
          {
               result.emplace_back(std::move(key_str));
          }
     }
     std::ranges::sort(result);
     result.erase(
       std::ranges::unique(result).begin(), std::ranges::end(result));
     return result;
}

toml::table *map_sprite::get_deswizzle_combined_toml_table(
  const std::string &file_name_str) const
{

     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections.");
          return nullptr;
     }
     toml::table *coo_table = get_deswizzle_combined_coo_table(
       {}, selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
     {
          return nullptr;
     }
     if (auto it_base = coo_table->find(file_name_str);
         it_base != coo_table->end() && it_base->second.is_table())
     {
          return it_base->second.as_table();
     }
     return nullptr;
}

[[nodiscard]] toml::table *map_sprite::rename_deswizzle_combined_toml_table(
  const std::string &old_file_name,
  const std::string &new_file_name)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections.");
          return nullptr;
     }
     toml::table *coo_table = get_deswizzle_combined_coo_table(
       {}, selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
     {
          spdlog::error("Failed to retrieve coo_table.");
          return nullptr;
     }

     // Check if new name already exists
     if (coo_table->contains(new_file_name))
     {
          spdlog::warn(
            "Cannot rename: new texture name '{}' already exists.",
            new_file_name);
          return nullptr;
     }

     // Lookup old table
     auto it_old = coo_table->find(old_file_name);
     if (it_old == coo_table->end() || !it_old->second.is_table())
     {
          spdlog::warn(
            "Cannot rename: old texture '{}' not found or is not a table.",
            old_file_name);
          return nullptr;
     }

     // Clone and insert under new name
     toml::table new_table = *it_old->second.as_table();
     auto &&[it_new, inserted]
       = coo_table->insert_or_assign(new_file_name, std::move(new_table));

     if (!inserted)
     {
          spdlog::error(
            "Unexpected: insert_or_assign returned false for '{}'.",
            new_file_name);
          return nullptr;
     }
     it_new->second.as_table()->insert_or_assign("old_key", old_file_name);

     spdlog::info(
       "Renamed texture entry from '{}' to '{}'.",
       old_file_name,
       new_file_name);

     // Remove old entry
     coo_table->erase(old_file_name);

     // Move framebuffer cache
     if (auto it_cache = m_cache_framebuffer.find(old_file_name);
         it_cache != m_cache_framebuffer.end())
     {
          m_cache_framebuffer[new_file_name] = std::move(it_cache->second);
          m_cache_framebuffer.erase(it_cache);
     }

     // Move tooltip cache
     if (auto it_tooltip = m_cache_framebuffer_tooltips.find(old_file_name);
         it_tooltip != m_cache_framebuffer_tooltips.end())
     {
          m_cache_framebuffer_tooltips[new_file_name]
            = std::move(it_tooltip->second);
          m_cache_framebuffer_tooltips.erase(it_tooltip);
     }

     if (auto it_tooltip = m_cache_framebuffer_pupuids.find(old_file_name);
         it_tooltip != m_cache_framebuffer_pupuids.end())
     {
          m_cache_framebuffer_pupuids[new_file_name]
            = std::move(it_tooltip->second);
          m_cache_framebuffer_pupuids.erase(it_tooltip);
     }

     return it_new->second.as_table();
}

[[nodiscard]] std::size_t
  map_sprite::remove_deswizzle_combined_toml_table(const std::string &name)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections.");
          return {};
     }
     size_t       removed_count = 0;

     toml::table *coo_table     = get_deswizzle_combined_coo_table(
       {}, selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
     {
          spdlog::error("Failed to retrieve coo_table.");
          return 0;
     }

     removed_count += coo_table->erase(name);
     removed_count += m_cache_framebuffer.erase(name);
     removed_count += m_cache_framebuffer_tooltips.erase(name);
     removed_count += m_cache_framebuffer_pupuids.erase(name);

     if (removed_count > 0)
     {
          spdlog::info("Removed texture entry '{}'.", name);
     }
     else
     {
          spdlog::debug("Texture entry '{}' not found in any table.", name);
     }

     return removed_count;
}

[[nodiscard]] toml::table *map_sprite::add_deswizzle_combined_toml_table(
  const std::string &new_file_name)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections.");
          return nullptr;
     }
     toml::table *coo_table = get_deswizzle_combined_coo_table(
       {}, selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
     {
          spdlog::error("Failed to retrieve coo_table.");
          return nullptr;
     }

     if (coo_table->contains(new_file_name))
     {
          spdlog::warn(
            "Cannot insert: entry '{}' already exists in coo_table.",
            new_file_name);
          return nullptr;
     }

     // Create and populate new entry
     toml::table new_table{};
     filter().update_table(new_table);// This mutates new_table directly

     // Insert into coo_table
     auto [it, inserted]
       = coo_table->insert(new_file_name, std::move(new_table));
     if (!inserted)
     {
          spdlog::error("Failed to insert new entry '{}'.", new_file_name);
          return nullptr;
     }

     spdlog::info("Inserted new texture entry '{}'.", new_file_name);
     m_cache_framebuffer_tooltips[new_file_name]
       = generate_deswizzle_combined_tool_tip(&new_table);
     cache_pupuids(new_file_name, filter());

     return it->second.as_table();
}

void map_sprite::refresh_tooltip(const std::string &file_name)
{
     if (const auto *table = get_deswizzle_combined_toml_table(file_name);
         table)
     {
          m_cache_framebuffer_tooltips[file_name]
            = generate_deswizzle_combined_tool_tip(table);
          m_cache_framebuffer_pupuids[file_name]
            = generate_deswizzle_combined_pupu_id(table);
     }
}

void map_sprite::apply_multi_pupu_filter_deswizzle_combined_toml_table(
  const std::string                              &file_name_key,
  const ff_8::filter<ff_8::FilterTag::MultiPupu> &new_filter)
{
     if (auto *table = get_deswizzle_combined_toml_table(file_name_key))
     {
          if (new_filter.enabled())
          {
               // Update in-memory filter state from the table
               ff_8::filter<ff_8::FilterTag::MultiPupu> copy_filter
                 = { new_filter.value(), ff_8::FilterSettings::Toggle_Enabled };

               copy_filter.combine(*table);
               copy_filter.update(*table);
          }
          else
          {
               ff_8::filter<ff_8::FilterTag::MultiPupu> copy_filter
                 = (ff_8::FilterSettings::Toggle_Enabled);
               copy_filter.reload(*table);
               ff_8::filter<ff_8::FilterTag::MultiPupu>::value_type tempvec
                 = {};

               std::ranges::remove_copy_if(
                 copy_filter.value(),
                 std::back_inserter(tempvec),
                 [&](const auto &value)
                 {
                      return std::ranges::find(new_filter.value(), value)
                             != std::ranges::end(new_filter.value());
                 });
               copy_filter.update(std::move(tempvec));
               copy_filter.update(*table);
          }
     }
}


toml::table *map_sprite::add_combine_deswizzle_combined_toml_table(
  const std::vector<std::string> &file_names,
  const std::string              &new_file_name)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections.");
          return nullptr;
     }
     toml::table *coo_table = get_deswizzle_combined_coo_table(
       {}, selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
     {
          return nullptr;
     }
     ff_8::filters tmp_filters = { false };
     for (const std::string &file_name_str : file_names)
     {
          if (auto it_base = coo_table->find(file_name_str);
              it_base != coo_table->end() && it_base->second.is_table())
          {
               tmp_filters.combine(*it_base->second.as_table());
          }
     }

     toml::table new_table{};
     tmp_filters.update_table(new_table);// This mutates new_table directly
     auto [it, inserted]
       = coo_table->insert(new_file_name, std::move(new_table));
     if (!inserted)
     {
          spdlog::error("Failed to insert new entry '{}'.", new_file_name);
          return nullptr;
     }
     spdlog::info("Inserted new texture entry '{}'.", new_file_name);
     m_cache_framebuffer_tooltips[new_file_name]
       = generate_deswizzle_combined_tool_tip(&new_table);
     cache_pupuids(new_file_name, tmp_filters);
     return it->second.as_table();
}


void map_sprite::copy_deswizzle_combined_toml_table(
  const std::vector<std::string>            &existing_names,
  std::move_only_function<std::string(void)> generate_name)
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections.");
          return;
     }
     toml::table *coo_table = get_deswizzle_combined_coo_table(
       {}, selections->get<ConfigKey::TOMLFailOverForEditor>());
     if (!coo_table)
          return;

     for (const std::string &name : existing_names)
     {
          auto it_src = coo_table->find(name);
          if (it_src == coo_table->end() || !it_src->second.is_table())
               continue;

          toml::table *source_table  = it_src->second.as_table();

          std::string  new_file_name = generate_name();
          if (new_file_name.empty())
          {
               spdlog::warn(
                 "Generated name is empty; skipping copy for '{}'.", name);
               continue;
          }

          toml::table new_table = *source_table;
          auto [it, inserted]
            = coo_table->insert(new_file_name, std::move(new_table));
          if (!inserted)
          {
               spdlog::error("Failed to insert new entry '{}'.", new_file_name);
               continue;
          }
          spdlog::info("Copied table '{}' to '{}'.", name, new_file_name);
     }
}


uint32_t map_sprite::get_max_texture_height() const
{
     if (!m_texture)
     {
          spdlog::error("{}:{} m_texture is nullptr", __FILE__, __LINE__);
          return 0;
     }
     auto transform_range
       = (*m_texture)
         | std::views::transform([](const glengine::Texture &texture)
                                 { return texture.height(); });
     auto     max_height_it = std::ranges::max_element(transform_range);
     uint32_t tex_height    = {};
     if (max_height_it != std::ranges::end(transform_range))
     {
          tex_height = static_cast<std::uint32_t>(*max_height_it);
          if (tex_height == 0)
          {
               spdlog::error(
                 "{}:{} Max texture height is 0", __FILE__,
                 __LINE__);//, falling back to mim_texture_height
               // tex_height = 256U; // Default to 256 if all heights are 0
          }
     }
     else
     {
          static constexpr std::uint16_t mim_texture_height = 256U;
          tex_height = mim_texture_height;
          spdlog::warn(
            "{}:{} No textures in m_texture, using default height {}", __FILE__,
            __LINE__, tex_height);
     }
     if (const auto tex_height_scale = static_cast<std::uint16_t>(
           static_cast<std::uint16_t>(m_imported_tile_size >> 4U) << 8U);
         m_using_imported_texture
         && std::cmp_greater(tex_height_scale, tex_height))
     {
          tex_height = tex_height_scale;
     }
     spdlog::trace(
       "{}:{} get_max_texture_height: returning {}", __FILE__, __LINE__,
       tex_height);
     return tex_height;
}
// std::filesystem::path map_sprite::save_path_coo(
//   fmt::format_string<
//     std::string_view,
//     std::string_view,
//     uint8_t>                   pattern,
//   const std::filesystem::path &path,
//   const std::string_view      &field_name,
//   const uint8_t                texture_page,
//   const open_viii::LangT       coo)
// {
//      return path
//             / fmt::vformat(
//               fmt::string_view(pattern),
//               fmt::make_format_args(
//                 field_name,
//                 open_viii::LangCommon::to_string(coo),
//                 texture_page));
// }
// std::filesystem::path map_sprite::save_path_coo(
//   fmt::format_string<
//     std::string_view,
//     std::string_view,
//     uint8_t,
//     uint8_t>                   pattern,
//   const std::filesystem::path &path,
//   const std::string_view      &field_name,
//   const uint8_t                texture_page,
//   const uint8_t                palette,
//   const open_viii::LangT       coo)
// {
//      return path
//             / fmt::vformat(
//               fmt::string_view(pattern),
//               fmt::make_format_args(
//                 field_name,
//                 open_viii::LangCommon::to_string(coo),
//                 texture_page,
//                 palette));
// }
// std::filesystem::path map_sprite::save_path_coo(
//   fmt::format_string<
//     std::string_view,
//     std::string_view,
//     ff_8::PupuID>              pattern,
//   const std::filesystem::path &path,
//   const std::string_view      &field_name,
//   const ff_8::PupuID           pupu,
//   const open_viii::LangT       coo)
// {
//      return path
//             / fmt::vformat(
//               fmt::string_view(pattern),
//               fmt::make_format_args(
//                 field_name, open_viii::LangCommon::to_string(coo), pupu));
// }
// std::filesystem::path map_sprite::save_path(
//   fmt::format_string<
//     std::string_view,
//     std::uint8_t>              pattern,
//   const std::filesystem::path &path,
//   const std::string_view      &field_name,
//   const std::uint8_t           texture_page)
// {
//      return path
//             / fmt::vformat(
//               fmt::string_view(pattern),
//               fmt::make_format_args(field_name, texture_page));
// }
// std::filesystem::path map_sprite::save_path(
//   fmt::format_string<
//     std::string_view,
//     std::uint8_t,
//     std::uint8_t>              pattern,
//   const std::filesystem::path &path,
//   const std::string_view      &field_name,
//   std::uint8_t                 texture_page,
//   std::uint8_t                 palette)
// {
//      return path
//             / fmt::vformat(
//               fmt::string_view(pattern),
//               fmt::make_format_args(field_name, texture_page, palette));
// }
// std::filesystem::path map_sprite::save_path(
//   fmt::format_string<
//     std::string_view,
//     ff_8::PupuID>              pattern,
//   const std::filesystem::path &path,
//   const std::string_view      &field_name,
//   ff_8::PupuID                 pupu)
// {
//      return path
//             / fmt::vformat(
//               fmt::string_view(pattern),
//               fmt::make_format_args(field_name, pupu));
// }


std::expected<
  void,
  std::set<DrawFailure>>
  map_sprite::generate_texture(const glengine::FrameBuffer &fbo) const
{
     const auto fbb = fbo.backup();
     fbo.bind();
     glengine::GlCall{}(glViewport, 0, 0, fbo.width(), fbo.height());
     fbo.clear_non_standard_color_attachments();
     glengine::Renderer::Clear();
     const auto brb = m_batch_renderer.backup();
     m_batch_renderer.bind();
     set_uniforms(fbo, m_batch_renderer.shader());
     m_batch_renderer.clear();
     if (auto exp = local_draw(fbo, m_batch_renderer); exp)
     {
          //(void)draw_imported(fbo);
          // generate mipmaps by binding
          fbo.bind_color_attachment(0);
          fbo.bind_color_attachment(1);
          return exp;
     }
     else
     {
          for (const auto &[index, err] : exp.error() | std::views::enumerate)
          {
               spdlog::warn("local_draw failed ({}): {}", index, err);
               // or if you made a to_string(DrawError) helper:
               // spdlog::warn("Draw failed: {}", to_string(err));
          }
          return exp;
     }
}
void map_sprite::load_map(
  const std::filesystem::path &src_path,
  const bool                   skip_update)
{
     const auto path = src_path.string();
     open_viii::tools::read_from_file(
       [&](std::istream &os)
       {
            (void)m_map_group.maps.copy_working(
              fmt::format("{}: {}", gui_labels::load_map, src_path));
            m_map_group.maps.original().visit_tiles(
              [this, &os](const auto &const_tiles)
              {
                   using tile_t
                     = std::remove_cvref_t<decltype(const_tiles.front())>;
                   m_map_group.maps.working()
                     = open_viii::graphics::background::Map(
                       [&os]()
                         -> open_viii::graphics::background::Map::variant_tile
                       {
                            tile_t     tile{};
                            const auto append = [&os](auto &t) -> bool
                            {
                                 // load tile
                                 std::array<char, sizeof(t)> data = {};
                                 if (!os.read(data.data(), data.size()))
                                 {
                                      return false;
                                 }
                                 t = std::bit_cast<
                                   std::remove_cvref_t<decltype(t)>>(data);

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
            m_map_group.maps.working().shift_to_origin();
            (void)m_map_group.maps.copy_working_to_original(
              fmt::format("{}: {}", gui_labels::load_map, src_path));
       },
       path);

     spdlog::info("Load map: {}", src_path.string());
     if (!skip_update)
     {
          update_render_texture();
     }
}
void map_sprite::test_map(const std::filesystem::path &saved_path) const
{
     if (!m_map_group.mim)
     {
          return;
     }
     const MimType &type = m_map_group.mim->mim_type();
     std::ignore = ff_8::test_if_map_same(saved_path, m_map_group.field, type);
}
void map_sprite::save_modified_map(const std::filesystem::path &dest_path) const
{
     if (m_using_imported_texture)
     {
          ff_8::save_modified_map(
            dest_path,
            m_map_group.maps.original(),
            m_map_group.maps.const_working(),
            &m_imported_tile_map);
     }
     else
     {
          ff_8::save_modified_map(
            dest_path,
            m_map_group.maps.original(),
            m_map_group.maps.const_working());
     }
     test_map(dest_path);
}
void map_sprite::update_render_texture(
  const glengine::Texture *const       p_texture,
  open_viii::graphics::background::Map map,
  const tile_sizes                     tile_size)
{
     m_imported_texture        = p_texture;
     m_imported_tile_map       = std::move(map);
     m_imported_tile_map_front = m_imported_tile_map;
     m_imported_tile_size      = static_cast<uint16_t>(tile_size);
     m_using_imported_texture  = p_texture != nullptr;
     update_render_texture(true);
}
// void map_sprite::enable_square(glm::uvec2 position)
// {
//      m_square = m_square.with_position(position);
//      m_square.enable();
// }
// void map_sprite::disable_square() const
// {
//      m_square.disable();
// }
void map_sprite::enable_disable_blends()
{
     m_settings.disable_blends = true;
     update_render_texture();
}
void map_sprite::disable_disable_blends()
{
     m_settings.disable_blends = false;
     update_render_texture();
}
void map_sprite::first_to_working_and_original(const bool skip_update)
{
     const std::string message = "restore .map from FF8";
     (void)m_map_group.maps.first_to_working(message);
     (void)m_map_group.maps.first_to_original(message);
     if (!skip_update)
     {
          update_render_texture();
     }
}
std::string map_sprite::str_to_lower(std::string input)
{
     std::string output{};
     output.reserve(std::size(input) + 1);
     std::ranges::transform(
       input,
       std::back_inserter(output),
       [](char character) -> char
       { return static_cast<char>(::tolower(character)); });
     return output;
}

void fme::map_sprite::consume_now(const bool update) const
{
     if (m_child_map_sprite)
     {
          m_child_map_sprite->consume_now(update);
     }
     m_future_of_future_consumer.consume_now();
     m_future_consumer.consume_now();

     if (update)
     {
          update_render_texture();
     }
}

bool fme::map_sprite::all_futures_done() const
{
     return m_future_of_future_consumer.done() && m_future_consumer.done()
            && (!m_child_map_sprite || m_child_map_sprite->all_futures_done());
}

bool fme::map_sprite::consume_one_future() const
{
     if (m_child_map_sprite && !m_child_map_sprite->all_futures_done())
     {
          if (m_child_map_sprite->consume_one_future())
          {
               update_render_texture();
               return true;
          }
          return false;
     }
     // If the outer future is still processing, advance it
     if (!m_future_of_future_consumer.done())
     {
          ++m_future_of_future_consumer;
          return true;
     }
     // If the outer future is done but has output, consume it
     else if (m_future_of_future_consumer.consumer_ready())
     {
          m_future_consumer += m_future_of_future_consumer.get_consumer();
          return true;
     }
     // If the inner future is still processing, advance it
     else if (!m_future_consumer.done())
     {
          ++m_future_consumer;
          update_render_texture();
          return true;
     }

     // Nothing to do, both futures are done and consumed
     return false;
}

std::string map_sprite::current_undo_description() const
{
     return fmt::format(
       "{}: {}",
       m_map_group.maps.current_undo_pushed(),
       m_map_group.maps.current_undo_description());
}
std::string map_sprite::current_redo_description() const
{
     return fmt::format(
       "{}: {}",
       m_map_group.maps.current_redo_pushed(),
       m_map_group.maps.current_redo_description());
}

void map_sprite::begin_multi_frame_working(std::string description)
{
     (void)m_map_group.maps.begin_multi_frame_working(std::move(description));
}

void map_sprite::end_multi_frame_working(std::string description)
{
     m_map_group.maps.end_multi_frame_working(std::move(description));
}

void map_sprite::undo()
{
     (void)m_map_group.maps.undo();
     update_render_texture();
}
void map_sprite::redo()
{
     (void)m_map_group.maps.redo();
     update_render_texture();
}
void map_sprite::undo_all()
{
     m_map_group.maps.undo_all();
     update_render_texture();
}
void map_sprite::redo_all()
{
     m_map_group.maps.redo_all();
     update_render_texture();
}
bool map_sprite::undo_enabled() const
{
     return m_map_group.maps.undo_enabled();
}
bool map_sprite::redo_enabled() const
{
     return m_map_group.maps.redo_enabled();
}
std::vector<std::size_t> map_sprite::find_intersecting(
  const Map        &map,
  const glm::ivec2 &pixel_pos,
  const uint8_t    &texture_page,
  bool              skip_filters,
  bool              find_all) const
{
     if (m_settings.draw_swizzle)
     {
          return ff_8::find_intersecting_swizzle(
            map, m_filters, pixel_pos, texture_page, skip_filters, find_all);
     }
     return ff_8::find_intersecting_deswizzle(
       map, m_filters, pixel_pos, skip_filters, find_all);
}
bool map_sprite::using_coo() const
{
     return m_map_group.opt_coo.operator bool();
}

std::string map_sprite::appends_prefix_base_name(std::string_view title) const
{
     const auto base_name = get_base_name();
     const auto prefix    = std::string_view{ base_name }.substr(0U, 2U);
     return fmt::format(
       "{} ({} {}{}{})",
       title,
       gui_labels::appends,
       prefix,
       char{ std::filesystem::path::preferred_separator },
       base_name);
}


std::move_only_function<std::vector<std::filesystem::path>()>
  generate_swizzle_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    std::uint8_t                      texture_page)
{
     assert(in_selections && "generate_map_paths: in_selections is null");
     // assert(in_map_sprite && "generate_map_paths: in_map_sprite is
     // null");
     return
       [ps = ff_8::path_search{ .selections = std::move(in_selections),
                                .opt_coo    = in_map_sprite.get_opt_coo(),
                                .field_name = in_map_sprite.get_base_name(),
                                .filters_swizzle_value_string
                                = in_map_sprite.filter()
                                    .value<ff_8::FilterTag::Swizzle>()
                                    .string() },
        texture_page]() -> std::vector<std::filesystem::path>
     {
          spdlog::debug(
            "Generating swizzle paths for field: '{}', texture_page: {} ",
            ps.field_name,
            texture_page);
          return ps.generate_swizzle_paths(texture_page, ".png");
     };
}

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_swizzle_as_one_image_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    std::optional<std::uint8_t>       palette)
{
     assert(
       in_selections
       && "generate_swizzle_as_one_image_paths: in_selections is null");
     // assert(in_map_sprite && "generate_map_paths: in_map_sprite is
     // null");
     return
       [ps = ff_8::path_search{ .selections = std::move(in_selections),
                                .opt_coo    = in_map_sprite.get_opt_coo(),
                                .field_name = in_map_sprite.get_base_name(),
                                .filters_swizzle_as_one_image_string
                                = in_map_sprite.filter()
                                    .value<ff_8::FilterTag::SwizzleAsOneImage>()
                                    .string() },
        palette]() -> std::vector<std::filesystem::path>
     {
          if (palette.has_value())
          {
               spdlog::debug(
                 "Generating swizzle as one image paths for field: '{}', "
                 "palette: {} ",
                 ps.field_name,
                 palette.value());
          }
          else
          {
               spdlog::debug(
                 "Generating swizzle as one image paths for field: '{}'",
                 ps.field_name);
          }
          return ps.generate_swizzle_as_one_image_paths(palette, ".png");
     };
}

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_swizzle_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    std::uint8_t                      texture_page,
    std::uint8_t                      palette)
{
     assert(in_selections && "generate_map_paths: in_selections is null");
     // assert(in_map_sprite && "generate_map_paths: in_map_sprite is
     // null");
     return
       [ps = ff_8::path_search{ .selections = std::move(in_selections),
                                .opt_coo    = in_map_sprite.get_opt_coo(),
                                .field_name = in_map_sprite.get_base_name(),
                                .filters_swizzle_value_string
                                = in_map_sprite.filter()
                                    .value<ff_8::FilterTag::Swizzle>()
                                    .string() },
        texture_page, palette]() -> std::vector<std::filesystem::path>
     {
          spdlog::debug(
            "Generating swizzle paths for field: '{}', texture_page: {}, "
            "palette: {}",
            ps.field_name,
            texture_page,
            palette);
          return ps.generate_swizzle_paths(texture_page, palette, ".png");
     };
}

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_deswizzle_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    const ff_8::PupuID                pupu_id)
{
     assert(in_selections && "generate_deswizzle_paths: in_selections is null");
     // assert(in_map_sprite && "generate_deswizzle_paths: in_map_sprite is
     // null");
     return
       [ps = ff_8::path_search{ .selections = std::move(in_selections),
                                .opt_coo    = in_map_sprite.get_opt_coo(),
                                .field_name = in_map_sprite.get_base_name(),
                                .filters_deswizzle_value_string
                                = in_map_sprite.filter()
                                    .value<ff_8::FilterTag::Deswizzle>()
                                    .string() },
        pupu_id]() -> std::vector<std::filesystem::path>
     {
          spdlog::debug(
            "Generating deswizzle paths for field: '{}', pupu_id: {}",
            ps.field_name,
            pupu_id);
          return ps.generate_deswizzle_paths(pupu_id, ".png");
     };
}

std::move_only_function<std::vector<std::filesystem::path>()>
  generate_full_filename_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite,
    const std::string                &filename)
{
     assert(
       in_selections && "generate_full_filename_paths: in_selections is null");
     // assert(in_map_sprite && "generate_full_filename_paths:
     // in_map_sprite is null");
     return
       [ps = ff_8::path_search{ .selections = std::move(in_selections),
                                .opt_coo    = in_map_sprite.get_opt_coo(),
                                .field_name = in_map_sprite.get_base_name(),
                                .filters_full_filename_value_string
                                = in_map_sprite.filter()
                                    .value<ff_8::FilterTag::FullFileName>()
                                    .string() },
        filename]() -> std::vector<std::filesystem::path>
     {
          spdlog::debug(
            "Generating full filename paths for field: '{}', filename: {}",
            ps.field_name,
            filename);
          return ps.generate_full_filename_paths(filename);
     };
}


std::move_only_function<std::vector<std::filesystem::path>()>
  generate_map_paths(
    std::shared_ptr<const Selections> in_selections,
    const map_sprite                 &in_map_sprite)
{
     assert(in_selections && "generate_map_paths: in_selections is null");
     // assert(in_map_sprite && "generate_map_paths: in_map_sprite is
     // null");
     return
       [ps = ff_8::path_search{ .selections = std::move(in_selections),
                                .opt_coo    = in_map_sprite.get_opt_coo(),
                                .field_name = in_map_sprite.get_base_name(),
                                .filters_map_value_string
                                = in_map_sprite.filter()
                                    .value<ff_8::FilterTag::Map>()
                                    .string() }]()
         -> std::vector<std::filesystem::path>
     {
          spdlog::debug("Generating map paths for field: '{}'", ps.field_name);
          return ps.generate_map_paths(".map");
     };
}


}// namespace fme