//
// Created by pcvii on 9/5/2021.
//

#ifndef MYPROJECT_ARCHIVES_GROUP_HPP
#define MYPROJECT_ARCHIVES_GROUP_HPP
#include "map_sprite.hpp"
#include "mim_sprite.hpp"
#include "open_viii/archive/Archives.hpp"
struct archives_group
{
private:
  open_viii::LangT                                      m_coo           = {};
  std::filesystem::path                                 m_path          = {};
  mutable bool                                          m_failed        = true;
  open_viii::archive::Archives                          m_archives      = {};
  std::vector<std::string>                              m_mapdata       = {};
  std::vector<const char *>                             m_mapdata_c_str = {};

  open_viii::archive::Archives                          get_archives() const;
  [[nodiscard]] const open_viii::archive::FIFLFS<true> &fields() const;
  std::vector<std::string>                              get_map_data() const;

public:
  [[nodiscard]] static std::vector<const char *> get_c_str(
    const std::vector<std::string> &in_vector);
  archives_group() = default;
  [[maybe_unused]] archives_group(const open_viii::LangT in_coo, auto &&in_path)
    : m_coo(in_coo), m_path(std::forward<decltype(in_path)>(in_path)),
      m_archives(get_archives()), m_mapdata(get_map_data()),
      m_mapdata_c_str(get_c_str(m_mapdata))
  {}
  /**
   * Only really needed if we need to reload files. In fields in remaster all
   * the different language files are in the same archive. They use a suffix if
   * a file has different languages. While other archives are split up and have
   * different ones for each language.
   * @param in_coo new language code
   * @return new copy of this object with new language loaded.
   */
  [[nodiscard]] archives_group with_coo(const open_viii::LangT in_coo) const
  {
    return { in_coo, m_path };
  }
  /**
   * Creates a new archive_group pointing to the provided path.
   * @param in_path new path.
   * @return new archive_group
   */
  [[nodiscard]] archives_group with_path(auto &&in_path) const
  {
    return { m_coo, std::forward<decltype(in_path)>(in_path) };
  }
  [[maybe_unused]] [[nodiscard]] const open_viii::LangT &coo() const;
  [[nodiscard]] const std::filesystem::path             &path() const;
  [[nodiscard]] const open_viii::archive::Archives      &archives() const;
  [[nodiscard]] bool                                     failed() const;
  [[nodiscard]] const std::vector<std::string>          &mapdata() const;
  [[nodiscard]] const std::vector<const char *>         &mapdata_c_str() const;
  [[nodiscard]] open_viii::archive::FIFLFS<false> field(int current_map) const;
  static bool ImGui_controls(archives_group &opt_archives,
    open_viii::archive::FIFLFS<false>       &field,
    mim_sprite                              &ms,
    map_sprite                              &map,
    int                                     &current_field,
    int                                     &coo_selected_item);
};
#endif// MYPROJECT_ARCHIVES_GROUP_HPP
