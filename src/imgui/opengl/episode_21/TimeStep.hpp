//
// Created by pcvii on 12/13/2021.
//

#ifndef MYPROJECT_TIMESTEP_HPP
#define MYPROJECT_TIMESTEP_HPP
class TimeStep
{
private:
  using Clock      = std::chrono::steady_clock;
public:
  using duration   = std::chrono::duration<float>;
  using time_point = std::chrono::time_point<Clock, duration>;

                    operator float() const;
  static time_point now();

private:
  mutable time_point last = now();
};

#endif// MYPROJECT_TIMESTEP_HPP
