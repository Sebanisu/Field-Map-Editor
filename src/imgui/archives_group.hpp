//
// Created by pcvii on 9/5/2021.
//

#ifndef MYPROJECT_ARCHIVES_GROUP_HPP
#define MYPROJECT_ARCHIVES_GROUP_HPP
#include "open_viii/archive/Archives.hpp"
#include "open_viii/paths/Paths.hpp"
struct archives_group
{
private:
  open_viii::LangT             m_coo      = {};
  std::filesystem::path        m_path     = {};
  mutable bool                 m_failed   = true;
  open_viii::archive::Archives m_archives = {};
  auto                         get_archives() const
  {
    // todo need a way to filter out versions of game that don't have a
    // language.
    std::cout << m_path << std::endl;
    auto archives = open_viii::archive::Archives(
      m_path, open_viii::LangCommon::to_string(m_coo));
    if (!static_cast<bool>(archives)) {
      std::cerr << "Failed to load path: " << m_path.string() << '\n';
      m_failed = true;
    }
    return archives;
  }

public:
  archives_group(const open_viii::LangT in_coo, auto &&in_path)
    : m_coo(in_coo), m_path(std::forward<decltype(in_path)>(in_path)),
      m_archives(get_archives())
  {}
  [[nodiscard]] auto with_coo(const open_viii::LangT in_coo) const
  {
    return archives_group(in_coo, m_path);
  }
  [[nodiscard]] auto with_path(auto &&in_path) const
  {
    return archives_group(m_coo, std::forward<decltype(in_path)>(in_path));
  }
  [[nodiscard]] const auto &coo() const { return m_coo; };
  [[nodiscard]] const auto &path() const { return m_path; };
  [[nodiscard]] const auto &archives() const { return m_archives; };
  [[nodiscard]] bool failed() const
  {
    return m_failed;
  }
};
#endif// MYPROJECT_ARCHIVES_GROUP_HPP
