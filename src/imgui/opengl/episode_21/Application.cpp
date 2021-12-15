//
// Created by pcvii on 12/9/2021.
//

#include "Application.hpp"
#include "Event/EventDispatcher.hpp"
#include "FrameBuffer.hpp"
#include "IndexBufferDynamic.hpp"
#include "Layer/LayerTests.hpp"
#include "Renderer.hpp"
#include "TimeStep.hpp"
#include "Vertex.hpp"
#include "VertexBuffer.hpp"

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
  const Renderer renderer  = {};
  const TimeStep time_step = {};
  while (running)
  {
    window->BeginFrame();// First thing you do on update;
    if (!minimize)
    {
      renderer.ClearColor(0.F, 0.F, 0.F, 0.F);
      renderer.Clear();
      layers.OnImGuiUpdate();
      layers.OnUpdate(time_step);
      FrameBuffer fb(FrameBufferSpecification{
        .width = current_window->Width(), .height = current_window->Height() });
      fb.Bind();
      renderer.Clear();
      layers.OnRender();
//      GLCall{}(glDrawBuffer, GL_FRONT);
      fb.UnBind();
       layers.OnRender();
            const auto ca = fb.GetColorAttachment();
            renderer.Draw(ca,VertexBuffer(CreateQuad({},{1.F,1.F,1.F,1.F},ca.ID(),{},{},{current_window->Width(),
            current_window->Height()})),IndexBufferDynamic(1));
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
