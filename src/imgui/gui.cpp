//
// Created by pcvii on 9/7/2021.
//
#include "gui.hpp"
#include "open_viii/paths/Paths.hpp"
#include <imgui-SFML.h>
#include <imgui.h>

void gui::start() const
{
  if (m_window.isOpen()) {
    scale_window(
      static_cast<float>(m_window_width), static_cast<float>(m_window_height));
    do {
      ImGui::SFML::Update(m_window, m_delta_clock.restart());
      m_changed = false;
      loop_events();
      loop();
    } while (m_window.isOpen());
    ImGui::SFML::Shutdown();
  }
}
void gui::loop() const
{
  if (ImGui::Begin("Control Panel",
        nullptr,
        static_cast<ImGuiWindowFlags>(
          static_cast<uint32_t>(ImGuiWindowFlags_AlwaysAutoResize)
          | static_cast<uint32_t>(ImGuiWindowFlags_MenuBar)))) {
    if (m_first) {
      ImGui::SetWindowPos({ 0U, 0U });
    }
    menu_bar();
    file_browser_locate_ff8();
    combo_draw();
    if (!m_paths.empty()) {
      file_browser_save_texture();
      combo_path();
      combo_coo();
      combo_field();

      if (!m_mim_sprite.fail()) {
        if (m_selected_draw == 0) {
          checkbox_mim_palette_texture();
          if (!m_mim_sprite.draw_palette()) {
            combo_mim_bpp();
            combo_mim_palette();
          }
          if (!m_mim_sprite.draw_palette()) {
            format_imgui_text("Width == Max Tiles");
          }
          if (m_changed) {
            scale_window();
          }
          slider_xy_sprite(m_mim_sprite);
        } else if (m_selected_draw == 1) {
          if (m_changed) {
            scale_window();
          }
          slider_xy_sprite(m_map_sprite);
        }
      }
    }
  }
  ImGui::End();

  m_window.clear();
  if (m_selected_draw == 0) {
    if (m_changed) {
      if (m_mim_sprite.draw_palette()) {
        m_grid =
          grid{ { 1U, 1U }, { m_mim_sprite.width(), m_mim_sprite.height() } };
      } else {
        m_grid =
          grid{ { 16U, 16U }, { m_mim_sprite.width(), m_mim_sprite.height() } };
      }
      m_grid.setPosition(m_mim_sprite.getPosition());
    }
    m_window.draw(m_mim_sprite);
  }
  if (m_selected_draw == 1) {
    if (m_changed) {
      m_grid =
        grid{ { 16U, 16U }, { m_map_sprite.width(), m_map_sprite.height() } };
      m_grid.setPosition(m_map_sprite.getPosition());
    }
    m_window.draw(m_map_sprite);
  }
  m_window.draw(m_grid);
  ImGui::SFML::Render(m_window);
  m_window.display();
  m_first = false;
}
void gui::combo_coo() const
{
  static constexpr auto coos       = open_viii::LangCommon::to_array();
  static constexpr auto coos_c_str = open_viii::LangCommon::to_c_str_array();
  if (ImGui::Combo("Language",
        &m_selected_coo,
        coos_c_str.data(),
        static_cast<int>(coos_c_str.size()),
        5)) {
    m_mim_sprite =
      m_mim_sprite.with_coo(coos.at(static_cast<size_t>(m_selected_coo)));
    m_map_sprite =
      m_map_sprite.with_coo(coos.at(static_cast<size_t>(m_selected_coo)));
    m_changed = true;
  }
}
void gui::combo_field() const
{
  if (ImGui::Combo("Field",
        &m_selected_field,
        m_archives_group.mapdata_c_str().data(),
        static_cast<int>(m_archives_group.mapdata_c_str().size()),
        20)) {
    update_field();
  }
}
void gui::update_field() const
{
  m_field      = m_archives_group.field(m_selected_field);
  m_mim_sprite = m_mim_sprite.with_field(m_field);
  m_map_sprite = m_map_sprite.with_field(m_field);
  m_changed    = true;
}
void gui::checkbox_mim_palette_texture() const
{
  if (ImGui::Checkbox("Draw Palette Texture", &draw_palette)) {
    m_mim_sprite = m_mim_sprite.with_draw_palette(draw_palette);
    m_changed    = true;
  }
}
void gui::combo_mim_bpp() const
{
  static constexpr std::array bpp_items =
    open_viii::graphics::background::Mim::bpp_selections_c_str();
  if (ImGui::Combo("BPP",
        &m_selected_bpp,
        bpp_items.data(),
        static_cast<int>(bpp_items.size()),
        static_cast<int>(bpp_items.size()))) {
    m_mim_sprite = m_mim_sprite.with_bpp(
      open_viii::graphics::background::Mim::bpp_selections().at(
        static_cast<size_t>(m_selected_bpp)));
    m_changed = true;
  }
}
void gui::combo_mim_palette() const
{
  if (m_selected_bpp != 2) {
    static constexpr std::array palette_items =
      open_viii::graphics::background::Mim::palette_selections_c_str();
    if (ImGui::Combo("Palette",
          &m_selected_palette,
          palette_items.data(),
          static_cast<int>(palette_items.size()),
          static_cast<int>(palette_items.size()))) {
      m_mim_sprite = m_mim_sprite.with_palette(static_cast<uint8_t>(
        open_viii::graphics::background::Mim::palette_selections().at(
          static_cast<size_t>(m_selected_palette))));
      m_changed    = true;
    }
  }
}
template<typename T>
void gui::slider_xy_sprite(T &sprite) const
{
  format_imgui_text(
    "X: {:>9.3f} px  Width:  {:>4} px", sprite.getPosition().x, sprite.width());
  format_imgui_text("Y: {:>9.3f} px  Height: {:>4} px",
    sprite.getPosition().y,
    sprite.height());
  if (ImGui::SliderFloat2("Adjust", xy.data(), -1.0F, 0.0F) || m_changed) {
    sprite.setPosition(
      xy[0] * (static_cast<float>(sprite.width()) - m_scale_width),
      xy[1] * static_cast<float>(sprite.height()));
    m_changed = true;
  }
}
void gui::menu_bar() const
{
  if (ImGui::BeginMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      menuitem_locate_ff8();
      menuitem_save_texture(save_texture_path(), !m_mim_sprite.fail());
      ImGui::EndMenu();
    }
    ImGui::EndMenuBar();
  }
}
std::string gui::save_texture_path() const
{
  if (m_archives_group.mapdata().empty()) {
    return {};
  }
  const std::string &field_name =
    m_archives_group.mapdata().at(static_cast<size_t>(m_selected_field));
  if (m_selected_draw == 0)// MIM
  {
    if (m_mim_sprite.draw_palette()) {
      return fmt::format("{}_mim_palettes.png", field_name);
    }
    const int bpp = static_cast<int>(
      open_viii::graphics::background::Mim::bpp_selections().at(
        static_cast<size_t>(m_selected_bpp)));
    return fmt::format(
      "{}_mim_{}bpp_{}.png", field_name, bpp, m_selected_palette);
  }
  return fmt::format("{}_map.png", field_name);
}
void gui::file_browser_locate_ff8() const
{
  m_directory_browser.Display();
  if (m_directory_browser.HasSelected()) {
    const auto selected_path = m_directory_browser.GetSelected();
    m_paths.emplace_back(selected_path.string());
    m_paths_c_str = archives_group::get_c_str(
      m_paths);// seems the pointers move when you push back above
    m_selected_path = static_cast<int>(m_paths.size()) - 1;
    update_path();
    m_directory_browser.ClearSelected();
  }
}
void gui::file_browser_save_texture() const
{
  m_save_file_browser.Display();
  if (m_save_file_browser.HasSelected()) {
    [[maybe_unused]] const auto selected_path =
      m_save_file_browser.GetSelected();
    if (m_selected_draw == 0) {
      m_mim_sprite.save(selected_path);
    } else {
      m_map_sprite.save(selected_path);
    }
    m_save_file_browser.ClearSelected();
  }
}
void gui::menuitem_locate_ff8() const
{
  if (ImGui::MenuItem("Locate a FF8 install")) {
    m_directory_browser.Open();
    m_directory_browser.SetTitle("Choose FF8 install directory");
    m_directory_browser.SetTypeFilters({ ".exe" });
  }
}
void gui::menuitem_save_texture(const std::string &path, bool disable) const
{
  if (ImGui::MenuItem("Save Displayed Texture", nullptr, false, disable)) {
    m_save_file_browser.Open();
    m_save_file_browser.SetTitle("Save Texture as...");
    m_save_file_browser.SetTypeFilters({ ".png", ".ppm" });
    m_save_file_browser.SetInputName(path.c_str());
  }
}
void gui::combo_draw() const
{
  if (ImGui::Combo("Draw",
        &m_selected_draw,
        m_draw_selections.data(),
        static_cast<int>(m_draw_selections.size()))) {
    m_changed = true;
  }
}
void gui::combo_path() const
{
  if (ImGui::Combo("Path",
        &m_selected_path,
        m_paths_c_str.data(),
        static_cast<int>(m_paths_c_str.size()),
        10)) {
    update_path();
  }
}
std::vector<std::string> gui::get_paths()
{
  std::vector<std::string> paths{};
  open_viii::Paths::for_each_path([&paths](const std::filesystem::path &p) {
    paths.emplace_back(p.string());
  });
  return paths;
}
void gui::loop_events() const
{
  while (m_window.pollEvent(m_event)) {
    ImGui::SFML::ProcessEvent(m_event);
    const auto event_variant = events::get(m_event);
    std::visit(events::make_visitor(
                 [this](const sf::Event::SizeEvent &size) {
                   scale_window(static_cast<float>(size.width),
                     static_cast<float>(size.height));
                   m_changed = true;
                 },
                 [this]([[maybe_unused]] const std::monostate &) {
                   if (m_event.type == sf::Event::Closed) {
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
    if (m_selected_draw == 1) {
      return static_cast<float>(m_map_sprite.height());
    }
    return static_cast<float>(m_mim_sprite.height());
  }();
  auto load = [](auto &saved, auto &not_saved) {
    if (not_saved < std::numeric_limits<float>::epsilon()) {
      not_saved = saved;
    } else {
      saved = not_saved;
    }
  };
  load(save_width, width);
  load(save_height, height);
  // this scales up the elements without losing the horizontal space. so
  // going from 4:3 to 16:9 will end up with wide screen.
  auto scale    = height / static_cast<float>(m_window_height);
  m_scale_width = width / height * img_height;
  if (scale < 1.0F) {
    scale = 1.0F;
  }
  ImGui::GetIO().FontGlobalScale = std::round(scale);
  ImGui::GetStyle() =
    m_original_style;// restore original before applying scale.
  ImGui::GetStyle().ScaleAllSizes(std::round(scale));
  m_window.setView(
    sf::View(sf::FloatRect(0.0F, 0.0F, m_scale_width, img_height)));
}
archives_group gui::get_archives_group() const
{
  if (!m_paths.empty()) {
    return { open_viii::LangCommon::to_array().front(), m_paths.front() };
  }
  return {};
}
sf::RenderWindow gui::get_render_window() const
{
  return { sf::VideoMode(m_window_width, m_window_height), m_title };
}
void gui::update_path() const
{
  m_archives_group = m_archives_group.with_path(
    m_paths.at(static_cast<std::size_t>(m_selected_path)));
  update_field();
}
std::vector<const char *> gui::get_paths_c_str() const
{
  return archives_group::get_c_str(m_paths);
}
mim_sprite gui::get_mim_sprite() const
{
  return { m_field,
    open_viii::graphics::background::Mim::bpp_selections().front(),
    static_cast<std::uint8_t>(
      open_viii::graphics::background::Mim::palette_selections().front()),
    open_viii::LangCommon::to_array().front() };
}
ImGuiStyle gui::init_and_get_style() const
{
  static constexpr auto fps_lock = 360U;
  m_window.setFramerateLimit(fps_lock);
  ImGui::SFML::Init(m_window);
  return ImGui::GetStyle();
}
gui::gui(std::uint32_t               width,
  std::uint32_t                      height,
  [[maybe_unused]] const char *const title)
  : m_window_width(width), m_window_height(height), m_title(title),
    m_window(get_render_window()), m_paths(get_paths()),
    m_paths_c_str(get_paths_c_str()), m_archives_group(get_archives_group()),
    m_field(m_archives_group.field(m_selected_field)),
    m_mim_sprite(get_mim_sprite()), m_map_sprite(get_map_sprite()),
    m_original_style(init_and_get_style())
{}
gui::gui(const char *title)
  : gui(default_window_width, default_window_height, title)
{}
gui::gui() : gui("") {}
map_sprite gui::get_map_sprite() const
{
  return { m_field,
    open_viii::LangCommon::to_array().at(
      static_cast<std::size_t>(m_selected_coo)) };
}
