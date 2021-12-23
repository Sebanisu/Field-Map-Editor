//
// Created by pcvii on 12/21/2021.
//

#ifndef MYPROJECT_PIXELBUFFER_HPP
#define MYPROJECT_PIXELBUFFER_HPP
#include "FrameBuffer.hpp"
#include "scope_guard.hpp"
#include "unique_value.hpp"
class PixelBuffer
{
  constexpr static inline std::size_t ARRAY_SIZE = { 2U };
  // wrapping this pointer to force it to call glUnmapBuffer. Should scope this.
  using unique_glubyte =
    std::unique_ptr<const uint8_t, decltype([](const GLubyte *const) {
                      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
                    })>;

public:
  PixelBuffer(const FrameBufferSpecification &fbs)
    : width(fbs.width)
    , height(fbs.height)
    , DATA_SIZE(fbs.height * fbs.width * 4)
    , pbos{ [](GLID_array<ARRAY_SIZE>::ParameterT ids) {
             glDeleteBuffers(
               static_cast<std::int32_t>(std::ranges::size(ids)),
               std::ranges::data(ids));
           },
            [&](std::ranges::contiguous_range auto &ids) {
              // create 2 pixel buffer objects, you need to delete them when
              // program exits. glBufferData() with NULL pointer reserves only
              // memory space.
              glGenBuffers(
                static_cast<std::int32_t>(std::ranges::size(ids)),
                std::ranges::data(ids));
              for (auto &id : ids)
              {
                glBindBuffer(GL_PIXEL_PACK_BUFFER, id);
                glBufferData(
                  GL_PIXEL_PACK_BUFFER, DATA_SIZE, 0, GL_STREAM_READ);
              }
            } }
  {
  }
  /**
   * Push front a frame buffer
   * @param fb where the image data has been rendered
   * @param path to be forwarded to call back for saving later.
   * @param full pointer to bool if all values are present.
   * @return true if any values are present
   */
  bool operator()(
    const FrameBuffer    &fb,
    std::filesystem::path path,
    bool *const           full = nullptr) const
  {
    Next();
    ToPBO(fb, std::move(path));
    if (full != nullptr)
    {
      *full = std::ranges::all_of(set, std::identity{});
    }
    return std::ranges::any_of(set, std::identity{});
  }
  /**
   * pop back the last one and run call back
   * @param call_back accepts image data and does something
   * @return true if any values are still present
   */
  bool operator()(
    std::invocable<std::span<const std::uint8_t>, std::filesystem::path> auto
      &&call_back) const
  {
    Next();
    FromPBO(std::forward<decltype(call_back)>(call_back));
    return std::ranges::any_of(set, std::identity{});
  }

private:
  void FromPBO(
    std::invocable<std::span<const std::uint8_t>, std::filesystem::path> auto
      &&call_back) const
  {
    // map the PBO to process its data by CPU
    const scope_guard unbind_pbo_buffer = (&UnBind);
    Bind(next_index);
    set[next_index] = false;
    {
      unique_glubyte ptr{ static_cast<const uint8_t *>(
        glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY)) };
      if (ptr)
      {
        std::invoke(
          call_back,
          std::span{ ptr.get(), static_cast<std::size_t>(DATA_SIZE) },
          std::move(paths[next_index]));
      }
    }
  }
  void ToPBO(const FrameBuffer &fb, std::filesystem::path path) const
  {
    // set the target framebuffer to read
    fb.Bind();
    const scope_guard unbind_frame_buffer = (&FrameBuffer::UnBind);
    glReadBuffer(GL_COLOR_ATTACHMENT0);

    // read pixels from framebuffer to PBO
    // glReadPixels() should return immediately.
    {
      const scope_guard unbind_pbo_buffer = (&UnBind);
      Bind(index);
      set[index]   = true;
      paths[index] = std::move(path);
      glReadPixels(0, 0, width, height, GL_BGRA, GL_UNSIGNED_BYTE, 0);
    }
  }
  void Next() const
  {
    // "index" is used to read pixels from framebuffer to a PBO
    // "nextIndex" is used to update pixels in the other PBO
    index      = (index + 1) % std::ranges::size(pbos);
    next_index = (index + 1) % std::ranges::size(pbos);
  }
  void Bind(std::size_t i) const
  {
    glBindBuffer(GL_PIXEL_PACK_BUFFER, pbos[i]);
  }
  static void UnBind()
  {
    // back to conventional pixel operation
    glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
  }
  mutable std::size_t index      = {};
  mutable std::size_t next_index = { (index + 1) % ARRAY_SIZE };
  std::int32_t        width      = {};
  std::int32_t        height     = {};
  std::int32_t        DATA_SIZE  = {};// number of bytes in image.
  mutable std::array<bool, ARRAY_SIZE>                  set   = {};
  mutable std::array<std::filesystem::path, ARRAY_SIZE> paths = {};
  GLID_array<ARRAY_SIZE>                                pbos  = {};
};
#endif// MYPROJECT_PIXELBUFFER_HPP
