//
// Created by pcvii on 3/7/2023.
//

#include "batch.hpp"
struct as_string
{
     template<typename T>
     std::string operator()(const T &value) const
     {
          return fmt::format("{}", value);
     }
};
void batch::combo_input_type(int &imgui_id)
{
     static const auto values = std::array{ input_types::mim, input_types::deswizzle, input_types::swizzle };
     if (fme::generic_combo(
           imgui_id, "Input Type", []() { return values; }, []() { return values | std::views::transform(as_string{}); }, m_input_type))
     {
     }
}
void batch::combo_output_type(int &imgui_id)
{
     static const auto values = std::array{ output_types::deswizzle, output_types::swizzle };
     if (fme::generic_combo(
           imgui_id, "Output Type", []() { return values; }, []() { return values | std::views::transform(as_string{}); }, m_output_type))
     {
     }
}
void batch::combo_compact_type(int &imgui_id)
{
     static const auto values = std::array{ compact_type::rows, compact_type::all, compact_type::map_order };
     if (fme::generic_combo(
           imgui_id,
           gui_labels::compact,
           []() { return values; },
           []() { return values | std::views::transform(as_string{}); },
           [&]() -> auto & { return m_compact_type; }))
     {
     }
}
void batch::combo_flatten_type(int &imgui_id)
{
     static const auto values = std::array{ flatten_type::bpp, flatten_type::palette, flatten_type::both };
     if (fme::generic_combo(
           imgui_id,
           gui_labels::flatten,
           []() { return values; },
           []() { return values | std::views::transform(as_string{}); },
           [&]() -> auto & { return m_flatten_type; }))
     {
     }
}
void batch::browse_input_path(int &imgui_id)
{
     browse_path(imgui_id, "Input Path", m_input_path_valid, m_input_path);
}
void batch::browse_output_path(int &imgui_id)
{
     browse_path(imgui_id, "Output Path", m_output_path_valid, m_output_path);
}
void batch::button_begin(int &imgui_id)
{
     const auto pop_id_right = scope_guard{ &ImGui::PopID };
     ImGui::PushID(++imgui_id);
     ImGui::BeginDisabled(
       (m_input_type == input_types::mim || (!m_input_path_valid && m_input_type != input_types::mim)) && !m_output_path_valid
       || !m_archives_group.operator bool());
     ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.5f, 0.0f, 1.0f));// Green
     ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.8f, 0.3f, 1.0f));// Light green hover
     ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.3f, 0.1f, 1.0f));// Dark green active
     if (ImGui::Button("Begin Batch Operation...", ImVec2{ ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeight() }))
     {
          m_fields_consumer = m_archives_group->fields();
          m_field.reset();
          m_lang_consumer.restart();
          m_coo.reset();
     }
     ImGui::PopStyleColor(3);
     ImGui::EndDisabled();
}
void batch::browse_path(int &imgui_id, std::string_view name, bool &valid_path, std::array<char, m_buffer_size> &path_buffer)
{
     const ImGuiStyle &style        = ImGui::GetStyle();
     const float       spacing      = style.ItemInnerSpacing.x;
     const float       button_size  = ImGui::GetFrameHeight();
     const float       button_width = button_size * 2.60F;
     const auto        pop_id       = scope_guard{ &ImGui::PopID };
     ImGui::PushID(++imgui_id);
     // ImGui text box with browse button
     // Highlight text box red if the folder doesn't exist
     {
          const float width = ImGui::CalcItemWidth();
          ImGui::PushItemWidth(width - (spacing * 1.0f) - button_width);
          const auto pop_item_width = scope_guard(&ImGui::PopItemWidth);
          if (!valid_path)
          {
               ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor::HSV(0.0F, 0.5F, 0.5F));
               ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));// lighter red on hover
               ImGui::PushStyleColor(ImGuiCol_FrameBgActive, (ImVec4)ImColor::HSV(0.0F, 0.5F, 0.5F));
          }
          const auto pop_color = scope_guard([valid = valid_path]() {
               if (!valid)
               {
                    ImGui::PopStyleColor(3);
               }
          });
          if (ImGui::InputText("##Empty", path_buffer.data(), path_buffer.size()))
          {
               // Check if the folder path exists when the text box changes
               const auto tmp = safedir(path_buffer.data());
               valid_path     = tmp.is_exists() && tmp.is_dir();
          }
     }
     ImGui::SameLine(0, spacing);
     {
          ;
          if (ImGui::Button("Browse", ImVec2{ button_width, button_size }))
          {
               // Trigger the chooseFolder function when the button is clicked
               // chooseFolder();
          }
     }
     ImGui::SameLine(0, spacing);
     format_imgui_text("{}", name.data());
}
void batch::update(sf::Time /*unused*/)
{
     choose_field_and_coo();
     if (!m_field || !m_coo || !m_field->operator bool())
     {
          return;
     }
     m_status = fmt::format("Processing {}:{}", m_field->get_base_name(), *m_coo);
     reset_for_next();
}
void batch::reset_for_next()
{
     m_coo.reset();
     if (m_lang_consumer.done())
     {
          m_field.reset();
          if (!m_fields_consumer.done())
          {
               m_lang_consumer.restart();
          }
     }
}
void batch::choose_field_and_coo()
{
     if ((!m_field || !m_field->operator bool()) && !m_fields_consumer.done())
     {
          m_field = std::make_shared<open_viii::archive::FIFLFS<false>>(*m_fields_consumer);
          ++m_fields_consumer;
     }
     if (!m_coo && !m_lang_consumer.done())
     {
          m_coo = *m_lang_consumer;
          ++m_lang_consumer;
     }
}
