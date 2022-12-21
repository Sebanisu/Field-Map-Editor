//
// Created by pcvii on 12/21/2022.
//

#include "batch_reswizzle.hpp"

void batch_reswizzle::disable()
{
  enabled = false;
}
void batch_reswizzle::enable(
  std::filesystem::path in_incoming,
  std::filesystem::path in_outgoing)
{
  enabled = true;
  pos     = 0;
  filters.deswizzle.update(std::move(in_incoming)).enable();
  outgoing = std::move(in_outgoing);
  asked    = false;
  start    = std::chrono::high_resolution_clock::now();
}