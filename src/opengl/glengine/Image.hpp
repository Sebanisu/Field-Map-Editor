#ifndef A6A086B2_DE54_4BBC_ABDB_8D3440CA5695
#define A6A086B2_DE54_4BBC_ABDB_8D3440CA5695
#include <filesystem>
#include <stb_image.h>
namespace glengine
{

struct STBImageDeleter
{
     void operator()(unsigned char *p) const noexcept
     {
          stbi_image_free(p);
     }
};
struct [[nodiscard]] Image
{
     int                                       width    = {};
     int                                       height   = {};
     [[maybe_unused]] int                      channels = {};
     std::filesystem::path                     path     = {};
     std::unique_ptr<stbi_uc, STBImageDeleter> png_data = {};
     Image()                                            = default;

     Image(Image &&)                                    = default;
     Image(
       std::filesystem::path in_path,
       bool                  in_flip = false);
};
}// namespace glengine

#endif /* A6A086B2_DE54_4BBC_ABDB_8D3440CA5695 */
