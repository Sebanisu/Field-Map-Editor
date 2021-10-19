//
// Created by pcvii on 9/29/2021.
//

#ifndef MYPROJECT_UPSCALES_HPP
#define MYPROJECT_UPSCALES_HPP
#include "open_viii/strings/LangCommon.hpp"
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
  std::filesystem::path             m_root       = {};
  std::string                       m_field_name = { "bccent_1" };
  /**
   * remaster uses x, other versions have 3 char coo
   */
  open_viii::LangT                  m_coo        = {};
  static constexpr std::string_view m_ext        = ".png";

public:
  upscales() = default;
  upscales(std::filesystem::path root,
    std::string                  field_name,
    const open_viii::LangT      &coo)
    : m_root(std::move(root))
    , m_field_name(std::move(field_name))
    , m_coo(coo)
  {
  }
  upscales(std::string field_name, const open_viii::LangT &coo)
    : m_root(std::filesystem::current_path())
    , m_field_name(std::move(field_name))
    , m_coo(coo)
  {
  }

  [[nodiscard]] auto
    get_file_paths(const std::filesystem::path &field_root,
      std::uint8_t                              texture_page,
      std::uint8_t                              palette,
      std::string_view                          ext = m_ext) const
  {
    return std::array{ field_root
                         / fmt::format("{}_{}_{}_{}{}",
                           m_field_name,
                           open_viii::LangCommon::to_string(m_coo),
                           texture_page,
                           palette,
                           ext),
      field_root
        / fmt::format("{}_{}_{}{}", m_field_name, texture_page, palette, ext) };
  }

  [[nodiscard]] auto
    get_file_paths(const std::filesystem::path &field_root,
      std::uint8_t                              texture_page,
      std::string_view                          ext = m_ext) const
  {
    return std::array{ field_root
                         / fmt::format("{}_{}_{}{}",
                           m_field_name,
                           open_viii::LangCommon::to_string(m_coo),
                           texture_page,
                           ext),
      field_root / fmt::format("{}_{}{}", m_field_name, texture_page, ext) };
  }

  [[nodiscard]] auto
    get_paths() const
  {
    const std::string_view prefix = std::string_view(m_field_name).substr(0, 2);

    return std::array{
      m_root
        / tl::string::replace_slashes(fmt::format(
          R"(DEMASTER_EXP\textures\field_bg\{}\{}\)", prefix, m_field_name)),
      m_root
        / tl::string::replace_slashes(
          fmt::format(R"(textures\fields\{}\{}\)", prefix, m_field_name)),
      m_root
        / tl::string::replace_slashes(
          fmt::format(R"(textures\{}\{}\)", prefix, m_field_name)),
      m_root
        / tl::string::replace_slashes(
          fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})",
            open_viii::LangCommon::to_string_3_char(m_coo),
            prefix,
            m_field_name)),
      m_root
        / tl::string::replace_slashes(
          fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})",
            open_viii::LangCommon::to_string_3_char(m_coo),
            prefix,
            m_field_name)),
      m_root
        / tl::string::replace_slashes(fmt::format(
          R"(ff8\Data\{}\field\mapdata\{}\{})", 'x', prefix, m_field_name)),
      m_root
        / tl::string::replace_slashes(fmt::format(
          R"(ff8\Data\{}\FIELD\mapdata\{}\{})", 'x', prefix, m_field_name))
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
