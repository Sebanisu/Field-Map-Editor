#ifndef D739A4E1_EA08_4597_AD79_1F65B0F07014
#define D739A4E1_EA08_4597_AD79_1F65B0F07014


#pragma once
#include "path_search.hpp"// And other dependencies
#include <array>
#include <future>
#include <map>
#include <Texture.hpp>// Assuming your GL engine headers
namespace fme
{
class map_textures
{
   public:
     using BPPT                               = open_viii::graphics::BPPT;
     static constexpr std::uint8_t  TILE_SIZE = 16U;
     static constexpr std::uint8_t  MAX_TEXTURE_PAGES = 14U;
     static constexpr std::uint8_t  MAX_PALETTES      = 16U;
     static constexpr std::uint8_t  BPP_COMBOS        = 2U;
     static constexpr std::uint16_t START_OF_NO_PALETTE_INDEX
       = MAX_PALETTES * MAX_TEXTURE_PAGES;
     static constexpr std::uint16_t BPP16_INDEX = MAX_PALETTES * BPP_COMBOS + 1;
     static constexpr auto          MAX_TEXTURES
       = (std::max)(static_cast<std::uint16_t>(
                      START_OF_NO_PALETTE_INDEX + MAX_TEXTURE_PAGES),
                    static_cast<std::uint16_t>(BPP16_INDEX + 1U));
     using TextureArray = std::array<glengine::Texture, MAX_TEXTURES>;

     map_textures(std::weak_ptr<Selections> selections);

     void queue_texture_loading(const ff_8::TextureFilters &filters);
     std::future<void> load_mim_textures(
       const BPPT    bpp,
       const uint8_t palette);
     // ... other load/save methods
     const glengine::Texture *get_texture(
       const BPPT    bpp,
       const uint8_t palette,
       const uint8_t texture_page) const;
     void     consume_futures(const bool update = true);
     bool     all_futures_done() const;
     uint32_t get_max_texture_height() const;

   private:
     std::shared_ptr<TextureArray> m_textures;
     std::shared_ptr<std::map<std::string, glengine::Texture>>
                               m_full_filename_textures;
     // ... other members like m_future_consumer, m_cache_framebuffer
     std::weak_ptr<Selections> m_selections;
     // Helper functions like get_texture_pos()
};
}// namespace fme
#endif /* D739A4E1_EA08_4597_AD79_1F65B0F07014 */
