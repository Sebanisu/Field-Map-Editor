//
// Created by pcvii on 12/1/2021.
//

#include "FF8LoadTextures.hpp"
#include <future>
#include <vector>
DelayedTextures
  ff8::LoadTextures(const open_viii::graphics::background::Mim &mim)
{
  DelayedTextures rdt{};
  rdt.futures.reserve(49U);
  auto bpps = std::views::iota(std::uint32_t{ 0 }, std::uint32_t{ 3 })
              | std::views::transform([](const std::uint32_t i) {
                  using namespace open_viii::graphics::literals;
                  switch (1U << (i + 2U))
                  {
                    case open_viii::graphics::BPPT::BPP4:
                    default:
                      return 4_bpp;
                    case open_viii::graphics::BPPT::BPP8:
                      return 8_bpp;
                    case open_viii::graphics::BPPT::BPP16:
                      return 16_bpp;
                    case open_viii::graphics::BPPT::BPP24:
                      return 24_bpp;
                  }
                });
  auto palettes = std::views::iota(std::uint8_t{}, std::uint8_t{ 16 });
  auto process  = [](
                   const open_viii::graphics::background::Mim in_mim,
                   const open_viii::graphics::BPPT            in_bpp,
                   const std::uint8_t                         in_palette,
                   Texture *in_out) -> DelayedTextures::return_data {
    auto r = DelayedTextures::return_data{
      .colors =
        in_mim.get_colors<open_viii::graphics::Color32RGBA>(in_bpp, in_palette),
      .width  = static_cast<int32_t>(in_mim.get_width(in_bpp)),
      .height = static_cast<int32_t>(in_mim.get_height()),
      .out    = in_out
    };
    Texture::flip(r.colors, r.width);
    return r;
  };
  for (std::size_t bpp_offset{}; const open_viii::graphics::BPPT bpp : bpps)
  {
    const auto width  = mim.get_width(bpp);
    const auto height = mim.get_height();
    if (width == 0 || height == 0)
      return rdt;
    for (const std::uint8_t palette : palettes)
    {
      fmt::print(
        "Loading BPP {}, Palette {} Texture\n",
        1U << (bpp_offset + 2U),
        palette);

      rdt.futures.emplace_back(std::async(
        std::launch::async,
        process,
        mim,
        bpp,
        palette,
        &rdt.textures->at(bpp_offset * 16 + palette)));
    }
    ++bpp_offset;
  }
  {
    fmt::print("Loading Palette Texture\n");

    rdt.futures.emplace_back(std::async(
      std::launch::async,
      [](const open_viii::graphics::background::Mim in_mim, Texture *in_out)
        -> DelayedTextures::return_data {
        auto r = DelayedTextures::return_data{
          .colors =
            in_mim.get_colors<open_viii::graphics::Color32RGBA>({}, {}, true),
          .width  = static_cast<std::int32_t>(in_mim.get_width({}, true)),
          .height = static_cast<std::int32_t>(in_mim.get_height(true)),
          .out    = in_out
        };
        Texture::flip(r.colors, r.width);
        return r;
      },
      mim,
      &rdt.textures->back()));
  }
  return rdt;
}