
#include "ArchivesGroup.hpp"
#include <ranges>
#include <spdlog/spdlog.h>
#include <string_view>
#include <vector>
namespace ff_8
{
open_viii::archive::Archives ArchivesGroup::get_archives() const
{
     // todo need a way to filter out versions of game that don't have a
     // language.
     spdlog::info("{}", m_path);
     auto archives = open_viii::archive::Archives(
       m_path, open_viii::LangCommon::to_string(m_coo));
     if (!static_cast<bool>(archives))
     {
          spdlog::warn(
            "{}:{} Failed to fully load path: \"{}\", but fields is loaded: "
            "{}",
            __FILE__,
            __LINE__,
            m_path,
            static_cast<bool>(fields()));
     }
     m_failed = !fields();
     return archives;
}
const open_viii::archive::FIFLFS<true> &ArchivesGroup::fields() const
{
     return m_archives.get<open_viii::archive::ArchiveTypeT::field>();
}
std::vector<std::string> ArchivesGroup::get_map_list() const
{
     if (!m_failed)
     {
          return fields().map_data_from_maplist();
     }
     return {};
}
std::vector<std::string> ArchivesGroup::get_map_data() const
{
     if (!m_failed)
     {
          return fields().map_data();
     }
     return {};
}
std::vector<const char *>
  ArchivesGroup::get_c_str(const std::vector<std::string> &in_vector)
{
     std::vector<const char *> ret{};
     ret.reserve(in_vector.size());
     std::ranges::transform(
       in_vector,
       std::back_inserter(ret),
       [](const std::string &str) { return str.c_str(); });
     return ret;
}

const std::filesystem::path &ArchivesGroup::path() const noexcept
{
     return m_path;
}
const open_viii::LangT &ArchivesGroup::coo() const noexcept
{
     return m_coo;
}
const open_viii::archive::Archives &ArchivesGroup::archives() const noexcept
{
     return m_archives;
}
bool ArchivesGroup::failed() const noexcept
{
     return m_failed;
}

const std::vector<std::string> &
  ArchivesGroup::map_data_from_maplist() const noexcept
{
     return m_maplist;
}
const std::vector<std::string> &ArchivesGroup::mapdata() const noexcept
{
     return m_mapdata;
}
std::shared_ptr<open_viii::archive::FIFLFS<false>>
  ArchivesGroup::field(const int current_map) const
{
     std::shared_ptr<open_viii::archive::FIFLFS<false>> archive{};
     if (!m_mapdata.empty() && std::cmp_less(current_map, m_mapdata.size()))
     {
          fields().execute_with_nested(
            { m_mapdata.at(static_cast<std::size_t>(current_map)) },
            [&archive](auto &&field)
            {
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
ArchivesGroup
  ArchivesGroup::with_path(const std::filesystem::path &in_path) const
{
     return { m_coo, in_path };
}

int ArchivesGroup::find_field(std::string_view needle) const
{
     const auto first = m_mapdata.cbegin();
     const auto last  = m_mapdata.cend();
     const auto it    = std::find_if(
       first,
       last,
       [&needle](const auto &name)
       { return open_viii::tools::i_find(name, needle); });
     if (it != last)
     {
          return static_cast<int>(std::distance(first, it));
     }
     return -1;
}
}// namespace ff_8