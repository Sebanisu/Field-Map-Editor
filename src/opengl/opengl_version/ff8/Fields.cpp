//
// Created by pcvii on 11/30/2021.
//

#include "Fields.hpp"
#include "GenericCombo.hpp"

namespace ff_8
{
static int current_index = {};
}

open_viii::graphics::background::Mim ff_8::LoadMim(
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
    spdlog::debug("loaded: {}", out_path);
    auto mim =
      open_viii::graphics::background::Mim{ buffer, field.get_base_name() };
    return mim;
  }
  return {};
}

open_viii::graphics::background::Map ff_8::LoadMap(
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
    spdlog::debug("loaded: {}", out_path);
    auto map =
      open_viii::graphics::background::Map{ mim.mim_type(), buffer, shift };
    return map;
  }
  return {};
}

bool ff_8::Fields::OnArchiveChange() const
{
  starttime = std::chrono::steady_clock::now();
  if (m_archive.on_im_gui_update())
  {
    m_map_data = m_archive.Fields().map_data();
    m_field    = load_field();
    endtime    = std::chrono::steady_clock::now();
    spdlog::debug("time to load fields = {:%S} seconds", endtime - starttime);
    return true;
  }
  return false;
}

bool ff_8::Fields::OnFieldChange() const
{
  if (glengine::GenericCombo("Field", current_index, m_map_data))
  {
    m_field = load_field();
    return true;
  }
  return false;
}

bool ff_8::Fields::on_im_gui_update() const
{
  return OnArchiveChange() | OnFieldChange();
}

open_viii::archive::FIFLFS<false> ff_8::Fields::load_field() const
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
    spdlog::warn(
      "{}:{} - Index out of range {} / {}",
      __FILE__,
      __LINE__,
      current_index,
      m_map_data.size());
  }
  return archive;
}

std::string_view ff_8::Fields::Coo() const
{
  return m_archive.Coo();
}

const std::string &ff_8::Fields::Map_Name() const
{
  if (std::cmp_less(current_index, std::ranges::size(m_map_data)))
  {
    return m_map_data[static_cast<std::size_t>(current_index)];
  }
  const static auto tmp = std::string("");
  return tmp;
}

ff_8::Fields::Fields()
  : m_map_data(m_archive.Fields().map_data())
  , m_field(load_field())
{
  spdlog::debug("time to load fields = {:%S} seconds", endtime - starttime);
  auto pos = std::ranges::find(
    m_map_data, std::string("bgmdele1"));//"feopen2" //crtower3
  if (pos != m_map_data.end())
  {
    current_index = static_cast<int>(std::distance(m_map_data.begin(), pos));
    m_field       = load_field();
  }
}

const open_viii::archive::FIFLFS<false> &ff_8::Fields::Field() const
{
  return m_field;
}
