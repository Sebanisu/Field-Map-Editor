//
// Created by pcvii on 12/23/2022.
//
#ifndef FIELD_MAP_EDITOR_SQUARE_HPP
#define FIELD_MAP_EDITOR_SQUARE_HPP
#include <array>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
struct square final
  : public sf::Drawable
  , public sf::Transformable
{
private:
  sf::Vector2u               m_position = {};
  sf::Vector2u               m_size     = {};
  sf::Color                  m_color    = sf::Color::Red;
  std::array<sf::Vertex, 5U> m_vertices = {};
  mutable bool               m_enable   = { false };

  std::array<sf::Vertex, 5U> get_vertices() const;

public:
  square(
    const sf::Vector2u &position,
    const sf::Vector2u &size,
    const sf::Color    &color = sf::Color::Red);

  void   draw(sf::RenderTarget &target, sf::RenderStates states) const final;
  square with_position_and_size(
    const sf::Vector2u &position,
    const sf::Vector2u &size) const;
  square with_position(
    const sf::Vector2u &position) const;
  square      with_color(const sf::Color &color);
  void        enable() const;
  void        disable() const;
  std::size_t count() const
  {
    return std::size(m_vertices);
  }
};
#endif// FIELD_MAP_EDITOR_SQUARE_HPP
