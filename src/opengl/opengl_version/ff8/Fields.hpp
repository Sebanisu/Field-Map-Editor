//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_FIELDS_HPP
#define MYPROJECT_FIELDS_HPP
#include "Archive.hpp"
#include "fmt/format.h"
namespace ff8
{
[[nodiscard]] open_viii::graphics::background::Mim LoadMim(
  open_viii::archive::FIFLFS<false> field,
  std::string_view                  coo,
  std::string                      &out_path,
  bool                             &coo_was_used);
[[nodiscard]] open_viii::graphics::background::Map LoadMap(
  open_viii::archive::FIFLFS<false>           field,
  std::string_view                            coo,
  const open_viii::graphics::background::Mim &mim,
  std::string                                &out_path,
  bool                                       &coo_was_used);
class Fields
{
public:
  Fields();
  void               OnUpdate(float) const {}
  void               OnRender() const {}
  [[nodiscard]] bool OnImGuiUpdate() const;
  void               OnEvent(const glengine::Event::Item &) const {}
  [[nodiscard]] const open_viii::archive::FIFLFS<false> &Field() const;
  [[nodiscard]] const std::string                       &Map_Name() const;
  [[nodiscard]] std::string_view                         Coo() const;

private:
  [[nodiscard]] open_viii::archive::FIFLFS<false> load_field() const;
  [[nodiscard]] bool                              OnArchiveChange() const;
  [[nodiscard]] bool                              OnFieldChange() const;

  mutable std::chrono::steady_clock::time_point   starttime =
    std::chrono::steady_clock::now();
  Archive                                       m_archive  = {};
  mutable std::vector<std::string>              m_map_data = {};
  mutable open_viii::archive::FIFLFS<false>     m_field    = {};
  mutable std::chrono::steady_clock::time_point endtime =
    std::chrono::steady_clock::now();
};
static_assert(glengine::Renderable<Fields>);
}// namespace ff8
#endif// MYPROJECT_FIELDS_HPP
