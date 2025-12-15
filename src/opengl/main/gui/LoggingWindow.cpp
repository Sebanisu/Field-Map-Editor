#include "LoggingWindow.hpp"


fme::LoggingWindow::LoggingWindow(
  std::shared_ptr<MemorySink> memory_sink,
  std::weak_ptr<Selections>   selections)
  : m_memory_sink(std::move(memory_sink))
  , m_selections(std::move(selections))
{
}

void fme::LoggingWindow::on_update(float ts) const
{
     m_refresh_timer -= ts;
     if (m_refresh_timer <= 0.0f)
     {
          refresh_messages();
          m_refresh_timer = m_refresh_interval;
     }
}

void fme::LoggingWindow::on_im_gui_update() const
{
     if (!m_open)
          return;

     if (!ImGui::Begin("Logging Window", &m_open))
     {
          ImGui::End();
          return;
     }

     ImGui::Checkbox("Auto-scroll", &m_auto_scroll);

     ImGui::Separator();

     draw_log_entries();

     ImGui::End();
}

void fme::LoggingWindow::on_im_gui_window_menu() const
{
     ImGui::MenuItem("Log", nullptr, &m_open);
}

void fme::LoggingWindow::refresh_messages() const
{
     if (!m_memory_sink)
          return;

     m_messages = m_memory_sink->last_formatted();
}

void fme::LoggingWindow::draw_log_entries() const
{

     if (!m_memory_sink)
          return;

     ImGui::BeginChild(
       "log_scroller",
       ImVec2(0, 0),
       false,
       ImGuiWindowFlags_HorizontalScrollbar);

     ImGuiListClipper clipper;
     clipper.Begin(static_cast<int>(m_messages.size()));

     while (clipper.Step())
     {
          const int start = clipper.DisplayStart;
          const int count = clipper.DisplayEnd - clipper.DisplayStart;

          const std::span<const std::string> visible(
            m_messages.data() + start, static_cast<size_t>(count));

          for (const auto &msg : visible)
          {
               ImGui::TextUnformatted(msg.c_str());
          }
     }

     if (m_auto_scroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
     {
          ImGui::SetScrollHereY(1.0f);
     }

     ImGui::EndChild();
}
