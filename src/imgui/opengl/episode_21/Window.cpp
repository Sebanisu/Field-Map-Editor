//
// Created by pcvii on 12/6/2021.
//
static bool glfw_init = false;
#include "Window.hpp"
#include "Event.hpp"
#include "Renderer.hpp"
#include <fmt/format.h>
void
  Window::OnUpdate() const
{
  /* Poll for and process events */
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
}
void
  Window::OnRender() const
{
  ImGui::Render();


  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  GLClearError();

  /* Swap front and back buffers */
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

  const char *glsl_version = "#version 130";
  InitGLFW();
  /* Init GLEW after context */
  if (glewInit() != GLEW_OK)
  {
    fmt::print(stderr, "Error! {}:{} GLEW NOT OKAY\n", __FILE__, __LINE__);
  }
  fmt::print("{}\n", GLCall{ glGetString, GL_VERSION }());
  InitImGui(glsl_version);


  InitCallbacks();
}
void
  Window::InitGLFW()
{
  /* Initialize the library */
  if (!glfw_init)
  {
    if (!glfwInit())
    {
      fmt::print(
        stderr, "Error! {}:{} GLFW Failed to Initialize\n", __FILE__, __LINE__);
      glfwTerminate();
    }
    glfw_init = true;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_window.reset(glfwCreateWindow(
    m_data.width, m_data.height, m_data.Title.c_str(), nullptr, nullptr));

  if (!m_window)
  {
    fmt::print(
      stderr,
      "Error! {}:{} GLFW Failed to create Window\n",
      __FILE__,
      __LINE__);
    glfwTerminate();
  }
  glfwMakeContextCurrent(m_window.get());
  glfwSetWindowUserPointer(m_window.get(), &m_data);
  BindInputPollingToWindow();
  EnableVSync();
  glfwGetFramebufferSize(
    m_window.get(), &m_data.frame_buffer_width, &m_data.frame_buffer_height);
}
void
  Window::InitImGui(const char *glsl_version) const
{
  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(m_window.get(), true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or
  // display an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and
  // stored into a texture when calling
  // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
  // below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);
}
void
  Window::InitCallbacks() const
{
  BeginErrorCallBack();
  // GLFW callBacks
  glfwSetErrorCallback(
    [](int error, const char *description)
    { fmt::print(stderr, "Error GLFW {}: {}\n", error, description); });

  glfwSetWindowSizeCallback(
    m_window.get(),
    [](GLFWwindow *window, int width, int height)
    {
      auto &data  = GetWindowData(window);
      data.width  = width;
      data.height = height;
      data.event_callback(Event::WindowResize(width, height));
    });

  glfwSetFramebufferSizeCallback(
    m_window.get(),
    [](GLFWwindow *window, int width, int height)
    {
      auto &data               = GetWindowData(window);
      data.frame_buffer_width  = width;
      data.frame_buffer_height = height;
      data.event_callback(Event::FrameBufferResize(width, height));
    });

  glfwSetWindowCloseCallback(
    m_window.get(),
    [](GLFWwindow *window)
    {
      auto &data = GetWindowData(window);
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
      auto &data = GetWindowData(window);
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
      auto &data = GetWindowData(window);
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
      auto &data = GetWindowData(window);
      data.event_callback(Event::MouseScroll(
        static_cast<float>(x_offset), static_cast<float>(y_offset)));
    });

  glfwSetCursorPosCallback(
    m_window.get(),
    [](GLFWwindow *window, double x, double y)
    {
      auto &data = GetWindowData(window);
      data.event_callback(
        Event::MouseMoved(static_cast<float>(x), static_cast<float>(y)));
    });

  glfwSetWindowPosCallback(
    m_window.get(),
    [](GLFWwindow *window, int x, int y)
    {
      auto &data = GetWindowData(window);
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
void
  Window::BindInputPollingToWindow() const
{
  Input::m_window = m_window.get();
}
