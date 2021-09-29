//
// Created by pcvii on 9/28/2021.
//

#ifndef MYPROJECT_GRID_HPP
#define MYPROJECT_GRID_HPP
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <vector>
struct grid
  : public sf::Drawable
  , public sf::Transformable
{
private:
  std::vector<sf::Vertex>        m_vertices = {};

  static std::vector<sf::Vertex> get_vertices(const sf::Vector2u &spacing,
    const sf::Vector2u                                           &size);

public:
  grid() = default;
  grid(const sf::Vector2u &spacing, const sf::Vector2u &size);

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
};
#endif// MYPROJECT_GRID_HPP
