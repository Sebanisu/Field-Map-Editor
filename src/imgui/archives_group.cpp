//
// Created by pcvii on 9/7/2021.
//
#include "archives_group.hpp"
#include <imgui.h>
#include <ranges>
#include <string_view>
#include <vector>
open_viii::archive::Archives archives_group::get_archives() const
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
const open_viii::archive::FIFLFS<true> &archives_group::fields() const
{
  return m_archives.get<open_viii::archive::ArchiveTypeT::field>();
}
std::vector<std::string> archives_group::get_map_data() const
{
  if (!m_failed && fields().all_set()) {
    return fields().map_data();
  }
  return {};
}
std::vector<const char *> archives_group::get_c_str(
  const std::vector<std::string> &in_vector)
{
  std::vector<const char *> ret{};
  ret.reserve(in_vector.size());
  std::ranges::transform(in_vector,
    std::back_inserter(ret),
    [](const std::string &str) { return str.c_str(); });
  return ret;
}

const std::filesystem::path &archives_group::path() const { return m_path; }
const open_viii::LangT      &archives_group::coo() const { return m_coo; }
const open_viii::archive::Archives &archives_group::archives() const
{
  return m_archives;
}
bool archives_group::failed() const { return m_failed; }
const std::vector<std::string> &archives_group::mapdata() const
{
  return m_mapdata;
}
const std::vector<const char *> &archives_group::mapdata_c_str() const
{
  return m_mapdata_c_str;
}
open_viii::archive::FIFLFS<false> archives_group::field(
  const int current_map) const
{
  open_viii::archive::FIFLFS<false> archive{};
  if (!m_mapdata.empty()) {
    fields().execute_with_nested(
      { m_mapdata.at(static_cast<std::size_t>(current_map)) },
      [&archive](
        auto &&field) { archive = std::forward<decltype(field)>(field); },
      {},
      true);
  }
  return archive;
}
bool archives_group::ImGui_controls(archives_group &opt_archives,
  open_viii::archive::FIFLFS<false>                &field,
  mim_sprite                                       &ms,
  map_sprite                                       &map,
  int                                              &current_field,
  int                                              &coo_selected_item)
{
  static constexpr auto coos       = open_viii::LangCommon::to_array();
  static constexpr auto coos_c_str = open_viii::LangCommon::to_c_str_array();
  bool                  changed    = false;
  if (ImGui::Combo("Language",
        &coo_selected_item,
        coos_c_str.data(),
        static_cast<int>(coos_c_str.size()),
        5)) {

    // field   = opt_archives.field(current_map);
    // ms      = ms.with_field(field);
    ms  = ms.with_coo(coos.at(static_cast<std::size_t>(coo_selected_item)));
    map = map.with_coo(coos.at(static_cast<std::size_t>(coo_selected_item)));
    changed = true;
  }
  if (ImGui::Combo("Field",
        &current_field,
        opt_archives.mapdata_c_str().data(),
        static_cast<int>(opt_archives.mapdata_c_str().size()),
        10)) {

    field   = opt_archives.field(current_field);
    ms      = ms.with_field(field);
    map     = map.with_field(field);
    changed = true;
  }
  return changed;
}