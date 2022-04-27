//
// Created by pcvii on 12/7/2021.
//

#ifndef MYPROJECT_EVENTDISPATCHER_HPP
#define MYPROJECT_EVENTDISPATCHER_HPP
#include "EventItem.hpp"
namespace glengine
{
namespace Event
{
  class Dispatcher
  {
    template<Event::is... Es>
    static bool check(const Event::Item &e, bool value)
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
      const Event::Item    &e,
      bool                  focused,
      bool                  hovered,
      std::invocable auto &&callable)
    {
      if (
        check<
          Event::MouseButtonPressed,
          Event::MouseButtonReleased,
          Event::MouseMoved,
          Event::MouseScroll>(e, hovered)
        || check<Event::KeyPressed, Event::KeyReleased>(e, focused))
      {
        callable();
      }
    }
    template<typename T, typename F>
      requires std::is_invocable_r_v<bool, F, const T &> bool
    Dispatch(F func) const
    {
      if (m_event.contains<T>())
      {
        const T *true_event   = m_event.Get<T>();
        true_event->m_handled = std::invoke(func, *true_event);
        return true;
      }
      return false;
    }

  private:
    Item m_event;
  };
}// namespace Event
}// namespace glengine
#endif// MYPROJECT_EVENTDISPATCHER_HPP
