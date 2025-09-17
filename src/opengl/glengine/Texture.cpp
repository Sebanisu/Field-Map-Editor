#include "Texture.hpp"
namespace glengine
{
Texture::Texture(
  std::filesystem::path path,
  bool                  in_flip)
  : Texture(
      Image{ std::move(path),
             in_flip })
{
}
Texture::Texture(Image image)
  : m_path(std::move(image.path))
  , m_width(image.width)
  , m_height(image.height)
{
     init_texture(image.png_data.get());
}

void Texture::bind(int slot) const
{
     GlCall{}(glActiveTexture, static_cast<GLenum>(GL_TEXTURE0 + slot));
     GlCall{}(glBindTexture, GL_TEXTURE_2D, m_renderer_id);
     // GLCall{}( glBindTextureUnit, slot, m_renderer_id );
}

void Texture::generate_mipmaps() const
{
     // Ensure the texture is bound before generating mipmaps
     bind(0);// Bind to slot 0 (or any slot you prefer)
     GlCall{}(glGenerateMipmap, GL_TEXTURE_2D);
}


void Texture::bind_read_only(int slot) const
{
     GlCall{}(
       glBindImageTexture,
       slot,
       m_renderer_id,
       0,
       static_cast<GLboolean>(GL_FALSE),
       0,
       GL_READ_ONLY,
       s_sized_interal_format);
     // bind(slot);
}
void Texture::bind_write_only(int slot) const
{
     GlCall{}(
       glBindImageTexture,
       slot,
       m_renderer_id,
       0,
       static_cast<GLboolean>(GL_FALSE),
       0,
       GL_WRITE_ONLY,
       s_sized_interal_format);
}


GlidCopy Texture::id() const noexcept
{
     return m_renderer_id;
}

}// namespace glengine