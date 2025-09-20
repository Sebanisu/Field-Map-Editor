#ifndef BEE279D3_2242_43DC_AA64_5AD626C6EE55
#define BEE279D3_2242_43DC_AA64_5AD626C6EE55
#include "GLCheck.hpp"
#include "UniqueValue.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace glengine
{
class DistanceBuffer
{
   public:
     DistanceBuffer(size_t count);
     void     bind(GLuint binding_point);
     void     read_back(std::vector<float> &data);

     GlidCopy id() const;
     size_t   count() const;
     void     reset() const;

   private:
     size_t        m_count;
     Glid          m_buffer_id;

     static GLuint create(size_t count);
     static void   destroy(const GLuint id);
};
}// namespace glengine
#endif /* BEE279D3_2242_43DC_AA64_5AD626C6EE55 */
