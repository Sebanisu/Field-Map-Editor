//
// Created by pcvii on 12/9/2021.
//

#include "Application.hpp"
//#include "BatchRenderer.hpp"
#include "Event/EventDispatcher.hpp"
#include "FrameBuffer.hpp"
#include "FrameBufferRenderer.hpp"
#include "ImGuiPushID.hpp"
#include "PixelBuffer.hpp"
#include "Renderer.hpp"
#include "test/LayerTests.hpp"
#include "TimeStep.hpp"

static glengine::Window *current_window = nullptr;
static constinit bool    running        = true;
static constinit bool    minimize       = false;
static ImVec2            viewport_size  = {};
static bool              OnWindowClose(const glengine::Event::WindowClose &)
{
  running = false;
  return true;
}
static bool OnWindowResize(const glengine::Event::WindowResize &e)
{
  minimize = e.Width() == 0 or e.Height() == 0;
  return true;
}

Application::Application(std::string Title, int width, int height)
  : window(glengine::Window::Create(glengine::Window::WindowData{
    .Title          = std::move(Title),
    .width          = std::move(width),
    .height         = std::move(height),
    .event_callback = [&](const glengine::Event::Item &e) {
      const glengine::Event::Dispatcher dispatcher = { e };
      const bool skip =(glengine::Event::HasFlag(e.category(),glengine::Event::Category::Mouse)
                   && ImGui::GetIO().WantCaptureMouse)
                  || (glengine::Event::HasFlag(e.category(),glengine::Event::Category::Keyboard)
                      && ImGui::GetIO().WantCaptureKeyboard);
      if (skip)
      {
        return;
      }
      dispatcher.Dispatch<glengine::Event::WindowClose>(&OnWindowClose);
      dispatcher.Dispatch<glengine::Event::WindowResize>(&OnWindowResize);
      layers.OnEvent(e);
      fmt::print("Event::{}\t{}\t{}\n", e.Name(), e.CategoryName(), e.Data());
    } }))
{
  layers.emplace_layers(std::in_place_type_t<Layer::Tests>{});
}
void Application::Run() const
{
  SetCurrentWindow();
  const glengine::TimeStep      time_step = {};
  glengine::FrameBufferRenderer fbr       = {};
  // auto                          last      = glengine::TimeStep::now();
  using namespace std::chrono_literals;
  glengine::FrameBuffer fb;// needed to be inscope somewhere because texture was
                           // being erased before it was drawn.
  while (running)
  {
    window->BeginFrame();// First thing you do on update;
    if (!minimize)
    {
      glengine::Renderer::Clear.Color({ 0.F, 0.F, 0.F, 0.F });
      glengine::Renderer::Clear();

      window->RenderDockspace();
#if 0
      static bool show_demo_window = true;
      if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
#else
      // constinit static std::size_t test_number = 0U;
      layers.OnImGuiUpdate();
      layers.OnUpdate(time_step);

      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.F, 0.F));
      if (ImGui::Begin("GameWindow"))
      {
        bool parent_window_hovered = ImGui::IsWindowHovered();
        bool parent_window_focused = ImGui::IsWindowFocused();
        // Using a Child allow to fill all the space of the window.
        // It also allows customization
        ImGui::BeginChild("GameRender");
        bool window_hovered = ImGui::IsWindowHovered();
        bool window_focused = ImGui::IsWindowFocused();
        // Get the size of the child (i.e. the whole draw size of the windows).
        viewport_size =
          ImGui::GetContentRegionAvail();// ImGui::GetWindowSize();
        if (
          !fb || fb.Specification().height != static_cast<int>(viewport_size.y)
          || fb.Specification().width != static_cast<int>(viewport_size.x))
          fb = glengine::FrameBuffer(glengine::FrameBufferSpecification{
            .width = static_cast<int>(
              viewport_size
                .x),// current_window->ViewWindowData().frame_buffer_width,
            .height = static_cast<int>(
              viewport_size
                .y)// current_window->ViewWindowData().frame_buffer_height
          });
        fb.Bind();
        glengine::Renderer::Clear();
        layers.OnRender();
        fb.UnBind();
        // Because I use the texture from OpenGL, I need to invert the V from
        // the UV.
        const auto convert = [](uint32_t r_id) -> ImTextureID {
          return reinterpret_cast<ImTextureID>(static_cast<intptr_t>(r_id));
        };
        auto       tmp  = convert(fb.GetColorAttachment().ID());


        const auto cPos = ImGui::GetCursorPos();
        ImGui::SetItemAllowOverlap();
        const auto color = ImVec4(0.F, 0.F, 0.F, 0.F);
        ImGui::PushStyleColor(ImGuiCol_Button, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
        if (ImGui::ImageButton(
              tmp,
              ImVec2(
                static_cast<float>(fb.Specification().width),
                static_cast<float>(fb.Specification().height)),
              ImVec2(0, 1),
              ImVec2(1, 0),
              0))
        {
          glengine::Input::SetViewPortFocused();
        }
        bool button_hovered   = ImGui::IsItemHovered();
        bool button_focused   = ImGui::IsItemFocused();
        bool button_activated = ImGui::IsItemActivated();
        if (button_focused || window_focused || parent_window_focused)
        {
          glengine::Input::SetViewPortFocused();
        }
        else
        {
          glengine::Input::SetViewPortNotFocused();
        }
        if (button_hovered || window_hovered || parent_window_hovered)
        {
          glengine::Input::SetViewPortHovered();
        }
        else
        {
          glengine::Input::SetViewPortNotHovered();
        }
        ImGui::PopStyleColor(3);
        ImGui::SetCursorPos(cPos);
        ImGui::Text(
          "%s",
          fmt::format(
            "Window Frame Buffer - Width {}, Height: {}\n"
            "ViewPort - Hovered: {}, Focused: {}, Width: {}, Height: {}, "
            "Texture ID: {}\nButton - Hovered: {}, Focused: {}, Activated: "
            "{}\nWindow - Hovered: {}, Focused: {}\nParent Window - Hovered: "
            "{}, Focused: {}",
            window->ViewWindowData().frame_buffer_width,
            window->ViewWindowData().frame_buffer_height,
            glengine::Input::ViewPortHovered(),
            glengine::Input::ViewPortFocused(),
            viewport_size.x,
            viewport_size.y,
            tmp,
            button_hovered,
            button_focused,
            button_activated,
            window_hovered,
            window_focused,
            parent_window_hovered,
            parent_window_focused)
            .c_str());
        //        if(ImGui::InvisibleButton("##dummy",ImVec2(
        //                                     static_cast<float>(fb.Specification().width),
        //                                     static_cast<float>(fb.Specification().height))))
        //        {
        //
        //        }

        //        fmt::print("Focused = {}, Hovered =
        //        {}\n",ImGui::IsAnyItemFocused(),ImGui::IsItemHovered());
        //        glengine::Window::ViewPortFocused(ImGui::IsWindowFocused());
        //        glengine::Window::ViewPortFocused(ImGui::IsWindowHovered());
        //        if (glengine::TimeStep::now() - last >
        //        glengine::TimeStep::duration(5s))
        //        {
        //          glengine::PixelBuffer pixel_buffer{ fb.Specification() };
        //          pixel_buffer(fb, fmt::format("test ({}).png",
        //          test_number++)); while
        //          (pixel_buffer(&glengine::Texture::save))
        //            ;
        //          last = glengine::TimeStep::now();
        //        }
        // fbr.Draw(fb); //render frame buffer to screen.
        ImGui::EndChild();
      }
      ImGui::End();
      ImGui::PopStyleVar();

#endif
      window->EndFrameRendered();// Last thing you do on render;
    }
    else
    {
      window->EndFrame();
    }
    window->UpdateViewPorts();// for multi viewports run after render loop.
    std::cout << std::flush;
  }
  running = true;
}
void Application::SetCurrentWindow() const
{
  current_window = window.get();
}
const glengine::Window *Application::CurrentWindow()
{
  return current_window;
}

void RestoreViewPortToFrameBuffer()
{
  if (Application::CurrentWindow())
  {
    //    GLCall{}(
    //      glViewport,
    //      GLint{ 0 },
    //      GLint{ 0 },
    //      static_cast<GLsizei>(
    //        Application::CurrentWindow()->ViewWindowData().frame_buffer_width),
    //      static_cast<GLsizei>(
    //        Application::CurrentWindow()->ViewWindowData().frame_buffer_height));
    GLCall{}(
      glViewport,
      GLint{},
      GLint{},
      static_cast<GLint>(viewport_size.x),
      static_cast<GLint>(viewport_size.y));
  }
}
glm::vec2 GetFrameBufferDims()
{

  if (Application::CurrentWindow())
  {
    return {viewport_size.x, viewport_size.y};
  }
  return {16.F,9.F};
}
float GetFrameBufferAspectRatio()
{
  if (Application::CurrentWindow())
  {
    //    const auto &window_data =
    //    Application::CurrentWindow()->ViewWindowData(); return
    //    static_cast<float>(window_data.frame_buffer_width)
    //           / static_cast<float>(window_data.frame_buffer_height);
    return viewport_size.x / viewport_size.y;
  }
  return (16.F / 9.F);
}
