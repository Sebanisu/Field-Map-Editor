//
// Created by pcvii on 9/5/2021.
//

#ifndef FIELD_MAP_EDITOR_ARCHIVES_GROUP_HPP
#define FIELD_MAP_EDITOR_ARCHIVES_GROUP_HPP
#include "map_sprite.hpp"
#include "mim_sprite.hpp"
#include "open_viii/archive/Archives.hpp"
struct archives_group
{
private:
  open_viii::LangT                                      m_coo           = {};
  std::string                                           m_path          = {};
  mutable bool                                          m_failed        = true;
  open_viii::archive::Archives                          m_archives      = {};
  std::vector<std::string>                              m_mapdata       = {};
  std::vector<const char *>                             m_mapdata_c_str = {};

  open_viii::archive::Archives                          get_archives() const;
  [[nodiscard]] const open_viii::archive::FIFLFS<true> &fields() const;
  std::vector<std::string>                              get_map_data() const;

public:
  [[nodiscard]] static std::vector<const char *>
    get_c_str(const std::vector<std::string> &in_vector);
  archives_group() = default;
  [[maybe_unused]] archives_group(
    const open_viii::LangT in_coo,
    const std::string     &in_path)
    : m_coo(in_coo)
    , m_path(in_path)
    , m_archives(get_archives())
    , m_mapdata(get_map_data())
    , m_mapdata_c_str(get_c_str(m_mapdata))
  {
  }
  /**
   * Only really needed if we need to reload files. In fields in remaster all
   * the different language files are in the same archive. They use a suffix if
   * a file has different languages. While other archives are split up and have
   * different ones for each language.
   * @param in_coo new language code
   * @return new copy of this object with new language loaded.
   */
  [[nodiscard]] archives_group with_coo(open_viii::LangT in_coo) const;
  /**
   * Creates a new archive_group pointing to the provided path.
   * @param in_path new path.
   * @return new archive_group
   */
  [[nodiscard]] archives_group with_path(const std::string &in_path) const;
  [[maybe_unused]] [[nodiscard]] const open_viii::LangT &coo() const noexcept;
  [[nodiscard]] const std::string                       &path() const noexcept;
  [[nodiscard]] const open_viii::archive::Archives &archives() const noexcept;
  [[nodiscard]] bool                                failed() const noexcept;
  [[nodiscard]] const std::vector<std::string>     &mapdata() const noexcept;
  [[nodiscard]] const std::vector<const char *> &mapdata_c_str() const noexcept;
  [[nodiscard]] std::shared_ptr<open_viii::archive::FIFLFS<false>>
                    field(int current_map) const;
  [[nodiscard]] int find_field(std::string_view needle) const;
};
#endif// FIELD_MAP_EDITOR_ARCHIVES_GROUP_HPP
