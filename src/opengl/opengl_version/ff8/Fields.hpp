//
// Created by pcvii on 11/30/2021.
//

#ifndef FIELD_MAP_EDITOR_FIELDS_HPP
#define FIELD_MAP_EDITOR_FIELDS_HPP
#include "Archive.hpp"
#include "fmt/format.h"
namespace ff_8
{
[[nodiscard]] open_viii::graphics::background::Mim LoadMim(
  open_viii::archive::FIFLFS<false> in_field,
  std::string_view                  coo,
  std::string                      &out_path,
  bool                             &coo_was_used);
[[nodiscard]] open_viii::graphics::background::Map LoadMap(
  open_viii::archive::FIFLFS<false>           in_field,
  std::string_view                            coo,
  const open_viii::graphics::background::Mim &mim,
  std::string                                &out_path,
  bool                                       &coo_was_used);
class Fields
{
public:
  Fields();
  void               on_update(float) const {}
  void               on_render() const {}
  [[nodiscard]] bool on_im_gui_update() const;
  void               on_event(const glengine::event::Item &) const {}
  [[nodiscard]] const open_viii::archive::FIFLFS<false> *operator->() const;
  [[nodiscard]] operator const open_viii::archive::FIFLFS<false> &() const;
  [[nodiscard]] std::string_view map_name() const;
  [[nodiscard]] std::string_view coo() const;

private:
  [[nodiscard]] open_viii::archive::FIFLFS<false> load_field() const;
  [[nodiscard]] bool                              on_archive_change() const;
  [[nodiscard]] bool                              on_field_change() const;

  mutable std::chrono::steady_clock::time_point   m_start_time =
    std::chrono::steady_clock::now();
  Archive                                       m_archive  = {};
  mutable std::vector<std::string>              m_map_data = {};
  mutable open_viii::archive::FIFLFS<false>     m_field    = {};
  mutable std::chrono::steady_clock::time_point m_end_time =
    std::chrono::steady_clock::now();
};
static_assert(glengine::Renderable<Fields>);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_FIELDS_HPP
