//
// Created by pcvii on 12/9/2021.
//

#ifndef FIELD_MAP_EDITOR_APPLICATION_HPP
#define FIELD_MAP_EDITOR_APPLICATION_HPP
#include <ff8/Fields.hpp>
#include <ff8/ImGuiTileDisplayWindow.hpp>
#include <glengine/Layer/LayerStack.hpp>
#include <ImGuiViewPortPreview.hpp>
#include <Window.hpp>
class Application
{
   public:
     Application() = default;
     Application(
       std::string,
       int,
       int);
     void run() const;
     // static const glengine::Window *CurrentWindow();
     void set_current_window() const;

   private:
     // window must be valid while other opengl objects exists.
     std::unique_ptr<glengine::Window>      window = nullptr;
     glengine::Layer::Stack                 layers = {};

     mutable glengine::ImGuiViewPortPreview local_preview{};
     mutable ff_8::ImGuiTileDisplayWindow   local_tile_display{};
     mutable ff_8::Fields                   local_fields{};
     mutable ff_8::MimData                  local_mim = {};
     mutable ff_8::MapHistoryData           local_map = {};
};
void                                      ReloadMimAndMap();
[[nodiscard]] const ff_8::Fields         &GetFields() noexcept;
[[nodiscard]] const ff_8::MapHistoryData &GetMapHistory() noexcept;
[[nodiscard]] const ff_8::MimData        &GetMim() noexcept;
[[nodiscard]] const glengine::Window     &GetWindow() noexcept;
const glengine::ImGuiViewPortPreview     &GetViewPortPreview() noexcept;
// glm::vec4 GetViewPortMousePos() noexcept ;
// glm::vec2 GetFrameBufferDims();
// void      RestoreViewPortToFrameBuffer();
// float     GetFrameBufferAspectRatio();
#endif// FIELD_MAP_EDITOR_APPLICATION_HPP
