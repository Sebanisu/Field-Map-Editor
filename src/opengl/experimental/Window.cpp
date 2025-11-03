//
// Created by pcvii on 12/6/2021.
//
#include "Window.hpp"
#include "ImGuiPushID.hpp"
#include <filesystem>
#include <glengine/BlendModeSettings.hpp>
#include <glengine/Event/Event.hpp>
#include <glengine/Renderer.hpp>
#include <spdlog/spdlog.h>

namespace glengine
{
static bool GlfwInit  = false;
static bool ImguiInit = false;
void        Window::begin_frame() const
{
     /* Poll for and process events */
     glfwPollEvents();

     // Start the Dear ImGui frame
     ImGui_ImplOpenGL3_NewFrame();
     ImGui_ImplGlfw_NewFrame();
     ImGui::NewFrame();
     glengine::ImGuiPushId.reset();
}
void Window::render_dockspace() const
{
     // If you strip some features of, this demo is pretty much equivalent to
     // calling DockSpaceOverViewport()! In most cases you should be able to
     // just call DockSpaceOverViewport() and ignore all the code below! In this
     // specific demo, we are not using DockSpaceOverViewport() because:
     // - we allow the host window to be floating/moveable instead of filling
     // the viewport (when opt_fullscreen == false)
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

     // We are using the ImGuiWindowFlags_NoDocking flag to make the parent
     // window not dockable into, because it would be confusing to have two
     // docking targets within each others.
     ImGuiWindowFlags          window_flags
       = ImGuiWindowFlags_NoDocking;// ImGuiWindowFlags_MenuBar
     if (opt_fullscreen)
     {
          const ImGuiViewport *viewport = ImGui::GetMainViewport();
          ImGui::SetNextWindowPos(viewport->WorkPos);
          ImGui::SetNextWindowSize(viewport->WorkSize);
          ImGui::SetNextWindowViewport(viewport->ID);
          ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
          ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
          window_flags |= ImGuiWindowFlags_NoTitleBar
                          | ImGuiWindowFlags_NoCollapse
                          | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
          window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus
                          | ImGuiWindowFlags_NoNavFocus;
     }
     else
     {
          dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
     }

     // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will
     // render our background and handle the pass-thru hole, so we ask Begin()
     // to not render a background.
     if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
          window_flags |= ImGuiWindowFlags_NoBackground;

     // Important: note that we proceed even if Begin() returns false (aka
     // window is collapsed). This is because we want to keep our DockSpace()
     // active. If a DockSpace() is inactive, all active windows docked into it
     // will lose their parent and become undocked. We cannot preserve the
     // docking relationship between an active window and an inactive docking,
     // otherwise any change of dockspace/settings would lead to windows being
     // stuck in limbo and never being visible.
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
void Window::end_frame_rendered() const
{
     ImGui::Render();


     ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
     GlClearError();

     /* Swap front and back buffers */
     glfwSwapBuffers(m_window.get());
}
void Window::update_view_ports() const
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
int Window::width() const
{
     return m_data.width;
}
int Window::height() const
{
     return m_data.height;
}
void Window::enable_v_sync()
{
     glfwSwapInterval(1);
     m_data.vsync = true;
}
void Window::disable_v_sync()
{
     glfwSwapInterval(0);
     m_data.vsync = false;
}
bool Window::v_sync() const
{
     return m_data.vsync;
}
std::unique_ptr<Window> Window::create(Window::WindowData data)
{
     return std::unique_ptr<Window>(new Window(std::move(data)));
}
Window::Window(Window::WindowData in_data)
  : m_data(std::move(in_data))
{

     const char *glsl_version = "#version 130";
     init_glfw();
     const char *gl_version
       = reinterpret_cast<const char *>(GlCall{}(glGetString, GL_VERSION));
     if (gl_version != nullptr)
     {
          spdlog::debug("{}", gl_version);
     }
     init_im_gui(glsl_version);
     init_callbacks();
}
void Window::init_glfw()
{
     /* Initialize the library */
     if (!GlfwInit)
     {
          if (!glfwInit())
          {
               spdlog::critical(
                 "{}:{} GLFW Failed to Initialize", __FILE__, __LINE__);
               std::exit(EXIT_FAILURE);
          }
          GlfwInit = true;
     }
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

     // const auto monitors = GetMonitors();
     m_window.reset(glfwCreateWindow(
       m_data.width,
       m_data.height,
       m_data.title.c_str(),
       nullptr,// monitors.front(),
       nullptr));

     if (!m_window)
     {
          spdlog::critical(
            "Error! {}:{} GLFW Failed to create Window", __FILE__, __LINE__);
          std::exit(EXIT_FAILURE);
     }
     glfwMakeContextCurrent(m_window.get());
     glfwSetWindowUserPointer(m_window.get(), &m_data);
     bind_input_polling_to_window();
     enable_v_sync();
     glfwGetFramebufferSize(
       m_window.get(), &m_data.frame_buffer_width, &m_data.frame_buffer_height);
     /* Init GLEW after context */
     if (glewInit() != GLEW_OK)
     {
          spdlog::critical("{}:{} GLEW NOT OKAY", __FILE__, __LINE__);
          std::exit(EXIT_FAILURE);
     }
     BlendModeSettings::enable_blending();
     BlendModeSettings::default_blend();
}
void Window::init_im_gui(const char *const glsl_version) const
{
     if (!ImguiInit)
     {
          ImguiInit = true;
          // Setup Dear ImGui context
          IMGUI_CHECKVERSION();
          ImGui::CreateContext();
          ImGuiIO          &io         = ImGui::GetIO();
          std::error_code   error_code = {};
          static const auto path
            = (std::filesystem::current_path(error_code) / "res"
               / "field-map-editor-experimental_imgui.ini")
                .string();
          io.IniFilename = path.c_str();
          if (error_code)
          {
               spdlog::warn(
                 "{}:{} - {}: {} path: \"{}\"",
                 __FILE__,
                 __LINE__,
                 error_code.value(),
                 error_code.message(),
                 path);
               error_code.clear();
          }
          // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
          // Keyboard Controls io.ConfigFlags |=
          // ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
          io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
          // io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;// events don't
          // work
          //  with viewports

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
     // - AddFontFromFileTTF() will return the ImFont* so you can store it if
     // you need to select the font among multiple.
     // - If the file cannot be loaded, the function will return NULL. Please
     // handle those errors in your application (e.g. use an assertion, or
     // display an error and quit).
     // - The fonts will be rasterized at a given size (w/ oversampling) and
     // stored into a texture when calling
     // ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame
     // below will call.
     // - Read 'docs/FONTS.md' for more instructions and details.
     // - Remember that in C/C++ if you want to include a backslash \ in a
     // string literal you need to write a double backslash \\ !
     // io.Fonts->AddFontDefault();
     // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
     // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
     // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
     // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
     // ImFont* font =
     // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
     // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);
}

void Window::init_callbacks() const
{

     BeginErrorCallBack();
     // GLFW callBacks

     glfwSetErrorCallback(
       [](int error, const char *description)
       {
            spdlog::error("GLFW {:X}: {}", error, description);
            throw;
       });

     glfwSetWindowSizeCallback(
       m_window.get(),
       [](GLFWwindow *window, int width, int height)
       {
            auto &data  = get_window_data(window);
            data.width  = width;
            data.height = height;
            data.event_callback(event::WindowResize(width, height));
       });

     glfwSetFramebufferSizeCallback(
       m_window.get(),
       [](GLFWwindow *window, int width, int height)
       {
            auto &data               = get_window_data(window);
            data.frame_buffer_width  = width;
            data.frame_buffer_height = height;
            GlCall{}(glViewport, 0, 0, width, height);
            data.event_callback(event::FrameBufferResize(width, height));
       });

     glfwSetWindowCloseCallback(
       m_window.get(),
       [](GLFWwindow *window)
       {
            auto &data = get_window_data(window);
            data.event_callback(event::WindowClose());
       });

     glfwSetKeyCallback(
       m_window.get(),
       [](
         GLFWwindow          *window,
         int                  key,
         [[maybe_unused]] int scancode,
         int                  action,
         int                  mods)
       {
            if (key == +Key::Unknown)
            {
                 //        spdlog::warn("glfwSetKeyCallback Unsupported Scan
                 //        Code");
                 return;
            }
            auto &data = get_window_data(window);
            switch (action)
            {
                 case +Key::Press:
                 {
                      data.event_callback(
                        event::KeyPressed(
                          glengine::Key{ key }, glengine::Mods{ mods }, false));
                      break;
                 }
                 case +Key::Release:
                 {
                      data.event_callback(
                        event::KeyReleased(
                          glengine::Key{ key }, glengine::Mods{ mods }));
                      break;
                 }
                 case +Key::Repeat:
                 {
                      data.event_callback(
                        event::KeyPressed(
                          glengine::Key{ key }, glengine::Mods{ mods }, true));
                      break;
                 }
            }
       });
     glfwSetMouseButtonCallback(
       m_window.get(),
       [](GLFWwindow *window, int button, int action, int mods)
       {
            using glengine::Mouse;
            auto &data = get_window_data(window);
            switch (action)
            {
                 case +Mouse::Press:
                 {
                      data.event_callback(
                        event::MouseButtonPressed(
                          Mouse{ button }, Mods{ mods }));
                      break;
                 }
                 case +Mouse::Release:
                 {
                      data.event_callback(
                        event::MouseButtonReleased(
                          Mouse{ button }, Mods{ mods }));
                      break;
                 }
            }
       });
     glfwSetScrollCallback(
       m_window.get(),
       [](GLFWwindow *window, double x_offset, double y_offset)
       {
            auto &data = get_window_data(window);
            data.event_callback(
              event::MouseScroll(
                static_cast<float>(x_offset), static_cast<float>(y_offset)));
       });

     glfwSetCursorPosCallback(
       m_window.get(),
       [](GLFWwindow *window, double x, double y)
       {
            auto &data = get_window_data(window);
            data.event_callback(
              event::MouseMoved(static_cast<float>(x), static_cast<float>(y)));
       });

     glfwSetWindowPosCallback(
       m_window.get(),
       [](GLFWwindow *window, int x, int y)
       {
            auto &data = get_window_data(window);
            data.event_callback(event::WindowMoved(x, y));
       });

     ImGui_ImplGlfw_InstallCallbacks(m_window.get());
}
Window::WindowData &Window::get_window_data(GLFWwindow *window)
{
     return *static_cast<WindowData *>(glfwGetWindowUserPointer(window));
}
bool Window::window_closing() const
{
     return glfwWindowShouldClose(m_window.get());
}
void Window::bind_input_polling_to_window() const
{
     glengine::Input::m_window = m_window.get();
}
void Window::end_frame() const
{
     ImGui::EndFrame();// call instead of render when minimized.
     std::this_thread::sleep_for(std::chrono::milliseconds(500));
}
}// namespace glengine