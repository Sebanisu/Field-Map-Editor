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
  friend void
    OnUpdate(const Archive &, float);
  friend void
    OnRender(const Archive &);
  friend bool
    OnImGuiRender(const Archive &);

  [[nodiscard]] const open_viii::archive::Archives &
    Archives() const
  {
    return m_archives;
  }
  [[nodiscard]] const open_viii::archive::FIFLFS<true> &
    Fields() const
  {
    return m_archives.get<open_viii::archive::ArchiveTypeT::field>();
  }

  [[nodiscard]] std::string_view
    Coo() const
  {
    return m_coos.Coo();
  }
  void init() const
  {
    m_paths.init();
    m_archives = { m_paths.Path(),
                   m_coos.Coo() };
  }
private:
  Paths                                m_paths    = {};
  Coos                                 m_coos     = {};
  mutable open_viii::archive::Archives m_archives = {};
};
inline void
  OnUpdate(const Archive &, float)
{
}
inline void
  OnRender(const Archive &)
{
}
inline bool
  OnImGuiRender(const Archive &self)
{
  if (OnImGuiRender(self.m_paths) || OnImGuiRender(self.m_coos))
  {
    self.m_archives = { self.m_paths.Path(), self.m_coos.Coo() };
    return true;
  }
  return false;
}
static_assert(test::Test<Archive>);
}// namespace ff8
#endif// MYPROJECT_ARCHIVE_HPP
