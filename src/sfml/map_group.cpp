//
// Created by pcvii on 2/28/2023.
//

#include "map_group.hpp"
#include "open_viii/strings/LangCommon.hpp"
#include <fmt/format.h>
static std::string str_to_lower(std::string input)
{
     std::string output{};
     output.reserve(std::size(input) + 1);
     std::ranges::transform(
       input, std::back_inserter(output), [](char character) -> char { return static_cast<char>(::tolower(character)); });
     return output;
}
namespace ff_8
{
static map_group::Mim load_mim(const map_group::SharedField &field, const map_group::Coo coo)
{
     if (!field)
     {
          return {};
     }
     auto lang_name = fmt::format("_{}{}", open_viii::LangCommon::to_string(coo), map_group::Mim::EXT);
     auto long_lang_name =
       fmt::format("{}_{}{}", field->get_base_name(), open_viii::LangCommon::to_string(coo), open_viii::graphics::background::Mim::EXT);
     auto long_name = fmt::format("{}{}", field->get_base_name(), open_viii::graphics::background::Mim::EXT);
     return { field->get_entry_data(
                { std::string_view(long_lang_name), std::string_view(long_name), std::string_view(lang_name), map_group::Mim::EXT }),
              str_to_lower(field->get_base_name()) };
}
static map_group::MapHistory load_map_history(
  const map_group::SharedField  &field,
  std::optional<map_group::Coo> &coo,
  const map_group::SharedMim    &mim,
  std::string                   *out_path,
  bool                           shift = true)
{
     if (!field)
     {
          return {};
     }
     return map_group::MapHistory{ load_map(field, coo, mim, out_path, shift) };
}
map_group::Map load_map(
  const map_group::SharedField  &field,
  std::optional<map_group::Coo> &coo,
  const map_group::SharedMim    &mim,
  std::string                   *out_path,
  bool                           shift)
{
     if (!field)
     {
          return {};
     }
     if (!coo)
     {
          coo = map_group::Coo::generic;
     }
     size_t out_path_pos = {};
     auto   lang_name    = fmt::format("_{}{}", open_viii::LangCommon::to_string(*coo), map_group::Map::EXT);
     auto   long_lang_name =
       fmt::format("{}_{}{}", field->get_base_name(), open_viii::LangCommon::to_string(*coo), open_viii::graphics::background::Map::EXT);
     auto long_name = fmt::format("{}{}", field->get_base_name(), open_viii::graphics::background::Map::EXT);
     auto map       = map_group::Map{
          mim->mim_type(),
          field->get_entry_data(
            { std::string_view(long_lang_name), std::string_view(long_name), std::string_view(lang_name), map_group::Map::EXT },
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
map_group::map_group(map_group::SharedField in_field, map_group::OptCoo in_coo = std::nullopt)
  : field{ std::move(in_field) }
  , mim{ std::make_shared<Mim>(load_mim(field, in_coo ? *in_coo : map_group::Coo::generic)) }
  , map_path{}
  , opt_coo{ in_coo }
  , maps{ load_map_history(field, opt_coo, mim, &map_path) }
{
}
}// namespace ff_8