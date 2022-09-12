//
// Created by pcvii on 12/9/2021.
//

#include "Application.hpp"
#include <Event/EventDispatcher.hpp>
#include <test/LayerTests.hpp>
#include <TimeStep.hpp>

[[maybe_unused]] static constinit glengine::Window *GlobalCurrentWindow =
  nullptr;
static constinit ff_8::Fields                   *GlobalFields       = nullptr;
static constinit ff_8::MapHistoryData           *GlobalMapHistory   = nullptr;
static constinit ff_8::MimData                  *GlobalMim          = nullptr;
static constinit glengine::ImGuiViewPortPreview *GlobalPreview      = {};
static constinit bool                            GlobalRunning      = true;
static constinit bool                            GlobalMinimize     = false;
[[maybe_unused]] static ImVec2                   GlobalViewportSize = {};
[[maybe_unused]] static constinit glm::vec4      GlobalViewportMousePos = {};
static bool OnWindowClose(const glengine::event::WindowClose &)
{
  GlobalRunning = false;
  return true;
}
static bool OnWindowResize(const glengine::event::WindowResize &e)
{
  GlobalMinimize = e.width() == 0 or e.height() == 0;
  return true;
}

Application::Application(std::string title, int width, int height)
  : window(glengine::Window::create(glengine::Window::WindowData{
    .title          = std::move(title),
    .width          = std::move(width),
    .height         = std::move(height),
    .event_callback = [&](const glengine::event::Item &e) {
      const glengine::event::Dispatcher dispatcher = { e };
      //      [[maybe_unused]]const bool skip
      //            =(glengine::event::HasFlag(e.category(),glengine::event::Category::Mouse)
      //                         && ImGui::GetIO().WantCaptureMouse)
      //                        ||
      //                        (glengine::event::HasFlag(e.category(),glengine::event::Category::Keyboard)
      //                            && ImGui::GetIO().WantCaptureKeyboard);

      dispatcher.Dispatch<glengine::event::WindowClose>(&OnWindowClose);
      dispatcher.Dispatch<glengine::event::WindowResize>(&OnWindowResize);
      dispatcher.Dispatch<glengine::event::Reload>(
        [](const glengine::event::Reload &reload) -> bool {
          if (reload)
          {
            ReloadMimAndMap();
          }
          return true;
        });
      layers.on_event(e);
      local_preview.on_event(e);
      local_tile_display.on_event(e);
      //      if (skip)
      //      {
      //        return;
      //      }
      spdlog::debug("event::{}\t{}\t{}", e.name(), e.category_name(), e.data());
    } }))
{
  local_mim        = ff_8::MimData(local_fields);
  local_map        = ff_8::MapHistoryData(local_fields, local_mim);
  GlobalPreview    = &local_preview;
  GlobalFields     = &local_fields;
  GlobalMim        = &local_mim;
  GlobalMapHistory = &local_map;
  layers.emplace_layers(std::in_place_type_t<layer::Tests>{});
}
void Application::run() const
{
  set_current_window();
  const glengine::TimeStep time_step = {};
  // auto                          last      = glengine::TimeStep::now();
  using namespace std::chrono_literals;
  while (GlobalRunning)
  {
    window->begin_frame();// First thing you do on update;
    if (!GlobalMinimize)
    {
      glengine::Renderer::Clear.Color({ 0.F, 0.F, 0.F, 0.F });
      glengine::Renderer::Clear();


      window->render_dockspace();
#if 0
      static bool show_demo_window = true;
      if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
#else
      // constinit static std::size_t test_number = 0U;
      {
        float step = time_step;// takes the current time when you get a float.
        local_tile_display.on_update(step);
        local_preview.on_update(step);
        layers.on_im_gui_update();
        local_preview.on_im_gui_update();
        local_tile_display.on_im_gui_update();
        layers.on_update(step);
      }
      glengine::Renderer::Clear();
      layers.on_render();
      GlobalPreview->on_render();
      local_tile_display.on_render();
#endif
      window->end_frame_rendered();// Last thing you do on render;
    }
    else
    {
      window->end_frame();
    }
    window->update_view_ports();// for multi viewports run after render loop.
    std::cout << std::flush;
  }
  GlobalRunning = true;
}
void Application::set_current_window() const
{
  GlobalCurrentWindow = window.get();
}
const glengine::Window &GetWindow() noexcept
{
  return *GlobalCurrentWindow;
}
// const glengine::Window *Application::CurrentWindow()
//{
//   return GlobalCurrentWindow;
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
//       static_cast<GLint>(GlobalViewportSize.x),
//       static_cast<GLint>(GlobalViewportSize.y));
//   }
// }
// glm::vec4 GetViewPortMousePos() noexcept
//{
//   return GlobalViewportMousePos;
// }
// glm::vec2 GetFrameBufferDims()
//{
//
//   if (Application::CurrentWindow())
//   {
//     return { GlobalViewportSize.x, GlobalViewportSize.y };
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
//     return GlobalViewportSize.x / GlobalViewportSize.y;
//   }
//   return (16.F / 9.F);
// }
void ReloadMimAndMap()
{
  *GlobalMim        = ff_8::MimData(*GlobalFields);
  *GlobalMapHistory = ff_8::MapHistoryData(*GlobalFields, *GlobalMim);
}
const ff_8::Fields &GetFields() noexcept
{
  return *GlobalFields;
}
const ff_8::MapHistoryData &GetMapHistory() noexcept
{
  return *GlobalMapHistory;
}
const ff_8::MimData &GetMim() noexcept
{
  return *GlobalMim;
}
const glengine::ImGuiViewPortPreview &GetViewPortPreview() noexcept
{
  return *GlobalPreview;
}
