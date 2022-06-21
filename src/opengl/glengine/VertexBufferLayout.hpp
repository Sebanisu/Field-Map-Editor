//
// Created by pcvii on 11/22/2021.
//

#ifndef FIELD_MAP_EDITOR_VERTEXBUFFERLAYOUT_HPP
#define FIELD_MAP_EDITOR_VERTEXBUFFERLAYOUT_HPP
namespace glengine
{
template<typename T>
concept is_VertexBufferElementType =
  requires(T t) {
    typename std::remove_cvref_t<T>::ValueType;
    {
      t.count()
      } -> decay_same_as<std::uint32_t>;
  };

struct VertexBufferElement
{
  std::uint32_t                m_type       = {};
  std::uint32_t                m_count      = {};
  std::uint8_t                 m_normalized = {};

  constexpr static std::size_t size_of_type(const std::uint32_t in_type)
  {
    switch (in_type)
    {
        // glVertexAttribPointer and glVertexAttribIPointer
      case GL_BYTE:
      case GL_UNSIGNED_BYTE: {
        static_assert(sizeof(GLbyte) == sizeof(GLubyte));
        return sizeof(GLbyte);
      }
      case GL_SHORT:
      case GL_UNSIGNED_SHORT: {
        static_assert(sizeof(GLshort) == sizeof(GLushort));
        return sizeof(GLshort);
      }
      case GL_INT:
      case GL_UNSIGNED_INT: {
        static_assert(sizeof(GLint) == sizeof(GLuint));
        return sizeof(GLint);
      }
        // glVertexAttribPointer
      case GL_FLOAT: {
        return sizeof(GLfloat);
      }
        // glVertexAttribLPointer or glVertexAttribPointer
      case GL_DOUBLE: {
        return sizeof(GLdouble);
      }
    }
    assert(false);
    return 0;
  }
  constexpr std::size_t size() const
  {
    return m_count * size_of_type(m_type);
  }
  constexpr std::size_t operator+(const VertexBufferElement &other)
  {
    return size() + other.size();
  }
  constexpr operator std::size_t() const
  {
    return size();
  }
};

template<typename T>
struct VertexBufferElementType
{
  constexpr VertexBufferElementType(std::uint32_t in_count)
    : m_count(in_count)
  {
  }
  using ValueType = T;
  constexpr operator VertexBufferElement() const
  {
    if constexpr (std::is_same_v<ValueType, std::uint8_t>)
    {
      return { m_type, m_count, std::uint8_t{ GL_TRUE } };
    }
    else
    {
      return { m_type, m_count, std::uint8_t{ GL_FALSE } };
    }
  };

  constexpr std::uint32_t count() const noexcept
  {
    return m_count;
  }

private:
  std::uint32_t m_count = {};
  std::uint32_t m_type  = []() {
    if constexpr (std::is_same_v<ValueType, GLdouble>)
    {
      return GL_DOUBLE;
    }
    if constexpr (std::is_same_v<ValueType, GLfloat>)
    {
      return GL_FLOAT;
    }
    else if constexpr (std::is_same_v<ValueType, GLint>)
    {
      return GL_INT;
    }
    else if constexpr (std::is_same_v<ValueType, GLuint>)
    {
      return GL_UNSIGNED_INT;
    }
    else if constexpr (std::is_same_v<ValueType, GLshort>)
    {
      return GL_SHORT;
    }
    else if constexpr (std::is_same_v<ValueType, GLushort>)
    {
      return GL_UNSIGNED_SHORT;
    }
    else if constexpr (std::is_same_v<ValueType, GLubyte>)
    {
      return GL_UNSIGNED_BYTE;
    }
    else if constexpr (std::is_same_v<ValueType, GLbyte>)
    {
      return GL_BYTE;
    }
  }();
};

template<std::size_t ElementCount>
class VertexBufferLayout
{
public:
  template<is_VertexBufferElementType... Ts>
  constexpr VertexBufferLayout(Ts &&...ts)
    : m_elements{ static_cast<VertexBufferElement>(std::forward<Ts>(ts))... }
    , m_stride(std::reduce(
        std::begin(m_elements),
        std::end(m_elements),
        std::size_t{ 0U },
        [](auto l, auto r) { return l + r; }))
  {
  }
  constexpr auto begin() const noexcept
  {
    using std::ranges::begin;
    return begin(m_elements);
  }
  constexpr auto cbegin() const noexcept
  {
    using std::ranges::cbegin;
    return cbegin(m_elements);
  }
  constexpr auto end() const noexcept
  {
    using std::ranges::end;
    return end(m_elements);
  }
  constexpr auto cend() const noexcept
  {
    using std::ranges::cend;
    return cend(m_elements);
  }
  constexpr auto size() const noexcept
  {
    using std::ranges::size;
    return size(m_elements);
  }
  constexpr std::int32_t stride() const noexcept
  {
    return static_cast<std::int32_t>(m_stride);
  }

private:
  std::array<VertexBufferElement, ElementCount> m_elements = {};
  std::size_t                                   m_stride   = {};
};
template<is_VertexBufferElementType... Ts>
VertexBufferLayout(Ts &&...) -> VertexBufferLayout<sizeof...(Ts)>;
static_assert(
  std::movable<VertexBufferLayout<1>> && std::copyable<VertexBufferLayout<1>>);
}// namespace glengine
#endif// FIELD_MAP_EDITOR_VERTEXBUFFERLAYOUT_HPP
