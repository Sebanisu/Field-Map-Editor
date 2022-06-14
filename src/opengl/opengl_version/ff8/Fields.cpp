//
// Created by pcvii on 11/30/2021.
//

#include "Fields.hpp"
#include "GenericCombo.hpp"
#include <fmt/chrono.h>

namespace ff8
{
static int current_index = {};
}

open_viii::graphics::background::Mim ff8::LoadMim(
  open_viii::archive::FIFLFS<false> field,
  std::string_view                  coo,
  std::string                      &out_path,
  bool                             &coo_was_used)
{
  std::size_t out_path_pos = {};
  auto        lang_name =
    fmt::format("_{}{}", coo, open_viii::graphics::background::Mim::EXT);

  auto buffer = field.get_entry_data(
    { std::string_view(lang_name), open_viii::graphics::background::Mim::EXT },
    &out_path,
    &out_path_pos);
  coo_was_used = out_path_pos == 0U;
  if (!std::ranges::empty(buffer))
  {
    fmt::print("loaded: {}\n", out_path);
    auto mim =
      open_viii::graphics::background::Mim{ buffer, field.get_base_name() };
    return mim;
  }
  return {};
}

open_viii::graphics::background::Map ff8::LoadMap(
  open_viii::archive::FIFLFS<false>           field,
  std::string_view                            coo,
  const open_viii::graphics::background::Mim &mim,
  std::string                                &out_path,
  bool                                       &coo_was_used)
{
  bool        shift        = false;
  std::size_t out_path_pos = {};
  auto        lang_name =
    fmt::format("_{}{}", coo, open_viii::graphics::background::Map::EXT);
  auto buffer = field.get_entry_data(
    { std::string_view(lang_name),
      open_viii::graphics::background::Map::Map::EXT },
    &out_path,
    &out_path_pos);
  coo_was_used = out_path_pos == 0U;// if true then the coo was picked.
  if (!std::ranges::empty(buffer))
  {
    fmt::print("loaded: {}\n", out_path);
    auto map =
      open_viii::graphics::background::Map{ mim.mim_type(), buffer, shift };
    return map;
  }
  return {};
}

bool ff8::Fields::OnArchiveChange() const
{
  starttime = std::chrono::steady_clock::now();
  if (m_archive.OnImGuiUpdate())
  {
    m_map_data = m_archive.Fields().map_data();
    m_field    = load_field();
    endtime    = std::chrono::steady_clock::now();
    fmt::print("time to load fields = {:%S} seconds\n", endtime - starttime);
    return true;
  }
  return false;
}

bool ff8::Fields::OnFieldChange() const
{
  if (glengine::GenericCombo("Field", current_index, m_map_data))
  {
    m_field = load_field();
    return true;
  }
  return false;
}

bool ff8::Fields::OnImGuiUpdate() const
{
  return OnArchiveChange() | OnFieldChange();
}

open_viii::archive::FIFLFS<false> ff8::Fields::load_field() const
{
  open_viii::archive::FIFLFS<false> archive{};
  if (!m_map_data.empty() && std::cmp_less(current_index, m_map_data.size()))
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
      current_index,
      m_map_data.size());
  }
  return archive;
}

std::string_view ff8::Fields::Coo() const
{
  return m_archive.Coo();
}

const std::string &ff8::Fields::Map_Name() const
{
  if (std::cmp_less(current_index, std::ranges::size(m_map_data)))
  {
    return m_map_data[static_cast<std::size_t>(current_index)];
  }
  const static auto tmp = std::string("");
  return tmp;
}

ff8::Fields::Fields()
  : m_map_data(m_archive.Fields().map_data())
  , m_field(load_field())
{
  fmt::print("time to load fields = {:%S} seconds\n", endtime - starttime);
  auto pos = std::ranges::find(
    m_map_data, std::string("bgmdele1"));//"feopen2" //crtower3
  if (pos != m_map_data.end())
  {
    current_index = static_cast<int>(std::distance(m_map_data.begin(), pos));
    m_field       = load_field();
  }
}

const open_viii::archive::FIFLFS<false> &ff8::Fields::Field() const
{
  return m_field;
}
