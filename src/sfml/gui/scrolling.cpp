//
// Created by pcvii on 12/21/2022.
//

#include "scrolling.hpp"
#include <cmath>

void scrolling::reset() noexcept
{
  left = right = up = down = false;
}
bool scrolling::scroll(std::array<float, 2U> &in_xy, const sf::Time &time)
{
  bool changed = false;
  if (!(left || right || up || down))
  {
    return changed;
  }
  const auto time_ms = static_cast<float>(time.asMicroseconds()) / 1000.F;
  if (left && right)
  {
  }
  else
  {
    float total_time = (in_xy[0] + 1.F) * total_scroll_time[0];
    //        spdlog::info("{:.2f} = ({:.2f} + 1.00) * {:.2f}",
    //          total_time,
    //          in_xy[0],
    //          total_scroll_time[0]);
    if (left)
    {
      in_xy[0] = std::lerp(
        -1.F,
        0.F,
        std::clamp((total_time + time_ms) / total_scroll_time[0], 0.F, 1.F));
      changed = true;
    }
    else if (right)
    {
      in_xy[0] = std::lerp(
        -1.F,
        0.F,
        std::clamp((total_time - time_ms) / total_scroll_time[0], 0.F, 1.F));
      changed = true;
    }
  }
  if (up && down)
  {
  }
  else
  {
    float total_time = (in_xy[1] + 1.F) * total_scroll_time[1];
    if (up)
    {
      in_xy[1] = std::lerp(
        -1.F,
        0.F,
        std::clamp((total_time + time_ms) / total_scroll_time[1], 0.F, 1.F));
      changed = true;
    }
    else if (down)
    {
      in_xy[1] = std::lerp(
        -1.F,
        0.F,
        std::clamp((total_time - time_ms) / total_scroll_time[1], 0.F, 1.F));
      changed = true;
    }
  }
  return changed;
}