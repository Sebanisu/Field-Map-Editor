#include "PupuID.hpp"
using namespace open_viii::graphics::background;
namespace ff_8
{
[[nodiscard]] std::string PupuID::create_summary() const
{
     return fmt::format(
       "Layer ID: {}\nBlend Mode: {}\nAnimation ID: {}\nAnimation State: {}\nOffset: {}\nX not aligned: {}\nY not aligned: {}",
       layer_id(),
       blend_mode(),
       animation_id(),
       animation_state(),
       offset(),
       is_x_not_aligned_to_grid(),
       is_y_not_aligned_to_grid());
}

// You are using all 32 bits without overlap.
// The layer field uses 7 bits, which are stored in bits 24-30 of the m_raw value.
// The blend field uses only 3 bits, which are stored in bits 20-22 of the m_raw value.
// The animation id and animation state fields are 8 bits each, which are stored in bits 12-19 and 4-11 respectively.
// The offset field uses 4 bits, which are stored in bits 0-3 of the m_raw value.
// Finally, you use two more bits (31 and 23) for the x_not_aligned_to_grid and y_not_aligned_to_grid flags, respectively.
// Therefore, all 32 bits are used without any overlap or waste of space.


// template PupuID::PupuID(const Tile1 &, std::uint8_t);
// template PupuID::PupuID(const Tile2 &, std::uint8_t);
// template PupuID::PupuID(const Tile3 &, std::uint8_t);


// PupuID = uint32_t(0U + (tile.layer_id() <<
// 24U)+(static_cast<std::uint32_t>(tile.blend_mode()) << 20U) +
// (tile.animation_id() <<12U)+ (tile.animation_state()<<4U))

// std::transform(Sprites.cbegin(), Sprites.cend() - 1, Sprites.cbegin() + 1,
// Sprites.begin()+1, [](const Sprite& first, Sprite second) {
//     static constexpr auto mask = 0xFFFFFFF0U;
//     if ((first.ID & mask) == (second.ID & mask))
//     {
//       second.ID = first.ID + 1;
//     }
//     return second;
//   });

// IDs.reserve(Sprites.size());
// std::transform(Sprites.cbegin(), Sprites.cend(), std::back_inserter(IDs),
// [](const Sprite& sprite) {return sprite.ID; }); std::sort(IDs.begin(),
// IDs.end()); auto it = std::unique(IDs.begin(), IDs.end()); IDs.erase(it,
// IDs.end());
}// namespace ff_8