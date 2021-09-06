#include "archives_group.hpp"
#include "dialog.hpp"
#include "filebrowser.hpp"
#include "mim_sprite.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include "imgui_format_text.hpp"
#include <utility>

void Game();
int  main() { Game(); }
void Game()
{
  // create a file browser instance
  ImGui::FileBrowser fileDialog{ ImGuiFileBrowserFlags_SelectDirectory };
  fileDialog.SetTypeFilters({ ".fs", ".fi", ".fl", ".zzz" });
  using namespace open_viii::graphics::literals;
  std::vector<std::string> paths{};
  open_viii::Paths::for_each_path([&paths](const std::filesystem::path &p) {
    paths.emplace_back(p.string());
  });
  if (paths.empty()) {
    return;
  }
  std::vector<const char *>
    paths_c_str{};// imgui doesn't support std::string or std::string_view or
                  // std::filesystem::path, only const char *
  paths_c_str.reserve(paths.size());
  std::ranges::transform(paths,
    std::back_inserter(paths_c_str),
    [](const std::string &p) { return p.c_str(); });

  static constexpr auto coos         = open_viii::LangCommon::to_array();
  static constexpr auto coos_c_str   = open_viii::LangCommon::to_c_str_array();
  auto           opt_archives = archives_group(coos.front(), paths.front());
  int        current_map = 0;
  auto                  field         = opt_archives.field(current_map);
  auto                  ms            = mim_sprite(field, 4_bpp, 0, {});
  static constexpr auto window_width  = 800;
  static constexpr auto window_height = 600;
  sf::RenderWindow      window(
         sf::VideoMode(window_width, window_height), "ImGui + SFML = <3");
  window.setFramerateLimit(360U);
  ImGui::SFML::Init(window);

  sf::CircleShape shape(100.0F);
  shape.setFillColor(sf::Color::Green);
  sf::Clock  deltaClock;
  const auto original_style = ImGui::GetStyle();
  bool       first          = true;
  auto       scale_window   = [&ms,
                        &original_style,
                        &window,
                        save_width  = float{},
                        save_height = float{}](
                        float width = 0, float height = 0) mutable {
    auto load = [](auto &saved, auto &not_saved) {
      if (not_saved == 0) {
        not_saved = saved;
      } else {
        saved = not_saved;
      }
    };
    load(save_width, width);
    load(save_height, height);
    // this scales up the elements without losing the horizontal space. so
    // going from 4:3 to 16:9 will end up with wide screen.
    auto       scale       = width / window_height;
    const auto scale_width = width / height * static_cast<float>(ms.height());
    if (scale < 1.0F) {
      scale = 1.0F;
    }
    ImGui::GetIO().FontGlobalScale = std::round(scale);
    ImGui::GetStyle() =
      original_style;// restore original before applying scale.
    ImGui::GetStyle().ScaleAllSizes(std::round(scale));
    window.setView(sf::View(
      sf::FloatRect(0.0F, 0.0F, scale_width, static_cast<float>(ms.height()))));
  };
  if (window.isOpen()) {
    scale_window(
      static_cast<float>(window_width), static_cast<float>(window_height));
  }
  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      if (event.type == sf::Event::Resized) {
        scale_window(static_cast<float>(event.size.width),
          static_cast<float>(event.size.height));
      } else if (event.type == sf::Event::Closed) {
        window.close();
      }
    }
    const auto view = window.getView();
    // const auto view_port = window.getViewport(view);
    ImGui::SFML::Update(window, deltaClock.restart());
    static constexpr float f2           = 2.0F;
    const auto             shape_bounds = shape.getLocalBounds();
    shape.setOrigin(shape_bounds.left + shape_bounds.width / f2,
      shape_bounds.top + shape_bounds.height / f2);
    shape.setPosition(view.getCenter());

    const static auto hello_world = dialog(
      "Hello, world!",
      ImVec2{ 0.0F, 0.0F },
      [&ms,
        xy                    = std::array<float, 2>{},
        bpp_selected_item     = int{},
        palette_selected_item = int{},
        coo_selected_item     = int{},
        path_selected_item    = int{},
        draw_palette          = false,
        &current_map,
        &scale_window,
        &paths_c_str,
        &field,
        &paths,
        &opt_archives,
        &fileDialog]() mutable {
        const auto get_bpp = [&bpp_selected_item]() {
          static constexpr std::array bpp = mim_sprite::bpp_selections();
          return bpp.at(bpp_selected_item);
        };
        bool changed = false;
        if (ImGui::Combo("Path",
              &path_selected_item,
              paths_c_str.data(),
              static_cast<int>(paths_c_str.size()),
              10)) {
          opt_archives = opt_archives.with_path(paths.at(path_selected_item));
          field        = opt_archives.field(current_map);
          ms           = ms.with_field(field);
          changed      = true;
        }
        // open file dialog when user clicks this button
        if (ImGui::Button("open file dialog")) fileDialog.Open();

        fileDialog.Display();
        if (ImGui::Combo("Language",
              &coo_selected_item,
              coos_c_str.data(),
              static_cast<int>(coos_c_str.size()),
              5)) {

          //          field   = set_field();
          ms      = ms.with_coo(coos.at(coo_selected_item));
          changed = true;
        }
        if (ImGui::Combo("Field",
              &current_map,
              opt_archives.mapdata_c_str().data(),
              static_cast<int>(opt_archives.mapdata_c_str().size()),
              10)) {

          field   = opt_archives.field(current_map);
          ms      = ms.with_field(field);
          changed = true;
        }
        changed =
          mim_sprite::ImGui_controls(ms, bpp_selected_item, palette_selected_item, draw_palette,xy) || changed;
        if (changed) {
          scale_window();
        }
      },
      static_cast<ImGuiWindowFlags>(ImGuiWindowFlags_AlwaysAutoResize));

    hello_world.draw(first);

    window.clear();
    window.draw(ms.sprite());
    // window.draw(shape);
    ImGui::SFML::Render(window);
    window.display();
    first = false;
  }

  ImGui::SFML::Shutdown();
}
