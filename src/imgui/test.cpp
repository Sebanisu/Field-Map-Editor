#include "archives_group.hpp"
#include "dialog.hpp"
#include "events.hpp"
#include "filebrowser.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

void update_path(std::vector<std::string> &paths,
  int                                      current_map,
  int                                      path_selected_item,
  archives_group                          &opt_archives,
  open_viii::archive::FIFLFS<false>       &field,
  mim_sprite                              &ms,
  bool                                    &changed)
{
  opt_archives = opt_archives.with_path(paths.at(path_selected_item));
  field        = opt_archives.field(current_map);
  ms           = ms.with_field(field);
  changed      = true;
}

void Game()
{
  // create a file browser instance
  ImGui::FileBrowser file_dialog{ ImGuiFileBrowserFlags_SelectDirectory };
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

  auto                  opt_archives  = archives_group({}, paths.front());
  int                   current_map   = 0;
  auto                  field         = opt_archives.field(current_map);
  auto                  ms            = mim_sprite(field, 4_bpp, 0, {});
  static constexpr auto window_width  = 800;
  static constexpr auto window_height = 600;
  sf::RenderWindow      window(
         sf::VideoMode(window_width, window_height), "ImGui + SFML = <3");
  window.setFramerateLimit(360U);
  ImGui::SFML::Init(window);
  sf::Clock  deltaClock;
  const auto original_style = ImGui::GetStyle();
  bool       first          = true;
  float      scale_width    = {};
  auto       scale_window   = [&ms,
                        &original_style,
                        save_width  = float{},
                        save_height = float{},
                        &window,
                        &scale_width](
                        float width = {}, float height = {}) mutable {
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
    auto scale  = width / window_height;
    scale_width = width / height * static_cast<float>(ms.height());
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
  sf::Event event{};
  while (window.isOpen()) {
    bool changed = false;
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      const auto event_variant = events::get(event);
      std::visit(events::make_visitor(
                   [&scale_window, &changed](const sf::Event::SizeEvent &size) {
                     scale_window(static_cast<float>(size.width),
                       static_cast<float>(size.height));
                     changed = true;
                   },
                   [&event, &window](const auto) {
                     if (event.type == sf::Event::Closed) {
                       window.close();
                     }
                   }),
        event_variant);
    }
    const auto view = window.getView();
    ImGui::SFML::Update(window, deltaClock.restart());
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
        &file_dialog,
        &scale_width,
        &changed]() mutable {
        if (ImGui::BeginMenuBar()) {
          if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Locate a FF8 install")) {
              file_dialog.Open();
              file_dialog.SetTitle("Choose FF8 install directory");
              file_dialog.SetTypeFilters(
                { ".exe" });//".fs", ".fi", ".fl", ".zzz"
            }

            if (ImGui::MenuItem(
                  "Save Displayed Texture", nullptr, false, !ms.fail())) {
            }
            ImGui::EndMenu();
          }
          ImGui::EndMenuBar();
        }
        if (ImGui::Combo("Path",
              &path_selected_item,
              paths_c_str.data(),
              static_cast<int>(paths_c_str.size()),
              10)) {
          update_path(paths,
            current_map,
            path_selected_item,
            opt_archives,
            field,
            ms,
            changed);
        }
        //        // open file dialog when user clicks this button
        //        if (ImGui::Button("Locate a FF8 install")) {
        //          file_dialog.Open();
        //          file_dialog.SetTitle("Choose FF8 install directory");
        //          file_dialog.SetTypeFilters({ ".exe" });//".fs", ".fi",
        //          ".fl", ".zzz"
        //        }

        file_dialog.Display();
        if (file_dialog.HasSelected()) {
          const auto selected_path = file_dialog.GetSelected();
          paths.emplace_back(selected_path.string());
          paths_c_str = archives_group::get_c_str(
            paths);// seems the pointers move when you push back above

          path_selected_item = static_cast<int>(paths.size()) - 1;
          update_path(paths,
            current_map,
            path_selected_item,
            opt_archives,
            field,
            ms,
            changed);
          file_dialog.ClearSelected();
        }
        changed = archives_group::ImGui_controls(
                    opt_archives, field, ms, current_map, coo_selected_item)
                  || changed;
        changed = mim_sprite::ImGui_controls(changed,
          ms,
          bpp_selected_item,
          palette_selected_item,
          draw_palette,
          xy,
          scale_width);
        if (changed) {
          scale_window();
        }
      },
      static_cast<ImGuiWindowFlags>(
        ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_MenuBar));

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

int main() { Game(); }