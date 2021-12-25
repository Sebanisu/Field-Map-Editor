//
// Created by pcvii on 9/28/2021.
//

#include "grid.hpp"
#include <iostream>
std::vector<sf::Vertex> grid::get_vertices() const
{
  std::vector<sf::Vertex> ret     = {};
  const unsigned int      x_count = m_size.x / m_spacing.x;
  const unsigned int      y_count = m_size.y / m_spacing.y;
  const auto              count   = x_count + y_count;
  if (count <= 2U)
  {
    return ret;
  }
  ret.reserve(count);

  const auto tovec = [](auto &&in_x, auto &&in_y) {
    return sf::Vector2f{ static_cast<float>(in_x), static_cast<float>(in_y) };
  };
  if (x_count != 0)
  {
    for (unsigned int x = 1U; x != x_count; ++x)
    {
      ret.emplace_back(tovec(x * m_spacing.x, 0.F), m_color);
      ret.emplace_back(tovec(x * m_spacing.x, m_size.y), m_color);
    }
  }
  if (y_count != 0)
  {
    for (unsigned int y = 1U; y != y_count; ++y)
    {
      ret.emplace_back(tovec(0.F, y * m_spacing.y), m_color);
      ret.emplace_back(tovec(m_size.x, y * m_spacing.y), m_color);
    }
  }
  return ret;
}
void grid::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  if (!m_enable)
  {
    return;
  }
  states.texture = nullptr;
  states.transform *= getTransform();
  states.blendMode = sf::BlendAlpha;
  target.draw(m_vertices.data(), m_vertices.size(), sf::Lines, states);
}
grid grid::with_spacing_and_size(
  const sf::Vector2u &spacing,
  const sf::Vector2u &size) const
{
  if (spacing == m_spacing && size == m_size)
  {
    return *this;
  }
  return { spacing, size, m_color };
}
grid grid::with_color(const sf::Color &color)
{
  if (color == m_color)
  {
    return *this;
  }

  return { m_spacing, m_size, color };
}
grid::grid(
  const sf::Vector2u &spacing,
  const sf::Vector2u &size,
  const sf::Color    &color)
  : m_spacing(spacing)
  , m_size(size)
  , m_color(color)
  , m_vertices(get_vertices())
{
}

void grid::enable() const
{
  m_enable = true;
}

void grid::disable() const
{
  m_enable = false;
}
