//
// Created by pcvii on 11/29/2021.
//
#include "TestBatchRendering.hpp"
#include "Application.hpp"
#include <glengine/Vertex.hpp>
#include <imgui_utils/ImGuiPushID.hpp>
static constinit bool Preview = false;
static_assert(glengine::Renderable<test::TestBatchRendering>);
void test::TestBatchRendering::on_im_gui_update() const
{
     constexpr float window_width = 16.F;
     float           window_height
       = window_width / m_imgui_viewport_window.view_port_aspect_ratio();
     m_imgui_viewport_window.set_image_bounds({ window_width, window_height });
     constexpr float clamp_width  = window_width / 2.F - 1.F;
     const float     clamp_height = window_height / 2.F - 1.F;
     {
          const auto pop = imgui_utils::ImGuiPushId();
          if (ImGui::SliderFloat2(
                "View Offset", &view_offset.x, -clamp_width, clamp_width))
          {
               view_offset.y
                 = std::clamp(view_offset.y, -clamp_height, clamp_height);
          }
     }
}
void test::TestBatchRendering::on_render() const
{
     m_imgui_viewport_window.on_render(
       [this]()
       {
            set_uniforms();
            render_frame_buffer();
       });

     GetViewPortPreview().on_render(
       m_imgui_viewport_window,
       [this]()
       {
            const auto pop_preview
              = glengine::ScopeGuard([]() { Preview = false; });
            Preview = true;
            set_uniforms();
            render_frame_buffer();
       });
}
void test::TestBatchRendering::set_uniforms() const
{
     const glm::mat4 mvp = [&]()
     {
          if (Preview)
          {
               return m_imgui_viewport_window.preview_view_projection_matrix();
          }
          return m_imgui_viewport_window.view_projection_matrix();
     }();
     m_shader.bind();
     m_shader.set_uniform("u_MVP", glm::translate(mvp, view_offset));
     m_shader.set_uniform("u_Tint", 1.F, 1.F, 1.F, 1.F);
}
void test::TestBatchRendering::render_frame_buffer() const
{

     glengine::Renderer::Draw(m_vertex_array, m_index_buffer);
}

test::TestBatchRendering::TestBatchRendering()
  : m_shader(
      std::filesystem::current_path() / "res" / "shader" / "basic2.shader")
{
     constexpr auto colors        = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                         glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                         glm::vec4{ 0.F, 0.F, 1.F, 1.F } };
     constexpr auto vertices_init = std::array{
          glengine::Vertex{ { -0.5F, -0.5F, 0.F }, colors[0] },// 0
          glengine::Vertex{ { 0.5F, -0.5F, 0.F }, colors[0] }, // 1
          glengine::Vertex{ { 0.5F, 0.5F, 0.F }, colors[0] },  // 2
          glengine::Vertex{ { -0.5F, 0.5F, 0.F }, colors[0] }, // 3
     };
     [[maybe_unused]] constexpr auto indices_init = std::array{
          // clang-format off
    0U, 1U, 2U, // 0
    2U, 3U, 0U  // 1
          // clang-format on
     };
     auto vertices = std::vector(vertices_init.begin(), vertices_init.end());
     //    auto translate =
     //      glm::translate(glm::mat4{ 1.F }, glm::vec3{ 2.F, 0.F, 0.F });
     for (int i = -1; const auto &color : colors)
     {
          std::ranges::transform(
            vertices_init, std::back_inserter(vertices),
            [&i, &color](glengine::Vertex vertex)
            {
                 vertex.location.x += 2.F * static_cast<float>(i);
                 vertex.color = color;
                 return vertex;
            });
          ++i;
     }
     m_vertex_buffer    = glengine::VertexBuffer{ vertices };
     auto       indices = std::vector(indices_init.begin(), indices_init.end());
     const auto quad_count = std::size(vertices) / std::size(vertices_init);
     for (std::size_t i = 1U; i != quad_count; ++i)
          std::ranges::transform(
            indices_init.cbegin(), indices_init.cend(),
            std::back_inserter(indices),
            [&vertices_init, &i](std::uint32_t index)
            {
                 return static_cast<std::uint32_t>(
                   index + (std::size(vertices_init) * i));
            });
     m_index_buffer = glengine::IndexBuffer{ indices };


     m_vertex_array.bind();
     m_vertex_array.push_back(m_vertex_buffer, glengine::Vertex::layout());
     m_shader.bind();
     m_shader.set_uniform("u_Tint", 1.F, 1.F, 1.F, 1.F);
     // m_shader.set_uniform("u_Texture", 0);
}
// #include "TestBatchRendering.hpp"