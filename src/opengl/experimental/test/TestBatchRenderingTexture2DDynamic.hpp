//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTBATCHRENDERINGTEXTURE2DDYNAMIC_HPP
#define FIELD_MAP_EDITOR_TESTBATCHRENDERINGTEXTURE2DDYNAMIC_HPP
#include "IndexBufferDynamic.hpp"
#include "Shader.hpp"

#include "ImGuiViewPortWindow.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBufferDynamic.hpp"


namespace test
{
class TestBatchRenderingTexture2DDynamic
{
   public:
     TestBatchRenderingTexture2DDynamic();
     void on_update(float) const;
     void on_render() const;
     void on_im_gui_update() const;
     void on_event(const glengine::event::Item &event) const
     {
          m_imgui_viewport_window.on_event(event);
     }


   private:
     void                                     render_frame_buffer() const;
     void                                     set_uniforms() const;
     glengine::VertexBufferDynamic            m_vertex_buffer   = { 1000 };
     glengine::IndexBufferDynamic             m_index_buffer    = { 1000 };
     mutable glengine::IndexBufferDynamicSize index_buffer_size = {};
     glengine::Shader                         m_shader          = {};
     glengine::VertexArray                    m_vertex_array    = {};
     std::vector<glengine::Texture>           m_textures        = {};
     mutable glm::vec3                        view_offset = { 0.F, 0.F, 0.F };
     mutable glm::vec3             model_offset_1         = { -4.F, 0.F, 0.F };
     mutable glm::vec3             model_offset_2         = { 0.F, 0.F, 0.F };
     mutable glm::vec3             model_offset_3         = { 4.F, 0.F, 0.F };
     glengine::ImGuiViewPortWindow m_imgui_viewport_window
       = { "Test Batch Rendering w/ Texture2D Dynamic" };
};

}// namespace test
#endif// FIELD_MAP_EDITOR_TESTBATCHRENDERINGTEXTURE2DDYNAMIC_HPP
