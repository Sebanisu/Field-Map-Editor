//
// Created by pcvii on 9/28/2021.
//

#ifndef FIELD_MAP_EDITOR_GRID_HPP
#define FIELD_MAP_EDITOR_GRID_HPP
// #include <SFML/Graphics/Drawable.hpp>
// #include <SFML/Graphics/RenderTarget.hpp>
// #include <SFML/Graphics/Transformable.hpp>
// #include <SFML/Graphics/Vertex.hpp>
#include "colors.hpp"
#include <glm/glm.hpp>
#include <vector>
struct grid
{
   private:
     glm::uvec2              m_spacing  = {};
     glm::uvec2              m_size     = {};
     fme::color              m_color    = fme::color::white;
     std::vector<sf::Vertex> m_vertices = {};
     mutable bool            m_enable   = { true };

     std::vector<sf::Vertex> get_vertices() const;

   public:
     grid() = default;
     grid(const sf::Vector2u &spacing, const sf::Vector2u &size, const sf::Color &color = sf::Color::White);

     void        draw(sf::RenderTarget &target, sf::RenderStates states) const final;
     grid        with_spacing_and_size(const sf::Vector2u &spacing, const sf::Vector2u &size) const;
     grid        with_color(const sf::Color &color);
     void        enable() const;
     void        disable() const;
     std::size_t count() const
     {
          return std::size(m_vertices);
     }
};
#endif// FIELD_MAP_EDITOR_GRID_HPP
