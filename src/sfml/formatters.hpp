//
// Created by pcvii on 11/15/2021.
//

#ifndef FIELD_MAP_EDITOR_FORMATTERS_HPP
#define FIELD_MAP_EDITOR_FORMATTERS_HPP
#include <filesystem>
#include <fmt/format.h>
#include <open_viii/graphics/background/BlendModeT.hpp>
#include <open_viii/graphics/BPPT.hpp>

template<>
struct fmt::formatter<open_viii::graphics::background::BlendModeT>
  : fmt::formatter<std::string_view>
{
  // parse is inherited from formatter<string_view>.
  template<typename FormatContext>
  constexpr auto format(
    open_viii::graphics::background::BlendModeT blend_mode_t,
    FormatContext                              &ctx) const
  {
    using namespace open_viii::graphics::background;
    using namespace std::string_view_literals;
    std::string_view name = {};
    switch (blend_mode_t)
    {
      case BlendModeT::add:
        name = "add"sv;
        break;
      case BlendModeT::half_add:
        name = "half add"sv;
        break;
      case BlendModeT::none:
        name = "none"sv;
        break;
      case BlendModeT::quarter_add:
        name = "quarter add"sv;
        break;
      case BlendModeT::subtract:
        name = "subtract"sv;
        break;
    }
    return fmt::formatter<std::string_view>::format(name, ctx);
  }
};

template<>
struct fmt::formatter<tile_sizes>
  : fmt::formatter<std::underlying_type_t<tile_sizes>>
{
  // parse is inherited from formatter<std::underlying_type_t<tile_sizes>>.
  template<typename FormatContext>
  constexpr auto format(tile_sizes tile_size, FormatContext &ctx) const
  {
    return fmt::formatter<std::underlying_type_t<tile_sizes>>::format(
      static_cast<std::underlying_type_t<tile_sizes>>(tile_size), ctx);
  }
};

template<>
struct fmt::formatter<open_viii::graphics::BPPT> : fmt::formatter<std::uint32_t>
{
  // parse is inherited from formatter<string_view>.
  template<typename FormatContext>
  constexpr auto
    format(open_viii::graphics::BPPT bppt, FormatContext &ctx) const
  {
    using namespace open_viii::graphics;
    using namespace std::string_view_literals;

    if (bppt.bpp8())
    {
      return fmt::formatter<std::uint32_t>::format(BPPT::BPP8, ctx);
    }
    if (bppt.bpp16())
    {
      return fmt::formatter<std::uint32_t>::format(BPPT::BPP16, ctx);
    }
    if (bppt.bpp24())
    {
      return fmt::formatter<std::uint32_t>::format(BPPT::BPP24, ctx);
    }
    return fmt::formatter<std::uint32_t>::format(BPPT::BPP4, ctx);
  }
};

template<>
struct fmt::formatter<std::filesystem::path> : fmt::formatter<std::string>
{
  // parse is inherited from formatter<string_view>.
  template<typename FormatContext>
  constexpr auto
    format(const std::filesystem::path &path, FormatContext &ctx) const
  {
    return fmt::formatter<std::string>::format(path.string(), ctx);
  }
};
#endif// FIELD_MAP_EDITOR_FORMATTERS_HPP
