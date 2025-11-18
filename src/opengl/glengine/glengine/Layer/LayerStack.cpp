//
// Created by pcvii on 12/8/2021.
//

#include "LayerStack.hpp"
namespace glengine
{
void Layer::Stack::on_render() const
{
     for (const Item &layer : m_layers)
     {
          layer.on_render();
     }
}
void Layer::Stack::on_im_gui_update() const
{
     for (const Item &layer : m_layers)
     {
          layer.on_im_gui_update();
     }
}
void Layer::Stack::on_im_gui_file_menu() const
{
     for (const Item &layer : m_layers)
     {
          layer.on_im_gui_file_menu();
     }
}
void Layer::Stack::on_im_gui_edit_menu() const
{
     for (const Item &layer : m_layers)
     {
          layer.on_im_gui_edit_menu();
     }
}
void Layer::Stack::on_im_gui_window_menu() const
{
     for (const Item &layer : m_layers)
     {
          layer.on_im_gui_window_menu();
     }
}
void Layer::Stack::on_im_gui_help_menu() const
{
     for (const Item &layer : m_layers)
     {
          layer.on_im_gui_help_menu();
     }
}
void Layer::Stack::on_update(float delta_time) const
{
     for (const Item &layer : m_layers)
     {
          layer.on_update(delta_time);
     }
}
void Layer::Stack::on_event(const event::Item &e) const
{
     auto reversed = m_layers | std::views::reverse;
     for (const Item &layer : reversed)
     {
          layer.on_event(e);
     }
}
}// namespace glengine