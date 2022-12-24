//
// Created by pcvii on 12/23/2022.
//

#include "square.hpp"
std::array<sf::Vertex, 5U> square::get_vertices() const
{
  std::array<sf::Vertex, 5U> ret = {
    sf::Vertex{ sf::Vector2f(0.f, 0.f), m_color },
    sf::Vertex{ sf::Vector2f(1.f, 0.f), m_color },
    sf::Vertex{ sf::Vector2f(1.f, 1.f), m_color },
    sf::Vertex{ sf::Vector2f(0.f, 1.f), m_color },
    sf::Vertex{ sf::Vector2f(0.f, 0.f), m_color }
  };
  std::ranges::transform(ret, ret.begin(), [this](sf::Vertex v) {
    v.position.x = (v.position.x * static_cast<float>(m_size.x))
                   + static_cast<float>(m_position.x);
    v.position.y = (v.position.y * static_cast<float>(m_size.y))
                   + static_cast<float>(m_position.y);
    return v;
  });
  return ret;
}
void square::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  if (!m_enable)
  {
    return;
  }
  states.texture = nullptr;
  states.transform *= getTransform();
  states.blendMode = sf::BlendAlpha;
  target.draw(
    m_vertices.data(), m_vertices.size(), sf::PrimitiveType::LineStrip, states);
}
square square::with_position_and_size(
  const sf::Vector2u &position,
  const sf::Vector2u &size) const
{
  if (position == m_position && size == m_size)
  {
    return *this;
  }
  return { position, size, m_color };
}
square square::with_position(const sf::Vector2u &position) const
{
  if (position == m_position)
  {
    return *this;
  }
  return { position, m_size, m_color };
}
square square::with_color(const sf::Color &color)
{
  if (color == m_color)
  {
    return *this;
  }

  return { m_position, m_size, color };
}
square::square(
  const sf::Vector2u &position,
  const sf::Vector2u &size,
  const sf::Color    &color)
  : m_position(position)
  , m_size(size)
  , m_color(color)
  , m_vertices(get_vertices())
{
}

void square::enable() const
{
  m_enable = true;
}

void square::disable() const
{
  m_enable = false;
}
