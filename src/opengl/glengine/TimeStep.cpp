//
// Created by pcvii on 12/13/2021.
//

#include "TimeStep.hpp"
namespace glengine
{
TimeStep::operator float() const
{
  time_point current = now();
  duration   frame   = current - last;
  // optional check for min frame time here.
  last               = current;
  return frame.count();
}
TimeStep::time_point TimeStep::now()
{
  return Clock::now();
}
}// namespace glengine