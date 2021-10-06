//
// Created by pcvii on 9/29/2021.
//

#ifndef MYPROJECT_UPSCALES_HPP
#define MYPROJECT_UPSCALES_HPP
#include "open_viii/strings/LangT.hpp"
#include "tl/string.hpp"
#include <array>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <string_view>
#include <utility>
struct upscales
{
private:
  std::filesystem::path m_root         = {};
  const std::string     m_field_name   = { "bccent_1" };
  /**
   * remaster uses x, other versions have 3 char coo
   */
  open_viii::LangT      m_coo          = {};
  std::uint8_t          m_texture_page = {};
  std::uint8_t          m_palette      = {};
  std::string           m_ext          = ".png";

public:
  upscales() = default;
  upscales(std::filesystem::path root,
    std::string                  field_name,
    const open_viii::LangT      &coo,
    std::uint8_t                 texture_page,
    std::uint8_t                 palette)
    : m_root(std::move(root))
    , m_field_name(std::move(field_name))
    , m_coo(coo)
    , m_texture_page(texture_page)
    , m_palette(palette)
  {
  }

  void
    get_path() const
  {
    const std::string_view prefix = std::string_view(m_field_name).substr(0, 2);
    const auto             filenames = std::array{
      fmt::format("{}_{}_{}{}", m_field_name, m_texture_page, m_palette, m_ext),
      fmt::format("{}_{}{}", m_field_name, m_texture_page, m_ext)
    };
    std::string three_character_coo_or_x = "x";// placeholder.
    const auto  paths                    = std::array{
      tl::string::replace_slashes(fmt::format(
                            R"(DEMASTER_EXP\textures\field_bg\{}\{}\)", prefix, m_field_name)),

      tl::string::replace_slashes(
                            fmt::format(R"(textures\fields\{}\{}\)", prefix, m_field_name)),

      tl::string::replace_slashes(
                            fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})",
                              three_character_coo_or_x,
                              prefix,
                              m_field_name)),

      tl::string::replace_slashes(
                            fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})",
                              three_character_coo_or_x,
                              prefix,
                              m_field_name))
    };
  }

  /*
   * Scan the directories looking for textures.
   *
   * Look for textures
   *
   *
   */
};
#endif// MYPROJECT_UPSCALES_HPP
