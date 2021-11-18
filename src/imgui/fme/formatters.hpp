//
// Created by pcvii on 11/15/2021.
//

#ifndef MYPROJECT_FORMATTERS_HPP
#define MYPROJECT_FORMATTERS_HPP
#include <filesystem>
#include <fmt/format.h>
#include <open_viii/graphics/background/BlendModeT.hpp>
#include <open_viii/graphics/BPPT.hpp>
template<>
struct fmt::formatter<open_viii::graphics::background::BlendModeT>
{
  // Presentation format: 'f' - fixed, 'e' - exponential.
  char presentation = 'f';

  // Parses format specifications of the form ['f' | 'e'].
  constexpr auto
    parse(format_parse_context &ctx) -> decltype(ctx.begin())
  {
    // [ctx.begin(), ctx.end()) is a character range that contains a part of
    // the format string starting from the format specifications to be parsed,
    // e.g. in
    //
    //   fmt::format("{:f} - BlendModeT of interest", BlendModeT{1, 2});
    //
    // the range will contain "f} - BlendModeT of interest". The formatter
    // should parse specifiers until '}' or the end of the range. In this
    // example the formatter should parse the 'f' specifier and return an
    // iterator BlendModeTing to '}'.

    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e'))
      presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the BlendModeT p using the parsed format specification
  // (presentation) stored in this formatter.
  template<typename FormatContext>
  auto
    format(
      const open_viii::graphics::background::BlendModeT &p,
      FormatContext &ctx) -> decltype(ctx.out())
  {
    // ctx.out() is an output iterator to write to.
    switch (p)
    {
    default:
      return fmt::format_to(ctx.out(), "{}", static_cast<int>(p));
    case open_viii::graphics::background::BlendModeT::none:
      return fmt::format_to(ctx.out(), "none");
    case open_viii::graphics::background::BlendModeT::subtract:
      return fmt::format_to(ctx.out(), "subtract");
    case open_viii::graphics::background::BlendModeT::add:
      return fmt::format_to(ctx.out(), "add");
    case open_viii::graphics::background::BlendModeT::half_add:
      return fmt::format_to(ctx.out(), "half add");
    case open_viii::graphics::background::BlendModeT::quarter_add:
      return fmt::format_to(ctx.out(), "quarter add");
    }
  }
};
template<>
struct fmt::formatter<open_viii::graphics::BPPT>
{
  // Presentation format: 'f' - fixed, 'e' - exponential.
  char presentation = 'f';

  // Parses format specifications of the form ['f' | 'e'].
  constexpr auto
    parse(format_parse_context &ctx) -> decltype(ctx.begin())
  {
    // [ctx.begin(), ctx.end()) is a character range that contains a part of
    // the format string starting from the format specifications to be parsed,
    // e.g. in
    //
    //   fmt::format("{:f} - BPPT of interest", BPPT{1, 2});
    //
    // the range will contain "f} - BPPT of interest". The formatter should
    // parse specifiers until '}' or the end of the range. In this example
    // the formatter should parse the 'f' specifier and return an iterator
    // BPPTing to '}'.

    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e'))
      presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the BPPT p using the parsed format specification (presentation)
  // stored in this formatter.
  template<typename FormatContext>
  auto
    format(const open_viii::graphics::BPPT &p, FormatContext &ctx)
      -> decltype(ctx.out())
  {
    // ctx.out() is an output iterator to write to.
    if (p.bpp4())
    {
      return fmt::format_to(ctx.out(), "{}", 4);
    }
    if (p.bpp8())
    {
      return fmt::format_to(ctx.out(), "{}", 8);
    }
    if (p.bpp16())
    {
      return fmt::format_to(ctx.out(), "{}", 16);
    }
    if (p.bpp24())
    {
      return fmt::format_to(ctx.out(), "{}", 24);
    }
    return fmt::format_to(ctx.out(), "{}", static_cast<int>(p));
  }
};
template<>
struct fmt::formatter<std::filesystem::path>
{
  // Presentation format: 'f' - fixed, 'e' - exponential.
  char presentation = 'f';

  // Parses format specifications of the form ['f' | 'e'].
  constexpr auto
    parse(format_parse_context &ctx) -> decltype(ctx.begin())
  {
    // [ctx.begin(), ctx.end()) is a character range that contains a part of
    // the format string starting from the format specifications to be parsed,
    // e.g. in
    //
    //   fmt::format("{:f} - std::filesystem::path of interest",
    //   std::filesystem::path{1, 2});
    //
    // the range will contain "f} - std::filesystem::path of interest". The
    // formatter should parse specifiers until '}' or the end of the range. In
    // this example the formatter should parse the 'f' specifier and return an
    // iterator std::filesystem::pathing to '}'.

    // Parse the presentation format and store it in the formatter:
    auto it = ctx.begin(), end = ctx.end();
    if (it != end && (*it == 'f' || *it == 'e'))
      presentation = *it++;

    // Check if reached the end of the range:
    if (it != end && *it != '}')
      throw format_error("invalid format");

    // Return an iterator past the end of the parsed range:
    return it;
  }

  // Formats the std::filesystem::path p using the parsed format specification
  // (presentation) stored in this formatter.
  template<typename FormatContext>
  auto
    format(const std::filesystem::path &p, FormatContext &ctx)
      -> decltype(ctx.out())
  {
    // ctx.out() is an output iterator to write to.
    return fmt::format_to(ctx.out(), "{}", p.string());
  }
};
#endif// MYPROJECT_FORMATTERS_HPP
