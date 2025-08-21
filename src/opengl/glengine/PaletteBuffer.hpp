#ifndef E511EAAD_9F53_49A1_BE6D_5B13F07E3D42
#define E511EAAD_9F53_49A1_BE6D_5B13F07E3D42
#include "GLCheck.hpp"
#include "UniqueValue.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace glengine
{
struct PaletteBuffer
{
   public:
     PaletteBuffer()
       : m_buffer_id(Glid{ create(), destroy })
       , m_size(0)
     {
     }
     void     initialize(const std::vector<glm::vec4> &data);
     void     bind(GLuint binding_point);

     GlidCopy id() const;
     size_t   size() const;

   private:
     Glid          m_buffer_id;
     size_t        m_size;

     static GLuint create()
     {
          GLuint temp_id = {};
          GlCall{}(glGenBuffers, 1, &temp_id);
          if (!temp_id)
          {
               spdlog::error("Failed to generate PaletteBuffer");
          }
          return temp_id;
     }
     static void destroy(const GLuint id)
     {
          GlCall{}(glDeleteBuffers, 1, &id);
     }
};
}// namespace glengine
#endif /* E511EAAD_9F53_49A1_BE6D_5B13F07E3D42 */
