#ifndef A70191A7_A943_4FCE_9EB3_0E7324FBB417
#define A70191A7_A943_4FCE_9EB3_0E7324FBB417

#include "Selections.hpp"
#include <glengine/Renderable.hpp>
#include <imgui.h>
#include <memory>
#include <spdlog/sinks/ringbuffer_sink.h>
#include <string>
#include <vector>

namespace fme
{

class LoggingWindow
{
   public:
     using MemorySink = spdlog::sinks::ringbuffer_sink_mt;

     LoggingWindow(
       std::shared_ptr<MemorySink> memory_sink,
       std::weak_ptr<Selections>   selections);

     // ------------------------------------------------------------
     // Renderable hooks
     // ------------------------------------------------------------

     void on_update(float ts) const;

     void on_im_gui_update() const;

     void on_im_gui_window_menu() const;

   private:
     // ------------------------------------------------------------
     // Helpers
     // ------------------------------------------------------------

     void refresh_messages() const;

     void draw_log_entries() const;


   private:
     std::shared_ptr<MemorySink>      m_memory_sink;
     std::weak_ptr<Selections>        m_selections;

     // Cached messages
     mutable std::vector<std::string> m_messages;

     // Refresh control
     float                            m_refresh_interval = 0.25f;// seconds
     mutable float                    m_refresh_timer    = 0.0f;

     // UI state (mutable to allow const callbacks)
     mutable bool                     m_open             = true;
     mutable bool                     m_auto_scroll      = true;
};
}// namespace fme


#endif /* A70191A7_A943_4FCE_9EB3_0E7324FBB417 */
