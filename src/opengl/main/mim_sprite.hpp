//
// Created by pcvii on 9/4/2021.
//

#ifndef FIELD_MAP_EDITOR_MIM_SPRITE_HPP
#define FIELD_MAP_EDITOR_MIM_SPRITE_HPP
#include "format_imgui_text.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "open_viii/graphics/Png.hpp"
#include <glengine/Texture.hpp>
#include <imgui.h>
#include <vector>
// this will hold class that has enough info to load and draw mim files.
namespace fme
{
struct mim_sprite
{
   private:
     std::weak_ptr<open_viii::archive::FIFLFS<false>> m_field     = {};
     open_viii::LangT                                 m_coo       = {};
     mutable std::string                              m_mim_path  = {};
     open_viii::graphics::background::Mim             m_mim       = {};
     open_viii::graphics::BPPT                        m_bpp       = {};
     std::uint8_t                                     m_palette   = {};
     bool                                          m_draw_palette = { false };
     std::vector<open_viii::graphics::Color32RGBA> m_colors       = {};
     std::shared_ptr<glengine::Texture>            m_texture      = {};
     [[nodiscard]] open_viii::graphics::background::Mim get_mim() const;
     [[nodiscard]] static open_viii::graphics::BPPT
       get_bpp(const open_viii::graphics::BPPT &in_bpp);
     [[nodiscard]] std::shared_ptr<glengine::Texture> find_texture() const;
     [[nodiscard]] std::vector<open_viii::graphics::Color32RGBA> get_colors();

   public:
     const glengine::Texture *get_texture() const
     {
          return m_texture.get();
     }
     mim_sprite() = default;
     /**
      * Get sprite of mim file with bpp and palette set
      * @param in_field
      * @param in_bpp
      * @param in_palette
      */
     [[maybe_unused]] mim_sprite(
       std::weak_ptr<open_viii::archive::FIFLFS<false>> in_field,
       const open_viii::graphics::BPPT                 &in_bpp,
       const std::uint8_t                              &in_palette,
       open_viii::LangT                                 in_coo,
       bool force_draw_palette = false);

     /**
      * create a new object with a new field and the same settings
      * @param in_field
      * @return mim_sprite object
      */
     [[nodiscard]] mim_sprite with_field(
       std::weak_ptr<open_viii::archive::FIFLFS<false>> in_field) const;

     /**
      * create a new object with a new bits per pixel and the same settings
      * @param in_bpp
      * @return mim_sprite object
      */
     [[nodiscard]] mim_sprite
       with_bpp(const open_viii::graphics::BPPT &in_bpp) const;

     /**
      * create a new object with a new palette and the same settings
      * @param in_bpp
      * @return mim_sprite object
      */
     [[nodiscard]] mim_sprite
       with_palette(const std::uint8_t &in_palette) const;

     /**
      * create a new object and change coo.
      */
     [[nodiscard]] mim_sprite    with_coo(open_viii::LangT in_coo) const;

     /**
      * create a new object and Toggle drawing palette.
      */
     [[nodiscard]] mim_sprite    with_draw_palette(bool in_draw_palette) const;

     /**
      * @return width in px
      */
     [[nodiscard]] std::uint32_t width() const noexcept;

     /**
      * @return height in px
      */
     [[nodiscard]] std::uint32_t height() const noexcept;

     /**
      * If in draw palette mode
      * @return true or false
      */
     [[nodiscard]] bool          draw_palette() const noexcept;

     /**
      * If failed state
      * @return true or false
      */
     [[nodiscard]] bool          fail() const noexcept;

     [[nodiscard]] const open_viii::graphics::background::Mim           &
       mim() const noexcept;

     void        save(const std::filesystem::path &dest_path) const;

     std::string mim_filename() const;

     void        mim_save(const std::filesystem::path &dest_path) const;
};
}// namespace fme
#endif