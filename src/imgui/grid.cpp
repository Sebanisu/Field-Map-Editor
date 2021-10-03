//
// Created by pcvii on 9/28/2021.
//

#include "grid.hpp"
#include <iostream>
std::vector<sf::Vertex> grid::get_vertices() const
{
  std::vector<sf::Vertex> ret     = {};
  const unsigned int      x_count = m_size.x / m_spacing.x + 1;
  const unsigned int      y_count = m_size.y / m_spacing.y + 1;
  const auto              count   = x_count + y_count;
  if (count <= 2U) {
    return ret;
  }
  ret.reserve(count);
  for (unsigned int x = 0U; x != x_count; ++x) {
    ret.emplace_back(
      sf::Vector2f{ static_cast<float>(x * m_spacing.x), 0.F }, m_color);
    ret.emplace_back(sf::Vector2f{ static_cast<float>(x * m_spacing.x),
                       static_cast<float>(m_size.y) },
      m_color);
  }
  for (unsigned int y = 0U; y != y_count; ++y) {
    ret.emplace_back(
      sf::Vector2f{ 0.F, static_cast<float>(y * m_spacing.y) }, m_color);
    ret.emplace_back(sf::Vector2f{ static_cast<float>(m_size.x),
                       static_cast<float>(y * m_spacing.y) },
      m_color);
  }
  return ret;
}
void grid::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  if (!m_enable) {
    return;
  }
  std::cout << m_vertices.size() << '\n';
  states.transform *= getTransform();
  states.blendMode = sf::BlendAlpha;
  target.draw(m_vertices.data(), m_vertices.size(), sf::Lines, states);
}
grid grid::with_spacing_and_size(const sf::Vector2u &spacing,
  const sf::Vector2u                                &size) const
{
  if (spacing == m_spacing && size == m_size) {
    return *this;
  }
  return { spacing, size, m_color };
}
grid grid::with_color(const sf::Color &color)
{
  if (color == m_color) {
    return *this;
  }

  return { m_spacing, m_size, color };
}
grid::grid(const sf::Vector2u &spacing,
  const sf::Vector2u          &size,
  const sf::Color             &color)
  : m_spacing(spacing), m_size(size), m_color(color), m_vertices(get_vertices())
{}

void grid::enable() const
{
  m_enable = true;
  std::cout << "enabled\n";
}

void grid::disable() const
{
  m_enable = false;
  std::cout << "disable\n";
}
