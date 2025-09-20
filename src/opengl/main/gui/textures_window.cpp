#include "textures_window.hpp"
#include "tool_tip.hpp"
#include <ranges>

void fme::textures_window::render() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error(
            "m_selections is no longer valid. File: {}, Line: {}",
            __FILE__,
            __LINE__);
          return;
     }
     auto map_sprite = m_map_sprite.lock();
     if (!map_sprite)
     {
          spdlog::error(
            "m_map_sprite is no longer valid. File: {}, Line: {}",
            __FILE__,
            __LINE__);
          return;
     }
     if (map_sprite->fail())
     {
          return;
     }
     if (!selections->get<ConfigKey::DisplayTexturesWindow>())
     {
          return;
     }

     bool      &visible = selections->get<ConfigKey::DisplayTexturesWindow>();
     const auto pop_visible = glengine::ScopeGuard{
          [&selections, &visible, was_visable = visible]
          {
               if (was_visable != visible)
               {
                    selections->update<ConfigKey::DisplayTexturesWindow>();
               }
          }
     };
     const auto the_end = glengine::ScopeGuard([]() { ImGui::End(); });

     if (!ImGui::Begin("Textures", &visible))
     {
          return;
     }
     handle_thumbnail_size_adjustment();

     ImGui::BeginChild(
       "##ScrollingTextures",
       ImVec2(0, 0),// full remaining size
       false,       // no border
       ImGuiWindowFlags_HorizontalScrollbar);
     format_imgui_text("map_sprite->get_textures()");
     ImGui::Columns(
       calc_column_count(m_thumb_size_width),
       "##get_map_sprite_textures",
       false);
     for (const auto &[index, texture] :
          map_sprite->get_textures() | std::views::enumerate)
     {
          if (texture.id() == 0)
          {
               continue;
          }
          std::string title = fmt::format("{}", index);
          render_thumbnail_button(title, texture, false, []() {});
          tool_tip(
            fmt::format(
              "Index: {}\nOpenGL ID: {}\nWidth: {}\nHeight: {}", index,
              static_cast<std::uint32_t>(texture.id()), texture.width(),
              texture.height()));
          draw_thumbnail_label(title);
          ImGui::NextColumn();
     }
     ImGui::Columns(1);
     format_imgui_text("map_sprite->get_full_filename_textures()");
     ImGui::Columns(
       calc_column_count(m_thumb_size_width),
       "##get_map_sprite_textures",
       false);
     for (const auto &[index, pair] :
          map_sprite->get_full_filename_textures() | std::views::enumerate)
     {
          const auto &[file_name, texture] = pair;
          if (texture.id() == 0)
          {
               continue;
          }
          render_thumbnail_button(file_name, texture, false, []() {});
          tool_tip(
            fmt::format(
              "Index: {}\nOpenGL ID: {}\nWidth: {}\nHeight: {}\nFilename: {}",
              index, static_cast<std::uint32_t>(texture.id()), texture.width(),
              texture.height(), file_name));
          draw_thumbnail_label(file_name);
          ImGui::NextColumn();
     }
     ImGui::Columns(1);
     format_imgui_text("map_sprite->child_textures_map() Main");
     ImGui::Columns(
       calc_column_count(m_thumb_size_width),
       "##get_map_sprite_textures",
       false);
     for (const auto &[index, pair] :
          map_sprite->child_textures_map() | std::views::enumerate)
     {
          const auto &[file_name, opt_framebuffer] = pair;
          if (!opt_framebuffer.has_value())
          {
               continue;
          }
          const auto texture_id = opt_framebuffer->color_attachment_id();
          if (texture_id == 0)
          {
               continue;
          }
          render_thumbnail_button(
            file_name, texture_id, opt_framebuffer->get_size(), false, []() {});
          tool_tip(
            fmt::format(
              "Index: {}\nOpenGL ID: {}\nWidth: {}\nHeight: {}\nFilename: {}",
              index, static_cast<std::uint32_t>(texture_id),
              opt_framebuffer->width(), opt_framebuffer->height(), file_name));
          draw_thumbnail_label(file_name);
          ImGui::NextColumn();
     }
     ImGui::Columns(1);
     format_imgui_text("map_sprite->child_textures_map() Masks");
     ImGui::Columns(
       calc_column_count(m_thumb_size_width),
       "##get_map_sprite_textures",
       false);
     for (const auto &[index, pair] :
          map_sprite->child_textures_map() | std::views::enumerate)
     {
          const auto &[file_name, opt_framebuffer] = pair;
          if (!opt_framebuffer.has_value())
          {
               continue;
          }
          const auto texture_id = opt_framebuffer->color_attachment_id(1);
          if (texture_id == 0)
          {
               continue;
          }
          render_thumbnail_button(
            file_name, texture_id, opt_framebuffer->get_size(), false, []() {});
          tool_tip(
            fmt::format(
              "Index: {}\nOpenGL ID: {}\nWidth: {}\nHeight: {}\nFilename: {}",
              index, static_cast<std::uint32_t>(texture_id),
              opt_framebuffer->width(), opt_framebuffer->height(), file_name));
          draw_thumbnail_label(file_name);
          ImGui::NextColumn();
     }
     ImGui::EndChild();
}

