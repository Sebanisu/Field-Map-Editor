//
// Created by pcvii on 12/21/2022.
//

#include "batch_deswizzle.hpp"
void batch_deswizzle::enable(std::filesystem::path in_outgoing)
{
  enabled  = true;
  pos      = 0;
  outgoing = std::move(in_outgoing);
  asked    = false;
  start    = std::chrono::high_resolution_clock::now();
}
void batch_deswizzle::disable()
{
  enabled = false;
}