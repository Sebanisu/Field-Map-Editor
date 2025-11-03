//
// Created by pcvii on 12/7/2021.
//

#ifndef FIELD_MAP_EDITOR_EVENTDISPATCHER_HPP
#define FIELD_MAP_EDITOR_EVENTDISPATCHER_HPP
#include "EventItem.hpp"
#include <functional>
namespace glengine
{
namespace event
{
     class Dispatcher
     {
          template<event::is... Es>
          static bool check(
            const event::Item &e,
            bool               value)
          {
               if (!value)
                    return false;
               return (e.contains<Es>() || ...);
          }

        public:
          Dispatcher(Item event)
            : m_event(std::move(event))
          {
          }
          static void Filter(
            const event::Item    &e,
            bool                  focused,
            bool                  hovered,
            std::invocable auto &&callable)
          {
               if (
                 check<
                   event::MouseButtonPressed,
                   event::MouseButtonReleased,
                   event::MouseMoved,
                   event::MouseScroll>(e, hovered)
                 || check<event::KeyPressed, event::KeyReleased>(e, focused))
               {
                    callable();
               }
          }
          template<
            typename T,
            typename F>
               requires std::is_invocable_r_v<
                 bool,
                 F,
                 const T &>
          bool Dispatch(F func) const
          {
               if (m_event.contains<T>())
               {
                    const T *true_event   = m_event.get<T>();
                    true_event->m_handled = std::invoke(func, *true_event);
                    return true;
               }
               return false;
          }

        private:
          Item m_event;
     };
}// namespace event
}// namespace glengine
#endif// FIELD_MAP_EDITOR_EVENTDISPATCHER_HPP
