//
// Created by pcvii on 11/30/2021.
//

#include "Fields.hpp"
#include "GenericCombo.hpp"

namespace ff_8
{
static int CurrentIndex = {};
}

open_viii::graphics::background::Mim ff_8::LoadMim(
  open_viii::archive::FIFLFS<false> in_field,
  std::string_view                  coo,
  std::string                      &out_path,
  bool                             &coo_was_used)
{
  std::size_t out_path_pos = {};
  auto        lang_name =
    fmt::format("_{}{}", coo, open_viii::graphics::background::Mim::EXT);

  auto buffer = in_field.get_entry_data(
    { std::string_view(lang_name), open_viii::graphics::background::Mim::EXT },
    &out_path,
    &out_path_pos);
  coo_was_used = out_path_pos == 0U;
  if (!std::ranges::empty(buffer))
  {
    spdlog::debug("loaded: {}", out_path);
    auto mim =
      open_viii::graphics::background::Mim{ buffer, in_field.get_base_name() };
    return mim;
  }
  return {};
}

open_viii::graphics::background::Map ff_8::LoadMap(
  open_viii::archive::FIFLFS<false>           in_field,
  std::string_view                            coo,
  const open_viii::graphics::background::Mim &mim,
  std::string                                &out_path,
  bool                                       &coo_was_used)
{
  bool        shift        = false;
  std::size_t out_path_pos = {};
  auto        lang_name =
    fmt::format("_{}{}", coo, open_viii::graphics::background::Map::EXT);
  auto buffer = in_field.get_entry_data(
    { std::string_view(lang_name),
      open_viii::graphics::background::Map::Map::EXT },
    &out_path,
    &out_path_pos);
  coo_was_used = out_path_pos == 0U;// if true then the coo was picked.
  if (!std::ranges::empty(buffer))
  {
    spdlog::debug("loaded: {}", out_path);
    auto map =
      open_viii::graphics::background::Map{ mim.mim_type(), buffer, shift };
    return map;
  }
  return {};
}

bool ff_8::Fields::on_archive_change() const
{
  m_start_time = std::chrono::steady_clock::now();
  if (m_archive.on_im_gui_update())
  {
    m_map_data = m_archive.fields().map_data();
    m_field    = load_field();
    m_end_time = std::chrono::steady_clock::now();
    spdlog::debug(
      "time to load fields = {:%S} seconds", m_end_time - m_start_time);
    return true;
  }
  return false;
}

bool ff_8::Fields::on_field_change() const
{
  if (glengine::GenericCombo("Field", CurrentIndex, m_map_data))
  {
    m_field = load_field();
    return true;
  }
  return false;
}

bool ff_8::Fields::on_im_gui_update() const
{
  return on_archive_change() | on_field_change();
}

open_viii::archive::FIFLFS<false> ff_8::Fields::load_field() const
{
  open_viii::archive::FIFLFS<false> archive{};
  if (!m_map_data.empty() && std::cmp_less(CurrentIndex, m_map_data.size()))
  {
    m_archive.fields().execute_with_nested(
      { map_name() },
      [&archive](auto &&field) {
        archive = std::forward<decltype(field)>(field);
      },
      {},
      [](auto &&) { return true; },
      true);
  }
  else
  {
    spdlog::warn(
      "{}:{} - Index out of range {} / {}",
      __FILE__,
      __LINE__,
      CurrentIndex,
      m_map_data.size());
  }
  return archive;
}

std::string_view ff_8::Fields::coo() const
{
  return m_archive.coo();
}

std::string_view ff_8::Fields::map_name() const
{
  if (std::cmp_less(CurrentIndex, std::ranges::size(m_map_data)))
  {
    return m_map_data[static_cast<std::size_t>(CurrentIndex)];
  }
  const static auto tmp = std::string("");
  return tmp;
}

ff_8::Fields::Fields()
  : m_map_data(m_archive.fields().map_data())
  , m_field(load_field())
{
  spdlog::debug(
    "time to load fields = {:%S} seconds", m_end_time - m_start_time);
  auto pos = std::ranges::find(
    m_map_data, std::string("bgmdele1"));//"feopen2" //crtower3
  if (pos != m_map_data.end())
  {
    CurrentIndex = static_cast<int>(std::distance(m_map_data.begin(), pos));
    m_field      = load_field();
  }
}

const open_viii::archive::FIFLFS<false> *ff_8::Fields::operator->() const
{
  return &m_field;
}
ff_8::Fields::operator const open_viii::archive::FIFLFS<false> &() const
{
  return m_field;
}
