//
// Created by pcvii on 11/29/2021.
//
#include "TestBatchRenderingTexture2D.hpp"
#include "Application.hpp"
#include <glengine/Vertex.hpp>
#include <imgui_utils/ImGuiPushID.hpp>
static constinit bool FitWidth  = true;
static constinit bool FitHeight = true;
static constinit bool Preview   = false;
static_assert(glengine::Renderable<test::TestBatchRenderingTexture2D>);
test::TestBatchRenderingTexture2D::TestBatchRenderingTexture2D()
  : m_shader(
      std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{
     m_textures.emplace_back(
       std::filesystem::current_path() / "res" / "textures"
       / "mitchell-luo-q9ZiOzsMAhE-unsplash.png");
     m_textures.emplace_back(
       std::filesystem::current_path() / "res" / "textures" / "logo.png");
     m_textures.emplace_back(
       std::filesystem::current_path() / "res" / "textures"
       / "math-yDq60_c-g2E-unsplash.png");
     constexpr auto colors = std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F },
                                         glm::vec4{ 0.F, 1.F, 0.F, 1.F },
                                         glm::vec4{ 0.F, 0.F, 1.F, 1.F } };


     std::vector<glengine::Vertex> vertices{};
     vertices.reserve(12U);
     constexpr glm::vec3 offset = { -0.5F, -0.5F, 0.F };
     vertices += glengine::CreateQuad(
                   glm::vec3{ -4.0F, 0.F, 0.F } + offset, colors[0], 1)
                 + glengine::CreateQuad(
                   glm::vec3{ 0.F, 0.F, 0.F } + offset, colors[1], 2)
                 + glengine::CreateQuad(
                   glm::vec3{ 4.F, 0.F, 0.F } + offset, colors[2], 3);

     m_vertex_buffer           = glengine::VertexBuffer{ vertices };
     constexpr auto quad_size  = std::size(glengine::Quad{});
     const auto     quad_count = std::size(vertices) / quad_size;
     m_index_buffer
       = glengine::IndexBuffer{ glengine::QuadIndices(quad_count) };

     m_vertex_array.bind();
     m_vertex_array.push_back(m_vertex_buffer, glengine::Vertex::layout());
     m_shader.bind();
     m_shader.set_uniform("u_Tint", 1.F, 1.F, 1.F, 1.F);
}
void test::TestBatchRenderingTexture2D::on_render() const
{
     set_uniforms();
     m_imgui_viewport_window.on_render([this]() { render_frame_buffer(); });
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
void test::TestBatchRenderingTexture2D::on_im_gui_update() const
{
     constexpr float window_width = 16.F;
     const float     window_height
       = window_width / m_imgui_viewport_window.view_port_aspect_ratio();
     m_imgui_viewport_window.set_image_bounds({ window_width, window_height });

     constexpr float clamp_width  = window_width / 2.F - 1.F;
     const float     clamp_height = window_height / 2.F - 1.F;
     {
          const auto pop = imgui_utils::ImGuiPushId();

          ImGui::Checkbox("fit Height", &FitHeight);
          ImGui::Checkbox("fit Width", &FitWidth);
     }
     {
          const auto pop = imgui_utils::ImGuiPushId();
          if (ImGui::SliderFloat3(
                "View Offset", &view_offset.x, -clamp_width, clamp_width))
          {
               view_offset.y
                 = std::clamp(view_offset.y, -clamp_height, clamp_height);
          }
     }
     ImGui::Separator();
     m_imgui_viewport_window.on_im_gui_update();
}
void test::TestBatchRenderingTexture2D::set_uniforms() const
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
void test::TestBatchRenderingTexture2D::render_frame_buffer() const
{
     {
          std::vector<std::int32_t> slots{ 0 };
          slots.reserve(std::size(m_textures) + 1U);
          for (std::int32_t i{}; auto &texture : m_textures)
          {
               texture.bind(slots.emplace_back(1 + i));
               ++i;
          }
          m_shader.set_uniform("u_Textures", slots);
          glengine::Renderer::Draw(m_vertex_array, m_index_buffer);
     }
}
void test::TestBatchRenderingTexture2D::on_event(
  const glengine::event::Item &event) const
{
     m_imgui_viewport_window.on_event(event);
}
void test::TestBatchRenderingTexture2D::on_update(float ts) const
{
     m_imgui_viewport_window.on_update(ts);
     m_imgui_viewport_window.fit(FitWidth, FitHeight);
}
