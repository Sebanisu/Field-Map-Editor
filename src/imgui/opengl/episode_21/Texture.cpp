// Do not delete this file. STB IMAGE requires one cpp file compiles it with
// macro ENABLED
#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"
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
  GLCall{ glGenTextures, 1, &m_renderer_id };
  GLCall{ glBindTexture, GL_TEXTURE_2D, m_renderer_id };
  GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
  GLCall{ &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };
  GLCall{
    &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE
  };
  GLCall{
    &glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE
  };
  GLCall{ &glTexImage2D,    GL_TEXTURE_2D, 0, GL_RGBA8,
          width(),          height(),      0, GL_RGBA,
          GL_UNSIGNED_BYTE, png.get() };
}
Texture::~Texture()
{
  GLCall{ glDeleteTextures, 1, &m_renderer_id };
  UnBind();
}
void
  Texture::Bind(int slot) const
{
  // GLCall{ glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot) };
  // GLCall{ glBindTexture, GL_TEXTURE_2D, m_renderer_id };
  GLCall{ glBindTextureUnit, slot, m_renderer_id };
}
void
  Texture::UnBind()
{
  GLCall{ glBindTexture, GL_TEXTURE_2D, 0U };
}
std::int32_t
  Texture::width() const
{
  return m_width_height.x();
}
std::int32_t
  Texture::height() const
{
  return m_width_height.y();
}
Texture::Texture(Texture &&other) noexcept
  : Texture()
{
  swap(*this, other);
}
Texture &
  Texture::operator=(Texture &&other) noexcept
{
  swap(*this, other);
  return *this;
}
void
  swap(Texture &first, Texture &second) noexcept// nothrow
{
  // enable ADL (not necessary in our case, but good practice)
  using std::swap;

  // by swapping the members of two objects,
  // the two objects are effectively swapped
  swap(first.m_renderer_id, second.m_renderer_id);
  swap(first.m_path, second.m_path);
  swap(first.m_width_height, second.m_width_height);
}
