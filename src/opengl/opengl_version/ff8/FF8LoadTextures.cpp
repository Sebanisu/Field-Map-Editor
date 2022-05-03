//
// Created by pcvii on 12/1/2021.
//

#include "FF8LoadTextures.hpp"
#include <stb_image.h>

// static_assert(struct_of_color_byte<glm::vec<4, std::uint8_t>>);
glengine::DelayedTextures<upscale_texture_count>
  ff8::LoadTextures(const std::filesystem::path &upscale_path)
{
  glengine::DelayedTextures<upscale_texture_count> rdt{};
  rdt.futures.reserve(upscale_texture_count);
  auto process = [](
                   const std::filesystem::path file_path,
                   glengine::Texture *in_out) -> glengine::DelayedTexturesData {
    stbi_set_flip_vertically_on_load(1);

    int                  x        = {};
    int                  y        = {};
    [[maybe_unused]] int channels = {};

    const auto           deleter  = [](stbi_uc *ptr) { stbi_image_free(ptr); };
    auto                 png      = std::unique_ptr<stbi_uc, decltype(deleter)>(
      stbi_load(file_path.string().c_str(), &x, &y, &channels, 4));
    auto r = glengine::DelayedTexturesData{
      .path = file_path, .width = x, .height = y, .out = in_out
    };
    r.colors.resize(static_cast<std::size_t>(x * y));
    std::memcpy(
      r.colors.data(), png.get(), static_cast<std::size_t>(x * y) * 4U);
    return r;
  };
  auto texture_pages = std::views::iota(std::uint8_t{}, std::uint8_t{ 13 });
  auto palettes      = std::views::iota(std::uint8_t{}, std::uint8_t{ 16 });
  std::string current_file_prefix = upscale_path.stem().string();
  for (const auto texture_page : texture_pages)
  {
    const auto current_file =
      upscale_path
      / fmt::format("{}_{}.png", current_file_prefix, +texture_page);
    const auto index = texture_page;
    if (!std::filesystem::exists(current_file))
      continue;
    fmt::print("Loading Texture: {}\n", current_file.string());
    rdt.futures.emplace_back(std::async(
      std::launch::async, process, current_file, &rdt.textures->at(index)));
  }
  for (const auto texture_page : texture_pages)
    for (const auto palette : palettes)
    {
      const auto current_file =
        upscale_path
        / fmt::format(
          "{}_{}_{}.png", current_file_prefix, +texture_page, +palette);
      const auto index = texture_page + texture_page_count * (palette + 1U);
      if (!std::filesystem::exists(current_file))
        continue;
      fmt::print("Loading Texture: {}\n", current_file.string());
      rdt.futures.emplace_back(std::async(
        std::launch::async, process, current_file, &rdt.textures->at(index)));
    }
  return rdt;
}
glengine::DelayedTextures<35U>
  ff8::LoadTextures(const open_viii::graphics::background::Mim &mim)
{
  glengine::DelayedTextures<35U> rdt{};
  rdt.futures.reserve(35U);
  auto bpps = std::views::iota(std::uint32_t{ 0 }, std::uint32_t{ 2 })
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
                   glengine::Texture *in_out) -> glengine::DelayedTexturesData {
    auto r = glengine::DelayedTexturesData{
      .colors =
        in_mim.get_colors<glm::vec<4, std::uint8_t>>(in_bpp, in_palette),
      .width  = static_cast<int32_t>(in_mim.get_width(in_bpp)),
      .height = static_cast<int32_t>(in_mim.get_height()),
      .out    = in_out
    };
    glengine::Texture::flip(r.colors, r.width);
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
    using namespace open_viii::graphics::literals;
    fmt::print("Loading BPP 16 Texture\n");
    rdt.futures.emplace_back(std::async(
      std::launch::async,
      process,
      mim,
      16_bpp,
      std::uint8_t{ 0 },
      &rdt.textures->at(2 * 16)));

    fmt::print("Loading BPP 24 Texture\n");
    rdt.futures.emplace_back(std::async(
      std::launch::async,
      process,
      mim,
      24_bpp,
      std::uint8_t{ 0 },
      &rdt.textures->at(2 * 16 + 1)));
  }
  {
    fmt::print("Loading Palette Texture\n");

    rdt.futures.emplace_back(std::async(
      std::launch::async,
      [](
        const open_viii::graphics::background::Mim in_mim,
        glengine::Texture *in_out) -> glengine::DelayedTexturesData {
        auto r = glengine::DelayedTexturesData{
          .colors = in_mim.get_colors<glm::vec<4, std::uint8_t>>({}, {}, true),
          .width  = static_cast<std::int32_t>(in_mim.get_width({}, true)),
          .height = static_cast<std::int32_t>(in_mim.get_height(true)),
          .out    = in_out
        };
        glengine::Texture::flip(r.colors, r.width);
        return r;
      },
      mim,
      &rdt.textures->back()));
  }
  return rdt;
}