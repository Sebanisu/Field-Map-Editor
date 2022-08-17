//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_MIM_HPP
#define FIELD_MAP_EDITOR_MIM_HPP
#include "BatchRenderer.hpp"
#include "BPPs.hpp"
#include "FF8LoadTextures.hpp"
#include "Fields.hpp"
#include "ImGuiViewPortWindow.hpp"
#include "Palettes.hpp"
#include <Counter.hpp>
namespace ff_8
{
class Mim
{
public:
  Mim() = default;
  Mim(const Fields &fields);
  void                     on_update(float) const;
  void                     on_render() const;
  void                     on_im_gui_update() const;
  void                     on_event(const glengine::event::Item &) const;
  std::size_t              index() const;
  const glengine::Texture &current_texture() const;


private:
  void                           render_frame_buffer() const;
  void                           set_uniforms() const;
  void                           save() const;
  void                           save_all() const;
  // 3 bpp x 16 palettes 48 possible textures + 1 for palette texture
  glengine::BatchRenderer        m_batch_renderer        = {};
  glengine::ImGuiViewPortWindow  m_imgui_viewport_window = { "Mim" };
  glengine::Counter              m_id                    = {};
};
static_assert(glengine::Renderable<Mim>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MIM_HPP
