//
// Created by pcvii on 9/7/2021.
//
#include "gui.hpp"
#include "open_viii/paths/Paths.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
using namespace open_viii::graphics::background;
using namespace open_viii::graphics;
using namespace open_viii::graphics::literals;
using namespace std::string_literals;
std::ostream &
  operator<<(std::ostream                             &os,
    const open_viii::graphics::background::BlendModeT &bm)
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

void
  gui::start() const
{
  if (m_window.isOpen())
  {
    scale_window(
      static_cast<float>(m_window_width), static_cast<float>(m_window_height));
    do
    {
      //      if(!map_test() && m_selected_draw == 1)
      //      {
      //        m_map_sprite = get_map_sprite();
      //      }
      m_changed = false;
      m_id      = {};
      loop_events();
      ImGui::SFML::Update(m_window, m_delta_clock.restart());
      loop();
    } while (m_window.isOpen());
    ImGui::SFML::Shutdown();
  }
}
void
  gui::loop() const
{
  static sf::Color clear_color = { 0, 0, 0, 255 };
  if (ImGui::Begin("Control Panel",
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
    static float clear_color_f[3]{};
    if (ImGui::ColorEdit3(
          "Background", clear_color_f, ImGuiColorEditFlags_DisplayRGB))
    {
      // changed color
      clear_color = { static_cast<sf::Uint8>(255U * clear_color_f[0]),
        static_cast<sf::Uint8>(255U * clear_color_f[1]),
        static_cast<sf::Uint8>(255U * clear_color_f[2]),
        255U };
    }
    combo_draw();
    if (!m_paths.empty())
    {
      file_browser_save_texture();
      combo_path();
      combo_coo();
      combo_field();
      const char *filter_title = "Filters";
      if (mim_test())
      {
        checkbox_mim_palette_texture();
        if (ImGui::CollapsingHeader(filter_title))
        {
          if (!m_mim_sprite.draw_palette())
          {
            combo_bpp();
            combo_palette();
          }
          if (!m_mim_sprite.draw_palette())
          {
            format_imgui_text("Width == Max Tiles");
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
        if (ImGui::CollapsingHeader(filter_title))
        {
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
      const auto    &mouse_pos = sf::Mouse::getPosition(m_window);
      const auto    &win_size  = m_window.getSize();
      constexpr auto in_bounds = [](auto i, auto low, auto high) {
        return std::cmp_greater_equal(i, low) && std::cmp_less_equal(i, high);
      };
      if (in_bounds(mouse_pos.x, 0, win_size.x)
          && in_bounds(mouse_pos.y, 0, win_size.y) && m_window.hasFocus())
      {
        const sf::Vector2i clamped_mouse_pos = {
          std::clamp(mouse_pos.x, 0, static_cast<int>(win_size.x)),
          std::clamp(mouse_pos.y, 0, static_cast<int>(win_size.y))
        };
        const auto pixel_pos = m_window.mapPixelToCoords(clamped_mouse_pos);

        if (((mim_test()
               && in_bounds(
                 static_cast<int>(pixel_pos.x), 0, m_mim_sprite.width())
               && in_bounds(
                 static_cast<int>(pixel_pos.y), 0, m_mim_sprite.height()))
              || (map_test()
                  && in_bounds(
                    static_cast<int>(pixel_pos.x), 0, m_map_sprite.width())
                  && in_bounds(
                    static_cast<int>(pixel_pos.y), 0, m_map_sprite.height())))
            && !ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
        {
          format_imgui_text("Mouse Pos: ({}, {})  Tile Pos: ({}, {})",
            static_cast<int>(pixel_pos.x),
            static_cast<int>(pixel_pos.y),
            static_cast<int>(pixel_pos.x) / 16,
            static_cast<int>(pixel_pos.y) / 16);
        }
      }
    }
  }
  ImGui::End();

  m_window.clear(clear_color);
  if (mim_test())
  {
    m_window.draw(
      m_mim_sprite.toggle_grids(m_draw_grid, m_draw_texture_page_grid));
  }
  else if (map_test())
  {
    m_window.draw(
      m_map_sprite.toggle_grid(m_draw_grid, m_draw_texture_page_grid));
  }
  ImGui::SFML::Render(m_window);
  m_window.display();
  m_first = false;
}
void
  gui::combo_coo() const
{
  ImGui::PushID(++m_id);
  static constexpr auto coos       = open_viii::LangCommon::to_array();
  static constexpr auto coos_c_str = open_viii::LangCommon::to_c_str_array();
  if (ImGui::Combo("Language",
        &m_selected_coo,
        coos_c_str.data(),
        static_cast<int>(coos_c_str.size()),
        5))
  {
    if (mim_test())
    {
      m_mim_sprite =
        m_mim_sprite.with_coo(coos.at(static_cast<size_t>(m_selected_coo)));
    }
    else if (map_test())
    {
      m_map_sprite =
        m_map_sprite.with_coo(coos.at(static_cast<size_t>(m_selected_coo)));
    }
    m_changed = true;
  }
  ImGui::PopID();
}
void
  gui::combo_field() const
{
  ImGui::PushID(++m_id);
  static constexpr auto items = 20;
  if (ImGui::Combo("Field",
        &m_selected_field,
        m_archives_group.mapdata_c_str().data(),
        static_cast<int>(m_archives_group.mapdata_c_str().size()),
        items))
  {
    update_field();
  }
  ImGui::PopID();
}

void
  gui::update_field() const
{
  m_field = m_archives_group.field(m_selected_field);
  if (m_selected_draw == 0)
  {
    m_mim_sprite = m_mim_sprite.with_field(m_field);
  }
  else if (m_selected_draw == 1)
  {
    m_map_sprite = m_map_sprite.with_field(m_field);
  }
  m_changed = true;
}

void
  gui::checkbox_map_swizzle() const
{
  if (ImGui::Checkbox("Swizzle", &m_map_swizzle))
  {
    if (m_map_swizzle)
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
  if (ImGui::Checkbox("Draw Palette Texture", &m_draw_palette))
  {
    m_mim_sprite = m_mim_sprite.with_draw_palette(m_draw_palette);
    m_changed    = true;
  }
}
static void
  update_bpp(mim_sprite &sprite, open_viii::graphics::BPPT bpp)
{
  sprite = sprite.with_bpp(bpp);
}
static void
  update_bpp(map_sprite &sprite, open_viii::graphics::BPPT bpp)
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
  static constexpr std::array bpp_items =
    open_viii::graphics::background::Mim::bpp_selections_c_str();
  if (ImGui::Combo("BPP",
        &m_selected_bpp,
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
    open_viii::graphics::background::Mim::palette_selections().at(
      static_cast<size_t>(m_selected_palette)));
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
  if (m_selected_bpp != 2)
  {
    static constexpr std::array palette_items =
      open_viii::graphics::background::Mim::palette_selections_c_str();
    if (ImGui::Combo("Palette",
          &m_selected_palette,
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
    "X: {:>9.3f} px  Width:  {:>4} px", m_cam_pos.x, sprite.width());
  format_imgui_text(
    "Y: {:>9.3f} px  Height: {:>4} px", m_cam_pos.y, sprite.height());
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
      menuitem_save_texture(save_texture_path(), mim_test() || map_test());
      menuitem_save_mim_file(m_mim_sprite.mim_filename(), mim_test());
      menuitem_save_map_file(m_map_sprite.map_filename(), map_test());
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Grid"))
    {
      ImGui::MenuItem("Draw Tile Grid", nullptr, &m_draw_grid);
      ImGui::MenuItem(
        "Draw Texture Page Grid", nullptr, &m_draw_texture_page_grid);
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}
bool
  gui::map_test() const
{
  return !m_map_sprite.fail() && m_selected_draw == 1;
}
bool
  gui::mim_test() const
{
  return !m_mim_sprite.fail() && m_selected_draw == 0;
}
std::string
  gui::save_texture_path() const
{
  if (m_archives_group.mapdata().empty())
  {
    return {};
  }
  const std::string &field_name =
    m_archives_group.mapdata().at(static_cast<size_t>(m_selected_field));
  if (mim_test())// MIM
  {
    if (m_mim_sprite.draw_palette())
    {
      return fmt::format("{}_mim_palettes.png", field_name);
    }
    const int bpp = static_cast<int>(
      open_viii::graphics::background::Mim::bpp_selections().at(
        static_cast<size_t>(m_selected_bpp)));
    return fmt::format(
      "{}_mim_{}bpp_{}.png", field_name, bpp, m_selected_palette);
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
    const auto selected_path = m_directory_browser.GetSelected();
    m_paths.emplace_back(selected_path.string());
    m_paths_c_str = archives_group::get_c_str(
      m_paths);// seems the pointers move when you push back above
    m_selected_path = static_cast<int>(m_paths.size()) - 1;
    update_path();
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
      if (open_viii::tools::i_ends_with(
            str_path, open_viii::graphics::background::Mim::EXT))
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
      if (open_viii::tools::i_ends_with(
            str_path, open_viii::graphics::background::Map::EXT))
      {
        m_map_sprite.map_save(selected_path);
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
  }
}
void
  gui::menuitem_save_texture(const std::string &path, bool disable) const
{
  if (ImGui::MenuItem("Save Displayed Texture", nullptr, false, disable))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Texture as...");
    m_save_file_browser.SetTypeFilters({ ".png", ".ppm" });
    m_save_file_browser.SetInputName(path.c_str());
  }
}
void
  gui::menuitem_save_mim_file(const std::string &path, bool disable) const
{
  if (ImGui::MenuItem("Save Mim File", nullptr, false, disable))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Mim as...");
    m_save_file_browser.SetTypeFilters(
      { open_viii::graphics::background::Mim::EXT.data() });
    m_save_file_browser.SetInputName(path);
  }
}
void
  gui::menuitem_save_map_file(const std::string &path, bool disable) const
{
  if (ImGui::MenuItem("Save Map File", nullptr, false, disable))
  {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Map as...");
    m_save_file_browser.SetTypeFilters(
      { open_viii::graphics::background::Map::EXT.data() });
    m_save_file_browser.SetInputName(path);
  }
}
void
  gui::combo_draw() const
{
  ImGui::PushID(++m_id);
  if (ImGui::Combo("Draw",
        &m_selected_draw,
        m_draw_selections.data(),
        static_cast<int>(m_draw_selections.size())))
  {
    if (m_selected_draw == 0)
    {
      m_mim_sprite = get_mim_sprite();
    }
    else if (m_selected_draw == 1)
    {
      m_map_sprite = m_map_sprite.update(m_field,
        open_viii::LangCommon::to_array().at(
          static_cast<std::size_t>(m_selected_coo)),
        m_map_swizzle);
    }
    m_changed = true;
  }
  ImGui::PopID();
}
void
  gui::combo_path() const
{
  ImGui::PushID(++m_id);
  if (ImGui::Combo("Path",
        &m_selected_path,
        m_paths_c_str.data(),
        static_cast<int>(m_paths_c_str.size()),
        10))
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
  while (m_window.pollEvent(m_event))
  {
    ImGui::SFML::ProcessEvent(m_event);
    const auto event_variant = events::get(m_event);
    std::visit(events::make_visitor(
                 [this](const sf::Event::SizeEvent &size)
                 {
                   scale_window(static_cast<float>(size.width),
                     static_cast<float>(size.height));
                   m_changed = true;
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
  auto scale    = height / static_cast<float>(m_window_height);
  m_scale_width = width / height * img_height;
  if (scale < 1.0F)
  {
    scale = 1.0F;
  }
  ImGui::GetIO().FontGlobalScale = std::round(scale);
  ImGui::GetStyle() =
    m_original_style;// restore original before applying scale.
  ImGui::GetStyle().ScaleAllSizes(std::round(scale));
  m_window.setView(sf::View(
    sf::FloatRect(m_cam_pos.x, m_cam_pos.y, m_scale_width, img_height)));
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
  return { sf::VideoMode(m_window_width, m_window_height), m_title };
}
void
  gui::update_path() const
{
  m_archives_group = m_archives_group.with_path(
    m_paths.at(static_cast<std::size_t>(m_selected_path)));
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
    open_viii::graphics::background::Mim::bpp_selections().at(
      static_cast<std::size_t>(m_selected_bpp)),
    static_cast<std::uint8_t>(
      open_viii::graphics::background::Mim::palette_selections().at(
        static_cast<std::size_t>(m_selected_palette))),
    open_viii::LangCommon::to_array().at(
      static_cast<std::size_t>(m_selected_coo)),
    m_draw_palette };
}
ImGuiStyle
  gui::init_and_get_style() const
{
  static constexpr auto fps_lock = 360U;
  m_window.setFramerateLimit(fps_lock);
  ImGui::SFML::Init(m_window);
  return ImGui::GetStyle();
}
gui::gui(std::uint32_t               width,
  std::uint32_t                      height,
  [[maybe_unused]] const char *const title)
  : m_window_width(width)
  , m_window_height(height)
  , m_title(title)
  , m_window(get_render_window())
  , m_paths(get_paths())
  , m_paths_c_str(get_paths_c_str())
  , m_archives_group(get_archives_group())
  , m_selected_field(get_selected_field())
  , m_field(m_archives_group.field(m_selected_field))
  , m_mim_sprite(get_mim_sprite())
  , m_map_sprite(get_map_sprite())
  , m_original_style(init_and_get_style())
{
}
gui::gui(const char *title)
  : gui(default_window_width, default_window_height, title)
{
}
gui::gui()
  : gui("")
{
}
map_sprite
  gui::get_map_sprite() const
{
  return { m_field,
    open_viii::LangCommon::to_array().at(
      static_cast<std::size_t>(m_selected_coo)),
    m_map_swizzle,
    {} };
}
int
  gui::get_selected_field()
{
  if (int field = m_archives_group.find_field("crtower3"); field != -1)
  {
    return field;
  }
  return 0;
}
static bool
  generic_combo(int &id,
    const char      *name,
    auto           &&value_lambda,
    auto           &&string_lambda,
    auto           &&filter_lambda)
{
  bool               changed     = false;
  static bool        checked     = false;
  const auto        &values      = value_lambda();
  const auto        &strings     = string_lambda();
  auto              &filter      = filter_lambda();
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
      checked = false;
      return true;
    }
    return false;
  }
  const char *current_item = strings[current_idx].c_str();

  ImGui::PushID(++id);
  if (ImGui::Checkbox("", &checked))
  {
    if (checked)
    {
      filter.enable();
      fmt::print("enable: \t{} \t{}\n", name, values[current_idx]);
    }
    else
    {
      filter.disable();
      fmt::print("disable: \t{} \t{}\n", name, values[current_idx]);
    }
    changed = true;
  }
  ImGui::PopID();
  ImGui::SameLine();
  ImGui::PushID(++id);
  const auto old = current_idx;
  if (ImGui::BeginCombo(name, current_item))
  // The second parameter is the label previewed
  // before opening the combo.
  {
    for (std::size_t n = 0; n != std::size(strings); ++n)
    {
      const bool  is_selected = (current_idx == n);
      // You can store your selection however you
      // want, outside or inside your objects
      // ImGui::PushID(++m_id);
      const char *v           = strings[n].c_str();
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
    }
    if (old != current_idx)
    {
      fmt::print("set: \t{} \t{}\n", name, values[current_idx]);
    }
    ImGui::EndCombo();
  }
  ImGui::PopID();
  changed =
    (filter.update(values[current_idx]).enabled() && (old != current_idx))
    || changed;
  return changed;
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
          "Palette",
          [&pair]() { return pair.values(); },
          [&pair]() { return pair.strings(); },
          [this]() -> auto & { return m_map_sprite.filter().palette; }))
    {
      m_map_sprite.update_render_texture();
      m_selected_palette = m_map_sprite.filter().palette.value();
      m_changed          = true;
    }
  }
}

void
  gui::combo_filtered_bpps() const
{
  const auto &pair = m_map_sprite.uniques().bpp();
  if (generic_combo(
        m_id,
        "BPP",
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().bpp; }))
  {
    m_map_sprite.update_render_texture();
    m_selected_bpp =
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
        "Blend Mode",
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
        "Layers",
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
        "Texture Page",
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
        "Animation ID",
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
        "Blend Other",
        [&pair]() { return pair.values(); },
        [&pair]() { return pair.strings(); },
        [this]() -> auto & { return m_map_sprite.filter().blend_other; }))
  {
    m_map_sprite.update_render_texture();
    m_changed = true;
  }
}

void
  gui::combo_z() const
{
  const auto &pair = m_map_sprite.uniques().z();
  if (generic_combo(
        m_id,
        "Z",
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
          "Animation Frame",
          [&pair]() { return pair.values(); },
          [&pair]() { return pair.strings(); },
          [this]() -> auto & { return m_map_sprite.filter().animation_frame; }))
    {
      m_map_sprite.update_render_texture();
      m_changed = true;
    }
  }
}
open_viii::graphics::BPPT
  gui::bpp() const
{
  return open_viii::graphics::background::Mim::bpp_selections().at(
    static_cast<size_t>(m_selected_bpp));
}
