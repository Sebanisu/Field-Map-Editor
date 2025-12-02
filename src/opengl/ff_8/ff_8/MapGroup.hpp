//
// Created by pcvii on 2/28/2023.
//

#ifndef FIELD_MAP_EDITOR_MAP_GROUP_HPP
#define FIELD_MAP_EDITOR_MAP_GROUP_HPP
#include "MapHistory.hpp"
#include "open_viii/archive/FIFLFS.hpp"
#include "open_viii/graphics/background/Map.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "open_viii/strings/LangT.hpp"
#include <memory>
namespace ff_8
{
struct MapGroup
{
     // required to initialize
     using Field                     = open_viii::archive::FIFLFS<false>;
     using WeakField                 = std::weak_ptr<Field>;
     using Mim                       = open_viii::graphics::background::Mim;
     using MimType                   = open_viii::graphics::background::MimType;
     using SharedMim                 = std::shared_ptr<Mim>;
     using Map                       = open_viii::graphics::background::Map;
     using MapHistory                = ff_8::MapHistory;

     using Coo                       = open_viii::LangT;
     using OptCoo                    = std::optional<Coo>;
     static constexpr auto TILE_SIZE = 16U;
     MapGroup()                      = default;
     MapGroup(
       WeakField,
       OptCoo);
     WeakField   field{};
     SharedMim   mim{};
     std::string map_path{};
     // if coo was discarded this is nullopt;
     OptCoo      opt_coo{};
     MapHistory  maps{};
};
MapGroup::Map load_map(
  const MapGroup::WeakField    &field,
  std::optional<MapGroup::Coo> &coo,
  const MapGroup::SharedMim    &mim,
  std::string                  *out_path,
  bool                          shift);
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_MAP_GROUP_HPP
