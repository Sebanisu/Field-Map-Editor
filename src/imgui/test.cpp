#include "mim_sprite.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/paths/Paths.hpp"
#include <fmt/core.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
template<typename T>
struct dialog
{
private:
  const char      *m_title;
  ImVec2           m_pos{};
  mutable T        m_f{};
  ImGuiWindowFlags m_flags{};

public:
  dialog(const char *const title,
    ImVec2                 pos,
    T                    &&f,
    ImGuiWindowFlags       flags = static_cast<ImGuiWindowFlags>(
      static_cast<unsigned int>(ImGuiWindowFlags_AlwaysAutoResize)
      | static_cast<unsigned int>(ImGuiWindowFlags_NoCollapse)))
    : m_title(title), m_pos(pos), m_f(std::forward<T>(f)), m_flags(flags)
  {}
  void draw(bool first = false) const
  {

    ImGui::Begin(m_title, nullptr, m_flags);
    // const auto current_size = ImGui::GetWindowSize();
    // auto size = ImVec2{ std::max(current_size.x, m_size.x),
    // std::max(std::round((current_size.y / current_size.x) * m_size.y),
    // m_size.y) }; ImGui::SetWindowSize(ImVec2{ m_size.x * scale, m_size.y *
    // scale });
    if (first) {
      ImGui::SetWindowPos(m_pos);
    }
    m_f();
    ImGui::End();
  }
};
void Game(const open_viii::archive::FIFLFS<true> &);
int  main()
{
  open_viii::Paths::for_each_path([](const std::filesystem::path &path) {
    std::cout << path << std::endl;
    static constexpr auto coo      = open_viii::LangT::en;
    const auto            archives = open_viii::archive::Archives(
                 path, open_viii::LangCommon::to_string<coo>());
    if (!static_cast<bool>(archives)) {
      std::cerr << "Failed to load path: " << path.string() << '\n';
      return;
    }
    const auto &fields =
      archives.get<open_viii::archive::ArchiveTypeT::field>();
    Game(fields);
  });
}
void Game(const open_viii::archive::FIFLFS<true> &fields)
{
  using namespace open_viii::graphics::literals;
  const auto map_data_string = fields.map_data();
  auto       map_data_c_str  = std::vector<const char *>{};
  map_data_c_str.reserve(map_data_string.size());
  std::ranges::transform(map_data_string,
    std::back_inserter(map_data_c_str),
    [](const std::string &str) { return str.c_str(); });
  int        current_map = 0;
  const auto set_field   = [&current_map, &fields, &map_data_string]() {
    open_viii::archive::FIFLFS<false> archive{};
    fields.execute_with_nested(
        { map_data_string.at(static_cast<std::size_t>(current_map)) },
        [&archive](
        auto &&field) { archive = std::forward<decltype(field)>(field); },
        {},
        true);
    return archive;
  };
  auto                  field         = set_field();
  auto                  ms            = mim_sprite(field, 4_bpp, 0);
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
  const auto scale_window   = [&ms, &original_style, &window](
                              const float width, const float height) {
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
    static const auto format_imgui_text =
      []<typename... T>(fmt::format_string<T...> fmt, T && ...items)
    {
      const auto data = fmt::format(std::forward<decltype(fmt)>(fmt),
        std::forward<decltype(items)>(items)...);
      ImGui::Text(
        "%s", data.c_str());// I hate this doesn't just take a std::string.
    };
    const static auto hello_world = dialog(
      "Hello, world!",
      ImVec2{ 0.0F, 0.0F },
      [&ms,
        xy                    = std::array<float, 2>{},
        bpp_selected_item     = int{},
        palette_selected_item = int{},
        &map_data_c_str,
        &current_map,
        &set_field,
        &field]() mutable {
        const auto get_bpp = [&bpp_selected_item]() {
          static constexpr std::array bpp = { 4_bpp, 8_bpp, 16_bpp };
          return bpp.at(bpp_selected_item);
        };
        bool changed = false;
        if (ImGui::Combo("Field",
              &current_map,
              map_data_c_str.data(),
              static_cast<int>(map_data_c_str.size()),
              10)) {

          field   = set_field();
          ms      = ms.with_field(field);
          changed = true;
        }
        static constexpr std::array bpp_items     = { "4", "8", "16" };
        static constexpr std::array palette_items = { "0",
          "1",
          "2",
          "3",
          "4",
          "5",
          "6",
          "7",
          "8",
          "9",
          "10",
          "11",
          "12",
          "13",
          "14",
          "15" };

        if (ImGui::Combo("BPP",
              &bpp_selected_item,
              bpp_items.data(),
              bpp_items.size(),
              3)) {
          ms      = ms.with_bpp(get_bpp());
          changed = true;
        }
        if (bpp_selected_item != 2) {
          if (ImGui::Combo("Palette",
                &palette_selected_item,
                palette_items.data(),
                palette_items.size(),
                10)) {
            ms =
              ms.with_palette(static_cast<std::uint8_t>(palette_selected_item));
            changed = true;
          }
        }
        format_imgui_text("X: {:>9.3f} px  Width:  {:>4} px",
          ms.sprite().getPosition().x,
          ms.width());
        format_imgui_text("Y: {:>9.3f} px  Height: {:>4} px",
          ms.sprite().getPosition().y,
          ms.height());
        format_imgui_text("Width == Max Tiles");
        if (ImGui::SliderFloat2("Adjust", xy.data(), -1.0, 0.0F) || changed) {
          ms.sprite().setPosition(xy[0] * static_cast<float>(ms.width()),
            xy[1] * static_cast<float>(ms.height()));
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
