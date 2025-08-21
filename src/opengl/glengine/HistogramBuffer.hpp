#ifndef D138C7B4_FB42_4FF9_8933_B2BABCFE9761
#define D138C7B4_FB42_4FF9_8933_B2BABCFE9761
#include "GLCheck.hpp"
#include "UniqueValue.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace glengine
{
class HistogramBuffer
{
   public:
     HistogramBuffer(size_t count);
     void     bind(GLuint binding_point);
     void     read_back(std::vector<GLuint> &data);

     GlidCopy id() const;
     size_t   count() const;

   private:
     size_t        m_count;
     Glid          m_buffer_id;

     static GLuint create(size_t count);
     static void   destroy(const GLuint id);
};
}// namespace glengine
#endif /* D138C7B4_FB42_4FF9_8933_B2BABCFE9761 */
