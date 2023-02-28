//
// Created by pcvii on 2/27/2023.
//

#ifndef FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
#define FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP

#include <SFML/Graphics.hpp>

[[nodiscard]] cppcoro::task<sf::Image> save_image_pbo(const sf::Texture &texture);
#endif// FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
