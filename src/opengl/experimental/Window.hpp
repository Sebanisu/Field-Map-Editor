//
// Created by pcvii on 12/6/2021.
//

#ifndef FIELD_MAP_EDITOR_WINDOW_HPP
#define FIELD_MAP_EDITOR_WINDOW_HPP
// clang-format off
#include <GL/glew.h>
#include <GLFW/glfw3.h>
// clang-format on
#include "Event/EventItem.hpp"
#include "Input.hpp"
#include <functional>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>

namespace glengine
{
class Window final
{
   public:
     void bind_input_polling_to_window() const;
     void begin_frame() const;
     void end_frame_rendered() const;
     // When not rendering Call this.
     void end_frame() const;
     int  width() const;
     int  height() const;
     void enable_v_sync();
     void disable_v_sync();
     bool v_sync() const;
     using EventCallbackFn = std::function<void(const event::Item &)>;
     //  void set_event_callback(EventCallbackFn function) const
     //  {
     //    auto &window_data          = get_window_data(m_window.get());
     //    window_data.event_callback = std::move(function);
     //  }
     struct WindowData
     {
          std::string     title               = {};
          int             width               = {};
          int             height              = {};
          int             frame_buffer_width  = {};
          int             frame_buffer_height = {};
          bool            vsync               = { true };
          EventCallbackFn event_callback      = {};
          GLFWmonitor    *monitor             = nullptr;
     };
     static std::unique_ptr<Window>  create(WindowData data);
     bool                            window_closing() const;
     static std::span<GLFWmonitor *> get_monitors()
     {
          int           count    = {};
          GLFWmonitor **monitors = glfwGetMonitors(&count);
          return { monitors, static_cast<size_t>(count) };
     }
     static std::span<const GLFWvidmode> get_video_modes(GLFWmonitor *monitor)
     {
          int                count        = {};
          const GLFWvidmode *glfw_vidmode = glfwGetVideoModes(monitor, &count);
          return { glfw_vidmode, static_cast<size_t>(count) };
     }
     void full_screen_mode(GLFWmonitor *monitor) const
     {
          const auto *current_mode = glfwGetVideoMode(monitor);
          full_screen_mode(monitor, current_mode->width, current_mode->height, current_mode->refreshRate);
     }
     void full_screen_mode(
       GLFWmonitor *monitor,
       int          width,
       int          height,
       int          refresh_rate = GLFW_DONT_CARE) const
     {
          glfwSetWindowMonitor(m_window.get(), monitor, 0, 0, width, height, refresh_rate);

          auto &data   = get_window_data(m_window.get());
          data.monitor = monitor;
     }
     void windowed_mode() const
     {
          auto       &data         = get_window_data(m_window.get());
          const auto *current_mode = glfwGetVideoMode(get_monitors().front());

          glfwSetWindowMonitor(
            m_window.get(),
            nullptr,
            current_mode->width / 2 - data.width / 2,
            current_mode->height / 2 - data.height / 2,
            data.width,
            data.height,
            GLFW_DONT_CARE);
          data.monitor = nullptr;
     }
     void trigger_refresh_image() const
     {
          auto &data = get_window_data(m_window.get());
          data.event_callback(event::RefreshImage());
     }
     void trigger_reload() const
     {
          auto &data = get_window_data(m_window.get());
          data.event_callback(event::Reload());
     }
     const WindowData &view_window_data() const
     {
          return get_window_data(m_window.get());
     }

     void update_view_ports() const;
     void render_dockspace() const;

   private:
     Window(WindowData);
     WindowData m_data                                                  = {};
     constexpr inline static void (*const destroy_window)(GLFWwindow *) = [](GLFWwindow *window) {
          // Cleanup
          ImGui_ImplOpenGL3_Shutdown();
          ImGui_ImplGlfw_Shutdown();
          // disable polling:
          Input::m_window = nullptr;
          glfwDestroyWindow(window);
     };
     std::unique_ptr<GLFWwindow, decltype(destroy_window)> m_window{ nullptr, destroy_window };
     void                                                  init_callbacks() const;
     void                                                  init_im_gui(const char *glsl_version) const;
     void                                                  init_glfw();
     static WindowData                                    &get_window_data(GLFWwindow *window);
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_WINDOW_HPP
