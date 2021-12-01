//
// Created by pcvii on 11/30/2021.
//
#include "TestBatchRenderer.hpp"

void
  test::TestBatchRenderer::GenerateQuads() const
{
  m_batch_renderer.Clear();
  auto x_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[0]));
  auto y_rng = std::views::iota(uint32_t{}, static_cast<uint32_t>(m_count[1]));
  auto text_index_rng =
    std::views::iota(uint32_t{})
    | std::views::transform([this](const auto i)
                            { return i % std::ranges::size(m_textures); });
  auto text_index = text_index_rng.begin();
  for (const auto x : x_rng)
  {
    for (const auto y : y_rng)
    {
      float r = static_cast<float>(x) / static_cast<float>(m_count[0]);
      float g = static_cast<float>(y) / static_cast<float>(m_count[1]);

      m_batch_renderer.DrawQuad(
        { static_cast<float>(x), static_cast<float>(y) },
        { r, g, 1.F, 1.F },
        m_textures.at(*text_index),
        static_cast<float>(1U << (*text_index)));
      ++text_index;
    }
  }
  m_batch_renderer.Draw();
}