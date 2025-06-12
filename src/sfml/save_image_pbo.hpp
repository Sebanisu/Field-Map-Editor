//
// Created by pcvii on 2/27/2023.
//

#ifndef FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
#define FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP

#include <future>
#include <SFML/Graphics.hpp>
#include <Texture.hpp>

[[nodiscard]] std::future<sf::Image> save_image_pbo(const glengine::Texture &texture);
#endif// FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
