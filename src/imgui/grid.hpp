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
  sf::Vector2u m_spacing = {};
    sf::Vector2u m_size = {};
  std::vector<sf::Vertex>        m_vertices = {};

  std::vector<sf::Vertex> get_vertices() const;

public:
  grid() = default;
  grid(const sf::Vector2u &spacing, const sf::Vector2u &size);

  void draw(sf::RenderTarget &target, sf::RenderStates states) const final;
  grid with_spacing_and_size(const sf::Vector2u &spacing,
    const sf::Vector2u                          &size) const;
};
#endif// MYPROJECT_GRID_HPP
