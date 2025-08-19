//
// Created by pcvii on 2/27/2023.
//

#ifndef FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
#define FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP

#include "PupuID.hpp"
#include <filesystem>
#include <FrameBuffer.hpp>
#include <future>
#include <GL/glew.h>
#include <glm/glm.hpp>

[[nodiscard]] std::future<void> save_image_pbo(
  std::filesystem::path,
  glengine::FrameBuffer,
  const GLenum attachment                      = GL_COLOR_ATTACHMENT0,
  std::vector<std::tuple<glm::vec4, ff_8::PupuID>> = {});
// [[nodiscard]] std::future<void>
//   save_rgba8ui_attachment_as_png(std::filesystem::path, glengine::FrameBuffer, const GLenum attachment = GL_COLOR_ATTACHMENT1);
#endif// FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
