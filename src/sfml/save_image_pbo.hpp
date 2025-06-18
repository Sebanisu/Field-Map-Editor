//
// Created by pcvii on 2/27/2023.
//

#ifndef FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
#define FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP

#include <future>
#include <filesystem>
#include <FrameBuffer.hpp>

[[nodiscard]] std::future<void> save_image_pbo(const std::filesystem::path &, const glengine::FrameBuffer &);
#endif// FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
