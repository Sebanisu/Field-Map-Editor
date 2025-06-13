#include "gui/gui.hpp"

static sf::RenderWindow get_render_window()
{
     return sf::RenderWindow{ sf::VideoMode(800, 600), sf::String{ fme::gui_labels::window_title.data() } };
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