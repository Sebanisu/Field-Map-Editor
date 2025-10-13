//
// Created by pcvii on 2/27/2023.
//
#include "save_image_pbo.hpp"
#include "formatters.hpp"
#include "PupuID.hpp"
#include <map>
#include <ranges>
#include <ScopeGuard.hpp>
#include <set>
#include <spdlog/spdlog.h>
#include <stb_image_write.h>
#include <version>
namespace fme
{
struct Vec4Less
{
     bool operator()(
       const glm::vec4 &a,
       const glm::vec4 &b) const noexcept
     {
          if (a.x != b.x)
               return a.x < b.x;
          if (a.y != b.y)
               return a.y < b.y;
          if (a.z != b.z)
               return a.z < b.z;
          return a.w < b.w;
     }
};
/**
 * @brief Asynchronously saves an OpenGL texture to an sf::Image using a Pixel
 * Buffer Object (PBO).
 *
 * This function uses a PBO to asynchronously retrieve texture pixel data from
 * GPU memory, allowing the CPU to continue processing without blocking while
 * the GPU completes the readback. The returned future will produce an sf::Image
 * once the pixel data is ready.
 *
 * @param texture The glengine::Texture to be saved into an sf::Image.
 * @return A deferred std::future that will contain the sf::Image once pixel
 * data is read back.
 */
std::future<void> save_image_pbo(
  std::filesystem::path in_path,
  glengine::FrameBuffer in_fbo,
  const GLenum          attachment,
  std::vector<std::tuple<
    glm::vec4,
    ff_8::PupuID>>      in_color_ids)
{
     // Backup currently bound framebuffer (restored by your helper)
     const auto backup_fbo = in_fbo.backup();
     in_fbo.bind();

     // Save GL state we’ll touch
     GLint prev_read_buffer = GL_COLOR_ATTACHMENT0;
     glengine::GlCall{}(glGetIntegerv, GL_READ_BUFFER, &prev_read_buffer);

     GLint prev_pack_alignment = 4;
     glengine::GlCall{}(glGetIntegerv, GL_PACK_ALIGNMENT, &prev_pack_alignment);

     GLint prev_pack_row_length = 0;
     glengine::GlCall{}(
       glGetIntegerv, GL_PACK_ROW_LENGTH, &prev_pack_row_length);

     // Set correct read target for RGBA8 normalized data
     glengine::GlCall{}(glReadBuffer, attachment);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ALIGNMENT, 1);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ROW_LENGTH, 0);

     const GLsizei    w        = in_fbo.width();
     const GLsizei    h        = in_fbo.height();
     const GLint      channels = 4;
     const GLsizeiptr buffer_size
       = GLsizeiptr{ w } * GLsizeiptr{ h } * GLsizeiptr{ channels };

     // Create and bind PBO
     GLuint pbo_id = 0;
     glGenBuffers(1, &pbo_id);
     glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo_id);
     glBufferData(GL_PIXEL_PACK_BUFFER, buffer_size, nullptr, GL_STREAM_READ);

     // Issue the readback (normalized RGBA8)
     glengine::GlCall{}(
       glReadPixels, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

     if (GLenum err = glGetError(); err != GL_NO_ERROR)
     {
          spdlog::error(
            "save_image_pbo: glReadPixels error: 0x{:X}",
            static_cast<unsigned>(err));
     }

     // Unbind PBO
     glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

     // Restore GL state
     glengine::GlCall{}(glReadBuffer, prev_read_buffer);
     glengine::GlCall{}(glPixelStorei, GL_PACK_ALIGNMENT, prev_pack_alignment);
     glengine::GlCall{}(
       glPixelStorei, GL_PACK_ROW_LENGTH, prev_pack_row_length);

     // Return deferred CPU-side save
     return std::async(
       std::launch::deferred,
       [pbo_id,
        buffer_size,
        w,
        h,
        channels,
        path      = std::move(in_path),
        fbo       = std::move(in_fbo),
        color_ids = std::move(in_color_ids)]()
       {
            // Ensure directory exists
            if (!path.parent_path().empty())
            {
                 std::error_code ec;
                 std::filesystem::create_directories(path.parent_path(), ec);
                 if (ec)
                 {
                      spdlog::error(
                        "Failed to create directories for '{}': {}",
                        path.string(),
                        ec.message());
                 }
            }

#ifdef __cpp_lib_smart_ptr_for_overwrite
            auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(
              static_cast<std::size_t>(buffer_size));
#else
            auto pixels = std::make_unique<std::uint8_t[]>(
              static_cast<std::size_t>(buffer_size));
#endif

            // Retrieve from PBO
            glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo_id);
            glengine::GlCall{}(
              glGetBufferSubData,
              GL_PIXEL_PACK_BUFFER,
              0,
              buffer_size,
              pixels.get());
            if (GLenum err = glGetError(); err != GL_NO_ERROR)
            {
                 spdlog::error(
                   "save_image_pbo: glGetBufferSubData error: 0x{:X}",
                   static_cast<unsigned>(err));
            }
            glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);

            // Delete PBO
            glengine::GlCall{}(glDeleteBuffers, 1, &pbo_id);

            // // Flip vertically
            // std::unique_ptr<std::uint8_t[]> flipped   =
            // std::make_unique<std::uint8_t[]>(static_cast<std::size_t>(buffer_size));
            const std::size_t row_bytes = static_cast<std::size_t>(w)
                                          * static_cast<std::size_t>(channels);
            // for (GLint y = 0; y < h; ++y)
            // {
            //      const std::size_t src = static_cast<std::size_t>(h - 1 - y)
            //      * row_bytes; const std::size_t dst =
            //      static_cast<std::size_t>(y) * row_bytes;
            //      std::memcpy(flipped.get() + dst, pixels.get() + src,
            //      row_bytes);
            // }

            // Save PNG
            {
                 int ok = stbi_write_png(
                   path.string().c_str(),
                   w,
                   h,
                   channels,
                   pixels.get(),
                   static_cast<int>(row_bytes));
                 if (!ok)
                 {
                      spdlog::error(
                        "stbi_write_png failed for '{}'", path.string());
                 }
                 else
                 {
                      spdlog::debug(
                        "Wrote RGBA8 attachment to '{}'", path.string());
                 }
            }
            if (std::ranges::empty(color_ids))
            {
                 return;
            }

            const auto span = std::span(
              reinterpret_cast<const fme::color *>(pixels.get()),
              static_cast<std::size_t>(w * h));
            // For each unique PupuID, create a vector of masks
            std::map<ff_8::PupuID, std::vector<fme::color>>  masks;
            std::set<std::pair<fme::color, fme::color>>      logged_colors;
            static std::map<glm::vec4, fme::color, Vec4Less> conv_cache;
            std::map<fme::color, ff_8::PupuID>
              best_id_cache;// memoizes best id for each pixel color

            for (const auto &[i, color_in] : span | std::views::enumerate)
            {
                 if (color_in == fme::colors::Transparent)
                      continue;

                 const auto best_id = [&]() -> ff_8::PupuID
                 {
                      auto cache_it = best_id_cache.find(color_in);
                      if (cache_it != best_id_cache.end())
                      {
                           return cache_it->second;// cached, skip min_element
                      }
                      else
                      {
                           auto it = std::ranges::min_element(
                             color_ids,
                             [&](const auto &a, const auto &b)
                             {
                                  auto get_conv = [](const glm::vec4 &c)
                                  {
                                       auto nested_it = conv_cache.find(c);
                                       if (nested_it != conv_cache.end())
                                            return nested_it->second;
                                       fme::color conv
                                         = static_cast<fme::color>(c);
                                       conv_cache.emplace(c, conv);
                                       return conv;
                                  };

                                  const auto conv_a = get_conv(std::get<0>(a));
                                  const auto conv_b = get_conv(std::get<0>(b));

                                  return color_in.difference(conv_a)
                                         < color_in.difference(conv_b);
                             });

                           if (it == color_ids.end())
                           {
                                return {};
                           }

                           best_id_cache[color_in]
                             = std::get<1>(*it);// store result for next time
                           return std::get<1>(*it);
                      }
                 }();
                 if (best_id.raw() == 0)
                 {
                      continue;
                 }
                 auto &mask = masks[best_id];
                 if (mask.empty())
                      mask.resize(span.size(), fme::colors::Black);

                 mask[static_cast<std::size_t>(i)] = fme::colors::White;

                 auto it                           = std::ranges::find_if(
                   color_ids,
                   [&](const auto &tup)
                   { return std::get<1>(tup) == best_id; });

                 fme::color conv_color_best;
                 if (it != color_ids.end())
                 {
                      conv_color_best = conv_cache[std::get<0>(*it)];
                      if (
                        color_in != conv_color_best
                        && logged_colors.insert({ color_in, conv_color_best })
                             .second)
                      {
                           spdlog::debug("{} != {}", color_in, conv_color_best);
                      }
                 }
            }

            for (auto &&[id, mask] : masks)
            {
                 const auto current_path = path.parent_path()
                                           / fmt::format(
                                             "{}_{}{}",
                                             path.stem().string(),
                                             id,
                                             path.extension().string());
                 int ok = stbi_write_png(
                   current_path.string().c_str(),
                   w,
                   h,
                   channels,
                   &mask.front(),
                   static_cast<int>(row_bytes));
                 if (!ok)
                 {
                      spdlog::error(
                        "stbi_write_png failed for '{}'", path.string());
                 }
                 else
                 {
                      spdlog::debug(
                        "Wrote RGBA attachment to '{}'", path.string());
                 }
            }
       });
}

