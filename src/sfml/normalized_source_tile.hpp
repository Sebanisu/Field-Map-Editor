#ifndef CC1E517B_6340_4FF2_B4A0_BE029215A6E6
#define CC1E517B_6340_4FF2_B4A0_BE029215A6E6
#include <open_viii/graphics/background/Map.hpp>
namespace open_viii::graphics::background
{
struct normalized_source_tile
{
   public:
     TexIdBuffer         m_tex_id_buffer                              = {};
     PaletteID           m_palette_id                                 = {};
     Point<std::uint8_t> m_source_xy                                  = {};
     LayerID             m_layer_id                                   = {};
     BlendModeT          m_blend_mode                                 = BlendModeT::none;
     std::uint8_t        m_animation_id                               = 0xFFU;
     std::uint8_t        m_animation_state                            = {};
     normalized_source_tile()                                         = default;
     constexpr auto operator<=>(const normalized_source_tile &) const = default;
     normalized_source_tile(const is_tile auto &tile)
     {
          m_tex_id_buffer =
            m_tex_id_buffer.with_id(tile.texture_id()).with_blend(tile.blend()).with_depth(tile.depth()).with_draw(tile.draw());
          m_palette_id = m_palette_id.with_id(tile.palette_id());
          assert(std::cmp_less(tile.source_x(), (std::numeric_limits<std::uint8_t>::max)()));
          assert(std::cmp_less(tile.source_y(), (std::numeric_limits<std::uint8_t>::max)()));
          m_source_xy       = Point<std::uint8_t>{ static_cast<std::uint8_t>(tile.source_x()), static_cast<std::uint8_t>(tile.source_y()) };
          m_layer_id        = m_layer_id.with_id(tile.layer_id());
          m_blend_mode      = tile.blend_mode();
          m_animation_id    = tile.animation_id();
          m_animation_state = tile.animation_state();
     }
     normalized_source_tile &operator=(const is_tile auto &tile)
     {
          m_tex_id_buffer =
            m_tex_id_buffer.with_id(tile.texture_id()).with_blend(tile.blend()).with_depth(tile.depth()).with_draw(tile.draw());
          m_palette_id      = m_palette_id.with_id(tile.palette_id());
          assert(std::cmp_less(tile.source_x(), (std::numeric_limits<std::uint8_t>::max)()));
          assert(std::cmp_less(tile.source_y(), (std::numeric_limits<std::uint8_t>::max)()));
          m_source_xy       = Point<std::uint8_t>{ static_cast<std::uint8_t>(tile.source_x()), static_cast<std::uint8_t>(tile.source_y()) };
          m_layer_id        = m_layer_id.with_id(tile.layer_id());
          m_blend_mode      = tile.blend_mode();
          m_animation_id    = tile.animation_id();
          m_animation_state = tile.animation_state();
     }
     normalized_source_tile(const normalized_source_tile &)                = default;
     normalized_source_tile &operator=(const normalized_source_tile &)     = default;
     normalized_source_tile(normalized_source_tile &&) noexcept            = default;
     normalized_source_tile &operator=(normalized_source_tile &&) noexcept = default;
};
}// namespace open_viii::graphics::background
#endif /* CC1E517B_6340_4FF2_B4A0_BE029215A6E6 */
