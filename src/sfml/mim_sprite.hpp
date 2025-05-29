//
// Created by pcvii on 9/4/2021.
//

#ifndef FIELD_MAP_EDITOR_MIM_SPRITE_HPP
#define FIELD_MAP_EDITOR_MIM_SPRITE_HPP
#include "format_imgui_text.hpp"
#include "grid.hpp"
#include "open_viii/archive/Archives.hpp"
#include "open_viii/graphics/background/Mim.hpp"
#include "open_viii/graphics/Png.hpp"
#include <imgui.h>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <vector>
// this will hold class that has enough info to load and draw mim files.
struct mim_sprite final
  : public sf::Drawable
  , public sf::Transformable
{


   private:
     std::weak_ptr<open_viii::archive::FIFLFS<false>>            m_field             = {};
     open_viii::LangT                                            m_coo               = {};
     mutable std::string                                         m_mim_path          = {};
     open_viii::graphics::background::Mim                        m_mim               = {};
     open_viii::graphics::BPPT                                   m_bpp               = {};
     std::uint8_t                                                m_palette           = {};
     bool                                                        m_draw_palette      = { false };
     std::vector<open_viii::graphics::Color32RGBA>               m_colors            = {};
     std::shared_ptr<sf::Texture>                                m_texture           = {};
     std::array<sf::Vertex, 4U>                                  m_vertices          = {};
     grid                                                        m_grid              = {};
     grid                                                        m_texture_page_grid = {};
     [[nodiscard]] open_viii::graphics::background::Mim          get_mim() const;
     [[nodiscard]] static open_viii::graphics::BPPT              get_bpp(const open_viii::graphics::BPPT &in_bpp);
     [[nodiscard]] std::shared_ptr<sf::Texture>                  find_texture() const;
     [[nodiscard]] std::vector<open_viii::graphics::Color32RGBA> get_colors();
     [[nodiscard]] std::array<sf::Vertex, 4U>                    get_vertices() const;

   public:
     const sf::Texture *get_texture() const
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
       const open_viii::graphics::BPPT                   &in_bpp,
       const std::uint8_t                                &in_palette,
       open_viii::LangT                                   in_coo,
       bool                                               force_draw_palette = false);
     //  /**
     //   * Get sprite of palette texture
     //   * @param in_field
     //   */
     //  explicit mim_sprite(const open_viii::archive::FIFLFS<false> &in_field)
     //    : m_field(&in_field), m_mim(get_mim()), m_draw_palette(true),
     //      m_colors(get_colors()), m_texture(get_texture()),
     //      m_sprite(get_sprite())
     //  {}
     /**
      * create a new object with a new field and the same settings
      * @param in_field
      * @return mim_sprite object
      */
     [[nodiscard]] mim_sprite    with_field(std::weak_ptr<open_viii::archive::FIFLFS<false>> in_field) const;
     /**
      * create a new object with a new bits per pixel and the same settings
      * @param in_bpp
      * @return mim_sprite object
      */
     [[nodiscard]] mim_sprite    with_bpp(const open_viii::graphics::BPPT &in_bpp) const;
     /**
      * create a new object with a new palette and the same settings
      * @param in_bpp
      * @return mim_sprite object
      */
     [[nodiscard]] mim_sprite    with_palette(const std::uint8_t &in_palette) const;
     /**
      * create a new object and change coo.
      */
     [[nodiscard]] mim_sprite    with_coo(open_viii::LangT in_coo) const;
     /**
      * create a new object and Toggle drawing palette.
      */
     [[nodiscard]] mim_sprite    with_draw_palette(bool in_draw_palette) const;
     //  /**
     //   * Getter for sprite, required for changing position and drawing.
     //   * @todo maybe want to control access.
     //   * @return Sprite
     //   */
     //  [[nodiscard]] sf::Sprite &sprite() const noexcept;
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
     [[nodiscard]] const open_viii::graphics::background::Mim &mim() const noexcept;
     void                                                      save(const std::filesystem::path &dest_path) const;

     void                                                      draw(sf::RenderTarget &target, sf::RenderStates states) const final;
     std::string                                               mim_filename() const;
     void                                                      mim_save(const std::filesystem::path &dest_path) const;
     const mim_sprite                                         &toggle_grids(bool enable_grid, bool enable_texture_page_grid);
};
#endif