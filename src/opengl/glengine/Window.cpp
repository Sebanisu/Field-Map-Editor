//
// Created by pcvii on 12/6/2021.
//
#include "Window.hpp"
#include "Event/Event.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"

namespace glengine
{
static bool glfw_init  = false;
static bool imgui_init = false;
void        Window::BeginFrame() const
{
  /* Poll for and process events */
  glfwPollEvents();

  // Start the Dear ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  glengine::ImGuiPushID.reset();
}
void Window::RenderDockspace() const
{
  // If you strip some features of, this demo is pretty much equivalent to
  // calling DockSpaceOverViewport()! In most cases you should be able to just
  // call DockSpaceOverViewport() and ignore all the code below! In this
  // specific demo, we are not using DockSpaceOverViewport() because:
  // - we allow the host window to be floating/moveable instead of filling the
  // viewport (when opt_fullscreen == false)
  // - we allow the host window to have padding (when opt_padding == true)
  // - we have a local menu bar in the host window (vs. you could use
  // BeginMainMenuBar() + DockSpaceOverViewport() in your code!) TL;DR; this
  // demo is more complicated than what you would normally use. If we removed
  // all the options we are showcasing, this demo would become:
  //     void ShowExampleAppDockSpace()
  //     {
  //         ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
  //     }

  static bool               opt_fullscreen  = true;
  static bool               opt_padding     = false;
  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

  // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window
  // not dockable into, because it would be confusing to have two docking
  // targets within each others.
  ImGuiWindowFlags          window_flags =
    ImGuiWindowFlags_NoDocking;// ImGuiWindowFlags_MenuBar
  if (opt_fullscreen)
  {
    const ImGuiViewport *viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                    | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |=
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }
  else
  {
    dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
  }

  // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render
  // our background and handle the pass-thru hole, so we ask Begin() to not
  // render a background.
  if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
    window_flags |= ImGuiWindowFlags_NoBackground;

  // Important: note that we proceed even if Begin() returns false (aka window
  // is collapsed). This is because we want to keep our DockSpace() active. If a
  // DockSpace() is inactive, all active windows docked into it will lose their
  // parent and become undocked. We cannot preserve the docking relationship
  // between an active window and an inactive docking, otherwise any change of
  // dockspace/settings would lead to windows being stuck in limbo and never
  // being visible.
  if (!opt_padding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("DockSpace Demo", nullptr, window_flags);
  if (!opt_padding)
    ImGui::PopStyleVar();

  if (opt_fullscreen)
    ImGui::PopStyleVar(2);

  dockspace_flags |= ImGuiDockNodeFlags_PassthruCentralNode;
  dockspace_flags = dockspace_flags & (~ImGuiDockNodeFlags_NoResize);
  // Submit the DockSpace
  ImGuiIO &io     = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
  {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
  }
  else
  {
    // ShowDockingDisabledMessage();
  }
  ImGui::End();
}
void Window::EndFrameRendered() const
{
  ImGui::Render();


  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  GLClearError();

  /* Swap front and back buffers */
  glfwSwapBuffers(m_window.get());
}
void Window::UpdateViewPorts() const
{
  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    GLFWwindow *backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}
int Window::Width() const
{
  return m_data.width;
}
int Window::Height() const
{
  return m_data.height;
}
void Window::EnableVSync()
{
  glfwSwapInterval(1);
  m_data.vsync = true;
}
void Window::DisableVSync()
{
  glfwSwapInterval(0);
  m_data.vsync = false;
}
bool Window::VSync() const
{
  return m_data.vsync;
}
std::unique_ptr<Window> Window::Create(Window::WindowData data)
{
  return std::unique_ptr<Window>(new Window(std::move(data)));
}
Window::Window(Window::WindowData in_data)
  : m_data(std::move(in_data))
{

  const char *glsl_version = "#version 130";
  InitGLFW();
  const char *gl_version =
    reinterpret_cast<const char *>(GLCall{}(glGetString, GL_VERSION));
  if (gl_version != nullptr)
  {
    fmt::print("{}\n", gl_version);
  }
  InitImGui(glsl_version);
  InitCallbacks();
}
void Window::InitGLFW()
{
  /* Initialize the library */
  if (!glfw_init)
  {
    if (!glfwInit())
    {
      fmt::print(
        stderr, "Error! {}:{} GLFW Failed to Initialize\n", __FILE__, __LINE__);
      std::exit(EXIT_FAILURE);
    }
    glfw_init = true;
  }
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // const auto monitors = GetMonitors();
  m_window.reset(glfwCreateWindow(
    m_data.width,
    m_data.height,
    m_data.Title.c_str(),
    nullptr,// monitors.front(),
    nullptr));

  if (!m_window)
  {
    fmt::print(
      stderr,
      "Error! {}:{} GLFW Failed to create Window\n",
      __FILE__,
      __LINE__);
    std::exit(EXIT_FAILURE);
  }
  glfwMakeContextCurrent(m_window.get());
  glfwSetWindowUserPointer(m_window.get(), &m_data);
  BindInputPollingToWindow();
  EnableVSync();
  glfwGetFramebufferSize(
    m_window.get(), &m_data.frame_buffer_width, &m_data.frame_buffer_height);
  /* Init GLEW after context */
  if (glewInit() != GLEW_OK)
  {
    fmt::print(stderr, "Error! {}:{} GLEW NOT OKAY\n", __FILE__, __LINE__);
    std::exit(EXIT_FAILURE);
  }
  GLCall{}(glEnable, GL_BLEND);
  DefaultBlend();
}
void Window::InitImGui(const char *const glsl_version) const
{
  if (!imgui_init)
  {

    imgui_init = true;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
    // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
    // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();
  }
  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(m_window.get(), false);
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

void Window::InitCallbacks() const
{

  BeginErrorCallBack();
  // GLFW callBacks

  glfwSetErrorCallback([](int error, const char *description) {
    fmt::print(stderr, "Error GLFW {}: {}\n", error, description);
    throw;
  });

  glfwSetWindowSizeCallback(
    m_window.get(), [](GLFWwindow *window, int width, int height) {
      auto &data  = GetWindowData(window);
      data.width  = width;
      data.height = height;
      data.event_callback(Event::WindowResize(width, height));
    });

  glfwSetFramebufferSizeCallback(
    m_window.get(), [](GLFWwindow *window, int width, int height) {
      auto &data               = GetWindowData(window);
      data.frame_buffer_width  = width;
      data.frame_buffer_height = height;
      GLCall{}(glViewport, 0, 0, width, height);
      data.event_callback(Event::FrameBufferResize(width, height));
    });

  glfwSetWindowCloseCallback(m_window.get(), [](GLFWwindow *window) {
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
      int                  mods) {
      ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
      auto &data = GetWindowData(window);
      switch (action)
      {
        case +KEY::PRESS: {
          data.event_callback(Event::KeyPressed(
            glengine::KEY{ key }, glengine::MODS{ mods }, false));
          break;
        }
        case +KEY::RELEASE: {
          data.event_callback(
            Event::KeyReleased(glengine::KEY{ key }, glengine::MODS{ mods }));
          break;
        }
        case +KEY::REPEAT: {
          data.event_callback(Event::KeyPressed(
            glengine::KEY{ key }, glengine::MODS{ mods }, true));
          break;
        }
      }
    });
  glfwSetMouseButtonCallback(
    m_window.get(), [](GLFWwindow *window, int button, int action, int mods) {
      ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
      using glengine::MOUSE;
      auto &data = GetWindowData(window);
      switch (action)
      {
        case +MOUSE::PRESS: {
          data.event_callback(
            Event::MouseButtonPressed(MOUSE{ button }, MODS{ mods }));
          break;
        }
        case +MOUSE::RELEASE: {
          data.event_callback(
            Event::MouseButtonReleased(MOUSE{ button }, MODS{ mods }));
          break;
        }
      }
    });
  glfwSetScrollCallback(
    m_window.get(), [](GLFWwindow *window, double x_offset, double y_offset) {
      ImGui_ImplGlfw_ScrollCallback(window, x_offset, y_offset);
      auto &data = GetWindowData(window);
      data.event_callback(Event::MouseScroll(
        static_cast<float>(x_offset), static_cast<float>(y_offset)));
    });

  glfwSetCursorPosCallback(
    m_window.get(), [](GLFWwindow *window, double x, double y) {
      ImGui_ImplGlfw_CursorPosCallback(window, x, y);
      auto &data = GetWindowData(window);
      data.event_callback(
        Event::MouseMoved(static_cast<float>(x), static_cast<float>(y)));
    });

  glfwSetWindowPosCallback(
    m_window.get(), [](GLFWwindow *window, int x, int y) {
      auto &data = GetWindowData(window);
      data.event_callback(Event::WindowMoved(x, y));
    });
  glfwSetWindowFocusCallback(
    m_window.get(), ImGui_ImplGlfw_WindowFocusCallback);
  glfwSetCursorEnterCallback(
    m_window.get(), ImGui_ImplGlfw_CursorEnterCallback);
  glfwSetCharCallback(m_window.get(), ImGui_ImplGlfw_CharCallback);
  glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);
}
Window::WindowData &Window::GetWindowData(GLFWwindow *window)
{
  return *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
}
bool Window::WindowClosing() const
{
  return glfwWindowShouldClose(m_window.get());
}
void Window::BindInputPollingToWindow() const
{
  glengine::Input::m_window = m_window.get();
}
void Window::EndFrame() const
{
  ImGui::EndFrame();// call instead of render when minimized.
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
void Window::DefaultBlend()
{
  GLCall{}(glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  GLCall{}(glBlendEquation, GL_FUNC_ADD);
}

void Window::AddBlend()
{
  GLCall{}(glBlendEquationSeparate, GL_FUNC_ADD, GL_FUNC_ADD);
}

void Window::SubtractBlend()
{
  GLCall{}(glBlendEquationSeparate, GL_FUNC_REVERSE_SUBTRACT, GL_FUNC_ADD);
}
}// namespace glengine