//
// Created by pcvii on 12/9/2021.
//

#include "Application.hpp"
//#include "BatchRenderer.hpp"
#include "Event/EventDispatcher.hpp"
#include "FrameBuffer.hpp"
#include "FrameBufferRenderer.hpp"
#include "PixelBuffer.hpp"
#include "Renderer.hpp"
#include "test/LayerTests.hpp"
#include "TimeStep.hpp"

static glengine::Window *current_window = nullptr;
static bool              running        = true;
static bool              minimize       = false;
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
  auto                          last      = glengine::TimeStep::now();
  using namespace std::chrono_literals;
  std::size_t test_number = 0;
  while (running)
  {
    window->BeginFrame();// First thing you do on update;
    if (!minimize)
    {
      glengine::Renderer::Clear.Color({ 0.F, 0.F, 0.F, 0.F });
      glengine::Renderer::Clear();
      layers.OnImGuiUpdate();
      layers.OnUpdate(time_step);
      glengine::FrameBuffer fb(glengine::FrameBufferSpecification{
        .width  = current_window->ViewWindowData().frame_buffer_width,
        .height = current_window->ViewWindowData().frame_buffer_height });
      fb.Bind();
      glengine::Renderer::Clear();
      layers.OnRender();
      fb.UnBind();
      if (glengine::TimeStep::now() - last > glengine::TimeStep::duration(5s))
      {
        glengine::PixelBuffer pixel_buffer{ fb.Specification() };
        pixel_buffer(fb, fmt::format("test ({}).png", test_number++));
        while (pixel_buffer(&glengine::Texture::save))
          ;
        last = glengine::TimeStep::now();
      }
      fbr.Draw(fb);
      window->EndFrameRendered();// Last thing you do on render;
    }
    else
    {
      window->EndFrame();
    }
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
    GLCall{}(
      glViewport,
      GLint{ 0 },
      GLint{ 0 },
      static_cast<GLsizei>(
        Application::CurrentWindow()->ViewWindowData().frame_buffer_width),
      static_cast<GLsizei>(
        Application::CurrentWindow()->ViewWindowData().frame_buffer_height));
  }
}
float Get_Frame_Buffer_Aspect_Ratio()
{
  if (Application::CurrentWindow())
  {
    const auto &window_data = Application::CurrentWindow()->ViewWindowData();
    return static_cast<float>(window_data.frame_buffer_width)
           / static_cast<float>(window_data.frame_buffer_height);
  }
  return (16.F / 9.F);
}
