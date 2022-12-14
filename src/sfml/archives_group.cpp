//
// Created by pcvii on 9/7/2021.
//
#include "archives_group.hpp"
#include <ranges>
#include <spdlog/spdlog.h>
#include <string_view>
#include <vector>
open_viii::archive::Archives archives_group::get_archives() const
{
  // todo need a way to filter out versions of game that don't have a
  // language.
  spdlog::info("{}",m_path);
  auto archives = open_viii::archive::Archives(
    m_path, open_viii::LangCommon::to_string(m_coo));
  if (!static_cast<bool>(archives))
  {
    spdlog::warn("Failed to load path: {}",m_path);
  }
  m_failed = !archives;
  return archives;
}
const open_viii::archive::FIFLFS<true> &archives_group::fields() const
{
  return m_archives.get<open_viii::archive::ArchiveTypeT::field>();
}
std::vector<std::string> archives_group::get_map_data() const
{
  if (!m_failed && fields().all_set())
  {
    return fields().map_data();
  }
  return {};
}
std::vector<const char *>
  archives_group::get_c_str(const std::vector<std::string> &in_vector)
{
  std::vector<const char *> ret{};
  ret.reserve(in_vector.size());
  std::ranges::transform(
    in_vector, std::back_inserter(ret), [](const std::string &str) {
      return str.c_str();
    });
  return ret;
}

const std::string &archives_group::path() const noexcept
{
  return m_path;
}
const open_viii::LangT &archives_group::coo() const noexcept
{
  return m_coo;
}
const open_viii::archive::Archives &archives_group::archives() const noexcept
{
  return m_archives;
}
bool archives_group::failed() const noexcept
{
  return m_failed;
}
const std::vector<std::string> &archives_group::mapdata() const noexcept
{
  return m_mapdata;
}
const std::vector<const char *> &archives_group::mapdata_c_str() const noexcept
{
  return m_mapdata_c_str;
}
std::shared_ptr<open_viii::archive::FIFLFS<false>>
  archives_group::field(const int current_map) const
{
  std::shared_ptr<open_viii::archive::FIFLFS<false>> archive{};
  if (!m_mapdata.empty() && std::cmp_less(current_map, m_mapdata.size()))
  {
    fields().execute_with_nested(
      { m_mapdata.at(static_cast<std::size_t>(current_map)) },
      [&archive](auto &&field) {
        archive = std::make_shared<open_viii::archive::FIFLFS<false>>(
          std::forward<decltype(field)>(field));
      },
      {},
      [](auto &&) { return true; },
      true);
  }
  else
  {
    spdlog::error(
      "{}:{} - Index out of range {} / {}\n",
      __FILE__,
      __LINE__,
      current_map,
      m_mapdata.size());
  }
  return archive;
}
archives_group archives_group::with_coo(const open_viii::LangT in_coo) const
{
  return { in_coo, m_path };
}
archives_group archives_group::with_path(const std::string &in_path) const
{
  return { m_coo, in_path };
}

int archives_group::find_field(std::string_view needle) const
{
  const auto first = m_mapdata.cbegin();
  const auto last  = m_mapdata.cend();
  const auto it    = std::find_if(first, last, [&needle](const auto &name) {
    return open_viii::tools::i_find(name, needle);
  });
  if (it != last)
  {
    return static_cast<int>(std::distance(first, it));
  }
  return -1;
}
