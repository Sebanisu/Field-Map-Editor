//
// Created by pcvii on 11/29/2021.
//

#ifndef FIELD_MAP_EDITOR_TESTBATCHQUADS_HPP
#define FIELD_MAP_EDITOR_TESTBATCHQUADS_HPP
#include "ImGuiViewPortWindow.hpp"
#include <glengine/IndexBufferDynamic.hpp>
#include <glengine/Shader.hpp>
#include <glengine/Texture.hpp>
#include <glengine/VertexArray.hpp>
#include <glengine/VertexBufferDynamic.hpp>


namespace test
{
class TestBatchQuads
{
   public:
     TestBatchQuads();


     void                         on_update(float) const;
     void                         on_render() const;
     void                         on_im_gui_update() const;
     void                         on_event(const glengine::event::Item &) const;
     void                         generate_quads() const;

     static constexpr std::size_t quad_count = { 100U };
     static constexpr std::size_t vert_count = { quad_count * 4U };

   private:
     void                                     draw() const;
     glengine::VertexBufferDynamic            m_vertex_buffer = { quad_count };
     glengine::IndexBufferDynamic             m_index_buffer  = { quad_count };
     mutable glengine::IndexBufferDynamicSize index_buffer_size = {};
     glengine::Shader                         m_shader          = {};
     glengine::VertexArray                    m_vertex_array    = {};
     mutable glm::vec3                        view_offset = { 0.F, 0.F, 0.F };
     mutable std::array<int, 2U>              m_count     = { 100, 100 };
     glengine::Texture                        m_blank
       = { (std::numeric_limits<std::uint32_t>::max)() };
     glengine::ImGuiViewPortWindow m_imgui_viewport_window
       = { "Test Batch Quads" };
     void set_uniforms() const;
};
}// namespace test
#endif// FIELD_MAP_EDITOR_TESTBATCHQUADS_HPP
