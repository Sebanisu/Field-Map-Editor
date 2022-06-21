//
// Created by pcvii on 12/1/2021.
//

#ifndef FIELD_MAP_EDITOR_PALETTES_HPP
#define FIELD_MAP_EDITOR_PALETTES_HPP


namespace ff_8
{
class Palettes
{
public:
  void             on_update(float) const {}
  void             on_render() const {}
  bool             on_im_gui_update() const;
  void             on_event(const glengine::event::Item &) const {}
  std::uint8_t     Palette() const;
  std::string_view String() const;
  int              Index() const;

private:
  static constexpr auto m_values = []() {
    return std::array<std::uint8_t, 16U>{ 0, 1, 2,  3,  4,  5,  6,  7,
                                          8, 9, 10, 11, 12, 13, 14, 15 };
  }();
  static constexpr auto m_strings = []() {
    using namespace std::string_view_literals;

    return std::array{ "0"sv,  "1"sv,  "2"sv,  "3"sv, "4"sv,  "5"sv,
                       "6"sv,  "7"sv,  "8"sv,  "9"sv, "10"sv, "11"sv,
                       "12"sv, "13"sv, "14"sv, "15"sv };
  }();
  mutable int m_current{};
};
static_assert(glengine::Renderable<Palettes>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_PALETTES_HPP
