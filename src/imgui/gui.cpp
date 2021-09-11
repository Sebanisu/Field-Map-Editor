//
// Created by pcvii on 9/7/2021.
//
#include "gui.hpp"
#include "dialog.hpp"
#include "open_viii/graphics/background/Map.hpp"
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
  const static auto hello_world = dialog(
    "Hello, world!",
    ImVec2{ 0.0F, 0.0F },
    [this]() mutable {
      if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
          if (ImGui::MenuItem("Locate a FF8 install")) {
            m_directory_browser.Open();
            m_directory_browser.SetTitle("Choose FF8 install directory");
            m_directory_browser.SetTypeFilters({ ".exe" });
          }
          if (ImGui::MenuItem("Save Displayed Texture",
                nullptr,
                false,
                !m_mim_sprite.fail())) {
            m_save_file_browser.Open();
            m_save_file_browser.SetTitle("Save Texture as...");
            m_save_file_browser.SetTypeFilters({ ".png", ".ppm" });
            m_save_file_browser.SetInputName("TEST.png");
          }
          ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
      }
      if (ImGui::Combo("Path",
            &m_selected_path,
            m_paths_c_str.data(),
            static_cast<int>(m_paths_c_str.size()),
            10)) {
        update_path();
      }
      m_save_file_browser.Display();
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
      if (m_save_file_browser.HasSelected()) {
        [[maybe_unused]] const auto selected_path =
          m_save_file_browser.GetSelected();
        m_mim_sprite.save(selected_path);
        m_save_file_browser.ClearSelected();
      }
      m_changed = archives_group::ImGui_controls(m_archives_group,
                    m_field,
                    m_mim_sprite,
                    m_map_sprite,
                    m_selected_field,
                    m_selected_coo)
                  || m_changed;
      m_changed = mim_sprite::ImGui_controls(m_changed,
        m_mim_sprite,
        m_selected_bpp,
        m_selected_palette,
        draw_palette,
        xy,
        m_scale_width);
      if (m_changed) {
        scale_window();
      }
    },
    static_cast<ImGuiWindowFlags>(
      static_cast<uint32_t>(ImGuiWindowFlags_AlwaysAutoResize)
      | static_cast<uint32_t>(ImGuiWindowFlags_MenuBar)));
  hello_world.draw(m_first);

  m_window.clear();
  m_window.draw(m_mim_sprite.sprite());
  ImGui::SFML::Render(m_window);
  m_window.display();
  m_first = false;
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
  auto        load        = [](auto &saved, auto &not_saved) {
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
  auto scale    = width / static_cast<float>(m_window_height);
  m_scale_width = width / height * static_cast<float>(m_mim_sprite.height());
  if (scale < 1.0F) {
    scale = 1.0F;
  }
  ImGui::GetIO().FontGlobalScale = std::round(scale);
  ImGui::GetStyle() =
    m_original_style;// restore original before applying scale.
  ImGui::GetStyle().ScaleAllSizes(std::round(scale));
  m_window.setView(sf::View(sf::FloatRect(
    0.0F, 0.0F, m_scale_width, static_cast<float>(m_mim_sprite.height()))));
}
archives_group gui::get_archives_group() const
{
  return { open_viii::LangCommon::to_array().front(), m_paths.front() };
}
sf::RenderWindow gui::get_render_window() const
{
  return { sf::VideoMode(m_window_width, m_window_height), m_title };
}
void gui::update_path() const
{
  m_archives_group = m_archives_group.with_path(m_paths.at(static_cast<std::size_t>(m_selected_path)));
  m_field          = m_archives_group.field(m_selected_field);
  m_mim_sprite     = m_mim_sprite.with_field(m_field);
  m_changed        = true;
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
