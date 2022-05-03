//
// Created by pcvii on 12/1/2021.
//

#ifndef FIELD_MAP_EDITOR_BPPS_HPP
#define FIELD_MAP_EDITOR_BPPS_HPP


namespace ff8
{
class BPPs
{
public:
  void                      OnUpdate(float) const {}
  void                      OnRender() const {}
  bool                      OnImGuiUpdate() const;
  void                      OnEvent(const glengine::Event::Item &) const {}
  open_viii::graphics::BPPT BPP() const
  {
    return m_values.at(static_cast<std::size_t>(m_current));
  }
  std::string_view String() const
  {
    return m_strings.at(static_cast<std::size_t>(m_current));
  }
  int Index() const
  {
    return m_current;
  }

private:
  static constexpr auto m_values = []() {
    using namespace open_viii::graphics::literals;
    return std::array{ 4_bpp, 8_bpp, 16_bpp };
  }();
  static constexpr auto m_strings = []() {
    using namespace std::string_view_literals;
    return std::array{ "4"sv, "8"sv, "16"sv };
  }();
  mutable int m_current{};
};
static_assert(glengine::Renderable<BPPs>);
}// namespace ff8
#endif// FIELD_MAP_EDITOR_BPPS_HPP
