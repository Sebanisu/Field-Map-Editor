//
// Created by pcvii on 11/24/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTBATCHRENDERING_HPP
#define FIELD_MAP_EDITOR_TESTBATCHRENDERING_HPP
#include "ImGuiViewPortWindow.hpp"
#include "IndexBuffer.hpp"
#include "Shader.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"

namespace test
{
class TestBatchRendering
{
   public:
     TestBatchRendering();
     void on_update(float ts) const
     {
          m_imgui_viewport_window.on_update(ts);
     }
     void on_render() const;
     void on_im_gui_update() const;
     void on_event(const glengine::event::Item &e) const
     {
          m_imgui_viewport_window.on_event(e);
     }

   private:
     glengine::VertexBuffer        m_vertex_buffer         = {};
     glengine::IndexBuffer         m_index_buffer          = {};
     glengine::Shader              m_shader                = {};
     glengine::VertexArray         m_vertex_array          = {};
     mutable glm::vec3             view_offset             = { 0.F, 0.F, 0.F };
     glengine::ImGuiViewPortWindow m_imgui_viewport_window = { "Test Batch Rendering" };
     void                          render_frame_buffer() const;
     void                          set_uniforms() const;
};
}// namespace test

#endif// FIELD_MAP_EDITOR_TESTBATCHRENDERING_HPP
