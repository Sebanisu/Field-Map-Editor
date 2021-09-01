#include <fmt/core.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
template<typename T>
struct dialog
{
private:
  const char      *m_title;
  ImVec2           m_pos{};
  T                m_f{};
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
int main()
{
  const auto       window_width  = 800;
  const auto       window_height = 600;
  sf::RenderWindow window(
    sf::VideoMode(window_width, window_height), "ImGui + SFML = <3");
  window.setFramerateLimit(360);
  ImGui::SFML::Init(window);

  sf::CircleShape shape(100.0F);
  shape.setFillColor(sf::Color::Green);
  sf::Clock  deltaClock;
  auto       scale          = 1.0F;
  const auto original_style = ImGui::GetStyle();
  bool       first          = true;
  while (window.isOpen()) {
    sf::Event event{};
    while (window.pollEvent(event)) {
      ImGui::SFML::ProcessEvent(event);
      if (event.type == sf::Event::Resized) {
        // this scales up the elements without losing the horizontal space. so
        // going from 4:3 to 16:9 will end up with wide screen.
        scale =
          std::round(static_cast<float>(event.size.height) / window_height);
        const auto scale_width = static_cast<float>(event.size.width)
                                 / static_cast<float>(event.size.height)
                                 * window_height;
        if (scale < 1.0F) {
          scale = 1.0F;
        }
        ImGui::GetIO().FontGlobalScale = scale;
        ImGui::GetStyle() =
          original_style;// restore original before applying scale.
        ImGui::GetStyle().ScaleAllSizes(scale);
        window.setView(
          sf::View(sf::FloatRect(0.0F, 0.0F, scale_width, window_height)));
      } else if (event.type == sf::Event::Closed) {
        window.close();
      }
    }


    const auto view      = window.getView();
    const auto view_port = window.getViewport(view);
    ImGui::SFML::Update(window, deltaClock.restart());
    static constexpr float f2           = 2.0F;
    const auto             shape_bounds = shape.getLocalBounds();
    shape.setOrigin(shape_bounds.left + shape_bounds.width / f2,
      shape_bounds.top + shape_bounds.height / f2);
    shape.setPosition(view.getCenter());
    const static auto hello_world = dialog(
      "Hello, world!",
      ImVec2{ 0.0F, 0.0F },
      [&view_port]() {
        ImGui::Button(fmt::format(
          "Look at this pretty button {} {}", view_port.width, view_port.height)
                        .c_str());
      },
      static_cast<ImGuiWindowFlags>(ImGuiWindowFlags_AlwaysAutoResize));
    hello_world.draw(first);

    window.clear();
    window.draw(shape);
    ImGui::SFML::Render(window);
    window.display();
    first = false;
  }

  ImGui::SFML::Shutdown();
}
