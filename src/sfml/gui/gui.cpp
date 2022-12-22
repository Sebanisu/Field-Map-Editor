//
// Created by pcvii on 9/7/2021.
//

#include "gui.hpp"
#include "gui_batch.hpp"
#include "gui_labels.hpp"
#include "safedir.hpp"
#include <open_viii/paths/Paths.hpp>
#include <SFML/Window/Mouse.hpp>
#include <utility>
// #define USE_THREADS
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;
/**
 * @see https://godbolt.org/z/xce9jEbqh
 * @tparam T enforced std::filesystem::path
 * @tparam U anything that converts to std::filesystem::path
 * @param lhs T value
 * @param rhs U value
 * @return lhs + rhs
 */
template<typename T, typename U>
  requires std::same_as<std::remove_cvref_t<T>, std::filesystem::path>
           && (!std::same_as<std::remove_cvref_t<U>, std::filesystem::path>)
           && std::convertible_to<std::remove_cvref_t<U>, std::filesystem::path>
inline std::filesystem::path operator+(const T &lhs, const U &rhs)
{
  auto tmp = lhs;
  tmp += rhs;
  return tmp;
}
/**
 * @see https://godbolt.org/z/xce9jEbqh
 * @tparam T enforced std::filesystem::path
 * @param lhs anything that converts to std::filesystem::path
 * @param rhs T value
 * @return lhs + rhs
 */
template<typename T>
  requires std::same_as<std::remove_cvref_t<T>, std::filesystem::path>
           || std::
             same_as<std::remove_cvref_t<T>, std::filesystem::directory_entry>
inline std::filesystem::path
  operator+(const std::filesystem::path &lhs, const T &rhs)
{
  auto tmp = lhs;
  tmp += rhs;
  return tmp;
}
std::ostream &operator<<(std::ostream &os, const BlendModeT &bm)
{
  switch (bm)
  {
    case BlendModeT::quarter_add:
      return os << "quarter add"s;
    case BlendModeT::half_add:
      return os << "half add"s;
    case BlendModeT::add:
      return os << "add"s;
    default:
    case BlendModeT::none:
      return os << "none"s;
    case BlendModeT::subtract:
      return os << "subtract"s;
  }
}

