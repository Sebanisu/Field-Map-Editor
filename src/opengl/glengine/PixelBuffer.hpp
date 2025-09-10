//
// Created by pcvii on 12/21/2021.
//

#ifndef FIELD_MAP_EDITOR_PIXELBUFFER_HPP
#define FIELD_MAP_EDITOR_PIXELBUFFER_HPP
#include "FrameBuffer.hpp"
#include "ScopeGuard.hpp"
#include "UniqueValue.hpp"
namespace glengine
{
class PixelBuffer
{
     // todo make array_size a template argument?
     constexpr static inline std::size_t array_size = { 2U };

     using ArrayT                                   = GlidArray<array_size>;
     using ValueT                                   = typename ArrayT::ValueT;
     using ParameterT                               = typename ArrayT::ParameterT;
     // wrapping this pointer to force it to call glUnmapBuffer. Should scope this.
     using UniqueGlubyte = std::unique_ptr<uint8_t, decltype([](const GLubyte *const) { glUnmapBuffer(GL_PIXEL_PACK_BUFFER); })>;

   public:
     PixelBuffer(const glengine::FrameBufferSpecification &fbs)
       : width(fbs.width)
       , height(fbs.height)
       , data_size(static_cast<std::ptrdiff_t>(fbs.height) * static_cast<std::ptrdiff_t>(fbs.width) * std::ptrdiff_t{ 4 })
       , pbos{ [](ParameterT ids) { GlCall{}(glDeleteBuffers, static_cast<std::int32_t>(std::ranges::size(ids)), std::ranges::data(ids)); },
               [&]() {
                    auto ids = ValueT{};
                    // create 2 pixel buffer objects, you need to delete them when
                    // program exits. glBufferData() with NULL pointer reserves only
                    // memory space.
                    GlCall{}(glGenBuffers, static_cast<std::int32_t>(std::ranges::size(ids)), std::ranges::data(ids));
                    for (auto &id : ids)
                    {
                         GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, id);
                         GlCall{}(glBufferData, GL_PIXEL_PACK_BUFFER, data_size, nullptr, GL_STREAM_READ);
                         // todo will need to alter DATA_SIZE and update buffers when
                         // window resizes
                    }
                    return ids;
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
       const glengine::FrameBuffer &fb,
       std::filesystem::path        path,
       bool *const                  full = nullptr) const
     {
          next();
          to_pbo(fb, std::move(path));
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
     bool operator()(std::invocable<
                     std::span<std::uint8_t>,
                     std::filesystem::path,
                     int,
                     int> auto &&call_back) const
     {
          next();
          from_pbo(std::forward<decltype(call_back)>(call_back));
          return std::ranges::any_of(set, std::identity{});
     }

   private:
     void from_pbo(
       std::invocable<
         std::span<std::uint8_t>,
         std::filesystem::path,
         int,
         int> auto &&call_back) const
     {
          // map the PBO to process its data by CPU
          const auto unbind_pbo_buffer = ScopeGuard([] { unbind(); });
          bind(next_index);
          set[next_index] = false;
          {
               UniqueGlubyte ptr{ static_cast<uint8_t *>(glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY)) };
               if (ptr)
               {
                    std::invoke(
                      call_back, std::span{ ptr.get(), static_cast<std::size_t>(data_size) }, std::move(paths[next_index]), width, height);
               }
          }
     }
     void to_pbo(
       const glengine::FrameBuffer &fb,
       std::filesystem::path        path) const
     {
          // set the target framebuffer to read
          fb.bind();
          const auto unbind_frame_buffer = ScopeGuard{ [] { glengine::FrameBuffer::unbind(); } };
          glReadBuffer(GL_COLOR_ATTACHMENT0);

          // read pixels from framebuffer to PBO
          // glReadPixels() should return immediately.
          {
               const auto unbind_pbo_buffer = ScopeGuard{ [] { unbind(); } };
               bind(index);
               set[index]   = true;
               paths[index] = std::move(path);
               GlCall{}(glReadPixels, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
          }
     }
     void next() const
     {
          // "index" is used to read pixels from framebuffer to a PBO
          // "nextIndex" is used to update pixels in the other PBO
          index      = (index + 1) % std::ranges::size(pbos);
          next_index = (index + 1) % std::ranges::size(pbos);
     }
     void bind(std::size_t i) const
     {
          GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, pbos[i]);
     }
     static void unbind()
     {
          // back to conventional pixel operation
          GlCall{}(glBindBuffer, GL_PIXEL_PACK_BUFFER, 0);
     }
     mutable std::size_t                                   index      = {};
     mutable std::size_t                                   next_index = { (index + 1) % array_size };
     std::int32_t                                          width      = {};
     std::int32_t                                          height     = {};
     std::ptrdiff_t                                        data_size  = {};// number of bytes in image.
     mutable std::array<bool, array_size>                  set        = {};
     mutable std::array<std::filesystem::path, array_size> paths      = {};
     ArrayT                                                pbos       = {};
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_PIXELBUFFER_HPP
