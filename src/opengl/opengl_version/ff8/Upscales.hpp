//
// Created by pcvii on 5/2/2022.
//

#ifndef FIELD_MAP_EDITOR_UPSCALES_HPP
#define FIELD_MAP_EDITOR_UPSCALES_HPP

namespace ff8
{
class Upscales
{
public:
  void OnUpdate(float) const {}
  void OnRender() const {}
  bool OnImGuiUpdate() const;
  void OnEvent(const glengine::Event::Item &) const {}
  [[nodiscard]] const std::string &Path() const;
  Upscales();
  
private:
  mutable std::vector<std::string> m_paths   = {};
  mutable int                      m_current = {};
};
static_assert(glengine::Renderable<Upscales>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_UPSCALES_HPP
