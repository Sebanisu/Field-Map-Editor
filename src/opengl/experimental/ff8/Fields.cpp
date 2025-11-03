//
// Created by pcvii on 11/30/2021.
//

#include "Fields.hpp"
#include "Configuration.hpp"
#include <fmt/chrono.h>
#include <glengine/GenericCombo.hpp>

namespace ff_8
{
static constexpr auto fields_index  = std::string_view("fields_index");
static constexpr auto fields_string = std::string_view("fields_string");
MimData::MimData(const Fields &fields)
  : mim(LoadMim(
      fields,
      fields.coo(),
      path,
      coo_chosen))
{
     if (!std::empty(path))
     {
          spdlog::debug("Loaded Mim {}", path);
          spdlog::debug("Loading Textures from Mim");
          delayed_textures = LoadTextures(mim);
     }
}
open_viii::graphics::background::Mim LoadMim(
  open_viii::archive::FIFLFS<false> in_field,
  std::string_view                  coo,
  std::string                      &out_path,
  bool                             &coo_was_used)
{
     std::size_t out_path_pos = {};
     auto        lang_name
       = fmt::format("_{}{}", coo, open_viii::graphics::background::Mim::EXT);
     auto long_lang_name = fmt::format(
       "{}_{}{}",
       in_field.get_base_name(),
       coo,
       open_viii::graphics::background::Mim::EXT);
     auto long_name = fmt::format(
       "{}{}",
       in_field.get_base_name(),
       open_viii::graphics::background::Mim::EXT);


     auto buffer = in_field.get_entry_data(
       { std::string_view(long_lang_name), std::string_view(long_name),
         std::string_view(lang_name),
         open_viii::graphics::background::Mim::EXT },
       &out_path,
       &out_path_pos);
     coo_was_used = out_path_pos == 0U;
     if (!std::ranges::empty(buffer))
     {
          spdlog::debug("loaded: {}", out_path);
          auto mim
            = open_viii::graphics::background::Mim{ buffer,
                                                    in_field.get_base_name() };
          return mim;
     }
     return {};
}

open_viii::graphics::background::Map LoadMap(
  open_viii::archive::FIFLFS<false> in_field,
  std::string_view                  coo,
  const MimData                    &mim,
  std::string                      &out_path,
  bool                             &coo_was_used)
{
     bool        shift        = false;
     std::size_t out_path_pos = {};
     auto        lang_name
       = fmt::format("_{}{}", coo, open_viii::graphics::background::Map::EXT);
     auto long_lang_name = fmt::format(
       "{}_{}{}",
       in_field.get_base_name(),
       coo,
       open_viii::graphics::background::Map::EXT);
     auto long_name = fmt::format(
       "{}{}",
       in_field.get_base_name(),
       open_viii::graphics::background::Map::EXT);
     auto buffer = in_field.get_entry_data(
       { std::string_view(long_lang_name), std::string_view(long_name),
         std::string_view(lang_name),
         open_viii::graphics::background::Map::Map::EXT },
       &out_path,
       &out_path_pos);
     coo_was_used = out_path_pos == 0U;// if true then the coo was picked.
     if (!std::ranges::empty(buffer))
     {
          spdlog::debug("loaded: {}", out_path);
          auto map = open_viii::graphics::background::Map{ mim->mim_type(),
                                                           buffer, shift };
          return map;
     }
     return {};
}

bool Fields::on_archive_change() const
{
     m_start_time = std::chrono::steady_clock::now();
     if (m_archive.on_im_gui_update())
     {
          m_map_data          = m_archive.fields().map_data();
          m_field             = load_field();
          m_end_time          = std::chrono::steady_clock::now();
          const auto duration = m_end_time - m_start_time;
          spdlog::debug("time to load fields = {:%S} seconds", duration);
          return true;
     }
     return false;
}

bool Fields::on_field_change() const
{
     if (glengine::GenericCombo("Field", m_current_index, m_map_data))
     {
          m_field = load_field();
          if (std::cmp_less(m_current_index, std::ranges::size(m_map_data)))
          {
               auto config = Configuration{};
               config->insert_or_assign(fields_index, m_current_index);

               config->insert_or_assign(
                 fields_string,
                 m_map_data[static_cast<std::size_t>(m_current_index)]);
               config.save();
          }
          return true;
     }
     return false;
}

bool Fields::on_im_gui_update() const
{
     return on_archive_change() | on_field_change();
}

open_viii::archive::FIFLFS<false> Fields::load_field() const
{
     open_viii::archive::FIFLFS<false> archive{};
     if (
       !m_map_data.empty() && std::cmp_less(m_current_index, m_map_data.size()))
     {
          m_archive.fields().execute_with_nested(
            { map_name() },
            [&archive](auto &&field)
            { archive = std::forward<decltype(field)>(field); },
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
            m_current_index,
            m_map_data.size());
     }
     return archive;
}

std::string_view Fields::coo() const
{
     return m_archive.coo();
}

std::string_view Fields::map_name() const
{
     if (std::cmp_less(m_current_index, std::ranges::size(m_map_data)))
     {
          return m_map_data[static_cast<std::size_t>(m_current_index)];
     }
     const static auto tmp = std::string("");
     return tmp;
}
Fields::Fields()
  : m_current_index(Configuration{}[fields_index].value_or(int{}))
  , m_map_data(m_archive.fields().map_data())
  , m_field(load_field())
{
     const auto duration = m_end_time - m_start_time;
     spdlog::debug("time to load fields = {:%S} seconds", duration);
     //  auto pos = std::ranges::find(
     //    m_map_data, std::string("bgmdele1"));//"feopen2" //crtower3
     //  if (pos != m_map_data.end())
     //  {
     //    m_current_index = static_cast<int>(std::distance(m_map_data.begin(),
     //    pos)); m_field         = load_field();
     //  }
}

const open_viii::archive::FIFLFS<false> *Fields::operator->() const
{
     return &m_field;
}
Fields::operator const open_viii::archive::FIFLFS<false> &() const
{
     return m_field;
}

}// namespace ff_8