//
// Created by pcvii on 11/29/2021.
//
#include "TestBatchRenderingTexture2DDynamic.hpp"
#include "Application.hpp"
#include "ImGuiPushID.hpp"
#include "Renderer.hpp"
#include "Vertex.hpp"
static constinit bool FitWidth  = true;
static constinit bool FitHeight = true;
static constinit bool Preview   = false;
static_assert(glengine::Renderable<test::TestBatchRenderingTexture2DDynamic>);
test::TestBatchRenderingTexture2DDynamic::TestBatchRenderingTexture2DDynamic()
  : m_shader(std::filesystem::current_path() / "res" / "shader" / "basic3.shader")
{
     m_textures.emplace_back(std::filesystem::current_path() / "res" / "textures" / "mitchell-luo-q9ZiOzsMAhE-unsplash.png");
     m_textures.emplace_back(std::filesystem::current_path() / "res" / "textures" / "logo.png");
     m_textures.emplace_back(std::filesystem::current_path() / "res" / "textures" / "math-yDq60_c-g2E-unsplash.png");

     m_vertex_array.bind();
     m_vertex_array.push_back(m_vertex_buffer, glengine::Vertex::layout());
     m_shader.bind();
     m_shader.set_uniform("u_Tint", 1.F, 1.F, 1.F, 1.F);
}
void test::TestBatchRenderingTexture2DDynamic::on_update(float ts) const
{
     constexpr auto colors =
       std::array{ glm::vec4{ 1.F, 0.F, 0.F, 1.F }, glm::vec4{ 0.F, 1.F, 0.F, 1.F }, glm::vec4{ 0.F, 0.F, 1.F, 1.F } };
     std::vector<glengine::Vertex> vertices{};
     vertices.reserve(12U);
     const auto offset = glm::vec3{ -0.5F, -0.5F, 0.F };
     vertices += glengine::CreateQuad(model_offset_1 + offset, colors[0], 1) + glengine::CreateQuad(model_offset_2 + offset, colors[1], 2)
                 + glengine::CreateQuad(model_offset_3 + offset, colors[2], 3);
     m_imgui_viewport_window.on_update(ts);
     m_imgui_viewport_window.fit(FitWidth, FitHeight);
     index_buffer_size = m_vertex_buffer.update(vertices);
}
void test::TestBatchRenderingTexture2DDynamic::on_render() const
{
     set_uniforms();
     m_imgui_viewport_window.on_render([this]() { render_frame_buffer(); });
     GetViewPortPreview().on_render(m_imgui_viewport_window, [this]() {
          const auto pop_preview = glengine::ScopeGuard([]() { Preview = false; });
          Preview                = true;
          set_uniforms();
          render_frame_buffer();
     });
}
void test::TestBatchRenderingTexture2DDynamic::on_im_gui_update() const
{

     const float window_width  = 16.F;
     const float window_height = (window_width / m_imgui_viewport_window.view_port_aspect_ratio());
     m_imgui_viewport_window.set_image_bounds({ window_width, window_height });
     const float clamp_width  = window_width / 2.F - 1.F;
     const float clamp_height = window_height / 2.F - 1.F;
     {
          const auto pop = glengine::ImGuiPushId();

          ImGui::Checkbox("fit Height", &FitHeight);
          ImGui::Checkbox("fit Width", &FitWidth);
     }
     {
          const auto pop = glengine::ImGuiPushId();
          if (ImGui::SliderFloat2("View Offset", &view_offset.x, -clamp_width, clamp_width))
          {
               view_offset.y = std::clamp(view_offset.y, -clamp_height, clamp_height);
          }
     }
     {
          const auto pop_2 = glengine::ImGuiPushId();
          if (ImGui::SliderFloat2("Model Offset 1", &model_offset_1.x, -clamp_width, clamp_width))
          {
               model_offset_1.y = std::clamp(model_offset_1.y, -clamp_height, clamp_height);
          }
     }
     {
          const auto pop_3 = glengine::ImGuiPushId();
          if (ImGui::SliderFloat2("Model Offset 2", &model_offset_2.x, -clamp_width, clamp_width))
          {
               model_offset_2.y = std::clamp(model_offset_2.y, -clamp_height, clamp_height);
          }
     }
     {
          const auto pop_4 = glengine::ImGuiPushId();
          if (ImGui::SliderFloat2("Model Offset 3", &model_offset_3.x, -clamp_width, clamp_width))
          {
               model_offset_3.y = std::clamp(model_offset_3.y, -clamp_height, clamp_height);
          }
     }
     ImGui::Separator();
     m_imgui_viewport_window.on_im_gui_update();
}
void test::TestBatchRenderingTexture2DDynamic::render_frame_buffer() const
{

     // const auto view = glm::translate(glm::mat4{ 1.F }, view_offset);
     {
          std::vector<std::int32_t> slots{ 0 };
          slots.reserve(std::size(m_textures) + 1U);
          for (std::int32_t i{}; auto &texture : m_textures)
          {
               texture.bind(slots.emplace_back(1 + i));
               ++i;
          }
          m_shader.set_uniform("u_Textures", slots);
          glengine::Renderer::Draw(index_buffer_size, m_vertex_array, m_index_buffer);
     }
}
void test::TestBatchRenderingTexture2DDynamic::set_uniforms() const
{
     const glm::mat4 mvp = [&]() {
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
