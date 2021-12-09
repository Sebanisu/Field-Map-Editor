//
// Created by pcvii on 12/6/2021.
//

#ifndef MYPROJECT_WINDOW_HPP
#define MYPROJECT_WINDOW_HPP
#include "EventItem.hpp"
#include "Input.hpp"
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <memory>
#include <string>
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
  using EventCallbackFn =
    std::function<void(const Event::Item &)>;// void (*)(const EventItem &);
  void SetEventCallback(EventCallbackFn function) const
  {
    auto & window_data =  GetWindowData(m_window.get());
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
  };
  static std::unique_ptr<Window> Create(WindowData);

  bool                           WindowClosing() const;


private:
  Window(WindowData);
  WindowData m_data = {};
  inline static void (*const destroy_window)(GLFWwindow *) =
    [](GLFWwindow *window) {
      // Cleanup
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      ImGui::DestroyContext();

      glfwDestroyWindow(window);
      // glfwTerminate();
    };
  std::unique_ptr<GLFWwindow, decltype(destroy_window)> m_window{
    nullptr,
    destroy_window
  };
  static WindowData &GetWindowData(GLFWwindow *);
  void               InitCallbacks() const;
  void               InitImGui(const char *glsl_version) const;
  void               InitGLFW();
};
#endif// MYPROJECT_WINDOW_HPP
