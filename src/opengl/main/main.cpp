
// clang-format off
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "gui/gui.hpp"
#include <BlendModeSettings.hpp>
#include <spdlog/sinks/basic_file_sink.h>

#ifdef _WIN32
#include <windows.h>

int main(
  int    argc,
  char **argv);// your existing main

int WINAPI WinMain(
  [[maybe_unused]] HINSTANCE hInstance,
  [[maybe_unused]] HINSTANCE hPrevInstance,
  [[maybe_unused]] LPSTR     lpCmdLine,
  [[maybe_unused]] int       nCmdShow)
{
     // Convert lpCmdLine to argc/argv if needed, or just call main(0,nullptr)
     return main(__argc, __argv);
}
#endif

static void setWindowIcon(GLFWwindow *const window)
{
     GLFWimage             images[1];

     std::error_code       error_code = {};
     std::filesystem::path path       = std::filesystem::current_path(error_code) / "res" / "temporary_icon_05.png";
     const auto            image      = glengine::Image(path);
     if (error_code)
     {
          spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), path);
          return;
     }
     error_code.clear();
     if (const bool exists = std::filesystem::exists(path, error_code); !exists || error_code)
     {
          spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), path);
          return;
     }

     if (image.png_data)
     {
          images[0].width  = image.width;
          images[0].height = image.height;
          images[0].pixels = image.png_data.get();
          glfwSetWindowIcon(window, 1, images);
     }
     else
     {
          spdlog::error("Failed to load icon.png\n");
     }
}

static GLFWwindow *create_glfw_window()
{
     using namespace fme;
     const Configuration config = {};
     const int           window_height
       = config[SelectionInfo<ConfigKey::WindowHeight>::id].value_or(SelectionInfo<ConfigKey::WindowHeight>::default_value());
     const int window_width
       = config[SelectionInfo<ConfigKey::WindowWidth>::id].value_or(SelectionInfo<ConfigKey::WindowWidth>::default_value());

     if (!glfwInit())
     {
          spdlog::error("Failed to initialize GLFW");
          return nullptr;
     }

     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);// OpenGL 4.3
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
     glfwWindowHint(GLFW_DEPTH_BITS, 24);
     glfwWindowHint(GLFW_STENCIL_BITS, 8);
     glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

     GLFWwindow *window = glfwCreateWindow(window_width, window_height, gui_labels::window_title.data(), nullptr, nullptr);

     if (!window)
     {
          spdlog::error("Failed to create GLFW window");
          glfwTerminate();
          return nullptr;
     }

     glfwMakeContextCurrent(window);
     glfwSwapInterval(1);// Enable vsync
     return window;
}
int main(
  [[maybe_unused]] int    argc,
  [[maybe_unused]] char **argv)
{
     try
     {
          // Create file logger and set as default
          auto file_logger = spdlog::basic_logger_mt("file_logger", "res/field_map_editor.log", true);

          // Remove logger name from output pattern
          file_logger->set_pattern(R"([%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v)");

          spdlog::set_default_logger(file_logger);

          // Set log level based on build type
          // #ifndef NDEBUG
          spdlog::set_level(spdlog::level::debug);// Debug build
          // #else
          //           spdlog::set_level(spdlog::level::info);// Release build
          // #endif

          // Optional: control flush policy
          spdlog::flush_on(spdlog::level::info);

          // Now log anywhere
          spdlog::info("App started");
     }
     catch (const spdlog::spdlog_ex &ex)
     {
          std::cerr << "Log init failed: " << ex.what() << std::endl;
     }
     GLFWwindow *const window = create_glfw_window();
     if (!window)
          return 0;
     setWindowIcon(window);
     const GLenum err = glewInit();
     if (std::cmp_not_equal(GLEW_OK, err))
     {
          spdlog::error("GLEW init failed: {}", reinterpret_cast<const char *>(glewGetErrorString(err)));
          glfwDestroyWindow(window);
          glfwTerminate();
          return -1;
     }

     glengine::Renderer::Clear();
     glfwSwapBuffers(window);

     const GLubyte *version = glGetString(GL_VERSION);
     if (version)
     {
          spdlog::info("OpenGL version: {}", reinterpret_cast<const char *>(version));
     }
     else
     {
          spdlog::error("Failed to get OpenGL version. Is the context initialized?");
     }
     glengine::BlendModeSettings::enable_blending();
     glengine::BlendModeSettings::default_blend();
     // Enable debug output
     glengine::GlCall{}(glEnable, GL_DEBUG_OUTPUT);
     glengine::GlCall{}(glEnable, GL_DEBUG_OUTPUT_SYNCHRONOUS);

     {
          auto the_gui = fme::gui{ window };
          the_gui.start(window);
     }

     // Cleanup
     glfwDestroyWindow(window);
     glfwTerminate();
     return 0;
}