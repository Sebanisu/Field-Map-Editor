//
// Created by pcvii on 12/6/2021.
//

#ifndef MYPROJECT_WINDOW_HPP
#define MYPROJECT_WINDOW_HPP
#include "Event/EventItem.hpp"
#include "Input.hpp"
namespace glengine
{
class Window final
{
public:
  void BindInputPollingToWindow() const;
  void BeginFrame() const;
  void EndFrameRendered() const;
  // When not rendering Call this.
  void EndFrame() const;
  int  Width() const;
  int  Height() const;
  void EnableVSync();
  void DisableVSync();
  bool VSync() const;
  using EventCallbackFn = std::function<void(const Event::Item &)>;
  void SetEventCallback(EventCallbackFn function) const
  {
    auto &window_data          = GetWindowData(m_window.get());
    window_data.event_callback = std::move(function);
  }
  struct WindowData
  {
    std::string     Title               = {};
    int             width               = {};
    int             height              = {};
    int             frame_buffer_width  = {};
    int             frame_buffer_height = {};
    bool            vsync               = { true };
    EventCallbackFn event_callback      = {};
    GLFWmonitor    *monitor             = nullptr;
  };
  static std::unique_ptr<Window>  Create(WindowData);

  bool                            WindowClosing() const;
  static std::span<GLFWmonitor *> GetMonitors()
  {
    int           count    = {};
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    return { monitors, static_cast<size_t>(count) };
  }
  static std::span<const GLFWvidmode> GetVideoModes(GLFWmonitor *monitor)
  {
    int                count        = {};
    const GLFWvidmode *glfw_vidmode = glfwGetVideoModes(monitor, &count);
    return { glfw_vidmode, static_cast<size_t>(count) };
  }
  void FullScreenMode(GLFWmonitor *monitor) const
  {
    const auto *current_mode = glfwGetVideoMode(monitor);
    FullScreenMode(
      monitor,
      current_mode->width,
      current_mode->height,
      current_mode->refreshRate);
  }
  void FullScreenMode(
    GLFWmonitor *monitor,
    int          width,
    int          height,
    int          refreshRate = GLFW_DONT_CARE) const
  {
    glfwSetWindowMonitor(
      m_window.get(), monitor, 0, 0, width, height, refreshRate);

    auto &data   = GetWindowData(m_window.get());
    data.monitor = monitor;
  }
  void WindowedMode() const
  {
    auto       &data         = GetWindowData(m_window.get());
    const auto *current_mode = glfwGetVideoMode(GetMonitors().front());

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
  const WindowData &ViewWindowData() const
  {
    return GetWindowData(m_window.get());
  }

  static void DefaultBlend();
  static void AddBlend();
  static void SubtractBlend();

private:
  Window(WindowData);
  WindowData m_data = {};
  constexpr inline static void (*const destroy_window)(GLFWwindow *) =
    [](GLFWwindow *window) {
      // Cleanup
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      // disable polling:
      Input::m_window = nullptr;
      glfwDestroyWindow(window);
    };
  std::unique_ptr<GLFWwindow, decltype(destroy_window)> m_window{
    nullptr,
    destroy_window
  };
  void               InitCallbacks() const;
  void               InitImGui(const char *glsl_version) const;
  void               InitGLFW();
  static WindowData &GetWindowData(GLFWwindow *);
};
}// namespace glengine
#endif// MYPROJECT_WINDOW_HPP
