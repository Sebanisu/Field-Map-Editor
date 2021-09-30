//
// Created by pcvii on 9/29/2021.
//

#ifndef MYPROJECT_UPSCALES_HPP
#define MYPROJECT_UPSCALES_HPP
#include "tl/string.hpp"
#include <array>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <string_view>
struct upscales
{
private:
  const std::string fieldname                = { "bccent_1" };
  /**
   * remaster uses x, other versions have 3 char coo
   */
  const std::string three_character_coo_or_x = "x";
  std::uint8_t      texture_page             = {};
  std::uint8_t      palette                  = {};
  std::string       ext                      = ".png";

private:
  void get_path() const
  {
    const std::string_view prefix    = std::string_view(fieldname).substr(0, 2);
    const auto             filenames = std::array{
      fmt::format("{}_{}_{}{}", fieldname, +texture_page, +palette, ext),
      fmt::format("{}_{}{}", fieldname, +texture_page, ext)
    };
    // fix slashes.
    const auto paths = std::array{
      tl::string::replace_slashes(fmt::format(
        R"(DEMASTER_EXP\textures\field_bg\{}\{}\)", prefix, fieldname)),

      tl::string::replace_slashes(
        fmt::format(R"(textures\fields\{}\{}\)", prefix, fieldname)),

      tl::string::replace_slashes(
        fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})",
          three_character_coo_or_x,
          prefix,
          fieldname)),

      tl::string::replace_slashes(
        fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})",
          three_character_coo_or_x,
          prefix,
          fieldname))
    };
  }
  //  std::string path = fmt::format("{}{}{}{}{}{}{}{}{}{}{}",
  //    "DEMASTER_EXP",
  //    std::filesystem::path::preferred_separator,
  //    "textures",
  //    std::filesystem::path::preferred_separator,
  //    "field_bg",
  //    std::filesystem::path::preferred_separator,
  //    "bc",
  //    std::filesystem::path::preferred_separator,
  //    "bccent_1",
  //    std::filesystem::path::preferred_separator,
  //    "bccent_1_0.png");
};
#endif// MYPROJECT_UPSCALES_HPP
