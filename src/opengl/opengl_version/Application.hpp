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
#include <ff8/ImGuiTileDisplayWindow.hpp>
class Application
{
public:
  Application() = default;
  Application(std::string Title, int width, int height);
  void Run() const;
  // static const glengine::Window *CurrentWindow();
  void SetCurrentWindow() const;

private:
  // window must be valid while other opengl objects exists.
  std::unique_ptr<glengine::Window> window = nullptr;
  glengine::Layer::Stack            layers = {};

  mutable glengine::ImGuiViewPortPreview    local_preview{};
  mutable ff8::ImGuiTileDisplayWindow       local_tile_display{};
  mutable ff8::Fields                       local_fields{};
};
const ff8::Fields                    &GetFields() noexcept;
const glengine::ImGuiViewPortPreview &GetViewPortPreview() noexcept;
// glm::vec4 GetViewPortMousePos() noexcept ;
// glm::vec2 GetFrameBufferDims();
// void      RestoreViewPortToFrameBuffer();
// float     GetFrameBufferAspectRatio();
#endif// FIELD_MAP_EDITOR_APPLICATION_HPP
