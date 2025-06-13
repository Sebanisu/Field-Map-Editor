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
     {// m_window.setVerticalSyncEnabled(true);
          // m_window.setFramerateLimit(0);// Disable manual frame limit
          // m_window.setVerticalSyncEnabled(false);// Disable vsync
          window.setVerticalSyncEnabled(true);
          // Clear the window to black and display it immediately
          window.clear(sf::Color::Black);
          window.display();
          // m_window.requestFocus();// Ensure the window has focus
          window.setActive(true);
          const GLubyte *version = glGetString(GL_VERSION);
          if (version)
          {
               spdlog::info("OpenGL version: {}", reinterpret_cast<const char *>(version));
          }
          else
          {
               spdlog::error("Failed to get OpenGL version. Is the context initialized?");
          }

          GLenum const err = glewInit();
          if (std::cmp_not_equal(GLEW_OK, err))
          {
               // GLEW initialization failed
               const GLubyte *error_msg = glewGetErrorString(err);
               spdlog::error("{}", reinterpret_cast<const char *>(error_msg));
               std::terminate();
          }
          // Enable debug output
          glEnable(GL_DEBUG_OUTPUT);
          glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
          auto the_gui = fme::gui{ window };
          the_gui.start(window);
     }
     window.close();
}