//
// Created by pcvii on 12/21/2021.
//

#ifndef MYPROJECT_PIXELBUFFER_HPP
#define MYPROJECT_PIXELBUFFER_HPP
#include "../../../../cmake-build-debug-wsl/src/imgui/opengl/episode_21/CMakeFiles/opengl_series_episode_21.dir/Debug/cmake_pch.hxx"
#include "FrameBuffer.hpp"
#include "scope_guard.hpp"
#include "unique_value.hpp"
class PixelBuffer
{
  // wrapping this pointer to force it to call glUnmapBuffer. Should scope this.
  using unique_glubyte =
    std::unique_ptr<const uint8_t, decltype([](const GLubyte *const) {
                      glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
                    })>;


  PixelBuffer(const FrameBufferSpecification &fbs)
    : width(fbs.width)
    , height(fbs.height)
    , DATA_SIZE(fbs.height * fbs.width * 4)
    , pbos{ [](GLID_array<2>::ParameterT ids) {
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
  bool operator()(
    const FrameBuffer                    &fb,
    std::invocable<std::uint8_t *> auto &&call_back) const
  {
    Next();
    ToPBO(fb);
    FromPBO(std::forward<decltype(call_back)>(call_back));
    return std::ranges::any_of(set, std::identity{});
  }
  bool operator()(std::invocable<std::uint8_t *> auto &&call_back) const
  {
    Next();
    FromPBO(std::forward<decltype(call_back)>(call_back));
    return std::ranges::any_of(set, std::identity{});
  }
  void FromPBO(std::invocable<std::uint8_t *> auto &&call_back) const
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
        std::invoke(call_back, ptr.get());
      }
    }
  }
  void ToPBO(const FrameBuffer &fb) const
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
      set[index] = true;
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
  mutable std::size_t         index      = {};
  mutable std::size_t         next_index = { index + 1 };
  std::int32_t                width      = {};
  std::int32_t                height     = {};
  std::int32_t                DATA_SIZE  = {};// number of bytes in image.
  mutable std::array<bool, 2> set        = {};
  GLID_array<2>               pbos       = {};
};
#endif// MYPROJECT_PIXELBUFFER_HPP
