//
// Created by pcvii on 11/30/2021.
//

#include "Fields.hpp"
[[nodiscard]] open_viii::graphics::background::Mim
  ff8::LoadMim(
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
[[nodiscard]] open_viii::graphics::background::Map
  ff8::LoadMap(
    open_viii::archive::FIFLFS<false>           field,
    std::string_view                            coo,
    const open_viii::graphics::background::Mim &mim,
    std::string                                &out_path,
    bool                                       &coo_was_used)
{
  bool        shift        = true;
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