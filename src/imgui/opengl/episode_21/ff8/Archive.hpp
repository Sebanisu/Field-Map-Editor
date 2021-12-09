//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_ARCHIVE_HPP
#define MYPROJECT_ARCHIVE_HPP
#include "Coos.hpp"
#include "Paths.hpp"
#include <open_viii/archive/Archives.hpp>
namespace ff8
{
class Archive
{
public:
  Archive();
  void OnUpdate(float) const {}
  void OnRender() const {}
  bool OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const {}

  [[nodiscard]] const open_viii::archive::Archives &Archives() const;
  [[nodiscard]] const open_viii::archive::FIFLFS<true> &Fields() const;
  [[nodiscard]] std::string_view Coo() const;

private:
  Paths                                m_paths    = {};
  Coos                                 m_coos     = {};
  mutable open_viii::archive::Archives m_archives = {};
};
static_assert(test::Test<Archive>);
}// namespace ff8
#endif// MYPROJECT_ARCHIVE_HPP
