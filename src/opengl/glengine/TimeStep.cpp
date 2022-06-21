//
// Created by pcvii on 12/13/2021.
//

#include "TimeStep.hpp"
namespace glengine
{
TimeStep::operator float() const
{
  TimePoint current = now();
  Duration  frame   = current - last;
  // optional check for min frame time here.
  last              = current;
  return frame.count();
}
TimeStep::TimePoint TimeStep::now()
{
  return LocalClockT::now();
}
}// namespace glengine