namespace fme
{
void gui::start() const
{
  if (m_window.isOpen())
  {
    scale_window(
      static_cast<float>(m_selections.window_width),
      static_cast<float>(m_selections.window_height));
    do
    {
      //      if(!map_test() && m_selections.draw == 1)
      //      {
      //        m_map_sprite = get_map_sprite();
      //      }
      m_changed = false;
      m_id      = {};
      loop_events();
      const sf::Time &dt = m_delta_clock.restart();
      if (
        m_selections.draw_swizzle || (!m_selections.draw_palette && mim_test()))
      {
        m_scrolling.total_scroll_time[0] = 4000.F;
      }
      else
      {
        m_scrolling.total_scroll_time[0] = 1000.F;
      }
      if (m_scrolling.scroll(xy, dt))
      {
        m_changed                     = true;
        m_mouse_positions.mouse_moved = true;
      }
      ImGui::SFML::Update(m_window, dt);
      loop();
    } while (m_window.isOpen());
    ImGui::SFML::Shutdown();
  }
}
void gui::loop() const
{
  using namespace std::string_view_literals;
  static sf::Color clear_color = {
    0, 0, 0, std::numeric_limits<sf::Uint8>::max()
  };
  if (ImGui::Begin(
        gui_labels::control_panel.data(),
        nullptr,
        static_cast<ImGuiWindowFlags>(
          static_cast<uint32_t>(ImGuiWindowFlags_AlwaysAutoResize)
          | static_cast<uint32_t>(ImGuiWindowFlags_MenuBar))))
  {
    if (m_first)
    {
      ImGui::SetWindowPos({ 0U, 0U });
    }
    menu_bar();
    file_browser_locate_ff8();
    static std::array<float, 3U> clear_color_f{};
    if (ImGui::ColorEdit3(
          gui_labels::background.data(),
          clear_color_f.data(),
          ImGuiColorEditFlags_DisplayRGB))
    {
      // changed color
      clear_color = {
        static_cast<sf::Uint8>(
          std::numeric_limits<sf::Uint8>::max() * clear_color_f[0]),
        static_cast<sf::Uint8>(
          std::numeric_limits<sf::Uint8>::max() * clear_color_f[1]),
        static_cast<sf::Uint8>(
          std::numeric_limits<sf::Uint8>::max() * clear_color_f[2]),
        std::numeric_limits<sf::Uint8>::max()
      };
    }
    ImGui::SameLine();
    {
      const auto framerate = ImGui::GetIO().Framerate;
      format_imgui_text("   {:>3.2f} fps", framerate);
    }
    combo_draw();
    if (!m_paths.empty())
    {
      file_browser_save_texture();
      combo_path();
      combo_coo();
      combo_field();
      combo_upscale_path();
      combo_deswizzle_path();
      if (mim_test())
      {
        checkbox_mim_palette_texture();
        if (ImGui::CollapsingHeader(gui_labels::filters.data()))
        {
          if (!m_mim_sprite.draw_palette())
          {
            combo_bpp();
            combo_palette();
          }
          if (!m_mim_sprite.draw_palette())
          {
            format_imgui_text(
              "{} == {}", gui_labels::width, gui_labels::max_tiles);
          }
        }
        if (m_changed)
        {
          scale_window();
        }
        slider_xy_sprite(m_mim_sprite);
      }
      else if (map_test())
      {
        checkbox_map_swizzle();
        checkbox_render_imported_image();
        checkbox_map_disable_blending();
        format_imgui_text("{}: ", gui_labels::compact);
        ImGui::SameLine();
        if (ImGui::Button(gui_labels::rows.data()))
        {
          m_map_sprite.compact();
        }
        ImGui::SameLine();
        if (ImGui::Button(gui_labels::all.data()))
        {
          m_map_sprite.compact2();
        }
        ImGui::SameLine();
        format_imgui_text("{}: ", gui_labels::flatten);
        ImGui::SameLine();
        if (ImGui::Button(gui_labels::bpp.data()))
        {
          m_map_sprite.flatten_bpp();
        }
        ImGui::SameLine();
        if (ImGui::Button(gui_labels::palette.data()))
        {
          m_map_sprite.flatten_palette();
        }
        if (ImGui::CollapsingHeader(gui_labels::filters.data()))
        {
          combo_pupu();
          combo_filtered_bpps();
          combo_filtered_palettes();
          combo_blend_modes();
          combo_blend_other();
          combo_layers();
          combo_texture_pages();
          combo_animation_ids();
          combo_animation_frames();
          combo_z();
          combo_draw_bit();
        }
        if (m_changed)
        {
          scale_window();
        }
        slider_xy_sprite(m_map_sprite);
      }

      m_mouse_positions.mouse_enabled = handle_mouse_cursor();
      text_mouse_position();
    }
  }
  else
  {
    m_mouse_positions.mouse_enabled = handle_mouse_cursor();
  }
  popup_batch_deswizzle();
  popup_batch_reswizzle();
  ImGui::End();
  batch_ops_ask_menu();
  begin_batch_embed_map_warning_window();
  popup_batch_embed();
  import_image_window();
  // Begin non imgui drawing.
  on_click_not_imgui();
  m_window.clear(clear_color);
  if (mim_test())
  {
    m_window.draw(m_mim_sprite.toggle_grids(
      m_selections.draw_grid, m_selections.draw_texture_page_grid));
  }
  else if (map_test())
  {
    m_window.draw(m_map_sprite.toggle_grid(
      m_selections.draw_grid, m_selections.draw_texture_page_grid));
  }
  //  m_mouse_positions.cover.setColor(clear_color);
  //  m_window.draw(m_mouse_positions.cover);
  m_window.draw(m_mouse_positions.sprite);
  ImGui::SFML::Render(m_window);
  m_window.display();
  m_first = false;
}
void gui::checkbox_render_imported_image() const
{
  if (loaded_image_texture.getSize() != sf::Vector2u{})
  {
    if (ImGui::Checkbox(
          "Render Imported Image", &m_selections.render_imported_image))
    {
      Configuration config{};
      config->insert_or_assign(
        "selections_render_imported_image", m_selections.render_imported_image);
      config.save();
      // pass texture and map and tile_size
      update_imported_render_texture();
      if (!m_selections.render_imported_image)
      {
        m_map_sprite.update_render_texture(nullptr, {}, 16);
      }
      m_changed = true;
    }
  }
}
void gui::update_imported_render_texture() const
{
  if (m_selections.render_imported_image)
  {
    m_map_sprite.update_render_texture(
      &loaded_image_render_texture.getTexture(),
      import_image_map,
      m_selections.tile_size_value);
  }
}
void gui::popup_batch_common_filter_start(
  ::filter<std::filesystem::path> &filter,
  std::string_view                 prefix,
  std::string_view                 base_name) const
{
  if (filter.enabled())
  {
    filter.update(filter.value() / prefix / base_name);
    safedir path = filter.value();
    if (!path.is_exists() || !path.is_dir())
    {
      filter.disable();
    }
  }
}

template<typename batch_opT, typename filterT, typename askT, typename processT>
void gui::popup_batch_common(
  batch_opT &&batch_op,
  filterT   &&filter,
  askT      &&ask,
  processT  &&process) const
{
  if (batch_op(
        m_archives_group.mapdata(),
        [&](
          const int            &pos,
          std::filesystem::path selected_path,
          filters               filters,
          auto &&...rest) {
          auto field = m_archives_group.field(pos);
          if (!field)
          {
            return;
          }
          const auto map_pairs = field->get_vector_of_indexes_and_files(
            { open_viii::graphics::background::Map::EXT });
          if (map_pairs.empty())
          {
            return;
          }
          std::string      base_name = str_to_lower(field->get_base_name());
          std::string_view prefix = std::string_view(base_name).substr(0U, 2U);
          popup_batch_common_filter_start(filter(filters), prefix, base_name);

          auto map = m_map_sprite.with_field(field)
                       .with_coo(open_viii::LangT::generic)
                       .with_filters(filters);
          if (map.fail())
          {
            return;
          }
          if (filter(filters).enabled())
          {
            auto    map_path = filter(filters).value() / map.map_filename();
            safedir safe_map_path = map_path;
            if (safe_map_path.is_exists())
            {
              map.load_map(map_path);
            }
          }
          selected_path = selected_path / prefix / base_name;
          if (std::filesystem::create_directories(selected_path))
          {
            format_imgui_text(
              "{} {}", gui_labels::directory_created, selected_path.string());
          }
          else
          {
            format_imgui_text(
              "{} {}", gui_labels::directory_exists, selected_path.string());
          }
          format_imgui_text(gui_labels::saving_textures);

          if (map_pairs.size() > 1U)
          {
            spdlog::debug(
              "{}:{} - {}: {}\t {}: {}",
              __FILE__,
              __LINE__,
              gui_labels::count_of_maps,
              map_pairs.size(),
              gui_labels::field,
              base_name);
            for (const auto &[i, file_path] : map_pairs)
            {
              const auto filename =
                std::filesystem::path(file_path).filename().stem().string();
              std::string_view filename_view = { filename };
              std::string_view basename_view = { base_name };
              if (
                filename_view.substr(
                  0,
                  std::min(std::size(filename_view), std::size(basename_view)))
                != basename_view.substr(
                  0,
                  std::min(std::size(filename_view), std::size(basename_view))))
              {
                continue;
              }
              if (filename.size() == base_name.size())
              {
                process(selected_path, map, rest...);
                continue;
              }
              const auto coo_view =
                filename_view.substr(std::size(basename_view) + 1U, 2U);
              spdlog::info("Filename and coo: {}\t{}", filename, coo_view);
              map = map.with_coo(open_viii::LangCommon::from_string(coo_view));
              process(selected_path, map, rest...);
            }
          }
          else
          {
            process(selected_path, map, rest...);
          }
        },
        ask))
  {
  }
}

void gui::popup_batch_deswizzle() const
{
  popup_batch_common(
    m_batch_deswizzle,
    [](::filters &filters) -> ::filter<std::filesystem::path> & {
      return filters.upscale;
    },
    [this](::filter<std::filesystem::path> &filter) {
      // do I need to do generate_upscale_paths(m_field->get_base_name(),
      // get_coo()); here?
      if (combo_upscale_path(filter))//, ""
      {
      }

      return ImGui::Button(gui_labels::start.data());
    },
    [&](const auto &selected_path, auto &map) {
      map.save_pupu_textures(selected_path);
      const std::filesystem::path map_path = selected_path / map.map_filename();
      map.save_modified_map(map_path);
      format_imgui_text(
        "{} {} {}: {}",
        gui_labels::saving,
        open_viii::graphics::background::Map::EXT,
        gui_labels::file,
        map_path.string());
    });
}

void gui::popup_batch_reswizzle() const
{
  popup_batch_common(
    m_batch_reswizzle,
    [](::filters &filters) -> ::filter<std::filesystem::path> & {
      return filters.deswizzle;
    },
    [this](
      filter<compact_type> &compact, bool &flatten_bpp, bool &flatten_palette) {
      combo_compact_type(compact);
      ImGui::Separator();
      format_imgui_text("Flatten: ");
      ImGui::Checkbox("BPP", &flatten_bpp);
      ImGui::SameLine();
      ImGui::Checkbox("Palette", &flatten_palette);
      return ImGui::Button("Start");
    },
    [&](
      const auto                 &selected_path,
      auto                       &map,
      const filter<compact_type> &compact,
      const bool                 &flatten_bpp,
      const bool                 &flatten_palette) {
      const auto c = [&] {
        if (compact.enabled())
        {
          if (compact.value() == compact_type::rows)
          {
            map.compact();
          }
          if (compact.value() == compact_type::all)
          {
            map.compact2();
          }
        }
      };
      c();
      if (flatten_bpp)
      {
        map.flatten_bpp();
      }
      if (flatten_palette)
      {
        map.flatten_palette();
      }
      if (flatten_bpp || flatten_palette)
      {
        c();
      }
      const std::filesystem::path map_path = selected_path / map.map_filename();
      map.save_new_textures(selected_path);
      map.save_modified_map(map_path);
      format_imgui_text("Saving Map file: {}", map_path.string());
    });
}

void gui::on_click_not_imgui() const
{
  if (m_mouse_positions.mouse_enabled)
  {
    m_mouse_positions.update_sprite_pos(m_selections.draw_swizzle);
    if (m_mouse_positions.left_changed())
    {
      if (map_test())
      {
        if (m_mouse_positions.left)
        {
          // left mouse down
          // m_mouse_positions.cover =
          m_mouse_positions.sprite = m_map_sprite.save_intersecting(
            m_mouse_positions.pixel,
            m_mouse_positions.tile,
            m_mouse_positions.texture_page);
          m_mouse_positions.max_tile_x = m_map_sprite.max_x_for_saved();
        }
        else
        {
          // left mouse up
          m_map_sprite.update_position(
            m_mouse_positions.pixel,
            m_mouse_positions.tile,
            m_mouse_positions.texture_page);
          // m_mouse_positions.cover =
          m_mouse_positions.sprite     = {};
          m_mouse_positions.max_tile_x = {};
        }
      }
    }
  }
  else
  {
    if (m_mouse_positions.left_changed() && !m_mouse_positions.left)
    {
      // m_mouse_positions.cover =
      m_mouse_positions.sprite = {};
      // mouse up off-screen ?
    }
  }
}
void gui::text_mouse_position() const
{
  if (m_mouse_positions.mouse_enabled)
  {
    format_imgui_text(
      "Mouse Pos: ({:4}, {:3})",
      m_mouse_positions.pixel.x,
      m_mouse_positions.pixel.y);
    ImGui::SameLine();
    if (map_test() || !m_selections.draw_palette)
    {
      format_imgui_text(
        "Tile Pos: ({:2}, {:2})",
        m_mouse_positions.tile.x,
        m_mouse_positions.tile.y);
    }
    if (m_selections.draw_swizzle)
    {
      format_imgui_text("Page: {:2}", m_mouse_positions.texture_page);
    }
  }
}
bool gui::handle_mouse_cursor() const
{
  bool           mouse_enabled = false;
  const auto    &mouse_pos     = sf::Mouse::getPosition(m_window);
  const auto    &win_size      = m_window.getSize();
  constexpr auto in_bounds     = [](auto i, auto low, auto high) {
    return std::cmp_greater_equal(i, low) && std::cmp_less_equal(i, high);
  };
  if (
    in_bounds(mouse_pos.x, 0, win_size.x)
    && in_bounds(mouse_pos.y, 0, win_size.y) && m_window.hasFocus())
  {
    const sf::Vector2i clamped_mouse_pos = {
      std::clamp(mouse_pos.x, 0, static_cast<int>(win_size.x)),
      std::clamp(mouse_pos.y, 0, static_cast<int>(win_size.y))
    };

    const auto pixel_pos    = m_window.mapPixelToCoords(clamped_mouse_pos);
    m_mouse_positions.pixel = { static_cast<int>(pixel_pos.x),
                                static_cast<int>(pixel_pos.y) };
    const auto &x           = m_mouse_positions.pixel.x;
    const auto &y           = m_mouse_positions.pixel.y;
    auto        io          = ImGui::GetIO();
    if (((mim_test() && in_bounds(x, 0, m_mim_sprite.width())
           && in_bounds(y, 0, m_mim_sprite.height()))
          || (map_test() && in_bounds(x, 0, m_map_sprite.width())
              && in_bounds(y, 0, m_map_sprite.height())))
        && !io.WantCaptureMouse)
    //! ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
    //&& !ImGui::IsAnyItemHovered())
    {
      mouse_enabled         = true;
      const auto is_swizzle = (map_test() && m_selections.draw_swizzle)
                              || (mim_test() && !m_selections.draw_palette);
      if (m_mouse_positions.mouse_moved)
      {
        static constexpr int tile_size       = 16;
        static constexpr int texture_page_t1 = 128;
        static constexpr int texture_page_t2 = 64;
        m_mouse_positions.tile = m_mouse_positions.pixel / tile_size;
        auto &tilex            = m_mouse_positions.tile.x;
        m_mouse_positions.texture_page =
          static_cast<std::uint8_t>(tilex / tile_size);// for 4bit swizzle.
        auto &texture_page = m_mouse_positions.texture_page;
        if (is_swizzle)
        {
          tilex %= tile_size;
          if (mim_test())
          {
            if (m_selections.bpp == 1)
            {
              texture_page = static_cast<std::uint8_t>(x / texture_page_t1);
            }
            else if (m_selections.bpp == 2)
            {
              texture_page = static_cast<std::uint8_t>(x / texture_page_t2);
            }
          }
        }
      }
    }
  }
  return mouse_enabled;
}
void gui::combo_coo() const
{
  if (generic_combo(
        m_id,
        gui_labels::language,
        []() {
          return std::views::iota(
            static_cast<int>(open_viii::LangT::begin),
            static_cast<int>(open_viii::LangT::end));
        },
        []() {
          static constexpr auto coos_string_array =
            open_viii::LangCommon::to_string_array();
          return coos_string_array;
        },
        m_selections.coo))
  {
    Configuration config{};
    config->insert_or_assign("selections_palette", m_selections.palette);
    config.save();
    if (mim_test())
    {
      m_mim_sprite = m_mim_sprite.with_coo(get_coo());
    }
    else if (map_test())
    {
      m_map_sprite = m_map_sprite.with_coo(get_coo());
    }
    m_changed = true;
    if (m_field)
      generate_upscale_paths(m_field->get_base_name(), get_coo());
  }
}
const open_viii::LangT &gui::get_coo() const
{

  static constexpr auto coos = open_viii::LangCommon::to_array();
  return coos.at(static_cast<size_t>(m_selections.coo));
}
void gui::combo_field() const
{
  if (generic_combo(
        m_id,
        gui_labels::field,
        [this]() {
          return std::views::iota(
            0,
            static_cast<int>(std::ranges::ssize(m_archives_group.mapdata())));
        },
        [this]() { return m_archives_group.mapdata(); },
        m_selections.field))
  //  static constexpr auto items = 20;
  //  if (ImGui::Combo("Field",
  //        &m_selections.field,
  //        m_archives_group.mapdata_c_str().data(),
  //        static_cast<int>(m_archives_group.mapdata_c_str().size()),
  //        items))
  {
    Configuration config{};
    const auto   &maps = m_archives_group.mapdata();
    config->insert_or_assign(
      "starter_field", *std::next(maps.begin(), m_selections.field));
    config.save();
    update_field();
  }
}

void gui::update_field() const
{
  m_field = m_archives_group.field(m_selections.field);
  if (m_selections.draw == 0)
  {
    m_mim_sprite = m_mim_sprite.with_field(m_field);
  }
  else if (m_selections.draw == 1)
  {
    m_map_sprite = m_map_sprite.with_field(m_field);
  }

  m_loaded_swizzle_texture_path   = std::filesystem::path{};
  m_loaded_deswizzle_texture_path = std::filesystem::path{};

  m_changed                       = true;
  if (m_field)
    generate_upscale_paths(m_field->get_base_name(), get_coo());
}

void gui::checkbox_map_swizzle() const
{
  if (ImGui::Checkbox("Swizzle", &m_selections.draw_swizzle))
  {
    Configuration config{};
    config->insert_or_assign(
      "selections_draw_swizzle", m_selections.draw_swizzle);
    config.save();
    if (m_selections.draw_swizzle)
    {
      m_map_sprite.enable_disable_blends();
      m_map_sprite.enable_draw_swizzle();
    }
    else
    {
      m_map_sprite.disable_draw_swizzle();
      if (!m_selections.draw_disable_blending)
      {
        m_map_sprite.disable_disable_blends();
      }
    }
    m_changed = true;
  }
}
void gui::checkbox_map_disable_blending() const
{
  if (
    !m_selections.draw_swizzle
    && ImGui::Checkbox("Disable Blending", &m_selections.draw_disable_blending))
  {
    Configuration config{};
    config->insert_or_assign(
      "selections_draw_disable_blending", m_selections.draw_disable_blending);
    config.save();
    if (m_selections.draw_disable_blending)
    {
      m_map_sprite.enable_disable_blends();
    }
    else
    {
      m_map_sprite.disable_disable_blends();
    }
    m_changed = true;
  }
}
void gui::checkbox_mim_palette_texture() const
{
  if (ImGui::Checkbox("draw Palette Texture", &m_selections.draw_palette))
  {
    Configuration config{};
    config->insert_or_assign(
      "selections_draw_palette", m_selections.draw_palette);
    config.save();
    m_mim_sprite = m_mim_sprite.with_draw_palette(m_selections.draw_palette);
    m_changed    = true;
  }
}
static void update_bpp(mim_sprite &sprite, BPPT bpp)
{
  sprite = sprite.with_bpp(bpp);
}
static void update_bpp(map_sprite &sprite, BPPT bpp)
{
  if (sprite.filter().bpp.update(bpp).enabled())
  {
    sprite.update_render_texture();
  }
}
scope_guard gui::PushPop() const
{
  ImGui::PushID(++m_id);
  return scope_guard{ &ImGui::PopID };
}
void gui::combo_bpp() const
{
  {
    const auto                  sg        = PushPop();
    static constexpr std::array bpp_items = Mim::bpp_selections_c_str();
    if (ImGui::Combo(
          "BPP",
          &m_selections.bpp,
          bpp_items.data(),
          static_cast<int>(bpp_items.size()),
          static_cast<int>(bpp_items.size())))
    {
      Configuration config{};
      config->insert_or_assign("selections_bpp", m_selections.bpp);
      config.save();
      if (mim_test())
      {
        update_bpp(m_mim_sprite, bpp());
      }
      if (map_test())
      {
        update_bpp(m_map_sprite, bpp());
      }
      m_changed = true;
    }
  }
  {
    const auto  sg                    = PushPop();
    static bool enable_palette_filter = false;
    if (map_test())
    {
      ImGui::SameLine();
      if (ImGui::Checkbox("", &enable_palette_filter))
      {
        if (enable_palette_filter)
        {
          m_map_sprite.filter().bpp.enable();
        }
        else
        {
          m_map_sprite.filter().bpp.disable();
        }
        m_map_sprite.update_render_texture();
        m_changed = true;
      }
    }
  }
}
std::uint8_t gui::palette() const
{
  return static_cast<uint8_t>(
    Mim::palette_selections().at(static_cast<size_t>(m_selections.palette)));
}
static void update_palette(mim_sprite &sprite, uint8_t palette)
{
  sprite = sprite.with_palette(palette);
}
static void update_palette(map_sprite &sprite, uint8_t palette)
{
  if (sprite.filter().palette.update(palette).enabled())
  {
    sprite.update_render_texture();
  }
}
void gui::combo_palette() const
{
  if (m_selections.bpp != 2)
  {
    static constexpr std::array palette_items = Mim::palette_selections_c_str();
    {
      const auto sg = PushPop();
      if (ImGui::Combo(
            "Palette",
            &m_selections.palette,
            palette_items.data(),
            static_cast<int>(palette_items.size()),
            static_cast<int>(palette_items.size())))
      {
        if (mim_test())
        {
          update_palette(m_mim_sprite, palette());
        }
        if (map_test())
        {
          update_palette(m_map_sprite, palette());
        }
        Configuration config{};
        config->insert_or_assign("selections_palette", m_selections.palette);
        config.save();
        m_changed = true;
      }
    }
    if (map_test())
    {
      ImGui::SameLine();
      {
        static bool enable_palette_filter = false;
        const auto  sg                    = PushPop();
        if (ImGui::Checkbox("", &enable_palette_filter))
        {
          if (enable_palette_filter)
          {
            m_map_sprite.filter().palette.enable();
          }
          else
          {
            m_map_sprite.filter().palette.disable();
          }
          m_map_sprite.update_render_texture();
          m_changed = true;
        }
      }
    }
  }
}

void gui::slider_xy_sprite(auto &sprite) const
{
  format_imgui_text(
    "X: {:>9.3f} px  Width:  {:>4} px", -std::abs(m_cam_pos.x), sprite.width());
  format_imgui_text(
    "Y: {:>9.3f} px  Height: {:>4} px",
    -std::abs(m_cam_pos.y),
    sprite.height());
  if (ImGui::SliderFloat2("Adjust", xy.data(), -1.0F, 0.0F) || m_changed)
  {
    m_cam_pos = { -xy[0] * (static_cast<float>(sprite.width()) - m_scale_width),
                  -xy[1] * static_cast<float>(sprite.height()) };
    m_changed = true;
    scale_window();
  }
}
void gui::menu_bar() const
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      menuitem_locate_ff8();
      menuitem_locate_custom_upscale();
      ImGui::Separator();
      menuitem_save_texture(save_texture_path(), mim_test() || map_test());
      if (mim_test())
      {
        ImGui::Separator();
        menuitem_save_mim_file(m_mim_sprite.mim_filename());
      }
      if (map_test())
      {
        ImGui::Separator();
        menuitem_save_map_file(m_map_sprite.map_filename());
        menuitem_save_map_file_modified(m_map_sprite.map_filename());
        menuitem_load_map_file(m_map_sprite.map_filename());
        ImGui::Separator();
        menuitem_save_swizzle_textures();
        menuitem_load_swizzle_textures();
        ImGui::Separator();
        menuitem_save_deswizzle_textures();
        menuitem_load_deswizzle_textures();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Grid"))
    {
      if (ImGui::MenuItem("draw Tile Grid", nullptr, &m_selections.draw_grid))
      {
        Configuration config{};
        config->insert_or_assign(
          "selections_draw_grid", m_selections.draw_grid);
        config.save();
      }
      if (ImGui::MenuItem(
            "draw Texture Page Grid",
            nullptr,
            &m_selections.draw_texture_page_grid))
      {
        Configuration config{};
        config->insert_or_assign(
          "selections_draw_texture_page_grid",
          m_selections.draw_texture_page_grid);
        config.save();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Batch"))
    {
      if (ImGui::MenuItem("Deswizzle"))
      {
        m_directory_browser.Open();
        // std::string base_name = m_map_sprite.get_base_name();
        // std::string prefix    = base_name.substr(0U, 2U);
        m_directory_browser.SetTitle("Choose directory to save textures");
        m_directory_browser.SetPwd(Configuration{}["deswizzle_path"].value_or(
          std::filesystem::current_path().string()));
        m_directory_browser.SetTypeFilters({ ".map", ".png" });
        m_modified_directory_map =
          map_directory_mode::batch_save_deswizzle_textures;
      }
      if (ImGui::MenuItem("Reswizzle"))
      {
        m_directory_browser.Open();
        // std::string base_name = m_map_sprite.get_base_name();
        // std::string prefix    = base_name.substr(0U, 2U);
        m_directory_browser.SetTitle(
          "Choose source directory of deswizzled textures (contains two letter "
          "directories)");
        m_directory_browser.SetPwd(Configuration{}["reswizzle_path"].value_or(
          std::filesystem::current_path().string()));
        m_directory_browser.SetTypeFilters({ ".map", ".png" });
        m_modified_directory_map =
          map_directory_mode::batch_load_deswizzle_textures;
      }

      if (ImGui::MenuItem("Embed .map files into Archives"))
      {
        m_selections.batch_embed_map_warning_window = true;
      }
      if (ImGui::MenuItem(
            "Test Batch Window", nullptr, &m_selections.test_batch_window))
      {
        Configuration config{};
        config->insert_or_assign(
          "selections_test_batch_window", m_selections.test_batch_window);
        config.save();
      }
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Import"))
    {
      if (ImGui::MenuItem(
            "Import Image", nullptr, &m_selections.display_import_image))
      {
        Configuration config{};
        config->insert_or_assign(
          "selections_display_import_image", m_selections.display_import_image);
        config.save();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}
void gui::browse_for_embed_map_dir() const
{
  m_directory_browser.Open();
  m_directory_browser.SetTitle(
    "Choose source directory of your textures and .map files "
    "(contains two letter directories)");
  m_directory_browser.SetPwd(Configuration{}["embed_source_path"].value_or(
    std::filesystem::current_path().string()));
  m_directory_browser.SetTypeFilters({ ".map" });
  m_modified_directory_map = map_directory_mode::batch_embed_map_files;
}
bool gui::map_test() const
{
  return !m_map_sprite.fail() && m_selections.draw == 1;
}
bool gui::mim_test() const
{
  return !m_mim_sprite.fail() && m_selections.draw == 0;
}
std::string gui::save_texture_path() const
{
  if (m_archives_group.mapdata().empty())
  {
    return {};
  }
  const std::string &field_name =
    m_archives_group.mapdata().at(static_cast<size_t>(m_selections.field));
  if (mim_test())// MIM
  {
    if (m_mim_sprite.draw_palette())
    {
      return fmt::format("{}_mim_palettes.png", field_name);
    }
    const int bpp = static_cast<int>(
      Mim::bpp_selections().at(static_cast<size_t>(m_selections.bpp)));
    return fmt::format(
      "{}_mim_{}bpp_{}.png", field_name, bpp, m_selections.palette);
  }
  else if (map_test())
  {
    return fmt::format("{}_map.png", field_name);
  }
  return {};
}
void gui::file_browser_locate_ff8() const
{
  m_directory_browser.Display();
  if (m_directory_browser.HasSelected())
  {
    static std::filesystem::path reswizzle_src = {};
    auto selected_path = m_directory_browser.GetSelected();
    if (m_modified_directory_map == map_directory_mode::ff8_install_directory)
    {
      m_paths.push_back(selected_path.string());
      sort_paths();
      m_selections.path = selected_path.string();
      update_path();
    }
    else if (
      m_modified_directory_map == map_directory_mode::custom_upscale_directory)
    {
      m_custom_upscale_paths.push_back(selected_path.string());
      // todo remove paths that don't exist.
      Configuration config{};
      config->insert_or_assign(
        "custom_upscale_paths_vector", m_custom_upscale_paths);
      config.save();
      if (m_field)
        generate_upscale_paths(m_field->get_base_name(), get_coo());
      // todo toggle filter enabled?
    }
    else if (
      m_modified_directory_map == map_directory_mode::save_swizzle_textures)
    {
      Configuration config{};
      config->insert_or_assign(
        "reswizzle_path", m_directory_browser.GetPwd().string());
      std::string base_name = m_map_sprite.get_base_name();
      std::string prefix    = base_name.substr(0U, 2U);
      selected_path         = selected_path / prefix / base_name;
      std::filesystem::create_directories(selected_path);
      // todo modify these two functions :P to use the imported image.
      m_map_sprite.save_new_textures(selected_path);// done.
      m_map_sprite.save_modified_map(
        selected_path / m_map_sprite.map_filename());// done.
    }
    else if (
      m_modified_directory_map == map_directory_mode::load_swizzle_textures)
    {
      Configuration config{};
      config->insert_or_assign(
        "single_swizzle_or_deswizzle_path",
        m_directory_browser.GetPwd().string());
      config.save();
      m_loaded_swizzle_texture_path = selected_path;
      if (m_field)
        generate_upscale_paths(m_field->get_base_name(), get_coo());
      m_map_sprite.filter().deswizzle.disable();
      m_map_sprite.filter()
        .upscale.update(m_loaded_swizzle_texture_path)
        .enable();
      auto map_path =
        m_loaded_swizzle_texture_path / m_map_sprite.map_filename();
      safedir safe_map_path = map_path;
      if (safe_map_path.is_exists())
      {
        m_map_sprite.load_map(map_path);
      }
      m_map_sprite.update_render_texture(true);
    }
    else if (
      m_modified_directory_map == map_directory_mode::save_deswizzle_textures)
    {
      Configuration config{};
      config->insert_or_assign(
        "deswizzle_path", m_directory_browser.GetPwd().string());
      config.save();
      std::string base_name = m_map_sprite.get_base_name();
      std::string prefix    = base_name.substr(0U, 2U);
      selected_path         = selected_path / prefix / base_name;
      std::filesystem::create_directories(selected_path);
      m_map_sprite.save_pupu_textures(selected_path);
      m_map_sprite.save_modified_map(
        selected_path / m_map_sprite.map_filename());
    }
    else if (
      m_modified_directory_map == map_directory_mode::load_deswizzle_textures)
    {
      Configuration config{};
      config->insert_or_assign(
        "single_swizzle_or_deswizzle_path",
        m_directory_browser.GetPwd().string());
      config.save();
      m_loaded_deswizzle_texture_path = selected_path;
      m_map_sprite.filter().upscale.disable();
      m_map_sprite.filter()
        .deswizzle.update(m_loaded_deswizzle_texture_path)
        .enable();
      auto map_path =
        m_loaded_deswizzle_texture_path / m_map_sprite.map_filename();
      safedir safe_map_path = map_path;
      if (safe_map_path.is_exists())
      {
        m_map_sprite.load_map(map_path);
      }
      m_map_sprite.update_render_texture(true);
    }
    else if (
      m_modified_directory_map
      == map_directory_mode::batch_save_deswizzle_textures)
    {
      Configuration config{};
      config->insert_or_assign(
        "deswizzle_path", m_directory_browser.GetPwd().string());
      config.save();
      m_batch_deswizzle.enable(selected_path);
    }
    else if (
      m_modified_directory_map == map_directory_mode::batch_embed_map_files)
    {
      Configuration config{};
      config->insert_or_assign(
        "embed_source_path", m_directory_browser.GetPwd().string());
      config.save();
      m_batch_embed.enable(selected_path);
    }
    else if (
      m_modified_directory_map
      == map_directory_mode::batch_save_swizzle_textures)
    {
      Configuration config{};
      config->insert_or_assign(
        "reswizzle_path", m_directory_browser.GetPwd().string());
      config.save();
      m_batch_reswizzle.enable(
        std::move(reswizzle_src), std::move(selected_path));
    }
    else if (
      m_modified_directory_map
      == map_directory_mode::batch_load_deswizzle_textures)
    {
      Configuration config{};
      config->insert_or_assign(
        "deswizzle_path", m_directory_browser.GetPwd().string());
      reswizzle_src = std::move(selected_path);
      m_directory_browser.Open();
      // std::string base_name = m_map_sprite.get_base_name();
      // std::string prefix    = base_name.substr(0U, 2U);
      m_directory_browser.SetTitle(
        "Choose destination directory where reswizzled textures will be "
        "saved.");
      m_directory_browser.SetTypeFilters({ ".map", ".png" });
      m_modified_directory_map =
        map_directory_mode::batch_save_swizzle_textures;
      config->insert_or_assign(
        "reswizzle_path", m_directory_browser.GetPwd().string());
      config.save();
    }
    m_directory_browser.ClearSelected();
  }
}
void gui::sort_paths() const
{// todo remove paths that don't exist.
  std::vector<std::string> tmp = {};
  tmp.reserve(std::ranges::size(m_paths));
  std::ranges::transform(
    m_paths, std::back_inserter(tmp), [](const toml::node &node) {
      return node.value_or(std::string{});
    });
  std::ranges::sort(tmp);
  const auto removal = std::ranges::unique(tmp);
  tmp.erase(removal.begin(), removal.end());
  m_paths.clear();
  std::ranges::for_each(
    tmp, [this](std::string &str) { m_paths.push_back(std::move(str)); });
  Configuration config{};
  config->insert_or_assign("paths_vector", m_paths);
  config.save();
}
void gui::file_browser_save_texture() const
{
  m_save_file_browser.Display();
  if (m_save_file_browser.HasSelected())
  {
    [[maybe_unused]] const auto selected_path =
      m_save_file_browser.GetSelected();
    if (mim_test())
    {
      const auto str_path = selected_path.string();
      if (open_viii::tools::i_ends_with(str_path, Mim::EXT))
      {
        m_mim_sprite.mim_save(selected_path);
        Configuration config{};
        config->insert_or_assign(
          "mim_path", m_save_file_browser.GetPwd().string());
        config.save();
      }
      else
      {
        m_mim_sprite.save(selected_path);
        Configuration config{};
        config->insert_or_assign(
          "save_image_path", m_save_file_browser.GetPwd().string());
        config.save();
      }
    }
    else if (map_test())
    {
      const auto str_path = selected_path.string();
      if (open_viii::tools::i_ends_with(str_path, Map::EXT))
      {
        switch (m_modified_map)
        {
          case map_dialog_mode::save_modified: {
            m_map_sprite.save_modified_map(selected_path);
            Configuration config{};
            config->insert_or_assign(
              "map_path", m_save_file_browser.GetPwd().string());
            config.save();
          }
          break;
          case map_dialog_mode::save_unmodified: {
            m_map_sprite.map_save(selected_path);
            Configuration config{};
            config->insert_or_assign(
              "map_path", m_save_file_browser.GetPwd().string());
            config.save();
          }
          break;
          case map_dialog_mode::load: {
            m_map_sprite.load_map(selected_path);
            Configuration config{};
            config->insert_or_assign(
              "map_path", m_save_file_browser.GetPwd().string());
            config.save();
            m_changed = true;
          }
          break;
        }
      }
      else
      {
        m_map_sprite.save(selected_path);
        Configuration config{};
        config->insert_or_assign(
          "save_image_path", m_save_file_browser.GetPwd().string());
        config.save();
      }
    }
    m_save_file_browser.ClearSelected();
  }
}
void gui::menuitem_locate_ff8() const
{
  if (ImGui::MenuItem("Locate a FF8 install"))
  {
    open_locate_ff8_filebrowser();
  }
}
void gui::open_locate_ff8_filebrowser() const
{
  m_directory_browser.Open();
  m_directory_browser.SetTitle("Choose FF8 install directory");
  m_directory_browser.SetTypeFilters({ ".exe" });
  m_modified_directory_map = map_directory_mode::ff8_install_directory;
}
void gui::menuitem_locate_custom_upscale() const
{
  if (ImGui::MenuItem("Locate a Custom Upscale directory"))
  {
    open_locate_custom_upscale();
  }
}
void gui::open_locate_custom_upscale() const
{
  m_directory_browser.Open();
  m_directory_browser.SetTitle("Choose Custom Upscale directory");
  m_modified_directory_map = map_directory_mode::custom_upscale_directory;
}
void gui::menuitem_save_swizzle_textures() const
{
  if (ImGui::MenuItem("Save Swizzled Textures", nullptr, false, true))
  {
    save_swizzle_textures();
  }
}
void gui::save_swizzle_textures() const
{
  m_directory_browser.Open();

  std::string base_name = m_map_sprite.get_base_name();
  std::string prefix    = base_name.substr(0U, 2U);
  auto        title     = fmt::format(
    "Choose directory to save textures (appends {}{}{})",
    prefix,
    char{ std::filesystem::path::preferred_separator },
    base_name);
  m_directory_browser.SetTitle(title);
  m_directory_browser.SetPwd(Configuration{}["reswizzle_path"].value_or(
    std::filesystem::current_path().string()));
  m_directory_browser.SetTypeFilters({ ".map", ".png" });
  m_modified_directory_map = map_directory_mode::save_swizzle_textures;
}
void gui::menuitem_save_deswizzle_textures() const
{
  if (ImGui::MenuItem("Save Deswizzled Textures (Pupu)", nullptr, false, true))
  {
    m_directory_browser.Open();
    std::string base_name = m_map_sprite.get_base_name();
    std::string prefix    = base_name.substr(0U, 2U);
    auto        title     = fmt::format(
      "Choose directory to save textures (appends {}{}{})",
      prefix,
      char{ std::filesystem::path::preferred_separator },
      base_name);
    m_directory_browser.SetTitle(title);
    m_directory_browser.SetPwd(Configuration{}["deswizzle_path"].value_or(
      std::filesystem::current_path().string()));
    m_directory_browser.SetTypeFilters({ ".map", ".png" });
    m_modified_directory_map = map_directory_mode::save_deswizzle_textures;
  }
}
void gui::menuitem_load_swizzle_textures() const
{
  if (ImGui::MenuItem("Load Swizzled Textures", nullptr, false, true))
  {
    open_swizzle_filebrowser();
  }
}
void gui::open_swizzle_filebrowser() const
{
  m_directory_browser.Open();
  m_directory_browser.SetTitle("Choose directory to load textures from");
  m_directory_browser.SetPwd(
    Configuration{}["single_swizzle_or_deswizzle_path"].value_or(
      std::filesystem::current_path().string()));
  m_directory_browser.SetTypeFilters({ ".map", ".png" });
  m_modified_directory_map = map_directory_mode::load_swizzle_textures;
}
void gui::menuitem_load_deswizzle_textures() const
{
  if (ImGui::MenuItem("Load Deswizzled Textures", nullptr, false, true))
  {
    open_deswizzle_filebrowser();
  }
}
void gui::open_deswizzle_filebrowser() const
{
  m_directory_browser.Open();
  m_directory_browser.SetTitle("Choose directory to load textures from");
  m_directory_browser.SetPwd(
    Configuration{}["single_swizzle_or_deswizzle_path"].value_or(
      std::filesystem::current_path().string()));
  m_directory_browser.SetTypeFilters({ ".map", ".png" });
  m_modified_directory_map = map_directory_mode::load_deswizzle_textures;
}
void gui::menuitem_save_texture(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Save Displayed Texture", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Texture as...");
    m_save_file_browser.SetPwd(Configuration{}["save_image_path"].value_or(
      std::filesystem::current_path().string()));
    m_save_file_browser.SetTypeFilters({ ".png", ".ppm" });
    m_save_file_browser.SetInputName(path.c_str());
  }
}
void gui::menuitem_save_mim_file(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Save Mim File", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Mim as...");
    m_save_file_browser.SetPwd(Configuration{}["mim_path"].value_or(
      std::filesystem::current_path().string()));
    m_save_file_browser.SetTypeFilters({ Mim::EXT.data() });
    m_save_file_browser.SetInputName(path);
  }
}
void gui::menuitem_save_map_file(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Save Map File (unmodified)", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Map as...");
    m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(
      std::filesystem::current_path().string()));
    m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
    m_save_file_browser.SetInputName(path);
    m_modified_map = map_dialog_mode::save_unmodified;
  }
}
void gui::menuitem_save_map_file_modified(const std::string &path, bool enabled)
  const
{
  if (ImGui::MenuItem("Save Map File (modified)", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Map as...");
    m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(
      std::filesystem::current_path().string()));
    m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
    m_save_file_browser.SetInputName(path);
    m_modified_map = map_dialog_mode::save_modified;
  }
}
void gui::menuitem_load_map_file(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Load Map File", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Load Map...");
    m_save_file_browser.SetPwd(Configuration{}["map_path"].value_or(
      std::filesystem::current_path().string()));
    m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
    m_save_file_browser.SetInputName(path);
    m_modified_map = map_dialog_mode::load;
  }
}
void gui::combo_draw() const
{
  if (generic_combo(
        m_id,
        gui_labels::draw,
        []() {
          return std::views::iota(
            0, static_cast<int>(std::size(m_draw_selections)));
        },
        [this]() { return m_draw_selections; },
        m_selections.draw))
  {
    Configuration config{};
    config->insert_or_assign("selections_draw", m_selections.draw);
    config.save();
    if (m_selections.draw == 0)
    {
      m_mim_sprite = get_mim_sprite();
    }
    else if (m_selections.draw == 1)
    {
      m_map_sprite =
        m_map_sprite.update(m_field, get_coo(), m_selections.draw_swizzle);
    }
    m_changed = true;
  }
}
bool gui::combo_path() const
{
  if (generic_combo(
        m_id,
        gui_labels::path,
        [this]() {
          return m_paths
                 | std::ranges::views::transform(
                   [](toml::node &item) -> std::string {
                     return item.value_or<std::string>({});
                   });
        },
        [this]() {
          return m_paths
                 | std::ranges::views::transform(
                   [](toml::node &item) -> std::string {
                     return item.value_or<std::string>({});
                   });
        },
        m_selections.path))
  {
    Configuration config{};
    config->insert_or_assign("selections_path", m_selections.path);
    config.save();
    update_path();
    return true;
  }
  return false;
}
toml::array gui::get_paths()
{
  const char   *paths_vector = "paths_vector";
  Configuration config{};
  if (!config->contains(paths_vector))
  {
    const auto &default_paths = open_viii::Paths::get();
    // todo get all default paths for linux and windows.
    toml::array paths_array{};
    paths_array.reserve(default_paths.size());
    for (const auto &path : default_paths)
    {
      paths_array.push_back(path);
    }
    config->insert_or_assign(paths_vector, std::move(paths_array));
    config.save();
  }
  return *(config->get_as<toml::array>(paths_vector));

  //  std::vector<std::string> paths{};
  //  open_viii::Paths::for_each_path([&paths](const std::filesystem::path &p) {
  //    paths.emplace_back(p.string());
  //  });
  //  return paths;
}
toml::array gui::get_custom_upscale_paths_vector()
{
  const char   *paths_vector = "custom_upscale_paths_vector";
  Configuration config{};
  if (!config->contains(paths_vector))
  {
    return {};
  }
  return *(config->get_as<toml::array>(paths_vector));

  //  std::vector<std::string> paths{};
  //  open_viii::Paths::for_each_path([&paths](const std::filesystem::path &p) {
  //    paths.emplace_back(p.string());
  //  });
  //  return paths;
}
void gui::loop_events() const
{
  m_mouse_positions.update();
  while (m_window.pollEvent(m_event))
  {
    ImGui::SFML::ProcessEvent(m_event);
    const auto  event_variant = events::get(m_event);
    const auto &type          = m_event.type;
    std::visit(
      events::make_visitor(
        [this](const sf::Event::SizeEvent &size) {
          scale_window(
            static_cast<float>(size.width), static_cast<float>(size.height));
          m_changed = true;
        },
        [this](const sf::Event::MouseMoveEvent &) {
          m_mouse_positions.mouse_moved = true;
          // TODO move setting mouse pos code here?
          // m_changed = true;
        },
        [this, type](const sf::Event::KeyEvent &key) {
          if (ImGui::GetIO().WantCaptureKeyboard)
          {
            m_scrolling.reset();
            return;
          }
          if (type == sf::Event::EventType::KeyReleased)
          {
            if (key.control && key.code == sf::Keyboard::Z)
            {
              m_map_sprite.undo();
            }
            else if (key.code == sf::Keyboard::Up)
            {
              m_scrolling.up = false;
            }
            else if (key.code == sf::Keyboard::Down)
            {
              m_scrolling.down = false;
            }
            else if (key.code == sf::Keyboard::Left)
            {
              m_scrolling.left = false;
            }
            else if (key.code == sf::Keyboard::Right)
            {
              m_scrolling.right = false;
            }
          }
          if (type == sf::Event::EventType::KeyPressed)
          {
            if (key.code == sf::Keyboard::Up)
            {
              m_scrolling.up = true;
            }
            else if (key.code == sf::Keyboard::Down)
            {
              m_scrolling.down = true;
            }
            else if (key.code == sf::Keyboard::Left)
            {
              m_scrolling.left = true;
            }
            else if (key.code == sf::Keyboard::Right)
            {
              m_scrolling.right = true;
            }
          }
        },
        [this, &type](const sf::Event::MouseButtonEvent &mouse) {
          const sf::Mouse::Button &button = mouse.button;
          if (!m_mouse_positions.mouse_enabled)
          {
            m_mouse_positions.left = false;
            return;
          }
          switch (type)
          {
            case sf::Event::EventType::MouseButtonPressed:
              ///< A mouse button was pressed (data in event.mouseButton)
              {
                switch (button)
                {
                  case sf::Mouse::Button::Left: {
                    m_mouse_positions.left = true;
                    spdlog::trace("{}", "Left Mouse Button Down");
                  }
                  break;
                  default:
                    break;
                }
              }
              break;
            case sf::Event::EventType::MouseButtonReleased:
              ///< A mouse button was released (data in
              ///< event.mouseButton)
              {
                switch (button)
                {
                  case sf::Mouse::Button::Left: {
                    m_mouse_positions.left = false;
                    spdlog::trace("{}", "Left Mouse Button Up");
                  }
                  break;
                  default:
                    break;
                }
              }
              break;
            default:
              break;
          }
        },
        [this]([[maybe_unused]] const std::monostate &) {
          if (m_event.type == sf::Event::Closed)
          {
            m_window.close();
          }
        },
        []([[maybe_unused]] const auto &) {}),
      event_variant);
  }
}
void gui::scale_window(float width, float height) const
{
  static auto save_width  = float{};
  static auto save_height = float{};
  float       img_height  = [this]() {
    if (map_test())
    {
      return static_cast<float>(m_map_sprite.height());
    }
    return static_cast<float>(m_mim_sprite.height());
  }();
  auto load = [](auto &saved, auto &not_saved) {
    if (not_saved < std::numeric_limits<float>::epsilon())
    {
      not_saved = saved;
    }
    else
    {
      saved = not_saved;
    }
  };
  load(save_width, width);
  load(save_height, height);
  // this scales up the elements without losing the horizontal space. so
  // going from 4:3 to 16:9 will end up with wide screen.
  // auto scale    = height / static_cast<float>(m_window_height);
  m_scale_width = std::round(width / height * img_height);
  //  if (scale < 1.0F)
  //  {
  //    scale = 1.0F;
  //  }
  // ImGui::GetIO().FontGlobalScale = std::round(scale);
  // ImGui::GetStyle() =
  // m_original_style;// restore original before applying scale.
  // ImGui::GetStyle().ScaleAllSizes(std::round(scale));
  m_window.setView(sf::View(sf::FloatRect(
    std::round(m_cam_pos.x),
    std::round(m_cam_pos.y),
    m_scale_width,
    std::round(img_height))));
}
archives_group gui::get_archives_group() const
{
  if (!std::ranges::empty(m_selections.path))
  {
    return { open_viii::LangCommon::to_array().front(), m_selections.path };
  }
  return {};
}
sf::RenderWindow gui::get_render_window() const
{
  return sf::RenderWindow{
    sf::VideoMode(m_selections.window_width, m_selections.window_height),
    sf::String{ gui_labels::window_title }
  };
}
void gui::update_path() const
{
  m_archives_group = m_archives_group.with_path(m_selections.path);
  update_field();
  if (m_batch_embed4.enabled())
  {
    m_batch_embed4.enable(m_selections.path, m_batch_embed4.start_time());
  }
}
mim_sprite gui::get_mim_sprite() const
{
  return { m_field,
           Mim::bpp_selections().at(static_cast<std::size_t>(m_selections.bpp)),
           static_cast<std::uint8_t>(Mim::palette_selections().at(
             static_cast<std::size_t>(m_selections.palette))),
           get_coo(),
           m_selections.draw_palette };
}
void gui::init_and_get_style() const
{
  //  static constexpr auto fps_lock = 360U;
  //  m_window.setFramerateLimit(fps_lock);
  m_window.setVerticalSyncEnabled(true);
  (void)ImGui::SFML::Init(m_window);
  if (m_field)
    generate_upscale_paths(m_field->get_base_name(), get_coo());
}
gui::gui()
  : m_window(get_render_window())
  , m_paths(get_paths())
  , m_custom_upscale_paths(get_custom_upscale_paths_vector())
  , m_archives_group(get_archives_group())
  , m_field(init_field())
  , m_mim_sprite(get_mim_sprite())
  , m_map_sprite(get_map_sprite())

