//
// Created by pcvii on 12/6/2021.
//

#ifndef FIELD_MAP_EDITOR_EVENT_HPP
#define FIELD_MAP_EDITOR_EVENT_HPP
#include "EventConcepts.hpp"
#include <fmt/format.h>
#include <variant>
namespace glengine
{
namespace event
{
#define EVENT_CLASS_TYPE(in_type)                                        \
   public:                                                               \
     using ValueType                       = in_type;                    \
     constexpr static auto      StaticName = std::string_view(#in_type); \
     constexpr std::string_view name() const                             \
     {                                                                   \
          return StaticName;                                             \
     }

#define EVENT_CLASS_CATEGORY(in_category)                                       \
   public:                                                                      \
     constexpr static auto StaticCategory     = in_category;                    \
     constexpr static auto StaticCategoryName = std::string_view(#in_category); \
     constexpr auto        category() const                                     \
     {                                                                          \
          return StaticCategory;                                                \
     }                                                                          \
     constexpr auto category_name() const                                       \
     {                                                                          \
          return StaticCategoryName;                                            \
     }
     class Dispatcher;
#define EVENT_HANDLED               \
   private:                         \
     mutable bool m_handled{};      \
     friend Dispatcher;             \
                                    \
   public:                          \
     constexpr bool handled() const \
     {                              \
          return m_handled;         \
     }


     class WindowResize
     {
          EVENT_CLASS_TYPE(WindowResize)
          EVENT_CLASS_CATEGORY(Category::Application)
          EVENT_HANDLED
        public:
          constexpr WindowResize() = default;
          constexpr WindowResize(int width, int height)
            : m_width(std::move(width))
            , m_height(std::move(height))
          {
          }

          constexpr int width() const
          {
               return m_width;
          }
          constexpr int height() const
          {
               return m_height;
          }
          std::string data() const
          {
               return fmt::format("{:>5}, {:>5}", m_width, m_height);
          }

        private:
          int m_width{};
          int m_height{};
     };
     static_assert(is_WindowResize<WindowResize>);

     class FrameBufferResize
     {
          EVENT_CLASS_TYPE(FrameBufferResize)
          EVENT_CLASS_CATEGORY(Category::Application)
          EVENT_HANDLED
        public:
          constexpr FrameBufferResize() = default;
          constexpr FrameBufferResize(int width, int height)
            : m_width(std::move(width))
            , m_height(std::move(height))
          {
          }

          constexpr int width() const
          {
               return m_width;
          }
          constexpr int height() const
          {
               return m_height;
          }
          std::string data() const
          {
               return fmt::format("{:>5}, {:>5}", m_width, m_height);
          }

        private:
          int m_width{};
          int m_height{};
     };
     static_assert(is_WindowResize<FrameBufferResize>);

     class WindowClose
     {
          EVENT_CLASS_TYPE(WindowClose)
          EVENT_CLASS_CATEGORY(Category::Application)
          EVENT_HANDLED

          std::string data() const
          {
               return {};
          }
     };
     static_assert(is<WindowClose>);
     class WindowMoved
     {
          EVENT_CLASS_TYPE(WindowMoved)
          EVENT_CLASS_CATEGORY(Category::Application)
          EVENT_HANDLED
        public:
          constexpr WindowMoved() = default;
          constexpr WindowMoved(int x, int y)
            : m_offset{ std::move(x), std::move(y) }
          {
          }
          constexpr std::array<int, 2U> position() const
          {
               return m_offset;
          }

          constexpr int x() const
          {
               return m_offset[0];
          }
          constexpr int y() const
          {
               return m_offset[1];
          }
          std::string data() const
          {
               return fmt::format("{:>5}, {:>5}", m_offset[0], m_offset[1]);
          }

        private:
          std::array<int, 2U> m_offset = {};
     };
     static_assert(is_WindowMove<WindowMoved>);

     class KeyPressed
     {
          EVENT_CLASS_TYPE(KeyPressed)
          EVENT_CLASS_CATEGORY(Category::Input | Category::Keyboard)
          EVENT_HANDLED
          constexpr KeyPressed() = default;
          constexpr KeyPressed(glengine::Key code, glengine::Mods mods, bool repeat = false)
            : m_key(code)
            , m_mods(mods)
            , m_repeat(repeat)
          {
          }
          constexpr glengine::Key key() const
          {
               return m_key;
          }
          constexpr glengine::Mods mods() const
          {
               return m_mods;
          }
          [[maybe_unused]] constexpr bool repeat() const
          {
               return m_repeat;
          }
          std::string data() const
          {
               if (m_mods == glengine::Mods::None)
               {
                    return fmt::format("{:>3}, {}", +m_key, m_repeat);
               }
               return fmt::format("{:>3} + {:>2},  {}", +m_key, +m_mods, m_repeat);
          }

        private:
          glengine::Key  m_key{};
          glengine::Mods m_mods{};
          bool           m_repeat{};
     };
     static_assert(is_Key<KeyPressed>);

     class KeyReleased
     {
          EVENT_CLASS_TYPE(KeyReleased)
          EVENT_CLASS_CATEGORY(Category::Input | Category::Keyboard)
          EVENT_HANDLED
          constexpr KeyReleased() = default;
          constexpr KeyReleased(glengine::Key code, glengine::Mods mods)
            : m_key(code)
            , m_mods(mods)
          {
          }
          constexpr glengine::Key key() const
          {
               return m_key;
          }
          constexpr glengine::Mods mods() const
          {
               return m_mods;
          }
          std::string data() const
          {
               if (m_mods == glengine::Mods::None)
               {
                    return fmt::format("{:>3}", +m_key);
               }
               return fmt::format("{:>3} + {:>2}", +m_key, +m_mods);
          }

        private:
          glengine::Key  m_key{};
          glengine::Mods m_mods{};
     };
     static_assert(is_Key<KeyReleased>);

     class MouseButtonPressed
     {
          EVENT_CLASS_TYPE(MouseButtonPressed)
          EVENT_CLASS_CATEGORY(Category::Input | Category::Mouse | Category::MouseButton)
          EVENT_HANDLED
          constexpr MouseButtonPressed() = default;
          constexpr MouseButtonPressed(glengine::Mouse code, glengine::Mods mods)
            : m_button(code)
            , m_mods(mods)
          {
          }
          constexpr glengine::Mouse button() const
          {
               return m_button;
          }
          constexpr glengine::Mods mods() const
          {
               return m_mods;
          }
          std::string data() const
          {
               if (m_mods == glengine::Mods::None)
               {
                    return fmt::format("{:>3}", +m_button);
               }
               return fmt::format("{:>3} + {:>2}", +m_button, +m_mods);
          }

        private:
          glengine::Mouse m_button{};
          glengine::Mods  m_mods{};
     };
     static_assert(is_MouseButton<MouseButtonPressed>);

     class MouseButtonReleased
     {
          EVENT_CLASS_TYPE(MouseButtonReleased)
          EVENT_CLASS_CATEGORY(Category::Input | Category::Mouse | Category::MouseButton)
          EVENT_HANDLED
          constexpr MouseButtonReleased() = default;
          constexpr MouseButtonReleased(glengine::Mouse code, glengine::Mods mods)
            : m_button(code)
            , m_mods(mods)
          {
          }
          constexpr glengine::Mouse button() const
          {
               return m_button;
          }
          constexpr glengine::Mods mods() const
          {
               return m_mods;
          }
          std::string data() const
          {
               if (m_mods == glengine::Mods::None)
               {
                    return fmt::format("{:>3}", +m_button);
               }
               return fmt::format("{:>3} + {:>2}", +m_button, +m_mods);
          }

        private:
          glengine::Mouse m_button{};
          glengine::Mods  m_mods{};
     };
     static_assert(is_MouseButton<MouseButtonReleased>);

     class MouseScroll
     {
          EVENT_CLASS_TYPE(MouseScroll)
          EVENT_CLASS_CATEGORY(Category::Input | Category::Mouse)
          EVENT_HANDLED
        public:
          constexpr MouseScroll() = default;
          constexpr MouseScroll(float x_offset, float y_offset)
            : m_offset{ std::move(x_offset), std::move(y_offset) }
          {
          }
          constexpr std::array<float, 2U> offsets() const
          {
               return m_offset;
          }

          constexpr float x_offset() const
          {
               return m_offset[0];
          }
          constexpr float y_offset() const
          {
               return m_offset[1];
          }

          std::string data() const
          {
               return fmt::format("{:>5.2f}, {:>5.2f}", m_offset[0], m_offset[1]);
          }

        private:
          std::array<float, 2U> m_offset = {};
     };
     static_assert(is_MouseScroll<MouseScroll>);

     class MouseMoved
     {
          EVENT_CLASS_TYPE(MouseMoved)
          EVENT_CLASS_CATEGORY(Category::Input | Category::Mouse)
          EVENT_HANDLED
        public:
          constexpr MouseMoved() = default;
          constexpr MouseMoved(float x, float y)
            : m_position{ std::move(x), std::move(y) }
          {
          }
          constexpr std::array<float, 2U> position() const
          {
               return m_position;
          }

          constexpr float x() const
          {
               return m_position[0];
          }
          constexpr float y() const
          {
               return m_position[1];
          }
          std::string data() const
          {
               return fmt::format("{:>5.2f}, {:>5.2f}", m_position[0], m_position[1]);
          }

        private:
          std::array<float, 2U> m_position = {};
     };
     class RefreshImage
     {
          EVENT_CLASS_TYPE(RefreshImage)
          EVENT_CLASS_CATEGORY(Category::Application)
          EVENT_HANDLED
        public:
          constexpr RefreshImage()
            : RefreshImage(true)
          {
          }
          constexpr RefreshImage(bool change)
            : m_changed{ std::move(change) }
          {
          }
          constexpr bool changed() const noexcept
          {
               return m_changed;
          }
          constexpr operator bool() const noexcept
          {
               return m_changed;
          }
          std::string data() const noexcept
          {
               return fmt::format("{}", m_changed);
          }

        private:
          bool m_changed = {};
     };
     class Reload
     {
          EVENT_CLASS_TYPE(Reload)
          EVENT_CLASS_CATEGORY(Category::Application)
          EVENT_HANDLED
        public:
          constexpr Reload()
            : Reload(true)
          {
          }
          constexpr Reload(bool change)
            : m_changed{ std::move(change) }
          {
          }
          constexpr bool changed() const noexcept
          {
               return m_changed;
          }
          constexpr operator bool() const noexcept
          {
               return m_changed;
          }
          std::string data() const noexcept
          {
               return fmt::format("{}", m_changed);
          }

        private:
          bool m_changed = {};
     };
     static_assert(is_MouseMove<MouseMoved>);

     using Types = std::variant<
       std::monostate,
       WindowResize,
       FrameBufferResize,
       WindowClose,
       WindowMoved,
       KeyPressed,
       KeyReleased,
       MouseButtonPressed,
       MouseButtonReleased,
       MouseScroll,
       MouseMoved,
       RefreshImage,
       Reload>;

#undef EVENT_HANDLED
#undef EVENT_CLASS_CATEGORY
#undef EVENT_CLASS_TYPE
}// namespace event
}// namespace glengine
#endif// FIELD_MAP_EDITOR_EVENT_HPP
