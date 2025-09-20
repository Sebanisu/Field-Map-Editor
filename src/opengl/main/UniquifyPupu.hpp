//
// Created by pcvii on 9/6/2022.
//

#ifndef FIELD_MAP_EDITOR_UNIQUIFYPUPU_HPP
#define FIELD_MAP_EDITOR_UNIQUIFYPUPU_HPP
#include "PupuID.hpp"
#include <map>
namespace ff_8
{
class UniquifyPupu
{
   public:
     struct PupuKey
     {
          PupuID       pupu_id                            = {};
          std::int16_t x                                  = {};
          std::int16_t y                                  = {};
          auto         operator<=>(const PupuKey &) const = default;
     };

     PupuID operator()(
       const open_viii::graphics::background::is_tile auto &tile_const);

   private:
     std::map<PupuKey, std::uint8_t> m_pupu_map = {};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_UNIQUIFYPUPU_HPP