{
  sort_paths();
  init_and_get_style();
}
std::shared_ptr<open_viii::archive::FIFLFS<false>> gui::init_field()
{
  m_selections.field = get_selected_field();
  return m_archives_group.field(m_selections.field);
}
map_sprite gui::get_map_sprite() const
{
  return { m_field,
           get_coo(),
           m_selections.draw_swizzle,
           {},
           m_selections.draw_disable_blending };
}
int gui::get_selected_field()
{
  if (const int field = m_archives_group.find_field(starter_field());
      field != -1)
  {
    return field;
  }
  return 0;
}
std::string gui::starter_field() const
{
  return Configuration{}["starter_field"].value_or(std::string("ecenter3"));
}


void gui::combo_pupu() const
{
  const auto &pair = m_map_sprite.uniques().pupu();
  if (generic_combo(
        m_id,
        gui_labels::pupu_id,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().pupu; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}

void gui::combo_draw_bit() const
{
  using namespace std::string_view_literals;
  if (generic_combo(
        m_id,
        gui_labels::draw_bit,
        []() {
          return std::array{ draw_bitT::all,
                             draw_bitT::enabled,
                             draw_bitT::disabled };
        },
        []() {
          return std::array{ "all"sv, "enabled"sv, "disabled"sv };
        },
        [this]() -> auto & { return m_map_sprite.filter().draw_bit; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}
void gui::combo_filtered_palettes() const
{
  const auto &map = m_map_sprite.uniques().palette();
  const auto &key = m_map_sprite.filter().bpp.value();
  if (map.contains(key))
  {
    const auto &pair = map.at(key);
    if (generic_combo(
          m_id,
          gui_labels::palette,
          [&pair]() { return pair.values(); },
          [&pair]() { return pair.strings(); },
          [this]() -> auto & { return m_map_sprite.filter().palette; }))
    {
      m_map_sprite.update_render_texture();
      m_selections.palette = m_map_sprite.filter().palette.value();
      Configuration config{};
      config->insert_or_assign("selections_palette", m_selections.palette);
      config.save();
      m_changed = true;
    }
  }
}

void gui::combo_filtered_bpps() const
{
  const auto &pair = m_map_sprite.uniques().bpp();
  if (generic_combo(
        m_id,
        gui_labels::bpp,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().bpp; }))
  {
    m_map_sprite.update_render_texture();
    m_selections.bpp =
      static_cast<int>(m_map_sprite.filter().bpp.value().raw() & 3U);
    Configuration config{};
    config->insert_or_assign("selections_bpp", m_selections.bpp);
    config.save();
    m_changed = true;
  }
}

void gui::combo_blend_modes() const
{
  const auto &pair = m_map_sprite.uniques().blend_mode();
  if (generic_combo(
        m_id,
        gui_labels::blend_mode,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().blend_mode; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}

void gui::combo_layers() const
{
  const auto &pair = m_map_sprite.uniques().layer_id();
  if (generic_combo(
        m_id,
        gui_labels::layers,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().layer_id; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}
void gui::combo_texture_pages() const
{
  const auto &pair = m_map_sprite.uniques().texture_page_id();
  if (generic_combo(
        m_id,
        gui_labels::texture_page,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().texture_page_id; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}
void gui::combo_animation_ids() const
{
  const auto &pair = m_map_sprite.uniques().animation_id();
  if (generic_combo(
        m_id,
        gui_labels::animation_id,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().animation_id; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}
void gui::combo_blend_other() const
{
  const auto &pair = m_map_sprite.uniques().blend_other();
  if (generic_combo(
        m_id,
        gui_labels::blend_other,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().blend_other; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}
void gui::combo_compact_type(::filter<compact_type> &compact) const
{
  if (generic_combo(
        m_id,
        gui_labels::compact,
        []() {
          return std::array{ compact_type::rows, compact_type::all };
        },
        []() {
          return std::array{ gui_labels::rows, gui_labels::all };
        },
        [&]() -> auto & { return compact; }))
  {
  }
}

void gui::combo_z() const
{
  const auto &pair = m_map_sprite.uniques().z();
  if (generic_combo(
        m_id,
        gui_labels::z,
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().z; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}

void gui::combo_animation_frames() const
{
  const auto &map = m_map_sprite.uniques().animation_frame();
  const auto &key = m_map_sprite.filter().animation_id.value();
  if (map.contains(key))
  {
    const auto &pair = map.at(key);
    if (generic_combo(
          m_id,
          gui_labels::animation_frame,
          [&pair]() { return pair.values(); },
          [&pair]() { return pair.strings(); },
          [this]() -> auto & { return m_map_sprite.filter().animation_frame; }))
    {
      m_map_sprite.update_render_texture();
      m_changed = true;
    }
  }
}
BPPT gui::bpp() const
{
  return Mim::bpp_selections().at(static_cast<size_t>(m_selections.bpp));
}
void gui::combo_deswizzle_path() const
{
  if (safedir deswizzle_texture_path = m_loaded_deswizzle_texture_path;
      !deswizzle_texture_path.is_exists() || !m_field)
  {
    return;
  }
  // std::vector<std::filesystem::path> values = {
  // m_loaded_deswizzle_texture_path };
  std::vector<std::string> strings = {
    m_loaded_deswizzle_texture_path.string()
  };
  if (generic_combo(
        m_id,
        gui_labels::deswizzle_path,
        //[&values]() { return values; },
        [&strings]() { return strings; },
        [&strings]() { return strings; },
        [this]() -> auto & { return m_map_sprite.filter().deswizzle; }))
  {
    if (m_map_sprite.filter().deswizzle.enabled())
    {
      m_map_sprite.filter().upscale.disable();
    }
    m_map_sprite.update_render_texture(true);
    m_changed = true;
  }
}
void gui::combo_upscale_path() const
{
  if (!m_field)
    return;
  if (combo_upscale_path(
        m_map_sprite.filter().upscale))//, m_field->get_base_name(), get_coo()
  {
    if (m_map_sprite.filter().upscale.enabled())
    {
      m_map_sprite.filter().deswizzle.disable();
    }
    m_map_sprite.update_render_texture(true);
    m_changed = true;
  }
}
void gui::generate_upscale_paths(
  const std::string &field_name,
  open_viii::LangT   coo) const
{
  m_upscale_paths.clear();
  auto transform_paths =
    m_paths | std::views::transform([](const toml::node &item) -> std::string {
      return item.value_or(std::string{});
    })
    | std::views::transform([this, &field_name, &coo](const std::string &path) {
        if (m_field)
          return upscales(std::filesystem::path(path), field_name, coo)
            .get_paths();
        return upscales{}.get_paths();
      });
  // std::views::join; broken in msvc.
  auto process = [this](const auto &temp_paths) {
    auto filter_paths = temp_paths | std::views::filter([](safedir path) {
                          return path.is_exists() && path.is_dir();
                        });
    for (auto &path : filter_paths)
    {
      m_upscale_paths.emplace_back(path.string());
    }
  };
  for (auto temp_paths : transform_paths)
  {
    process(temp_paths);
  }
  if (safedir(m_loaded_swizzle_texture_path).is_exists())
  {
    m_upscale_paths.push_back(m_loaded_swizzle_texture_path.string());
  }
  if (m_field)
  {
    process(
      upscales(std::filesystem::current_path(), field_name, coo).get_paths());
    for (const auto &up : m_custom_upscale_paths)
    {
      process(
        upscales(up.value_or(std::string{}), field_name, coo).get_paths());
    }
  }
  std::ranges::sort(m_upscale_paths);
  const auto to_remove = std::ranges::unique(m_upscale_paths);
  m_upscale_paths.erase(to_remove.begin(), to_remove.end());
}
bool gui::combo_upscale_path(::filter<std::filesystem::path> &filter) const
{
  if (
    m_field
    && generic_combo(
      m_id,
      gui_labels::upscale_path,
      [this]() { return m_upscale_paths; },
      [this]() { return m_upscale_paths; },
      [&filter]() -> auto & { return filter; }))
  {
    return true;
  }

  return false;
}

bool gui::combo_upscale_path(
  std::filesystem::path &path,
  const std::string     &field_name,
  open_viii::LangT       coo) const
{
  std::vector<std::string> paths = {};
  auto                     transform_paths =
    m_paths | std::views::transform([](const toml::node &item) -> std::string {
      return item.value_or(std::string{});
    })
    | std::views::transform(
      [this, &field_name, &coo](const std::string &in_path) {
        if (m_field)
          return upscales(std::filesystem::path(in_path), field_name, coo)
            .get_paths();
        return upscales{}.get_paths();
      });
  // std::views::join; broken in msvc.
  auto process = [&paths](const auto &temp_paths) {
    auto filter_paths = temp_paths | std::views::filter([](safedir in_path) {
                          return in_path.is_exists() && in_path.is_dir();
                        });
    for (auto &in_path : filter_paths)
    {
      paths.emplace_back(in_path.string());
    }
  };
  for (auto temp_paths : transform_paths)
  {
    process(temp_paths);
  }
  if (safedir(m_loaded_swizzle_texture_path).is_exists())
  {
    paths.push_back(m_loaded_swizzle_texture_path.string());
  }
  if (m_field)
  {
    process(
      upscales(std::filesystem::current_path(), field_name, coo).get_paths());

    if (generic_combo(
          m_id,
          gui_labels::upscale_path,
          [&paths]() { return paths; },
          [&paths]() { return paths; },
          path))
    {
      return true;
    }
  }
  return false;
}

std::vector<std::filesystem::path>
  gui::find_maps_in_directory(std::filesystem::path src, size_t reserve)
{
  std::vector<std::filesystem::path> r{};
  r.reserve(reserve);
  for (auto path : std::filesystem::recursive_directory_iterator{ src })
  {
    if (
      path.path().has_extension()
      && open_viii::tools::i_ends_with(
        path.path().extension().string(),
        open_viii::graphics::background::Map::EXT))
    {
      r.emplace_back(std::move(path.path()));
    }
  }
  return r;
}


[[nodiscard]] inline bool any_matches(
  const std::vector<std::filesystem::path>       &paths,
  const std::vector<open_viii::archive::FileData> all_file_data)
{
  return std::ranges::any_of(
    all_file_data, [&paths](const open_viii::archive::FileData &file_data) {
      const auto in_path = file_data.get_path();
      return in_path.has_filename()
             && open_viii::archive::any_matches(paths, in_path);
    });
}
template<bool Nested>
std::vector<std::filesystem::path> gui::replace_entries(
  const open_viii::archive::FIFLFS<Nested> &field,
  const std::vector<std::filesystem::path> &paths) const
{
  auto tmp = open_viii::archive::replace_files<Nested>(field, paths);
  return tmp;
}

void gui::popup_batch_embed() const
{
  if (m_batch_embed(m_archives_group.mapdata()))
  {
    m_batch_embed2.enable({}, m_batch_embed.start_time());
  }
  else if (m_batch_embed2(std::array{ "fields" }))
  {
    if (open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
    {
      m_batch_embed3.enable({}, m_batch_embed2.start_time());
    }
    else
    {
      m_batch_embed4.enable(m_selections.path, m_batch_embed2.start_time());
    }
  }
  else if (m_batch_embed3(std::array{ "fields" }))
  {
    m_batch_embed4.enable(m_selections.path, m_batch_embed3.start_time());
  }
  else if (m_batch_embed4(std::array{ "save" }))
  {
    if (open_viii::archive::fiflfs_in_main_zzz(m_archives_group.archives()))
    {
    }
  }
}

bool gui::check_futures() const
{
  const auto romoval =
    std::ranges::remove_if(m_futures, [](const std::future<void> &f) {
      return !f.valid()
             || f.wait_for(std::chrono::seconds{}) == std::future_status::ready;
    });
  m_futures.erase(romoval.begin(), romoval.end());
  return !std::ranges::empty(m_futures);
}
template<typename T, typename... argsT>
void gui::launch_async(T &&task, argsT &&...args) const
{
#ifdef USE_THREADS
  m_futures.emplace_back(std::async(
    std::launch::async, std::forward<T>(task), std::forward<argsT>(args)...));
#undef USE_THREADS
#else
  task(std::forward<argsT>(args)...);
#endif
}
void gui::batch_ops_ask_menu() const
{
#if 1
  if (m_selections.test_batch_window)
  {
    static gui_batch test{ m_archives_group };
    test(&m_id);
  }
#else
  using namespace std::string_view_literals;
  if (ImGui::Begin(
        "Batch Operations", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    if (ImGui::CollapsingHeader("Source", ImGuiTreeNodeFlags_DefaultOpen))
    {
      static int            selected_src_type = {};

      static constexpr auto src_types         = std::array{ "Fields Archive"sv,
                                                    "Upscales or Swizzles"sv,
                                                    "Deswizzles"sv };
      generic_combo(
        m_id,
        "Type",
        []() {
          return std::views::iota(
            0, static_cast<int>(std::ranges::ssize(src_types)));
        },
        []() { return src_types; },
        selected_src_type);
      static std::filesystem::path selected_src_swizzle_path   = {};
      static std::filesystem::path selected_src_deswizzle_path = {};
      if (selected_src_type == 0)
      {
        combo_path();
        {
          const auto sg = PushPop();
          if (ImGui::Button("Browse"))
          {
            open_locate_ff8_filebrowser();
          }
        }
      }
      else if (selected_src_type == 1)
      {
        if (combo_upscale_path(selected_src_swizzle_path, "", {}))
        {
        }
        {
          const auto sg = PushPop();
          if (ImGui::Button("Browse"))
          {
            open_swizzle_filebrowser();
          }
        }
      }
      else if (selected_src_type == 2)
      {
        format_imgui_text("Directory: {}", selected_src_deswizzle_path);
        {
          const auto sg = PushPop();
          if (ImGui::Button("Browse"))
          {
            open_deswizzle_filebrowser();
          }
        }
      }
    }
    static constexpr auto task_types =
      std::array{ "None"sv, "Deswizzle"sv, "Swizzle"sv };
    static int                  task_type       = {};
    static filter<compact_type> compact         = {};
    static bool                 flatten_bpp     = {};
    static bool                 flatten_palette = {};
    if (ImGui::CollapsingHeader(
          "Transformation", ImGuiTreeNodeFlags_DefaultOpen))
    {
      generic_combo(
        m_id,
        "Task"sv,
        []() {
          return std::views::iota(
            0, static_cast<int>(std::ranges::ssize(task_types)));
        },
        []() { return task_types; },
        task_type);
      combo_compact_type(compact);
      format_imgui_text("Flatten: ");
      ImGui::SameLine();
      ImGui::Checkbox("BPP", &flatten_bpp);
      ImGui::SameLine();
      ImGui::Checkbox("Palette", &flatten_palette);
      ImGui::Separator();
    }
    if (
      task_type != 0
      && ImGui::CollapsingHeader("Destination", ImGuiTreeNodeFlags_DefaultOpen))
    {
      static std::filesystem::path selected_dst_path{};
      format_imgui_text("Directory: {}", selected_dst_path);
      {
        const auto sg = PushPop();
        if (ImGui::Button("Browse"))
        {
        }
      }
    }
    if (ImGui::CollapsingHeader(
          "Embed .map(s) into archives", ImGuiTreeNodeFlags_DefaultOpen))
    {
      static bool embed_maps   = {};
      static bool reload_after = { true };
      {
        const auto sg = PushPop();
        if (ImGui::Checkbox("", &embed_maps))
        {
        }
      }
      if (embed_maps)
      {
        ImGui::SameLine();
        combo_path();
        {
          const auto sg = PushPop();
          if (ImGui::Checkbox("Reload after?", &reload_after))
          {
          }
        }
      }
    }
    ImGui::Separator();
    {
      const auto sg = PushPop();
      if (ImGui::Button("Start"))
      {
      }
    }
    ImGui::SameLine();
    {
      const auto sg = PushPop();
      if (ImGui::Button("Cancel"))
      {
      }
    }
  }
  ImGui::End();
#endif
}

std::variant<
  std::monostate,
  open_viii::graphics::background::Tile1,
  open_viii::graphics::background::Tile2,
  open_viii::graphics::background::Tile3> &
  gui::combo_selected_tile() const
{
  // combo box with all the tiles.
  static std::string current_item_str = {};
  static std::variant<std::monostate, Tile1, Tile2, Tile3> current_tile{
    std::monostate{}
  };
  m_map_sprite.const_visit_tiles([this](const auto &tiles) {
    if (
      m_selections.selected_tile < 0
      || std::cmp_greater_equal(m_selections.selected_tile, tiles.size()))
    {
      current_tile = std::monostate{};
      return;
    }
    std::visit(
      [&tiles, this](const auto &tile) {
        if (std::cmp_less(m_selections.selected_tile, tiles.size()))
        {
          const auto &tmp_tile =
            tiles[static_cast<std::size_t>(m_selections.selected_tile)];
          if constexpr (std::is_same_v<
                          std::decay_t<decltype(tile)>,
                          std::decay_t<decltype(tmp_tile)>>)
          {
            if (tile != tmp_tile)
            {
              current_tile     = tmp_tile;
              current_item_str = fmt::format("{}", m_selections.selected_tile);
            }
          }
          else if constexpr (!is_tile<std::decay_t<decltype(tile)>>)
          {
            current_tile     = tmp_tile;
            current_item_str = fmt::format("{}", m_selections.selected_tile);
          }
        }
      },
      current_tile);
  });


  ImVec2     combo_pos    = ImGui::GetCursorScreenPos();
  const auto the_end_id_0 = scope_guard([]() { ImGui::PopID(); });
  ImGui::PushID(++m_id);
  if (ImGui::BeginCombo(
        "Select Existing Tile", "", ImGuiComboFlags_HeightLargest))
  {
    const auto the_end_combo = scope_guard([]() { ImGui::EndCombo(); });
    m_map_sprite.const_visit_tiles([this](const auto &tiles) {
      for (int i{}; const auto &tile : tiles)
      {
        const auto the_end_id_1 = scope_guard([]() { ImGui::PopID(); });
        ImGui::PushID(++m_id);
        bool is_selected =
          (m_selections.selected_tile
           == i);// You can store your selection however you
                 // want, outside or inside your objects

        const std::string &i_as_string = fmt::format("{}", i);
        if (std::ranges::any_of(
              std::array{ ImGui::Selectable("", is_selected),
                          []() -> bool {
                            ImGui::SameLine();
                            return false;
                          }(),
                          create_tile_button(tile),
                          []() -> bool {
                            ImGui::SameLine();
                            return false;
                          }(),
                          [&i_as_string]() -> bool {
                            ImGui::Text("%s", i_as_string.c_str());
                            return false;
                          }() },
              std::identity{}))
        {
          m_selections.selected_tile = i;
          Configuration config{};
          config->insert_or_assign(
            "selections_selected_tile", m_selections.selected_tile);
          config.save();
          current_item_str = std::move(i_as_string);
          current_tile     = tile;
        }
        if (is_selected)
        {
          ImGui::SetItemDefaultFocus();// You may set the initial focus when
                                       // opening the combo (scrolling + for
                                       // keyboard navigation support)
        }
        ++i;
      }
    });
  }
  ImVec2      backup_pos = ImGui::GetCursorScreenPos();
  ImGuiStyle &style      = ImGui::GetStyle();
  ImGui::SetCursorScreenPos(ImVec2(
    combo_pos.x + style.FramePadding.x, combo_pos.y + style.FramePadding.y));
  (void)std::visit(
    [this](const auto &tile) -> bool {
      if constexpr (!std::
                      is_same_v<std::decay_t<decltype(tile)>, std::monostate>)
      {
        return create_tile_button(tile);
      }
      else
      {
        return false;
      }
    },
    current_tile);
  ImGui::SameLine();
  ImGui::Text("%s", current_item_str.c_str());
  ImGui::SetCursorScreenPos(backup_pos);
  return current_tile;
}
void gui::begin_batch_embed_map_warning_window() const
{
  if (!m_selections.batch_embed_map_warning_window)
  {
    return;
  }
  // begin imgui window
  const auto pop_id = PushPop();
  ImGui::SetNextWindowPos(
    ImGui::GetMainViewport()->GetCenter(),
    ImGuiCond_Always,
    ImVec2(0.5F, 0.5F));
  const char *id = "Batch embed '.map' files.";
  ImGui::OpenPopup(id);
  if (!ImGui::BeginPopupModal(
        id,
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
  {
    return;
  }
  const auto g = scope_guard([]() { ImGui::EndPopup(); });
  ImGui::TextWrapped(
    "%s",
    fmt::format(
      "This will take the currently loaded Final Fantasy 8 archive in \"{}\" "
      "and create new `field.fi`, `field.fl`, and `field.fs` files. "
      "Replacing any `.map` file with ones found in the chosen path.",
      m_selections.path)
      .c_str());
  if (ImGui::Button("Browse to begin..."))
  {
    browse_for_embed_map_dir();
    ImGui::CloseCurrentPopup();
    m_selections.batch_embed_map_warning_window = false;
  }
  ImGui::SameLine();
  if (ImGui::Button("Close"))
  {
    ImGui::CloseCurrentPopup();
    m_selections.batch_embed_map_warning_window = false;
  }
}
void gui::import_image_window() const
{
  if (!m_selections.display_import_image)
  {
    return;
  }
  // begin imgui window
  const auto the_end = scope_guard([]() { ImGui::End(); });
  if (!ImGui::Begin("Import Image", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    return;
  }
  //   * So I need to choose an existing tile to base the new tiles on.
  [[maybe_unused]] const auto &current_tile = combo_selected_tile();
  // add text showing the tile's info.
  collapsing_tile_info(current_tile);
  //   * I need to browse for an image file.
  bool changed          = browse_for_image_display_preview();
  //   * We need to adjust the scale to fit
  // maybe i can just create an imgui window filled with the image
  // scale the image to be the selected tile size. 16,32,64,128,256.
  changed               = combo_tile_size() || changed;
  //   * We need to adjust the position
  // have a px offset? or something?
  //   * I'd probably store the new tiles in their own map.
  const auto tiles_wide = static_cast<std::uint32_t>(std::ceil(
    static_cast<float>(loaded_image_texture.getSize().x)
    / static_cast<float>(m_selections.tile_size_value)));
  const auto tiles_high = static_cast<std::uint32_t>(std::ceil(
    static_cast<float>(loaded_image_texture.getSize().y)
    / static_cast<float>(m_selections.tile_size_value)));
  ImGui::Text(
    "%s",
    fmt::format(
      "Possible Tiles: {} wide, {} high, {} total",
      tiles_wide,
      tiles_high,
      tiles_wide * tiles_high)
      .c_str());
  if (changed && tiles_wide * tiles_high != 0U)
  {
    import_image_map =
      open_viii::graphics::background::Map([this,
                                            &current_tile,
                                            x_tile = uint8_t{},
                                            y_tile = uint8_t{},
                                            &tiles_high,
                                            &tiles_wide]() mutable {
        return std::visit(
          [&](auto tile) -> std::variant<
                           open_viii::graphics::background::Tile1,
                           open_viii::graphics::background::Tile2,
                           open_viii::graphics::background::Tile3,
                           std::monostate> {
            if constexpr (is_tile<std::decay_t<decltype(tile)>>)
            {
              if (x_tile == tiles_wide)
              {
                x_tile = 0;
                ++y_tile;
              }
              if (y_tile == tiles_high)
              {
                return std::monostate{};
              }
              //   * Set new tiles to 4 bit to get max amount of tiles.
              tile = tile.with_depth(BPPT::BPP4_CONST())
                       .with_source_xy({ static_cast<uint8_t>(x_tile * 16U),
                                         static_cast<uint8_t>(y_tile * 16U) })
                       .with_xy({ static_cast<int16_t>(x_tile * 16),
                                  static_cast<int16_t>(y_tile * 16) });

              // iterate
              ++x_tile;
              return tile;
            }
            else
            {
              return std::monostate{};
            }
          },
          current_tile);
      });


    //* Filter empty tiles
    loaded_image_cpu = loaded_image_texture.copyToImage();
    import_image_map.visit_tiles([this](auto &tiles) {
      const auto rem_range =
        std::ranges::remove_if(tiles, [this](const auto &tile) -> bool {
          const auto x_start = tile.x() / 16 * m_selections.tile_size_value;
          auto       y_start = tile.y() / 16 * m_selections.tile_size_value;
          const int  xmax    = x_start + m_selections.tile_size_value;
          const sf::Vector2u &imgsize = loaded_image_cpu.getSize();
          const auto x_end = (std::min)(static_cast<int>(imgsize.x), xmax);
          const int  ymax  = y_start + m_selections.tile_size_value;
          const auto y_end = (std::min)(static_cast<int>(imgsize.y), ymax);
          for (auto x = x_start; std::cmp_less(x, x_end); ++x)
          {
            for (auto y = y_start; std::cmp_less(y, y_end); ++y)
            {
              const auto color = loaded_image_cpu.getPixel(
                static_cast<unsigned int>(x), static_cast<unsigned int>(y));
              if (std::cmp_greater(color.a, 0U))
              {
                return false;
              }
            }
          }
          return true;
        });
      tiles.erase(rem_range.begin(), rem_range.end());
    });
    loaded_image_cpu = {};
  }
  if (ImGui::CollapsingHeader(import_image_map
                                .visit_tiles([](auto &&tiles) {
                                  return fmt::format(
                                    "Generated Tiles: {}", std::size(tiles));
                                })
                                .c_str()))
  {
    if (ImGui::BeginTable("import_tiles_table", 9))
    {
      const auto the_end_tile_table = scope_guard([]() { ImGui::EndTable(); });
      import_image_map.visit_tiles([this](auto &tiles) {
        for (const auto &tile : tiles)
        {
          ImGui::TableNextColumn();
          sf::Sprite sprite(
            loaded_image_texture,
            sf::IntRect(
              tile.x() / 16 * m_selections.tile_size_value,
              tile.y() / 16 * m_selections.tile_size_value,
              m_selections.tile_size_value,
              m_selections.tile_size_value));
          const auto the_end_tile_table_tile =
            scope_guard([]() { ImGui::PopID(); });
          ImGui::PushID(++m_id);
          ImGui::ImageButton(sprite, sf::Vector2f(32.F, 32.F), 0);
        }
      });
    }
  }
  // I need to detect the last used texture page and the highest source_y.
  m_map_sprite.const_visit_tiles([this, &changed](const auto &tiles) {
    if (std::ranges::empty(tiles))
    {
      return;
    }
    const auto minmax_x = (std::ranges::minmax)(
      tiles, {}, [](const auto &tile) { return tile.x(); });
    const auto minmax_y = (std::ranges::minmax)(
      tiles, {}, [](const auto &tile) { return tile.y(); });
    //    static constexpr auto adjust = [](
    //                                     std::integral auto OldValue,
    //                                     std::integral auto OldMin,
    //                                     std::integral auto OldMax,
    //                                     std::integral auto NewMin,
    //                                     std::integral auto NewMax) {
    //      using T = std::common_type_t<
    //        decltype(OldValue),
    //        decltype(OldMax),
    //        decltype(OldMin),
    //        decltype(NewMin),
    //        decltype(NewMax)>;
    //      const auto OldRange = static_cast<T>(OldMax) -
    //      static_cast<T>(OldMin); if (OldRange == 0)
    //        return static_cast<T>(NewMin);
    //      else
    //      {
    //        const auto NewRange = static_cast<T>(NewMax) -
    //        static_cast<T>(NewMin); return static_cast<T>(
    //          (static_cast<float>(
    //             (static_cast<T>(OldValue) - static_cast<T>(OldMin))
    //             * static_cast<T>(NewRange))
    //           / static_cast<float>(OldRange))
    //          + static_cast<float>(NewMin));
    //      }
    //    };
    const auto max_texture_id_tile = (std::ranges::max)(
      tiles, {}, [](const auto &tile) { return tile.texture_id(); });
    const auto max_source_y_tile = (std::ranges::max)(
      tiles
        | std::ranges::views::filter([&max_texture_id_tile](const auto &tile) {
            return tile.texture_id() == max_texture_id_tile.texture_id();
          }),
      {},
      [](const auto &tile) { return tile.source_y(); });
    int tile_y = max_source_y_tile.source_y() / 16;
    ImGui::Text(
      "%s",
      fmt::format(
        "Last Used Texture Page {}, and Source Y / 16 = {}",
        max_texture_id_tile.texture_id(),
        tile_y)
        .c_str());
    auto               next_source_y = static_cast<uint8_t>((tile_y + 1) % 16);
    const std::uint8_t next_texture_page =
      tile_y + 1 == 16 ? max_texture_id_tile.texture_id() + 1
                       : max_texture_id_tile.texture_id();
    if (changed)
    {
      import_image_map.visit_tiles(
        [&next_texture_page, &next_source_y, this, &minmax_y, &minmax_x](
          auto &&import_tiles) {
          auto       tile_i   = import_tiles.begin();
          const auto tile_end = import_tiles.end();
          for (std::uint8_t tp = next_texture_page; tp < 16; ++tp)
          {
            for (std::uint8_t y = next_source_y; y < 16; ++y)
            {
              next_source_y = 0;
              for (std::uint8_t x = 0; x < 16; ++x)
              {
                if (tile_i == tile_end)
                {
                  return;
                }
                *tile_i = tile_i->with_source_xy(x * 16, y * 16)
                            .with_texture_id(tp) /*.with_xy(
static_cast<std::int16_t>(adjust(
tile_i->x() * m_selections.tile_size_value,
0,
loaded_image.getSize().x,
minmax_x.min.x(),
minmax_x.max.x() + 16)),
static_cast<std::int16_t>(adjust(
tile_i->y() * m_selections.tile_size_value,
0,
loaded_image.getSize().y,
minmax_y.min.y(),
minmax_y.max.y() + 16)))*/
                  ;
                ++tile_i;
              }
            }
          }
        });
      update_scaled_up_render_texture();
      update_imported_render_texture();
    }
  });
  // I think I have the new tiles coords generated correctly for the swizzle and
  // for drawing in game. just need to render it into an image and merge the new
  // tiles into the `.map`
  //    * Then we can swap between swizzle and deswizzle views to show what they
  //    look like
  //    * At the end we need to be able to save and merge them with the '.map'
  //    file.
  //    * They'll probably insert before the last tile.
  //  Save button that'll save the swizzled images with new '.map'
  if (ImGui::Button("Save Swizzle"))
  {
    save_swizzle_textures();
  }
  // have a cancel button to hide window.
  ImGui::SameLine();
  if (ImGui::Button("Cancel"))
  {
    // hide window and save that it's hidden.
    m_selections.display_import_image = false;
    Configuration config{};
    config->insert_or_assign(
      "selections_display_import_image", m_selections.display_import_image);
    config.save();
    reset_imported_image();
  }
  // have a reset button to reset window state?
  ImGui::SameLine();
  if (ImGui::Button("Reset"))
  {
    reset_imported_image();
  }
}
void gui::reset_imported_image() const
{
  m_map_sprite.update_render_texture(nullptr, {}, 16);
  import_image_map                   = {};
  loaded_image_texture               = {};
  loaded_image_cpu                   = {};
  m_import_image_path                = {};
  m_selections.render_imported_image = false;
  Configuration config{};
  config->insert_or_assign(
    "selections_render_imported_image", m_selections.render_imported_image);
  config.save();
}
bool gui::combo_tile_size() const
{
  if (!generic_combo(
        m_id,
        std::string_view("Tile Size"),
        []() {
          return std::array{ uint16_t{ 16U },
                             uint16_t{ 32U },
                             uint16_t{ 64U },
                             uint16_t{ 128U },
                             uint16_t{ 256U } };
        },
        []() {
          return std::array{ std::string_view{ " 1x  16 px" },
                             std::string_view{ " 2x  32 px" },
                             std::string_view{ " 4x  64 px" },
                             std::string_view{ " 8x 128 px" },
                             std::string_view{ "16x 256 px" } };
        },
        m_selections.tile_size_value))
  {
    return false;
  }
  Configuration config{};
  config->insert_or_assign(
    "selections_tile_size_value", m_selections.tile_size_value);
  config.save();
  return true;
}
bool gui::browse_for_image_display_preview() const
{
  bool changed = false;
  ImGui::InputText(
    "##image_path",
    m_import_image_path.data(),
    m_import_image_path.size(),
    ImGuiInputTextFlags_ReadOnly);
  ImGui::SameLine();
  if (ImGui::Button("Browse"))
  {
    m_load_file_browser.Open();
    m_load_file_browser.SetTitle("Load Image File...");
    m_load_file_browser.SetTypeFilters({ ".png" });
    m_load_file_browser.SetPwd(Configuration{}["load_image_path"].value_or(
      std::filesystem::current_path().string()));
    m_load_file_browser.SetInputName(m_import_image_path.data());
  }
  m_load_file_browser.Display();

  if (m_load_file_browser.HasSelected())
  {
    Configuration config{};
    config->insert_or_assign(
      "load_image_path", m_load_file_browser.GetPwd().string());
    config.save();
    [[maybe_unused]] const auto selected_path =
      m_load_file_browser.GetSelected();
    m_import_image_path = selected_path.string();
    m_load_file_browser.ClearSelected();
    loaded_image_texture.loadFromFile(m_import_image_path);// stored on gpu.
    loaded_image_texture.setRepeated(false);
    loaded_image_texture.setSmooth(false);
    loaded_image_texture.generateMipmap();
    changed = true;
  }
  if (
    loaded_image_texture.getSize().x == 0
    || loaded_image_texture.getSize().y == 0)
  {
    return false;
  }
  if (ImGui::CollapsingHeader("Selected Image Preview"))
  {
    sf::Sprite  sprite(loaded_image_texture);
    const float w = std::max(
      (ImGui::GetContentRegionAvail().x /* - ImGui::GetStyle().ItemSpacing.x*/),
      1.0f);
    const auto  size  = loaded_image_texture.getSize();

    float       scale = w / static_cast<float>(size.x);
    const float h     = static_cast<float>(size.y) * scale;
    ImVec2      p     = ImGui::GetCursorScreenPos();
    const auto  sg    = PushPop();
    ImGui::ImageButton(sprite, sf::Vector2f(w, h), 0);
    if (ImGui::Checkbox("Draw Grid", &m_selections.import_image_grid))
    {
      Configuration config{};
      config->insert_or_assign(
        "selections_import_image_grid", m_selections.import_image_grid);
      config.save();
    }
    if (m_selections.import_image_grid)
    {
      for (std::uint32_t x{ m_selections.tile_size_value }; x < size.x;
           x += m_selections.tile_size_value)
      {
        ImGui::GetWindowDrawList()->AddLine(
          ImVec2(p.x + (static_cast<float>(x) * scale), p.y),
          ImVec2(
            p.x + (static_cast<float>(x) * scale),
            p.y + (static_cast<float>(size.y) * scale)),
          IM_COL32(255, 0, 0, 255),
          2.0f);
      }
      for (std::uint32_t y{ m_selections.tile_size_value }; y < size.y;
           y += m_selections.tile_size_value)
      {
        ImGui::GetWindowDrawList()->AddLine(
          ImVec2(p.x, p.y + (static_cast<float>(y) * scale)),
          ImVec2(
            p.x + (static_cast<float>(size.x) * scale),
            p.y + (static_cast<float>(y) * scale)),
          IM_COL32(255, 0, 0, 255),
          2.0f);
      }
    }
  }
  return changed;
}
void gui::update_scaled_up_render_texture() const
{
  const auto scale_up_dim = [this](uint32_t dim) {
    return static_cast<uint32_t>(
      ceil(
        static_cast<double>(dim)
        / static_cast<double>(m_selections.tile_size_value))
      * static_cast<double>(m_selections.tile_size_value));
  };
  const auto size = loaded_image_texture.getSize();
  loaded_image_render_texture.create(
    scale_up_dim(size.x), scale_up_dim(size.y));
  loaded_image_render_texture.setActive(true);
  loaded_image_render_texture.clear(sf::Color::Transparent);
  sf::Sprite sprite = sf::Sprite(loaded_image_texture);
  sprite.setScale(1.f, -1.f);
  sprite.setPosition(
    0.f, static_cast<float>(loaded_image_render_texture.getSize().y));
  loaded_image_render_texture.draw(sprite);
  loaded_image_render_texture.setRepeated(false);
  loaded_image_render_texture.setSmooth(false);
  loaded_image_render_texture.generateMipmap();
}
void gui::collapsing_tile_info(
  const std::variant<
    std::monostate,
    open_viii::graphics::background::Tile1,
    open_viii::graphics::background::Tile2,
    open_viii::graphics::background::Tile3> &current_tile) const
{
  std::visit(
    [this](const auto &tile) {
      if constexpr (is_tile<std::decay_t<decltype(tile)>>)
      {
        if (ImGui::CollapsingHeader("Selected Tile Info"))
        {
          if (ImGui::BeginTable("table_tile_info", 2))
          {
            const auto the_end_table = scope_guard([]() { ImGui::EndTable(); });
            m_map_sprite.format_tile_text(
              tile, [](const std::string_view text, const auto value) {
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%s", text.data());
                ImGui::TableNextColumn();
                ImGui::Text("%s", fmt::format("{}", value).c_str());
              });
          }
          const auto w = std::max(
                           (ImGui::GetContentRegionAvail()
                              .x /* - ImGui::GetStyle().ItemSpacing.x*/),
                           1.F)
                         / 2.F;
          ImVec2      backup_pos = ImGui::GetCursorScreenPos();
          ImGuiStyle &style      = ImGui::GetStyle();
          ImGui::SetCursorScreenPos(ImVec2(
            backup_pos.x + w * 1.1F,
            backup_pos.y - w * .9F - style.FramePadding.y * 2.F));
          (void)create_tile_button(tile, { w * .9F, w * .9F });
          ImGui::SetCursorScreenPos(backup_pos);
        }
      }
    },
    current_tile);
}
}// namespace fme