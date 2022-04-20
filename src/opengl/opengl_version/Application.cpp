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
      constinit static std::size_t test_number = 0U;
      layers.OnImGuiUpdate();
      layers.OnUpdate(time_step);


      ImGui::Begin("GameWindow");
      {
        // Using a Child allow to fill all the space of the window.
        // It also alows customization
        ImGui::BeginChild("GameRender");
        // Get the size of the child (i.e. the whole draw size of the windows).
        ImVec2 wsize = ImGui::GetContentRegionAvail();// ImGui::GetWindowSize();
        fmt::print("ViewPort = {}, {}\n", wsize.x, wsize.y);
        if(!fb || fb.Specification().height != static_cast<int>(wsize.y) || fb.Specification().width != wsize.x)
        fb = glengine::FrameBuffer(glengine::FrameBufferSpecification{
          .width = static_cast<int>(
            wsize.x),// current_window->ViewWindowData().frame_buffer_width,
          .height = static_cast<int>(
            wsize.y)// current_window->ViewWindowData().frame_buffer_height
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
        auto tmp = convert(fb.GetColorAttachment().ID());
        fmt::print("{}\n", tmp);
        ImGui::Image(
          tmp,
          ImVec2(
            static_cast<float>(fb.Specification().width),
            static_cast<float>(fb.Specification().height)),
          ImVec2(0, 1),
          ImVec2(1, 0));
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
        //fbr.Draw(fb); //render frame buffer to screen.
        ImGui::EndChild();
      }
      ImGui::End();

#endif
      window->EndFrameRendered();// Last thing you do on render;
      window->UpdateViewPorts();
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
