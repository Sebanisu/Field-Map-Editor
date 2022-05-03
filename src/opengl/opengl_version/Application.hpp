//
// Created by pcvii on 12/9/2021.
//

#ifndef FIELD_MAP_EDITOR_APPLICATION_HPP
#define FIELD_MAP_EDITOR_APPLICATION_HPP
#include "ff8/Fields.hpp"
#include "ImGuiViewPortPreview.hpp"
#include "Layer/LayerStack.hpp"
#include "scope_guard.hpp"
#include "Window.hpp"
class Application
{
public:
  Application() = default;
  Application(std::string Title, int width, int height);
  void                           Run() const;
  //static const glengine::Window *CurrentWindow();
  void                           SetCurrentWindow() const;

private:
  // window must be valid while other opengl objects exists.
  std::unique_ptr<glengine::Window> window = nullptr;
  glengine::Layer::Stack            layers = {};
};
const ff8::Fields & GetFields() noexcept;
const  glengine::ImGuiViewPortPreview &GetViewPortPreview() noexcept;
//glm::vec4 GetViewPortMousePos() noexcept ;
//glm::vec2 GetFrameBufferDims();
//void      RestoreViewPortToFrameBuffer();
//float     GetFrameBufferAspectRatio();
#endif// FIELD_MAP_EDITOR_APPLICATION_HPP
