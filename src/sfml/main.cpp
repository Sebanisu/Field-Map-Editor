#include "gui/gui.hpp"

static sf::RenderWindow get_render_window()
{
     const fme::Configuration config = {};
     const auto window_height        = config[key_to_string(fme::ConfigKey::WindowHeight)].value_or(fme::Selections::window_height_default);
     const auto window_width         = config[key_to_string(fme::ConfigKey::WindowWidth)].value_or(fme::Selections::window_width_default);
     return sf::RenderWindow{ sf::VideoMode(window_width, window_height), sf::String{ fme::gui_labels::window_title.data() } };
}
int main()
{
     auto window = get_render_window();
     if (!window.isOpen())
     {
          return 0;
     }
     {
          auto the_gui = fme::gui{ window };
          the_gui.start(window);
     }
     window.close();
}