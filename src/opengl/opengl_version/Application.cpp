//
// Created by pcvii on 12/9/2021.
//

#include "Application.hpp"
#include "Event/EventDispatcher.hpp"
#include "ff8/Fields.hpp"
#include "FrameBuffer.hpp"
#include "FrameBufferRenderer.hpp"
#include "Renderer.hpp"
#include "test/LayerTests.hpp"
#include "TimeStep.hpp"
#include <ff8/ImGuiTileDisplayWindow.hpp>

static glengine::Window        *current_window     = nullptr;
static ff8::Fields             *fields             = nullptr;
glengine::ImGuiViewPortPreview *preview            = {};
static constinit bool           running            = true;
static constinit bool           minimize           = false;
static ImVec2                   viewport_size      = {};
static constinit glm::vec4      viewport_mouse_pos = {};
static bool OnWindowClose(const glengine::Event::WindowClose &)
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
      const bool skip
            =(glengine::Event::HasFlag(e.category(),glengine::Event::Category::Mouse)
                         && ImGui::GetIO().WantCaptureMouse)
                        ||
                        (glengine::Event::HasFlag(e.category(),glengine::Event::Category::Keyboard)
                            && ImGui::GetIO().WantCaptureKeyboard);

      dispatcher.Dispatch<glengine::Event::WindowClose>(&OnWindowClose);
      dispatcher.Dispatch<glengine::Event::WindowResize>(&OnWindowResize);
      layers.OnEvent(e);
      local_preview.OnEvent(e);
      local_tile_display.OnEvent(e);
      if (skip)
      {
        return;
      }
      fmt::print("Event::{}\t{}\t{}\n", e.Name(), e.CategoryName(), e.Data());
    } }))
{
  layers.emplace_layers(std::in_place_type_t<Layer::Tests>{});
  preview = &local_preview;
  fields  = &local_fields;
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
      {
        float step = time_step;// takes the current time when you get a float.
        local_tile_display.OnUpdate(step);
        local_preview.OnUpdate(step);
        layers.OnImGuiUpdate();
        local_preview.OnImGuiUpdate();
        local_tile_display.OnImGuiUpdate();
        layers.OnUpdate(step);
      }
      glengine::Renderer::Clear();
      layers.OnRender();
      preview->OnRender();
      local_tile_display.OnRender();
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
// const glengine::Window *Application::CurrentWindow()
//{
//   return current_window;
// }
//
// void RestoreViewPortToFrameBuffer()
//{
//   if (Application::CurrentWindow())
//   {
//     //    GLCall{}(
//     //      glViewport,
//     //      GLint{ 0 },
//     //      GLint{ 0 },
//     //      static_cast<GLsizei>(
//     // Application::CurrentWindow()->ViewWindowData().frame_buffer_width),
//     //      static_cast<GLsizei>(
//     // Application::CurrentWindow()->ViewWindowData().frame_buffer_height));
//     GLCall{}(
//       glViewport,
//       GLint{},
//       GLint{},
//       static_cast<GLint>(viewport_size.x),
//       static_cast<GLint>(viewport_size.y));
//   }
// }
// glm::vec4 GetViewPortMousePos() noexcept
//{
//   return viewport_mouse_pos;
// }
// glm::vec2 GetFrameBufferDims()
//{
//
//   if (Application::CurrentWindow())
//   {
//     return { viewport_size.x, viewport_size.y };
//   }
//   return { 16.F, 9.F };
// }
// float GetFrameBufferAspectRatio()
//{
//   if (Application::CurrentWindow())
//   {
//     //    const auto &window_data =
//     //    Application::CurrentWindow()->ViewWindowData(); return
//     //    static_cast<float>(window_data.frame_buffer_width)
//     //           / static_cast<float>(window_data.frame_buffer_height);
//     return viewport_size.x / viewport_size.y;
//   }
//   return (16.F / 9.F);
// }
const ff8::Fields &GetFields() noexcept
{
  return *fields;
}
const glengine::ImGuiViewPortPreview &GetViewPortPreview() noexcept
{
  return *preview;
}
