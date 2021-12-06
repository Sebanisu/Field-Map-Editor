//
// Created by pcvii on 12/6/2021.
//

#ifndef MYPROJECT_INDEXTYPE_HPP
#define MYPROJECT_INDEXTYPE_HPP
#include <GL/glew.h>
#include <cassert>
#include <cstdint>
enum class IndexType : GLenum
{
  none           = 0,
  UNSIGNED_BYTE  = GL_UNSIGNED_BYTE,
  UNSIGNED_SHORT = GL_UNSIGNED_SHORT,
  UNSIGNED_INT   = GL_UNSIGNED_INT,
};

inline constexpr GLenum
  operator+(IndexType it)
{
  return static_cast<GLenum>(it);
}
inline constexpr IndexType
  operator+(IndexType l, IndexType r)
{
  auto value = static_cast<IndexType>((+l) + (+r));
  assert(
    value == IndexType::none || value == IndexType::UNSIGNED_BYTE
    || value == IndexType::UNSIGNED_SHORT || value == IndexType::UNSIGNED_INT);
  return value;
}
template<typename T>
inline constexpr IndexType
  GetIndexType() = delete;
template<>
inline constexpr IndexType
  GetIndexType<GLbyte>()
{
  return IndexType::UNSIGNED_BYTE;
}
template<>
inline constexpr IndexType
  GetIndexType<GLushort>()
{
  return IndexType::UNSIGNED_SHORT;
}
template<>
inline constexpr IndexType
  GetIndexType<GLuint>()
{
  return IndexType::UNSIGNED_INT;
}
template<typename T>
concept has_Type_for_IndexType = requires(const T &t)
{
  t.Type() + IndexType::none;
};
#endif// MYPROJECT_INDEXTYPE_HPP
