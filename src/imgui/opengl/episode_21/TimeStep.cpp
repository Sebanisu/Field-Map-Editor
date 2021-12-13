//
// Created by pcvii on 12/13/2021.
//

#include "TimeStep.hpp"
TimeStep::operator float() const
{
  time_point current = Clock::now();
  auto frame = current - last;
  //optional check for min frame time here.
  last = current;
  return frame.count();
}
