//
// Created by pcvii on 12/21/2022.
//

#include "batch_embed.hpp"
void batch_embed::enable(
  std::filesystem::path                                       in_outgoing,
  std::chrono::time_point<std::chrono::high_resolution_clock> start)

{
  m_enabled        = { true };
  m_pos            = {};
  m_outgoing       = { std::move(in_outgoing) };
  m_asked          = { false };
  m_start          = { start };
}
void batch_embed::disable()
{
  m_enabled = { false };
}
std::chrono::time_point<std::chrono::high_resolution_clock>
  batch_embed::start_time() const noexcept
{
  return m_start;
}
bool batch_embed::enabled() const noexcept
{
  return m_enabled;
}
