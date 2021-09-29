//
// Created by pcvii on 9/28/2021.
//

#include "grid.hpp"
std::vector<sf::Vertex> grid::get_vertices(const sf::Vector2u &spacing,
  const sf::Vector2u                                          &size)
{
  std::vector<sf::Vertex> ret     = {};
  const unsigned int      x_count = size.x / spacing.x+1;
  const unsigned int      y_count = size.y / spacing.y+1;
  const auto              count   = x_count + y_count;
  if (count <= 2U) {
    return ret;
  }
  ret.reserve(count);
  for (unsigned int x = 0U; x != x_count; ++x) {
    ret.emplace_back(sf::Vector2f{ static_cast<float>(x * spacing.x), 0.F });
    ret.emplace_back(sf::Vector2f{
      static_cast<float>(x * spacing.x), static_cast<float>(size.y) });
  }
  for (unsigned int y = 0U; y != y_count; ++y) {
    ret.emplace_back(sf::Vector2f{ 0.F, static_cast<float>(y * spacing.y) });
    ret.emplace_back(sf::Vector2f{
      static_cast<float>(size.x), static_cast<float>(y * spacing.y) });
  }
  return ret;
}
void grid::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
  states.transform *= getTransform();
  target.draw(m_vertices.data(), m_vertices.size(), sf::Lines, states);
}
grid::grid(const sf::Vector2u &spacing, const sf::Vector2u &size)
  : m_vertices(get_vertices(spacing, size))
{}
