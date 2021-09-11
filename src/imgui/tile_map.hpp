//
// Created by pcvii on 9/8/2021.
//

#ifndef MYPROJECT_TILE_MAP_HPP
#define MYPROJECT_TILE_MAP_HPP
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>
/**
 * I cleaned up the code from this example. I need to adapt this to how Map
 * stores it's data. Basically the I need to create a vertex array for every
 * pupu layer. I'd need a unique set of textures I could assign. Because each
 * palette and bpp combo is different. Easy step would be getting all the
 * textures. Maybe next converting the x,y's to quads. This code has no z axis.
 * I wonder what to do with that.
 * @see
 * https://www.sfml-dev.org/tutorials/2.5/graphics-vertex-array.php#example-tile-map
 */
class [[maybe_unused]] tile_map
  : public sf::Drawable
  , public sf::Transformable
{
public:
  [[maybe_unused]] bool load(const std::string &tileset,
    sf::Vector2u                                tileSize,
    std::vector<std::uint32_t>                  tiles,
    std::uint32_t                               width,
    std::uint32_t                               height)
  {
    // load the tileset texture
    if (!m_tileset.loadFromFile(tileset)) {
      return false;
    }

    // resize the vertex array to fit the level size
    m_vertices.setPrimitiveType(sf::Quads);
    m_vertices.resize(width * height * 4);

    // populate the vertex array, with one quad per tile
    open_viii::tools::for_each_xy(width,
      height,
      [&tiles, this, &tileSize, &width](std::uint32_t i, std::uint32_t j) {
        // get the current tile number
        std::uint32_t tileNumber = tiles[i + (j * width)];

        // find its position in the tileset texture
        std::uint32_t tu = tileNumber % (m_tileset.getSize().x / tileSize.x);
        std::uint32_t tv = tileNumber / (m_tileset.getSize().x / tileSize.x);

        // get a pointer to the current tile's quad
        std::span<sf::Vertex> quad{ &m_vertices[(i + j * width) * 4], 4 };

        // define its 4 corners
        quad[0].position = sf::Vector2f(static_cast<float>(i * tileSize.x),
          static_cast<float>(j * tileSize.y));
        quad[1].position =
          sf::Vector2f(static_cast<float>((i + 1) * tileSize.x),
            static_cast<float>(j * tileSize.y));
        quad[2].position =
          sf::Vector2f(static_cast<float>((i + 1) * tileSize.x),
            static_cast<float>((j + 1) * tileSize.y));
        quad[3].position  = sf::Vector2f(static_cast<float>(i * tileSize.x),
           static_cast<float>((j + 1) * tileSize.y));

        // define its 4 texture coordinates
        quad[0].texCoords = sf::Vector2f(static_cast<float>(tu * tileSize.x),
          static_cast<float>(tv * tileSize.y));
        quad[1].texCoords =
          sf::Vector2f(static_cast<float>((tu + 1) * tileSize.x),
            static_cast<float>(tv * tileSize.y));
        quad[2].texCoords =
          sf::Vector2f(static_cast<float>((tu + 1) * tileSize.x),
            static_cast<float>((tv + 1) * tileSize.y));
        quad[3].texCoords = sf::Vector2f(static_cast<float>(tu * tileSize.x),
          static_cast<float>((tv + 1) * tileSize.y));
      });

    return true;
  }

private:
  void draw(sf::RenderTarget &target, sf::RenderStates states) const final
  {
    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &m_tileset;

    // draw the vertex array
    target.draw(m_vertices, states);
  }

  sf::VertexArray m_vertices;
  sf::Texture     m_tileset;
};
#endif// MYPROJECT_TILE_MAP_HPP
