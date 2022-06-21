//
// Created by pcvii on 11/30/2021.
//
#include "Archive.hpp"

bool ff_8::Archive::on_im_gui_update() const
{
  if (m_paths.on_im_gui_update() || m_coos.on_im_gui_update())
  {
    m_archives = { m_paths.Path(), m_coos.Coo() };
    return true;
  }
  return false;
}

ff_8::Archive::Archive()
{
  m_archives = { m_paths.Path(), m_coos.Coo() };
}

const open_viii::archive::Archives &ff_8::Archive::Archives() const
{
  return m_archives;
}

const open_viii::archive::FIFLFS<true> &ff_8::Archive::Fields() const
{
  return m_archives.get<open_viii::archive::ArchiveTypeT::field>();
}

std::string_view ff_8::Archive::Coo() const
{
  return m_coos.Coo();
}