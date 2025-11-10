//
// Created by pcvii on 2/28/2023.
//

#include "MapGroup.hpp"
#include "open_viii/strings/LangCommon.hpp"
#include <fmt/format.h>
static std::string str_to_lower(std::string input)
{
     std::string output{};
     output.reserve(std::size(input) + 1);
     std::ranges::transform(
       input,
       std::back_inserter(output),
       [](char character) -> char
       { return static_cast<char>(::tolower(character)); });
     return output;
}

namespace ff_8
{
static MapGroup::Mim load_mim(
  const MapGroup::WeakField &weak_field,
  const MapGroup::Coo        coo)
{
     const auto field = weak_field.lock();
     if (!field)
     {
          spdlog::error("Failed to lock weak_field: shared_ptr is expired.");
          return {};
     }
     auto lang_name = fmt::format(
       "_{}{}", open_viii::LangCommon::to_string(coo), MapGroup::Mim::EXT);
     auto long_lang_name = fmt::format(
       "{}_{}{}",
       field->get_base_name(),
       open_viii::LangCommon::to_string(coo),
       open_viii::graphics::background::Mim::EXT);
     auto long_name = fmt::format(
       "{}{}",
       field->get_base_name(),
       open_viii::graphics::background::Mim::EXT);
     return { field->get_entry_data(
                { std::string_view(long_lang_name), std::string_view(long_name),
                  std::string_view(lang_name), MapGroup::Mim::EXT }),
              str_to_lower(std::string{ field->get_base_name() }) };
}
static MapGroup::MapHistory load_map_history(
  const MapGroup::WeakField    &weak_field,
  std::optional<MapGroup::Coo> &coo,
  const MapGroup::SharedMim    &mim,
  std::string                  *out_path,
  bool                          shift = true)
{
     const auto field = weak_field.lock();
     if (!field)
     {
          spdlog::error("Failed to lock weak_field: shared_ptr is expired.");
          return {};
     }
     return MapGroup::MapHistory{ load_map(field, coo, mim, out_path, shift) };
}
MapGroup::Map load_map(
  const MapGroup::WeakField    &weak_field,
  std::optional<MapGroup::Coo> &coo,
  const MapGroup::SharedMim    &mim,
  std::string                  *out_path,
  bool                          shift)
{
     const auto field = weak_field.lock();
     if (!field)
     {
          spdlog::error("Failed to lock weak_field: shared_ptr is expired.");
          return {};
     }
     if (!coo)
     {
          coo = MapGroup::Coo::generic;
     }
     size_t out_path_pos = {};
     auto   lang_name    = fmt::format(
       "_{}{}", open_viii::LangCommon::to_string(*coo), MapGroup::Map::EXT);
     auto long_lang_name = fmt::format(
       "{}_{}{}",
       field->get_base_name(),
       open_viii::LangCommon::to_string(*coo),
       open_viii::graphics::background::Map::EXT);
     auto long_name = fmt::format(
       "{}{}",
       field->get_base_name(),
       open_viii::graphics::background::Map::EXT);
     auto map = MapGroup::Map{
          mim->mim_type(),
          field->get_entry_data(
            { std::string_view(long_lang_name), std::string_view(long_name),
              std::string_view(lang_name), MapGroup::Map::EXT },
            out_path,
            &out_path_pos),
          shift
     };
     if (out_path_pos != 0U)
     {
          coo = std::nullopt;
     }
     return map;
}
MapGroup::MapGroup(
  MapGroup::WeakField in_field,
  MapGroup::OptCoo    in_coo = std::nullopt)
  : field{ std::move(in_field) }
  , mim{ std::make_shared<Mim>(load_mim(
      field,
      in_coo ? *in_coo : MapGroup::Coo::generic)) }
  , map_path{}
  , opt_coo{ in_coo }
  , maps{ load_map_history(
      field,
      opt_coo,
      mim,
      &map_path) }
{
}
}// namespace ff_8