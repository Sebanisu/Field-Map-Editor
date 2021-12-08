//
// Created by pcvii on 12/6/2021.
//

#ifndef MYPROJECT_WINDOW_HPP
#define MYPROJECT_WINDOW_HPP
#include "EventItem.hpp"
#include <functional>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
class Window final
{
public:
  void
    OnUpdate() const;
  void
    OnRender() const;
  int
    Width() const;
  int
    Height() const;
  void
    EnableVSync();
  void
    DisableVSync();
  bool
    VSync() const;
  using EventCallbackFn =
    std::function<void(const Event::Item &)>;// void (*)(const EventItem &);
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

  bool
    WindowClosing() const;


private:
  Window(WindowData);
  WindowData m_data = {};
  inline static void (*const destroy_window)(GLFWwindow *) =
    [](GLFWwindow *window)
  {
    // Cleanup
    //    ImGui_ImplOpenGL3_Shutdown();
    //    ImGui_ImplGlfw_Shutdown();
    //    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    // glfwTerminate();
  };
  std::unique_ptr<GLFWwindow, decltype(destroy_window)> m_window{
    nullptr,
    destroy_window
  };
  static WindowData &
    GetWindowData(GLFWwindow *);
};
#endif// MYPROJECT_WINDOW_HPP
