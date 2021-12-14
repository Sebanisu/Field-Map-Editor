//
// Created by pcvii on 11/30/2021.
//

#ifndef MYPROJECT_FIELDS_HPP
#define MYPROJECT_FIELDS_HPP
#include "Archive.hpp"
#include "fmt/format.h"


namespace ff8
{
[[nodiscard]] open_viii::graphics::background::Mim LoadMim(
  open_viii::archive::FIFLFS<false> field,
  std::string_view                  coo,
  std::string                      &out_path,
  bool                             &coo_was_used);
[[nodiscard]] open_viii::graphics::background::Map LoadMap(
  open_viii::archive::FIFLFS<false>           field,
  std::string_view                            coo,
  const open_viii::graphics::background::Mim &mim,
  std::string                                &out_path,
  bool                                       &coo_was_used);
class Fields
{
public:
  void OnUpdate(float) const {}
  void OnRender() const {}
  bool OnImGuiUpdate() const;
  void OnEvent(const Event::Item &) const {}
  [[nodiscard]] const open_viii::archive::FIFLFS<false> &Field() const
  {
    return m_field;
  }
  [[nodiscard]] const std::string &Map_Name() const
  {
    if (std::cmp_less(m_current, std::ranges::size(m_map_data)))
    {
      return m_map_data.at(static_cast<std::size_t>(m_current));
    }
    const static auto tmp = std::string("");
    return tmp;
  }
  [[nodiscard]] std::string_view Coo() const
  {
    return m_archive.Coo();
  }

  Fields()
    : m_map_data(m_archive.Fields().map_data())
    , m_field(load_field())
  {
    fmt::print("time to load fields = {:%S} seconds\n", endtime - starttime);
  }

private:
  open_viii::archive::FIFLFS<false> load_field() const
  {
    open_viii::archive::FIFLFS<false> archive{};
    if (!m_map_data.empty() && std::cmp_less(m_current, m_map_data.size()))
    {
      m_archive.Fields().execute_with_nested(
        { Map_Name() },
        [&archive](auto &&field) {
          archive = std::forward<decltype(field)>(field);
        },
        {},
        [](auto &&) { return true; },
        true);
    }
    else
    {
      fmt::print(
        stderr,
        "{}:{} - Index out of range {} / {}\n",
        __FILE__,
        __LINE__,
        m_current,
        m_map_data.size());
    }
    return archive;
  }

  mutable std::chrono::steady_clock::time_point starttime =
    std::chrono::steady_clock::now();
  Archive                                       m_archive  = {};
  mutable std::vector<std::string>              m_map_data = {};
  mutable open_viii::archive::FIFLFS<false>     m_field    = {};
  inline static int                             m_current  = {};
  mutable std::chrono::steady_clock::time_point endtime =
    std::chrono::steady_clock::now();
};
static_assert(Renderable<Fields>);
}// namespace ff8
#endif// MYPROJECT_FIELDS_HPP
