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
namespace fme
{
/*
 *  @brief Asynchronously saves an OpenGL texture to a PNG file using a Pixel
 *  Buffer Object (PBO).
 *  This function uses a PBO to asynchronously retrieve texture pixel data from
 *  GPU memory, allowing the CPU to continue processing without blocking while
 *  the data is being transferred. The returned future will complete once the
 *  pixel data is ready.
 *  @param in_path The filesystem path where the PNG file will be saved.
 *  @param in_fbo The glengine::FrameBuffer to be saved into a PNG file.
 *  @param attachment The framebuffer attachment to read from (e.g.,
 * GL_COLOR_ATTACHMENT0).
 *  @param in_pupu_ids Optional vector of PupuIDs associated with the pixels.
 *  @return A deferred std::future that will complete once pixel data is read
 *  back.
 */

[[nodiscard]] std::future<void> save_image_pbo(
  std::filesystem::path in_path,
  glengine::FrameBuffer in_fbo,
  const GLenum          attachment = GL_COLOR_ATTACHMENT0,
  std::vector<std::tuple<
    glm::vec4,
    ff_8::PupuID>>      in_pupu_ids
  = {});

/*
 *  @brief Asynchronously saves an OpenGL texture to an sf::Image using a Pixel
 *  Buffer Object (PBO).
 *  This function uses a PBO to asynchronously retrieve texture pixel data from
 *  GPU memory, allowing the CPU to continue processing without blocking while
 *  the data is being transferred. The returned future will complete once the
 *  pixel data is ready.
 *  @param in_path The filesystem path where the PNG file will be saved.
 *  @param texture The glengine::Texture to be saved into a PNG file.
 *  @return A deferred std::future that will complete once pixel data is read
 *  back.
 */
[[nodiscard]] std::future<void> save_image_texture_pbo(
  std::filesystem::path       in_path,
  const glengine::SubTexture &texture);
}// namespace fme
#endif// FIELD_MAP_EDITOR_SAVE_IMAGE_PBO_HPP
