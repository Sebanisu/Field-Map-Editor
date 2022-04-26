//
// Created by pcvii on 12/9/2021.
//

#ifndef MYPROJECT_APPLICATION_HPP
#define MYPROJECT_APPLICATION_HPP
#include "Layer/LayerStack.hpp"
#include "scope_guard.hpp"
#include "Window.hpp"
class Application
{
public:
  Application() = default;
  Application(std::string Title, int width, int height);
  void                           Run() const;
  static const glengine::Window *CurrentWindow();
  void                           SetCurrentWindow() const;

private:
  // window must be valid while other opengl objects exists.
  std::unique_ptr<glengine::Window> window = nullptr;
  glengine::Layer::Stack            layers = {};
};
glm::vec4 GetViewPortMousePos() noexcept ;
glm::vec2 GetFrameBufferDims();
void      RestoreViewPortToFrameBuffer();
float     GetFrameBufferAspectRatio();
#endif// MYPROJECT_APPLICATION_HPP
