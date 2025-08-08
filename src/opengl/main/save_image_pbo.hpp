//
// Created by pcvii on 2/27/2023.
//

#ifndef FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
#define FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP

#include <filesystem>
#include <FrameBuffer.hpp>
#include <future>

[[nodiscard]] std::future<void> save_image_pbo(std::filesystem::path, glengine::FrameBuffer);
[[nodiscard]] std::future<void> save_rgba8ui_attachment_as_png(std::filesystem::path, glengine::FrameBuffer);
#endif// FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
