//
// Created by pcvii on 12/13/2021.
//

#ifndef MYPROJECT_TIMESTEP_HPP
#define MYPROJECT_TIMESTEP_HPP
class TimeStep
{
public:
  operator float() const;

private:
  using Clock             = std::chrono::steady_clock;
  using duration          = std::chrono::duration<float>;
  using time_point        = std::chrono::time_point<Clock, duration>;

  mutable time_point last = Clock::now();
};

#endif// MYPROJECT_TIMESTEP_HPP
