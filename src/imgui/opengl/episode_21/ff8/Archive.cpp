//
// Created by pcvii on 11/30/2021.
//
#include "Archive.hpp"

bool ff8::Archive::OnImGuiUpdate() const
{
  if (m_paths.OnImGuiUpdate() || m_coos.OnImGuiUpdate())
  {
    m_archives = { m_paths.Path(), m_coos.Coo() };
    return true;
  }
  return false;
}

ff8::Archive::Archive()
{
  m_archives = { m_paths.Path(), m_coos.Coo() };
}

const open_viii::archive::Archives &ff8::Archive::Archives() const
{
  return m_archives;
}

const open_viii::archive::FIFLFS<true> &ff8::Archive::Fields() const
{
  return m_archives.get<open_viii::archive::ArchiveTypeT::field>();
}

std::string_view ff8::Archive::Coo() const
{
  return m_coos.Coo();
}