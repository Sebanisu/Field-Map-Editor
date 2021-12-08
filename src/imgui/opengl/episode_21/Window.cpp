//
// Created by pcvii on 12/6/2021.
//
static bool glfw_init = false;
#include "Window.hpp"
#include "Event.hpp"
#include <fmt/format.h>
void
  Window::OnUpdate() const
{
  glfwPollEvents();
}
void
  Window::OnRender() const
{
  glfwSwapBuffers(m_window.get());
}
int
  Window::Width() const
{
  return m_data.width;
}
int
  Window::Height() const
{
  return m_data.height;
}
void
  Window::EnableVSync()
{
  glfwSwapInterval(1);
  m_data.vsync = true;
}
void
  Window::DisableVSync()
{
  glfwSwapInterval(0);
  m_data.vsync = false;
}
bool
  Window::VSync() const
{
  return m_data.vsync;
}
std::unique_ptr<Window>
  Window::Create(Window::WindowData data)
{
  return std::unique_ptr<Window>(new Window(std::move(data)));
}
Window::Window(Window::WindowData in_data)
  : m_data(std::move(in_data))
{
  /* Initialize the library */
  if (!glfw_init)
  {
    if (!glfwInit())
      throw;
    glfw_init = true;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window.reset(glfwCreateWindow(
    m_data.width, m_data.height, m_data.Title.c_str(), nullptr, nullptr));

  glfwMakeContextCurrent(m_window.get());
  glfwSetWindowUserPointer(m_window.get(), &m_data);
  EnableVSync();
  glfwGetFramebufferSize(
    m_window.get(), &m_data.frame_buffer_width, &m_data.frame_buffer_height);

  // GLFW callBacks
  glfwSetErrorCallback(
    [](int error, const char *description)
    { fmt::print(stderr, "Error GLFW {}: {}\n", error, description); });

  glfwSetWindowSizeCallback(
    m_window.get(),
    [](GLFWwindow *window, int width, int height)
    {
      auto &data  = Window::GetWindowData(window);
      data.width  = width;
      data.height = height;
      data.event_callback(Event::WindowResize(width, height));
    });

  glfwSetFramebufferSizeCallback(
    m_window.get(),
    [](GLFWwindow *window, int width, int height)
    {
      auto &data               = Window::GetWindowData(window);
      data.frame_buffer_width  = width;
      data.frame_buffer_height = height;
      data.event_callback(Event::FrameBufferResize(width, height));
    });

  glfwSetWindowCloseCallback(
    m_window.get(),
    [](GLFWwindow *window)
    {
      auto &data = Window::GetWindowData(window);
      data.event_callback(Event::WindowClose());
    });

  glfwSetKeyCallback(
    m_window.get(),
    [](
      GLFWwindow          *window,
      int                  key,
      [[maybe_unused]] int scancode,
      int                  action,
      [[maybe_unused]] int mods)
    {
      auto &data = Window::GetWindowData(window);
      switch (action)
      {
        case GLFW_PRESS:
        {
          data.event_callback(Event::KeyPressed(KEY{ key }, false));
          break;
        }
        case GLFW_RELEASE:
        {
          data.event_callback(Event::KeyReleased(KEY{ key }));
          break;
        }
        case GLFW_REPEAT:
        {
          data.event_callback(Event::KeyPressed(KEY{ key }, true));
          break;
        }
      }
    });

  glfwSetMouseButtonCallback(
    m_window.get(),
    [](GLFWwindow *window, int button, int action, [[maybe_unused]] int mods)
    {
      auto &data = Window::GetWindowData(window);
      switch (action)
      {
        case GLFW_PRESS:
        {
          data.event_callback(Event::MouseButtonPressed(MOUSE{ button }));
          break;
        }
        case GLFW_RELEASE:
        {
          data.event_callback(Event::MouseButtonReleased(MOUSE{ button }));
          break;
        }
      }
    });
  glfwSetScrollCallback(
    m_window.get(),
    [](GLFWwindow *window, double x_offset, double y_offset)
    {
      auto &data = Window::GetWindowData(window);
      data.event_callback(Event::MouseScroll(
        static_cast<float>(x_offset), static_cast<float>(y_offset)));
    });

  glfwSetCursorPosCallback(
    m_window.get(),
    [](GLFWwindow *window, double x, double y)
    {
      auto &data = Window::GetWindowData(window);
      data.event_callback(
        Event::MouseMoved(static_cast<float>(x), static_cast<float>(y)));
    });

  glfwSetWindowPosCallback(
    m_window.get(),
    [](GLFWwindow *window, int x, int y)
    {
      auto &data = Window::GetWindowData(window);
      data.event_callback(Event::WindowMoved(x, y));
    });
}
Window::WindowData &
  Window::GetWindowData(GLFWwindow *window)
{
  return *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
}
bool
  Window::WindowClosing() const
{
  return glfwWindowShouldClose(m_window.get());
}
