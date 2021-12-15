#include "Texture.hpp"
#include <stb_image.h>
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
  m_width_height = { x, y };
  init_texture(png.get());
}
void Texture::Bind(int slot) const
{
  GLCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot));
  GLCall{}(glBindTexture, GL_TEXTURE_2D, m_renderer_id);
  // GLCall{}( glBindTextureUnit, slot, m_renderer_id );
}
void Texture::UnBind()
{
  GLCall{}(glBindTexture, GL_TEXTURE_2D, 0U);
}
std::int32_t Texture::width() const
{
  return m_width_height.x();
}
std::int32_t Texture::height() const
{
  return m_width_height.y();
}
GLID_copy Texture::ID() const noexcept
{
  return m_renderer_id;
}
