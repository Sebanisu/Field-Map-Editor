//
// Created by pcvii on 9/6/2022.
//

#ifndef FIELD_MAP_EDITOR_UNIQUIFYPUPU_HPP
#define FIELD_MAP_EDITOR_UNIQUIFYPUPU_HPP
#include <ff_8/PupuID.hpp>
#include <map>

class UniquifyPupu
{
   public:
     struct PupuKey
     {
          ff_8::PupuID pupu_id                            = {};
          std::int16_t x                                  = {};
          std::int16_t y                                  = {};
          auto         operator<=>(const PupuKey &) const = default;
     };
     std::map<PupuKey, std::uint8_t> m_pupu_map = {};

     ff_8::PupuID                    operator()(
       const open_viii::graphics::background::is_tile auto &tile_const)
     {
          const auto tile_size = 16;
          const auto x_position
            = static_cast<int16_t>(tile_const.x() / tile_size);
          const auto y_position
            = static_cast<int16_t>(tile_const.y() / tile_size);
          auto input_value
            = PupuKey{ ff_8::PupuID(tile_const), x_position, y_position };
          auto insert_key = [&](PupuKey key) -> ff_8::PupuID
          {
               if (m_pupu_map.contains(key))
               {
                    ++(m_pupu_map.at(key));
                    return key.pupu_id + m_pupu_map.at(key);
               }
               else
               {
                    m_pupu_map.emplace(key, std::uint8_t{});
                    return key.pupu_id + m_pupu_map.at(key);
               }
          };
          return insert_key(input_value);
     }
     //  operator UniqueValues<ff_8::PupuID>() const
     //  {
     //    std::vector<ff_8::PupuID> values{};
     //    std::ranges::transform(m_pupu_map,
     //    std::back_insert_iterator(values),[](auto &&
     //    key_value)->ff_8::PupuID{
     //      const auto & [key,value] = key_value;
     //      return key.pupu_id;
     //    });
     //    UniqueValues<ff_8::PupuID>{std::move(values)};
     //  }
};
#endif// FIELD_MAP_EDITOR_UNIQUIFYPUPU_HPP
