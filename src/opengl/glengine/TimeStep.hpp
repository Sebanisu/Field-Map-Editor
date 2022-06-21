//
// Created by pcvii on 12/13/2021.
//

#ifndef FIELD_MAP_EDITOR_TIMESTEP_HPP
#define FIELD_MAP_EDITOR_TIMESTEP_HPP
namespace glengine
{
class TimeStep
{
private:
  using LocalClockT = std::chrono::steady_clock;

public:
  using Duration  = std::chrono::duration<float>;
  using TimePoint = std::chrono::time_point<LocalClockT, Duration>;

                   operator float() const;
  static TimePoint now();

private:
  mutable TimePoint last = now();
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_TIMESTEP_HPP
