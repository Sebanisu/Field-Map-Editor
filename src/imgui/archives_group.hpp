//
// Created by pcvii on 9/5/2021.
//

#ifndef MYPROJECT_ARCHIVES_GROUP_HPP
#define MYPROJECT_ARCHIVES_GROUP_HPP
#include "open_viii/archive/Archives.hpp"
#include "open_viii/paths/Paths.hpp"
#include <ranges>
#include <string>
#include <string_view>
#include <vector>
struct archives_group
{
private:
  open_viii::LangT                        m_coo           = {};
  std::filesystem::path                   m_path          = {};
  mutable bool                            m_failed        = true;
  open_viii::archive::Archives            m_archives      = {};
  const open_viii::archive::FIFLFS<true> *m_fields        = {};
  std::vector<std::string>                m_mapdata       = {};
  std::vector<const char *>               m_mapdata_c_str = {};

  auto                                    get_archives() const
  {
    // todo need a way to filter out versions of game that don't have a
    // language.
    std::cout << m_path << std::endl;
    auto archives = open_viii::archive::Archives(
      m_path, open_viii::LangCommon::to_string(m_coo));
    if (!static_cast<bool>(archives)) {
      std::cerr << "Failed to load path: " << m_path.string() << '\n';
    }
    m_failed = false;
    return archives;
  }
  std::vector<std::string> get_mapdata() const
  {
    if (!m_failed && m_fields != nullptr) {
      return m_fields->map_data();
    }
    return {};
  }

public:
  [[nodiscard]] static std::vector<const char *> get_c_str(
    const std::vector<std::string> &in_vector)
  {
    std::vector<const char *> ret{};
    ret.reserve(in_vector.size());
    std::ranges::transform(in_vector,
      std::back_inserter(ret),
      [](const std::string &str) { return str.c_str(); });
    return ret;
  }
  archives_group(const open_viii::LangT in_coo, auto &&in_path)
    : m_coo(in_coo), m_path(std::forward<decltype(in_path)>(in_path)),
      m_archives(get_archives()),
      m_fields(&m_archives.get<open_viii::archive::ArchiveTypeT::field>()),
      m_mapdata(get_mapdata()), m_mapdata_c_str(get_c_str(m_mapdata))
  {}
  /**
   * Only really needed if we need to reload files. In fields in remaster all
   * the different language files are in the same archive. They use a suffix if
   * a file has different languages. While other archives are split up and have
   * different ones for each language.
   * @param in_coo new language code
   * @return new copy of this object with new language loaded.
   */
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
  [[nodiscard]] bool        failed() const { return m_failed; }
  [[nodiscard]] const auto *fields() const { return m_fields; }
  [[nodiscard]] const auto &mapdata() const { return m_mapdata; }
  [[nodiscard]] const auto &mapdata_c_str() const { return m_mapdata_c_str; }
  [[nodiscard]] auto        field(const int current_map) const
  {
    open_viii::archive::FIFLFS<false> archive{};
    if (!m_mapdata.empty()) {
      m_fields->execute_with_nested(
        { m_mapdata.at(static_cast<std::size_t>(current_map)) },
        [&archive](
          auto &&field) { archive = std::forward<decltype(field)>(field); },
        {},
        true);
    }
    return archive;
  }
};
#endif// MYPROJECT_ARCHIVES_GROUP_HPP