void fme::textures_window::handle_thumbnail_size_adjustment() const
{
     ImGui::SliderFloat("Thumbnail Size", &m_thumb_size_width, 96.f, 3072.f);
     bool  ctrl  = ImGui::GetIO().KeyCtrl;
     float wheel = ImGui::GetIO().MouseWheel;

     if (ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows) && ctrl)
     {
          static const constexpr auto speed = 20.f;
          if (wheel > 0.0f)
               m_thumb_size_width
                 = std::min(m_thumb_size_width + (wheel * speed), 3072.f);
          else if (wheel < 0.0f)
               m_thumb_size_width = std::max(
                 m_thumb_size_width + (wheel * speed),
                 96.f);// add because wheel is negative
     }
}

int fme::textures_window::calc_column_count(float width) const
{
     const ImVec2 region_size = ImGui::GetContentRegionAvail();
     const float  padding     = ImGui::GetStyle().FramePadding.x * 2.0f
                           + ImGui::GetStyle().ItemSpacing.x;
     const int count = static_cast<int>(region_size.x / (width + padding));
     return count > 0 ? count : 1;
}

void fme::textures_window::render_thumbnail_button(
  const std::string              &file_name,
  const glengine::Texture        &texture,
  const bool                      selected,
  std::move_only_function<void()> on_click) const
{

     ImTextureID tex_id
       = glengine::ConvertGliDtoImTextureId<ImTextureID>(texture.id());
     m_aspect_ratio = static_cast<float>(texture.height())
                      / static_cast<float>(texture.width());
     const ImVec2 thumb_size
       = { m_thumb_size_width, m_thumb_size_width * m_aspect_ratio };

     const auto pop_style_color
       = glengine::ScopeGuard{ [selected]()
                               {
                                    if (selected)
                                    {
                                         ImGui::PopStyleColor(3);
                                    }
                               } };
     if (selected)
     {
          ImGui::PushStyleColor(ImGuiCol_Button, colors::ButtonGreen);
          ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered, colors::ButtonGreenHovered);
          ImGui::PushStyleColor(
            ImGuiCol_ButtonActive, colors::ButtonGreenActive);
     }

     const auto pop_id = PushPopID();
     if (ImGui::ImageButton(file_name.c_str(), tex_id, thumb_size))
     {
          std::invoke(on_click);
     }
}


void fme::textures_window::render_thumbnail_button(
  const std::string              &file_name,
  const uint32_t                 &id,
  const glm::ivec2               &size,
  const bool                      selected,
  std::move_only_function<void()> on_click) const
{

     ImTextureID tex_id = glengine::ConvertGliDtoImTextureId<ImTextureID>(id);
     m_aspect_ratio = static_cast<float>(size.y) / static_cast<float>(size.x);
     const ImVec2 thumb_size
       = { m_thumb_size_width, m_thumb_size_width * m_aspect_ratio };

     const auto pop_style_color
       = glengine::ScopeGuard{ [selected]()
                               {
                                    if (selected)
                                    {
                                         ImGui::PopStyleColor(3);
                                    }
                               } };
     if (selected)
     {
          ImGui::PushStyleColor(ImGuiCol_Button, colors::ButtonGreen);
          ImGui::PushStyleColor(
            ImGuiCol_ButtonHovered, colors::ButtonGreenHovered);
          ImGui::PushStyleColor(
            ImGuiCol_ButtonActive, colors::ButtonGreenActive);
     }

     const auto pop_id = PushPopID();
     if (ImGui::ImageButton(file_name.c_str(), tex_id, thumb_size))
     {
          std::invoke(on_click);
     }
}

void fme::textures_window::draw_thumbnail_label(
  const std::string &file_name) const
{
     // Label under image (optional)
     const float button_width = ImGui::GetFrameHeight();
     const float text_area_width
       = m_thumb_size_width - button_width + ImGui::GetStyle().FramePadding.x;
     // Remember the top-left of where we want to start
     const ImVec2 text_start_pos = ImGui::GetCursorScreenPos();

     ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + text_area_width);
     ImGui::Text("%s", file_name.c_str());
     ImGui::PopTextWrapPos();
     const ImVec2 backup_pos = ImGui::GetCursorScreenPos();
     // Position the button at top-right of this block (same Y as the start
     // of the text)
     ImGui::SetCursorScreenPos(ImVec2(
       text_start_pos.x + text_area_width + ImGui::GetStyle().FramePadding.x,
       text_start_pos.y));
     const auto pop_id = PushPopID();
     //  if (ImGui::Button(ICON_FA_TRASH, button_size))
     //  {
     //       m_remove_queue.push_back(file_name);
     //  }
     //  else
     //  {
     //       tool_tip("Remove");
     //  }
     ImGui::SetCursorScreenPos(ImVec2(
       backup_pos.x, (std::max)(ImGui::GetCursorScreenPos().y, backup_pos.y)));
}

void fme::textures_window::update(
  const std::shared_ptr<map_sprite> &new_map_sprite) const
{
     m_map_sprite = new_map_sprite;
}

void fme::textures_window::update(
  const std::shared_ptr<Selections> &new_selections) const
{
     m_selections = new_selections;
}