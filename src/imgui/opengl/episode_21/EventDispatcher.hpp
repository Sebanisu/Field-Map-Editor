//
// Created by pcvii on 12/7/2021.
//

#ifndef MYPROJECT_EVENTDISPATCHER_HPP
#define MYPROJECT_EVENTDISPATCHER_HPP
#include "EventItem.hpp"
#include <functional>
namespace Event
{
class Dispatcher
{
public:
  Dispatcher(Item event)
    : m_event(std::move(event))
  {
  }

  template<typename T, typename F>
  requires std::is_invocable_r_v<bool, F, const T &>
  bool Dispatch(F func)
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
#endif// MYPROJECT_EVENTDISPATCHER_HPP
