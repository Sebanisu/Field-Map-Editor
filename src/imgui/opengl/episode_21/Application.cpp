//
// Created by pcvii on 12/9/2021.
//

#include "Application.hpp"
#include "EventDispatcher.hpp"
#include "LayerTests.hpp"
#include "Renderer.hpp"

static bool running  = true;
static bool minimize = false;
static bool OnWindowClose(const Event::WindowClose &)
{
  running = false;
  return true;
}
static bool OnWindowResize(const Event::WindowResize &e)
{
  minimize = e.Width() == 0 or e.Height() == 0;
  return true;
}

Application::Application(std::string Title, int width, int height)
  : window(Window::Create(Window::WindowData{
    .Title          = std::move(Title),
    .width          = std::move(width),
    .height         = std::move(height),
    .event_callback = [&](const Event::Item &e) {
      const Event::Dispatcher dispatcher = { e };
      const bool skip =(Event::HasFlag(e.category(),Event::Category::Mouse)
                   && ImGui::GetIO().WantCaptureMouse)
                  || (Event::HasFlag(e.category(),Event::Category::Keyboard)
                      && ImGui::GetIO().WantCaptureKeyboard);
      if (skip)
      {
        return;
      }
      dispatcher.Dispatch<Event::WindowClose>(&OnWindowClose);
      dispatcher.Dispatch<Event::WindowResize>(&OnWindowResize);
      layers.OnEvent(e);
      fmt::print("Event::{}\t{}\t{}\n", e.Name(), e.CategoryName(), e.Data());
    } }))
{
  layers.emplace_layers(std::in_place_type_t<Layer::Tests>{});
}
void Application::Run() const
{
  SetCurrentWindow();
  const Renderer renderer = {};
  while (running)
  {
    window->BeginFrame();// First thing you do on update;
    if (!minimize)
    {
      renderer.Clear();
      layers.OnImGuiUpdate();
      layers.OnUpdate({});
      layers.OnRender();
      window->EndFrameRendered();// Last thing you do on render;
    }
    else
    {
      window->EndFrame();
    }
  }
  running = true;
}
void Application::SetCurrentWindow() const
{
  current_window = window.get();
}
const Window *Application::CurrentWindow()
{
  return current_window;
}
