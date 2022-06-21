//
// Created by pcvii on 5/2/2022.
//

#ifndef FIELD_MAP_EDITOR_UPSCALES_HPP
#define FIELD_MAP_EDITOR_UPSCALES_HPP

namespace ff_8
{
class Upscales
{
public:
  void               on_update(float) const {}
  void               on_render() const {}
  [[nodiscard]] bool on_im_gui_update() const;
  void               on_event(const glengine::event::Item &) const {}
  [[nodiscard]] const std::string &Path() const;
  Upscales();

private:
  mutable std::vector<std::string> m_paths   = {};
  mutable int                      m_current = {};
};
static_assert(glengine::Renderable<Upscales>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_UPSCALES_HPP
