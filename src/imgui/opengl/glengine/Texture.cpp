#include "Texture.hpp"
#include <stb_image.h>
namespace glengine
{
Texture::Texture(std::filesystem::path path)
  : m_path(std::move(path))
{
  if (m_path.empty())
    return;
  stbi_set_flip_vertically_on_load(1);

  int                  x        = {};
  int                  y        = {};
  [[maybe_unused]] int channels = {};
  const auto           deleter  = [](stbi_uc *ptr) { stbi_image_free(ptr); };
  auto                 png      = std::unique_ptr<stbi_uc, decltype(deleter)>(
    stbi_load(m_path.string().c_str(), &x, &y, &channels, 4));
  m_width  = x;
  m_height = y;
  init_texture(png.get());
}
void Texture::Bind(int slot) const
{
  GLCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot));
  GLCall{}(glBindTexture, GL_TEXTURE_2D, m_renderer_id);
  // GLCall{}( glBindTextureUnit, slot, m_renderer_id );
}


GLID_copy Texture::ID() const noexcept
{
  return m_renderer_id;
}
}// namespace glengine