std::future<void> save_image_texture_pbo(
  std::filesystem::path       in_path,
  const glengine::SubTexture &texture)
{
     static constexpr const GLint channels = 4;
     const GLsizeiptr buffer_size = static_cast<GLsizeiptr>(texture.width())
                                    * static_cast<GLsizeiptr>(texture.height())
                                    * channels;

     // Create PBO
     GLuint pbo_id = 0;
     glengine::GlCall{}(glGenBuffers, 1, &pbo_id);
     glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo_id);
     glengine::GlCall{}(
       glBufferData,
       GL_PIXEL_PACK_BUFFER,
       buffer_size,
       nullptr,
       GL_STREAM_READ);

     // Bind texture for readback
     texture.bind();

     // Read texture data into PBO (RGBA8 expected)
     glengine::GlCall{}(
       glGetTexImage, GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

     if (GLenum err = glGetError(); err != GL_NO_ERROR)
     {
          spdlog::error(
            "save_image_texture_pbo: glGetTexImage error: 0x{:X}",
            static_cast<unsigned>(err));
     }

     // Unbind
     texture.unbind();
     glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);

     // Async CPU-side save
     return std::async(
       std::launch::deferred,
       [pbo_id,
        buffer_size,
        w = texture.width(),
        h = texture.height(),
        channels,
        path = std::move(in_path)]()
       {
            // Ensure directory exists
            if (path.has_parent_path())
            {
                 std::error_code ec;
                 std::filesystem::create_directories(path.parent_path(), ec);
                 if (ec)
                 {
                      spdlog::error(
                        "Failed to create directories for '{}': {}",
                        path.string(),
                        ec.message());
                 }
            }

#ifdef __cpp_lib_smart_ptr_for_overwrite
            auto pixels = std::make_unique_for_overwrite<std::uint8_t[]>(
              static_cast<std::size_t>(buffer_size));
#else
            auto pixels = std::make_unique<std::uint8_t[]>(
              static_cast<std::size_t>(buffer_size));
#endif

            // Retrieve from PBO
            glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbo_id);
            glengine::GlCall{}(
              glGetBufferSubData,
              GL_PIXEL_PACK_BUFFER,
              0,
              buffer_size,
              pixels.get());
            glengine::GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);
            glengine::GlCall{}(glDeleteBuffers, 1, &pbo_id);

            const std::size_t row_bytes = static_cast<std::size_t>(w)
                                          * static_cast<std::size_t>(channels);

            // Save PNG
            int ok = stbi_write_png(
              path.string().c_str(),
              w,
              h,
              channels,
              pixels.get(),
              static_cast<int>(row_bytes));

            if (!ok)
                 spdlog::error("stbi_write_png failed for '{}'", path.string());
            else
                 spdlog::debug("Saved texture to '{}'", path.string());
       });
}
}// namespace fme