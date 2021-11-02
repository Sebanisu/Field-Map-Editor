//
// Created by pcvii on 9/7/2021.
//
#include "gui.hpp"
#include "gui_labels.hpp"
#include "open_viii/paths/Paths.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Window/Mouse.hpp>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;


std::ostream &
  operator<<(std::ostream &os, const BlendModeT &bm)
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

template<>
struct fmt::formatter<BPPT>
{
  // Presentation format: 'f' - fixed, 'e' - exponential.
  char presentation = 'f';

  // Parses format specifications of the form ['f' | 'e'].
  constexpr auto
    parse(format_parse_context &ctx) -> decltype(ctx.begin())
  {
    // [ctx.begin(), ctx.end()) is a character range that contains a part of
    // the format string starting from the format specifications to be parsed,
    // e.g. in
    //
    //   fmt::format("{:f} - BPPT of interest", BPPT{1, 2});
    //
    // the range will contain "f} - BPPT of interest". The formatter should
    // parse specifiers until '}' or the end of the range. In this example
    // the formatter should parse the 'f' specifier and return an iterator
    // BPPTing to '}'.

    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e'))
      presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the BPPT p using the parsed format specification (presentation)
  // stored in this formatter.
  template<typename FormatContext>
  auto
    format(const BPPT &p, FormatContext &ctx) -> decltype(ctx.out())
  {
    // ctx.out() is an output iterator to write to.
    return format_to(ctx.out(), "{}", static_cast<int>(p));
  }
};
template<typename T>
concept returns_range_concept = requires(std::decay_t<T> t)
{
  {
    t()
    } -> std::ranges::range;
};
template<typename T>
concept filter_concept = requires(std::decay_t<T> t)
{
  {
    t.enabled()
    } -> std::convertible_to<bool>;
  {
    t.update(t.value())
    } -> std::convertible_to<T>;
  {
    t.enable()
    } -> std::convertible_to<T>;
  {
    t.disable()
    } -> std::convertible_to<T>;
};
template<typename T>
concept returns_filter_concept = requires(std::decay_t<T> t)
{
  {
    t()
    } -> filter_concept;
};
template<
  returns_range_concept  value_lambdaT,
  returns_range_concept  string_lambdaT,
  returns_filter_concept filter_lambdaT>
static bool
  generic_combo(
    int             &id,
    std::string_view name,
    value_lambdaT  &&value_lambda,
    string_lambdaT &&string_lambda,
    filter_lambdaT &&filter_lambda)
{
  bool               changed     = false;
  const auto        &values      = value_lambda();
  const auto        &strings     = string_lambda();
  auto              &filter      = filter_lambda();
  bool               checked     = filter.enabled();
  static std::size_t current_idx = {};
  if (const auto it = std::find(values.begin(), values.end(), filter.value());
      it != values.end())
  {
    current_idx = static_cast<size_t>(std::distance(values.begin(), it));
  }
  else
  {
    current_idx = 0;
    if (!std::empty(values))
    {
      filter.update(values.front());
    }
    changed = true;
  }
  if (std::empty(values) || std::empty(strings))
  {
    if (checked)
    {
      filter.disable();
      return true;
    }
    return false;
  }

  const auto *current_item = strings.at(current_idx).data();

  ImGui::PushID(++id);
  static constexpr auto pattern = "{}: \t{} \t{}\n";
  if (ImGui::Checkbox("", &checked))
  {
    if (checked)
    {
      filter.enable();
      fmt::print(pattern, gui_labels::enable, name, values.at(current_idx));
    }
    else
    {
      filter.disable();
      fmt::print(pattern, gui_labels::disable, name, values.at(current_idx));
    }
    changed = true;
  }
  ImGui::PopID();
  ImGui::SameLine();
  ImGui::PushID(++id);
  const auto old = current_idx;
  if (ImGui::BeginCombo(name.data(), current_item))
  // The second parameter is the label previewed
  // before opening the combo.
  {
    for (std::size_t n = 0; auto &string : strings)
    {
      const bool  is_selected = (current_idx == n);
      // You can store your selection however you
      // want, outside or inside your objects
      // ImGui::PushID(++m_id);
      const char *v           = string.data();
      if (ImGui::Selectable(v, is_selected))
      {
        current_idx = n;
        changed     = true;
      }
      // ImGui::PopID();
      if (is_selected)
      {
        ImGui::SetItemDefaultFocus();
        // You may set the initial focus when
        // opening the combo (scrolling + for
        // keyboard navigation support)
      }
      ++n;
    }
    if (old != current_idx)
    {
      fmt::print(pattern, gui_labels::set, name, values.at(current_idx));
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
  changed =
    (filter.update(values.at(current_idx)).enabled() && (old != current_idx))
    || changed;
  return changed;
}
template<
  returns_range_concept value_lambdaT,
  returns_range_concept string_lambdaT,
  typename valueT>
requires requires(value_lambdaT v)
{
  {
    *(v().begin())
    } -> std::convertible_to<valueT>;
  {
    *(v().begin())
    } -> std::equality_comparable_with<valueT>;
}
static bool
  generic_combo(
    int             &id,
    std::string_view name,
    value_lambdaT  &&value_lambda,
    string_lambdaT &&string_lambda,
    valueT          &value)
{
  bool        changed   = false;
  const auto &values    = value_lambda();
  const auto &strings   = string_lambda();
  using dT              = decltype(std::distance(values.begin(), values.end()));
  static dT current_idx = {};
  {
    if (const auto it = std::find(values.begin(), values.end(), value);
        it != values.end())
    {
      current_idx = std::ranges::distance(std::ranges::cbegin(values), it);
    }
    else
    {
      current_idx = 0;
      if (!std::empty(values))
      {
        value = values.front();
      }
      changed = true;
    }
  }
  if (std::empty(values) || std::empty(strings))
  {
    return false;
  }
  const auto next = [](const auto &r, const auto &idx)
  { return std::ranges::next(std::ranges::cbegin(r), idx); };
  const auto            current_item = next(strings, current_idx);
  static constexpr auto pattern      = "{}: \t{} \t{}\n";
  ImGui::PushID(++id);
  const auto old = current_idx;
  if (ImGui::BeginCombo(
        name.data(), current_item->data(), ImGuiComboFlags_HeightLarge))
  // The second parameter is the label previewed
  // before opening the combo.
  {
    std::ranges::for_each(
      strings,
      [&](const auto &string)
      {
        const bool  is_selected = (*current_item == string);
        // You can store your selection however you
        // want, outside or inside your objects
        // ImGui::PushID(++m_id);
        const char *c_str_value = std::ranges::data(string);
        if (ImGui::Selectable(c_str_value, is_selected))
        {
          current_idx = std::distance(std::ranges::data(strings), &string);
          changed     = true;
        }
        // ImGui::PopID();
        if (is_selected)
        {
          ImGui::SetItemDefaultFocus();
          // You may set the initial focus when
          // opening the combo (scrolling + for
          // keyboard navigation support)
        }
      });
    if (old != current_idx)
    {
      fmt::print(pattern, gui_labels::set, name, *next(values, current_idx));
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
  value = *next(values, current_idx);
  return old != current_idx || changed;
}
void
  gui::start() const
{
  if (m_window.isOpen())
  {
    scale_window(
      static_cast<float>(m_window_width), static_cast<float>(m_window_height));
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
void
  gui::loop() const
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
      popup_batch_deswizzle();
      popup_batch_reswizzle();
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
  ImGui::End();
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
void
  gui::popup_batch_common_filter_start(
    ::filter<std::filesystem::path> filter,
    std::string_view                prefix,
    std::string_view                base_name) const
{// filters.upscale
  if (filter.enabled())
  {
    filter.update(filter.value() / prefix / base_name);
    if (
      !std::filesystem::exists(filter.value())
      || !std::filesystem::is_directory(filter.value()))
    {
      filter.disable();
    }
  }
}

template<typename batch_opT, typename filterT, typename askT, typename processT>
void
  gui::popup_batch_common(
    batch_opT &&batch_op,
    filterT   &&filter,
    askT      &&ask,
    processT  &&process) const
{
  if (batch_op(
        m_archives_group.mapdata(),
        [&](
          const int &pos, std::filesystem::path selected_path, filters filters, auto&&... rest)
        {
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
            auto map_path = filter(filters).value() / map.map_filename();
            if (std::filesystem::exists(map_path))
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
            fmt::print(
              "{}:{} - {}: {}\t {}: {}\n",
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
                  0, std::min(std::size(filename), std::size(base_name)))
                != basename_view.substr(
                  0, std::min(std::size(filename), std::size(base_name))))
              {
                continue;
              }
              if (filename.size() == base_name.size())
              {
                process(selected_path, map, rest...);
                continue;
              }
              const auto coo_view =
                filename_view.substr(std::size(base_name) + 1U, 2U);
              fmt::print("\t{}\t{}\n", filename, coo_view);
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

void
  gui::popup_batch_deswizzle() const
{
  popup_batch_common(
    m_batch_deswizzle,
    [](::filters &filters) -> ::filter<std::filesystem::path> & {
      return filters.upscale;
    },
    [this](::filter<std::filesystem::path> &filter)
    {
      if (combo_upscale_path(filter, ""))
      {
      }

      return ImGui::Button(gui_labels::start.data());
    },
    [&](const auto &selected_path, auto &map)
    {
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
//{
//  if (m_batch_deswizzle(
//        m_archives_group.mapdata(),
//        [this](
//          const int &pos, std::filesystem::path selected_path, filters
//          filters)
//        {
//          auto field = m_archives_group.field(pos);
//          if (!field)
//          {
//            return;
//          }
//          const auto map_pairs = field->get_vector_of_indexes_and_files(
//            { open_viii::graphics::background::Map::EXT });
//          if (map_pairs.empty())
//          {
//            return;
//          }
//          std::string      base_name = str_to_lower(field->get_base_name());
//          std::string_view prefix = std::string_view(base_name).substr(0U,
//          2U); popup_batch_common_filter_start(filters.upscale, prefix,
//          base_name);
//
//          auto map = m_map_sprite.with_field(field)
//                       .with_coo(open_viii::LangT::generic)
//                       .with_filters(filters);
//          //               map_sprite{ m_field, open_viii::LangT::en, {},
//          //               filters };
//          if (map.fail())
//          {
//            return;
//          }
//          if (filters.upscale.enabled())
//          {
//            auto map_path = filters.upscale.value() / map.map_filename();
//            if (std::filesystem::exists(map_path))
//            {
//              map.load_map(map_path);
//            }
//          }
//          selected_path = selected_path / prefix / base_name;
//          if (std::filesystem::create_directories(selected_path))
//          {
//            format_imgui_text(
//              "{} {}", gui_labels::directory_created, selected_path.string());
//          }
//          else
//          {
//            format_imgui_text(
//              "{} {}", gui_labels::directory_exists, selected_path.string());
//          }
//          map.save_pupu_textures(selected_path);
//          format_imgui_text(gui_labels::saving_textures);
//          const auto process = [&]()
//          {
//            const std::filesystem::path map_path =
//              selected_path / map.map_filename();
//            map.save_modified_map(map_path);
//            format_imgui_text(
//              "{} {} {}: {}",
//              gui_labels::saving,
//              open_viii::graphics::background::Map::EXT,
//              gui_labels::file,
//              map_path.string());
//          };
//          if (map_pairs.size() > 1U)
//          {
//            fmt::print(
//              "{}:{} - {}: {}\t {}: {}\n",
//              __FILE__,
//              __LINE__,
//              gui_labels::count_of_maps,
//              map_pairs.size(),
//              gui_labels::field,
//              base_name);
//            for (const auto &[i, file_path] : map_pairs)
//            {
//              const auto filename =
//                std::filesystem::path(file_path).filename().stem().string();
//              std::string_view filename_view = { filename };
//              std::string_view basename_view = { base_name };
//              if (
//                filename_view.substr(
//                  0, std::min(std::size(filename), std::size(base_name)))
//                != basename_view.substr(
//                  0, std::min(std::size(filename), std::size(base_name))))
//              {
//                continue;
//              }
//              if (filename.size() == base_name.size())
//              {
//                process();
//                continue;
//              }
//              const auto coo_view =
//                filename_view.substr(std::size(base_name) + 1U, 2U);
//              fmt::print("\t{}\t{}\n", filename, coo_view);
//              map =
//              map.with_coo(open_viii::LangCommon::from_string(coo_view));
//              process();
//            }
//          }
//          else
//          {
//            process();
//          }
//        },
//        [this](filter<std::filesystem::path> &filter)
//        {
//          if (combo_upscale_path(filter, ""))
//          {
//          }
//
//          return ImGui::Button(gui_labels::start.data());
//        }))
//  {
//  }
//}

void
  gui::popup_batch_reswizzle() const
{
  popup_batch_common(
    m_batch_reswizzle,
    [](::filters &filters) -> ::filter<std::filesystem::path> & {
      return filters.deswizzle;
    },
    [this](
      filter<compact_type> &compact, bool &flatten_bpp, bool &flatten_palette)
    {
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
      const bool                 &flatten_palette)
    {
      const auto c = [&]
      {
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
//void
//  gui::popup_batch_reswizzle() const
//{
//  if (m_batch_reswizzle(
//        m_archives_group.mapdata(),
//        [this](
//          const int            &pos,
//          std::filesystem::path selected_path,
//          filters               filters,
//          filter<compact_type> &compact,
//          bool                 &flatten_bpp,
//          bool                 &flatten_palette)
//        {
//          auto field = m_archives_group.field(pos);
//          if (!field)
//          {
//            return;
//          }
//          const auto map_pairs = field->get_vector_of_indexes_and_files(
//            { open_viii::graphics::background::Map::EXT });
//          if (map_pairs.empty())
//          {
//            return;
//          }
//          // todo get all languages this only get the selected or default
//          std::string      base_name = str_to_lower(field->get_base_name());
//          std::string_view prefix = std::string_view(base_name).substr(0U, 2U);
//          if (filters.deswizzle.enabled())
//          {
//            filters.deswizzle.update(
//              filters.deswizzle.value() / prefix / base_name);
//            if (
//              !std::filesystem::exists(filters.deswizzle.value())
//              || !std::filesystem::is_directory(filters.deswizzle.value()))
//            {
//              filters.deswizzle.disable();
//            }
//          }
//          auto map = m_map_sprite.with_field(field)
//                       .with_coo(open_viii::LangT::generic)
//                       .with_filters(filters);
//          //               map_sprite{ m_field, open_viii::LangT::en, {},
//          //               filters };
//          if (map.fail())
//          {
//            return;
//          }
//          if (filters.deswizzle.enabled())
//          {
//            auto map_path = filters.deswizzle.value() / map.map_filename();
//            if (std::filesystem::exists(map_path))
//            {
//              map.load_map(map_path);
//            }
//          }
//          selected_path = selected_path / prefix / base_name;
//          if (std::filesystem::create_directories(selected_path))
//          {
//            format_imgui_text("Directory Created {}", selected_path.string());
//          }
//          else
//          {
//            format_imgui_text("Directory Exists {}", selected_path.string());
//          }
//          format_imgui_text("Saving Textures");
//          const auto process = [&]()
//          {
//            const auto c = [&]
//            {
//              if (compact.enabled())
//              {
//                if (compact.value() == compact_type::rows)
//                {
//                  map.compact();
//                }
//                if (compact.value() == compact_type::all)
//                {
//                  map.compact2();
//                }
//              }
//            };
//            c();
//            if (flatten_bpp)
//            {
//              map.flatten_bpp();
//            }
//            if (flatten_palette)
//            {
//              map.flatten_palette();
//            }
//            if (flatten_bpp || flatten_palette)
//            {
//              c();
//            }
//            const std::filesystem::path map_path =
//              selected_path / map.map_filename();
//            map.save_new_textures(selected_path);
//            map.save_modified_map(map_path);
//            format_imgui_text("Saving Map file: {}", map_path.string());
//          };
//
//          if (map_pairs.size() > 1U)
//          {
//            fmt::print(
//              "{}:{} - count of maps: {}\t field: {}\n",
//              __FILE__,
//              __LINE__,
//              map_pairs.size(),
//              base_name);
//            for (const auto &[i, file_path] : map_pairs)
//            {
//              const auto filename =
//                std::filesystem::path(file_path).filename().stem().string();
//              std::string_view filename_view = { filename };
//              std::string_view basename_view = { base_name };
//              if (
//                filename_view.substr(
//                  0, std::min(std::size(filename), std::size(base_name)))
//                != basename_view.substr(
//                  0, std::min(std::size(filename), std::size(base_name))))
//              {
//                continue;
//              }
//              if (filename.size() == base_name.size())
//              {
//                process();
//                continue;
//              }
//              const auto coo_view =
//                filename_view.substr(std::size(base_name) + 1U, 2U);
//              fmt::print("\t{}\t{}\n", filename, coo_view);
//              map = map.with_coo(open_viii::LangCommon::from_string(coo_view));
//              process();
//            }
//          }
//          else
//          {
//            process();
//          }
//        },
//        [this](
//          filter<compact_type> &compact,
//          bool                 &flatten_bpp,
//          bool                 &flatten_palette)
//        {
//          combo_compact_type(compact);
//          ImGui::Separator();
//          format_imgui_text("Flatten: ");
//          ImGui::Checkbox("BPP", &flatten_bpp);
//          ImGui::SameLine();
//          ImGui::Checkbox("Palette", &flatten_palette);
//          return ImGui::Button("Start");
//        }))
//  {
//  }
//}
void
  gui::on_click_not_imgui() const
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
void
  gui::text_mouse_position() const
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
bool
  gui::handle_mouse_cursor() const
{
  bool           mouse_enabled = false;
  const auto    &mouse_pos     = sf::Mouse::getPosition(m_window);
  const auto    &win_size      = m_window.getSize();
  constexpr auto in_bounds     = [](auto i, auto low, auto high)
  { return std::cmp_greater_equal(i, low) && std::cmp_less_equal(i, high); };
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
void
  gui::combo_coo() const
{
  if (generic_combo(
        m_id,
        gui_labels::language,
        []()
        {
          return std::views::iota(
            static_cast<int>(open_viii::LangT::begin),
            static_cast<int>(open_viii::LangT::end));
        },
        []()
        {
          static constexpr auto coos_string_array =
            open_viii::LangCommon::to_string_array();
          return coos_string_array;
        },
        m_selections.coo))
  {
    if (mim_test())
    {
      m_mim_sprite = m_mim_sprite.with_coo(get_coo());
    }
    else if (map_test())
    {
      m_map_sprite = m_map_sprite.with_coo(get_coo());
    }
    m_changed = true;
  }
}
const open_viii::LangT &
  gui::get_coo() const
{

  static constexpr auto coos = open_viii::LangCommon::to_array();
  return coos.at(static_cast<size_t>(m_selections.coo));
}
void
  gui::combo_field() const
{
  if (generic_combo(
        m_id,
        gui_labels::field,
        [this]()
        {
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
    update_field();
  }
}

void
  gui::update_field() const
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
}

void
  gui::checkbox_map_swizzle() const
{
  if (ImGui::Checkbox("Swizzle", &m_selections.draw_swizzle))
  {
    if (m_selections.draw_swizzle)
    {
      m_map_sprite.enable_draw_swizzle();
    }
    else
    {
      m_map_sprite.disable_draw_swizzle();
    }
    m_changed = true;
  }
}

void
  gui::checkbox_mim_palette_texture() const
{
  if (ImGui::Checkbox("Draw Palette Texture", &m_selections.draw_palette))
  {
    m_mim_sprite = m_mim_sprite.with_draw_palette(m_selections.draw_palette);
    m_changed    = true;
  }
}
static void
  update_bpp(mim_sprite &sprite, BPPT bpp)
{
  sprite = sprite.with_bpp(bpp);
}
static void
  update_bpp(map_sprite &sprite, BPPT bpp)
{
  if (sprite.filter().bpp.update(bpp).enabled())
  {
    sprite.update_render_texture();
  }
}
void
  gui::combo_bpp() const
{
  ImGui::PushID(++m_id);
  static constexpr std::array bpp_items = Mim::bpp_selections_c_str();
  if (ImGui::Combo(
        "BPP",
        &m_selections.bpp,
        bpp_items.data(),
        static_cast<int>(bpp_items.size()),
        static_cast<int>(bpp_items.size())))
  {
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
  ImGui::PopID();
  ImGui::PushID(++m_id);
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
  ImGui::PopID();
}
std::uint8_t
  gui::palette() const
{
  return static_cast<uint8_t>(
    Mim::palette_selections().at(static_cast<size_t>(m_selections.palette)));
}
static void
  update_palette(mim_sprite &sprite, uint8_t palette)
{
  sprite = sprite.with_palette(palette);
}
static void
  update_palette(map_sprite &sprite, uint8_t palette)
{
  if (sprite.filter().palette.update(palette).enabled())
  {
    sprite.update_render_texture();
  }
}
void
  gui::combo_palette() const
{
  ImGui::PushID(++m_id);
  if (m_selections.bpp != 2)
  {
    static constexpr std::array palette_items = Mim::palette_selections_c_str();
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
      m_changed = true;
    }
    static bool enable_palette_filter = false;
    if (map_test())
    {
      ImGui::SameLine();
      ImGui::PushID(++m_id);
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
      ImGui::PopID();
    }
  }
  ImGui::PopID();
}

void
  gui::slider_xy_sprite(auto &sprite) const
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
void
  gui::menu_bar() const
{
  if (ImGui::BeginMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      menuitem_locate_ff8();
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
      ImGui::MenuItem("Draw Tile Grid", nullptr, &m_selections.draw_grid);
      ImGui::MenuItem(
        "Draw Texture Page Grid",
        nullptr,
        &m_selections.draw_texture_page_grid);
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
        m_directory_browser.SetTypeFilters({ ".map", ".png" });
        m_modified_directory_map =
          map_directory_mode::batch_load_deswizzle_textures;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}
bool
  gui::map_test() const
{
  return !m_map_sprite.fail() && m_selections.draw == 1;
}
bool
  gui::mim_test() const
{
  return !m_mim_sprite.fail() && m_selections.draw == 0;
}
std::string
  gui::save_texture_path() const
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
void
  gui::file_browser_locate_ff8() const
{
  m_directory_browser.Display();
  if (m_directory_browser.HasSelected())
  {
    static std::filesystem::path reswizzle_src = {};
    auto selected_path = m_directory_browser.GetSelected();
    if (m_modified_directory_map == map_directory_mode::ff8_install_directory)
    {
      m_paths.emplace_back(selected_path.string());
      m_paths_c_str = archives_group::get_c_str(
        m_paths);// seems the pointers move when you push back above
      m_selections.path = static_cast<int>(m_paths.size()) - 1;
      update_path();
    }
    else if (
      m_modified_directory_map == map_directory_mode::save_swizzle_textures)
    {
      std::string base_name = m_map_sprite.get_base_name();
      std::string prefix    = base_name.substr(0U, 2U);
      selected_path         = selected_path / prefix / base_name;
      std::filesystem::create_directories(selected_path);
      m_map_sprite.save_new_textures(selected_path);
      m_map_sprite.save_modified_map(
        selected_path / m_map_sprite.map_filename());
    }
    else if (
      m_modified_directory_map == map_directory_mode::load_swizzle_textures)
    {
      m_loaded_swizzle_texture_path = selected_path;
      m_map_sprite.filter()
        .upscale.update(m_loaded_swizzle_texture_path)
        .enable();
      auto map_path =
        m_loaded_swizzle_texture_path / m_map_sprite.map_filename();
      if (std::filesystem::exists(map_path))
      {
        m_map_sprite.load_map(map_path);
      }
      m_map_sprite.update_render_texture(true);
    }
    else if (
      m_modified_directory_map == map_directory_mode::save_deswizzle_textures)
    {
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
      m_loaded_deswizzle_texture_path = selected_path;
      m_map_sprite.filter()
        .deswizzle.update(m_loaded_deswizzle_texture_path)
        .enable();
      auto map_path =
        m_loaded_deswizzle_texture_path / m_map_sprite.map_filename();
      if (std::filesystem::exists(map_path))
      {
        m_map_sprite.load_map(map_path);
      }
      m_map_sprite.update_render_texture(true);
    }
    else if (
      m_modified_directory_map
      == map_directory_mode::batch_save_deswizzle_textures)
    {
      m_batch_deswizzle.enable(selected_path);
    }
    else if (
      m_modified_directory_map
      == map_directory_mode::batch_save_swizzle_textures)
    {
      m_batch_reswizzle.enable(
        std::move(reswizzle_src), std::move(selected_path));
    }
    else if (
      m_modified_directory_map
      == map_directory_mode::batch_load_deswizzle_textures)
    {
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
    }
    m_directory_browser.ClearSelected();
  }
}
void
  gui::file_browser_save_texture() const
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
      }
      else
      {
        m_mim_sprite.save(selected_path);
      }
    }
    else if (map_test())
    {
      const auto str_path = selected_path.string();
      if (open_viii::tools::i_ends_with(str_path, Map::EXT))
      {
        switch (m_modified_map)
        {
        case map_dialog_mode::save_modified:
        {
          m_map_sprite.save_modified_map(selected_path);
        }
        break;
        case map_dialog_mode::save_unmodified:
        {
          m_map_sprite.map_save(selected_path);
        }
        break;
        case map_dialog_mode::load:
        {
          m_map_sprite.load_map(selected_path);
          m_changed = true;
        }
        break;
        }
      }
      else
      {
        m_map_sprite.save(selected_path);
      }
    }
    m_save_file_browser.ClearSelected();
  }
}
void
  gui::menuitem_locate_ff8() const
{
  if (ImGui::MenuItem("Locate a FF8 install"))
  {
    m_directory_browser.Open();
    m_directory_browser.SetTitle("Choose FF8 install directory");
    m_directory_browser.SetTypeFilters({ ".exe" });
    m_modified_directory_map = map_directory_mode::ff8_install_directory;
  }
}
void
  gui::menuitem_save_swizzle_textures() const
{
  if (ImGui::MenuItem("Save Swizzled Textures", nullptr, false, true))
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
    m_directory_browser.SetTypeFilters({ ".map", ".png" });
    m_modified_directory_map = map_directory_mode::save_swizzle_textures;
  }
}
void
  gui::menuitem_save_deswizzle_textures() const
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
    m_directory_browser.SetTypeFilters({ ".map", ".png" });
    m_modified_directory_map = map_directory_mode::save_deswizzle_textures;
  }
}
void
  gui::menuitem_load_swizzle_textures() const
{
  if (ImGui::MenuItem("Load Swizzled Textures", nullptr, false, true))
  {
    m_directory_browser.Open();
    m_directory_browser.SetTitle("Choose directory to load textures from");
    m_directory_browser.SetTypeFilters({ ".map", ".png" });
    m_modified_directory_map = map_directory_mode::load_swizzle_textures;
  }
}
void
  gui::menuitem_load_deswizzle_textures() const
{
  if (ImGui::MenuItem("Load Deswizzled Textures", nullptr, false, true))
  {
    m_directory_browser.Open();
    m_directory_browser.SetTitle("Choose directory to load textures from");
    m_directory_browser.SetTypeFilters({ ".map", ".png" });
    m_modified_directory_map = map_directory_mode::load_deswizzle_textures;
  }
}
void
  gui::menuitem_save_texture(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Save Displayed Texture", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Texture as...");
    m_save_file_browser.SetTypeFilters({ ".png", ".ppm" });
    m_save_file_browser.SetInputName(path.c_str());
  }
}
void
  gui::menuitem_save_mim_file(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Save Mim File", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Mim as...");
    m_save_file_browser.SetTypeFilters({ Mim::EXT.data() });
    m_save_file_browser.SetInputName(path);
  }
}
void
  gui::menuitem_save_map_file(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Save Map File (unmodified)", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Map as...");
    m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
    m_save_file_browser.SetInputName(path);
    m_modified_map = map_dialog_mode::save_unmodified;
  }
}
void
  gui::menuitem_save_map_file_modified(const std::string &path, bool enabled)
    const
{
  if (ImGui::MenuItem("Save Map File (modified)", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Map as...");
    m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
    m_save_file_browser.SetInputName(path);
    m_modified_map = map_dialog_mode::save_modified;
  }
}
void
  gui::menuitem_load_map_file(const std::string &path, bool enabled) const
{
  if (ImGui::MenuItem("Load Map File", nullptr, false, enabled))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Load Map...");
    m_save_file_browser.SetTypeFilters({ Map::EXT.data() });
    m_save_file_browser.SetInputName(path);
    m_modified_map = map_dialog_mode::load;
  }
}
void
  gui::combo_draw() const
{
  if (generic_combo(
        m_id,
        gui_labels::draw,
        []()
        {
          return std::views::iota(
            0, static_cast<int>(std::size(m_draw_selections)));
          ;
        },
        [this]() { return m_draw_selections; },
        m_selections.draw))
  {
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
void
  gui::combo_path() const
{
  ImGui::PushID(++m_id);
  if (generic_combo(
        m_id,
        gui_labels::path,
        [this]()
        { return std::views::iota(0, static_cast<int>(std::size(m_paths))); },
        [this]() { return m_paths; },
        m_selections.path))
  {
    update_path();
  }
  ImGui::PopID();
}
std::vector<std::string>
  gui::get_paths()
{
  std::vector<std::string> paths{};
  open_viii::Paths::for_each_path([&paths](const std::filesystem::path &p)
                                  { paths.emplace_back(p.string()); });
  return paths;
}
void
  gui::loop_events() const
{
  m_mouse_positions.update();
  while (m_window.pollEvent(m_event))
  {
    ImGui::SFML::ProcessEvent(m_event);
    const auto  event_variant = events::get(m_event);
    const auto &type          = m_event.type;
    std::visit(
      events::make_visitor(
        [this](const sf::Event::SizeEvent &size)
        {
          scale_window(
            static_cast<float>(size.width), static_cast<float>(size.height));
          m_changed = true;
        },
        [this](const sf::Event::MouseMoveEvent &)
        {
          m_mouse_positions.mouse_moved = true;
          // TODO move setting mouse pos code here?
          // m_changed = true;
        },
        [this, type](const sf::Event::KeyEvent &key)
        {
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
        [this, &type](const sf::Event::MouseButtonEvent &mouse)
        {
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
              case sf::Mouse::Button::Left:
              {
                m_mouse_positions.left = true;
                std::cout << "Left Mouse Button Down" << std::endl;
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
              case sf::Mouse::Button::Left:
              {
                m_mouse_positions.left = false;
                std::cout << "Left Mouse Button Up" << std::endl;
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
        [this]([[maybe_unused]] const std::monostate &)
        {
          if (m_event.type == sf::Event::Closed)
          {
            m_window.close();
          }
        },
        []([[maybe_unused]] const auto &) {}),
      event_variant);
  }
}
void
  gui::scale_window(float width, float height) const
{
  static auto save_width  = float{};
  static auto save_height = float{};
  float       img_height  = [this]()
  {
    if (map_test())
    {
      return static_cast<float>(m_map_sprite.height());
    }
    return static_cast<float>(m_mim_sprite.height());
  }();
  auto load = [](auto &saved, auto &not_saved)
  {
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
archives_group
  gui::get_archives_group() const
{
  if (!m_paths.empty())
  {
    return { open_viii::LangCommon::to_array().front(), m_paths.front() };
  }
  return {};
}
sf::RenderWindow
  gui::get_render_window() const
{
  return sf::RenderWindow{ sf::VideoMode(m_window_width, m_window_height),
                           sf::String{ gui_labels::window_title } };
}
void
  gui::update_path() const
{
  m_archives_group = m_archives_group.with_path(
    m_paths.at(static_cast<std::size_t>(m_selections.path)));
  update_field();
}
std::vector<const char *>
  gui::get_paths_c_str() const
{
  return archives_group::get_c_str(m_paths);
}
mim_sprite
  gui::get_mim_sprite() const
{
  return { m_field,
           Mim::bpp_selections().at(static_cast<std::size_t>(m_selections.bpp)),
           static_cast<std::uint8_t>(Mim::palette_selections().at(
             static_cast<std::size_t>(m_selections.palette))),
           get_coo(),
           m_selections.draw_palette };
}
void
  gui::init_and_get_style() const
{
  //  static constexpr auto fps_lock = 360U;
  //  m_window.setFramerateLimit(fps_lock);
  m_window.setVerticalSyncEnabled(true);
  ImGui::SFML::Init(m_window);
}
gui::gui(std::uint32_t width, std::uint32_t height)
  : m_window_width(width)
  , m_window_height(height)
  , m_window(get_render_window())
  , m_paths(get_paths())
  , m_paths_c_str(get_paths_c_str())
  , m_archives_group(get_archives_group())
  , m_field(init_field())
  , m_mim_sprite(get_mim_sprite())
  , m_map_sprite(get_map_sprite())

{
  init_and_get_style();
}
std::shared_ptr<open_viii::archive::FIFLFS<false>>
  gui::init_field()
{
  m_selections.field = get_selected_field();
  return m_archives_group.field(m_selections.field);
}
gui::gui()
  : gui(default_window_width, default_window_height)
{
}
map_sprite
  gui::get_map_sprite() const
{
  return { m_field, get_coo(), m_selections.draw_swizzle, {} };
}
int
  gui::get_selected_field()
{
  if (const int field = m_archives_group.find_field(starter_field());
      field != -1)
  {
    return field;
  }
  return 0;
}
std::string_view
  gui::starter_field() const
{
  return "crtower3";
}


void
  gui::combo_pupu() const
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
void
  gui::combo_filtered_palettes() const
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
      m_changed            = true;
    }
  }
}

void
  gui::combo_filtered_bpps() const
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
    m_changed = true;
  }
}

void
  gui::combo_blend_modes() const
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

void
  gui::combo_layers() const
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
void
  gui::combo_texture_pages() const
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
void
  gui::combo_animation_ids() const
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
void
  gui::combo_blend_other() const
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
void
  gui::combo_compact_type(::filter<compact_type> &compact) const
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

void
  gui::combo_z() const
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

void
  gui::combo_animation_frames() const
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
BPPT
  gui::bpp() const
{
  return Mim::bpp_selections().at(static_cast<size_t>(m_selections.bpp));
}
void
  gui::combo_deswizzle_path() const
{
  if (!std::filesystem::exists(m_loaded_deswizzle_texture_path) || !m_field)
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
    m_map_sprite.update_render_texture(true);
    m_changed = true;
  }
}
void
  gui::combo_upscale_path() const
{
  if (!m_field)
    return;
  if (combo_upscale_path(
        m_map_sprite.filter().upscale, m_field->get_base_name(), get_coo()))
  {
    m_map_sprite.update_render_texture(true);
    m_changed = true;
  }
}
bool
  gui::combo_upscale_path(
    ::filter<std::filesystem::path> &filter,
    const std::string               &field_name,
    open_viii::LangT                 coo) const
{
  std::vector<std::string> paths = {};
  auto                     transform_paths =
    m_paths
    | std::views::transform(
      [this, &field_name, &coo](const std::string &path)
      {
        if (m_field)
          return upscales(std::filesystem::path(path), field_name, coo)
            .get_paths();
        return upscales{}.get_paths();
      });
  // std::views::join; broken in msvc.
  auto process = [&paths](const auto &temp_paths)
  {
    auto filter_paths = temp_paths
                        | std::views::filter(
                          [](const std::filesystem::path &path)
                          {
                            return std::filesystem::exists(path)
                                   && std::filesystem::is_directory(path);
                          });
    for (auto &path : filter_paths)
    {
      paths.emplace_back(path.string());
    }
  };
  for (auto temp_paths : transform_paths)
  {
    process(temp_paths);
  }
  if (std::filesystem::exists(m_loaded_swizzle_texture_path))
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
          [ this, &filter ]() -> auto & { return filter; }))
    {
      return true;
    }
  }
  return false;
}
void
  gui::batch_deswizzle::enable(std::filesystem::path in_outgoing)
{
  enabled  = true;
  pos      = 0;
  outgoing = std::move(in_outgoing);
  asked    = false;
  start    = std::chrono::high_resolution_clock::now();
}
void
  gui::batch_deswizzle::disable()
{
  enabled = false;
}
template<typename lambdaT, typename askT>
bool
  gui::batch_deswizzle::operator()(
    const std::vector<std::string> &fields,
    lambdaT                       &&lambda,
    askT                          &&ask_lambda)
{
  if (!enabled)
  {
    return false;
  }
  ImGui::SetNextWindowPos(
    ImGui::GetMainViewport()->GetCenter(),
    ImGuiCond_Always,
    ImVec2(0.5F, 0.5F));
  ImGui::OpenPopup(gui_labels::batch_deswizzle_title.data());
  if (ImGui::BeginPopupModal(
        gui_labels::batch_deswizzle_title.data(),
        nullptr,
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
  {
    if (fields.size() <= pos)
    {
      auto current = std::chrono::high_resolution_clock::now();
      fmt::print(
        "{:%H:%M:%S} - {}\n",
        current - start,
        gui_labels::batch_deswizzle_finish);
      disable();
      return pos > 0U;
    }
    if (!asked)
    {
      asked = ask_lambda(filters.upscale);
    }
    else
    {
      auto current = std::chrono::high_resolution_clock::now();
      format_imgui_text(
        "{:%H:%M:%S} - {:>3.2f}% - {} {}...",
        current - start,
        static_cast<float>(pos) * 100.F / static_cast<float>(std::size(fields)),
        gui_labels::processing,
        fields[pos]);
      ImGui::Separator();
      lambda(static_cast<int>(pos), outgoing, filters);
      ++pos;
    }
    ImGui::EndPopup();
  }
  return false;
}
