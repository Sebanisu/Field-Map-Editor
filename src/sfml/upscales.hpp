//
// Created by pcvii on 9/29/2021.
//

#ifndef FIELD_MAP_EDITOR_UPSCALES_HPP
#define FIELD_MAP_EDITOR_UPSCALES_HPP
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
     std::optional<open_viii::LangT>   m_coo        = { std::nullopt };
     static constexpr std::string_view m_ext        = ".png";

   public:
     upscales() = default;
     upscales(std::filesystem::path root, std::string field_name, std::optional<open_viii::LangT> coo)
       : m_root(std::move(root))
       , m_field_name(std::move(field_name))
       , m_coo(coo)
     {
     }
     upscales(std::string field_name, std::optional<open_viii::LangT> coo)
       : m_root(std::filesystem::current_path())
       , m_field_name(std::move(field_name))
       , m_coo(coo)
     {
     }

     [[nodiscard]] auto generate_upscale_paths(
       const std::filesystem::path &field_root,
       std::uint8_t                 texture_page,
       std::uint8_t                 palette,
       std::string_view             ext = m_ext) const
     {
          const std::string_view prefix = std::string_view(m_field_name).substr(0, 2);
          if (m_coo)
          {
               return std::array{
                    field_root
                      / fmt::format("{}_{}_{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), texture_page, palette, ext),
                    field_root / prefix / m_field_name
                      / fmt::format("{}_{}_{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), texture_page, palette, ext)
               };
          }
          return std::array{ field_root / fmt::format("{}_{}_{}{}", m_field_name, texture_page, palette, ext),
                             field_root / prefix / m_field_name / fmt::format("{}_{}_{}{}", m_field_name, texture_page, palette, ext) };
     }

     [[nodiscard]] auto
       generate_upscale_paths(const std::filesystem::path &field_root, std::uint8_t texture_page, std::string_view ext = m_ext) const
     {

          const std::string_view prefix = std::string_view(m_field_name).substr(0, 2);
          if (m_coo)
          {
               return std::array{
                    field_root / fmt::format("{}_{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), texture_page, ext),
                    field_root / prefix / m_field_name
                      / fmt::format("{}_{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), texture_page, ext)
               };
          }
          return std::array{ field_root / fmt::format("{}_{}{}", m_field_name, texture_page, ext),
                             field_root / prefix / m_field_name / fmt::format("{}_{}{}", m_field_name, texture_page, ext) };
     }

     [[nodiscard]] auto get_paths() const
     {
          if (m_field_name.empty())
          {
               return std::array{
                    m_root / tl::string::replace_slashes(fmt::format(R"(DEMASTER_EXP\textures\field_bg)")),
                    m_root / "field_bg",
                    m_root / tl::string::replace_slashes(fmt::format(R"(textures\fields)")),
                    m_root / tl::string::replace_slashes(fmt::format(R"(textures)")),
                    m_root
                      / tl::string::replace_slashes(fmt::format(
                        R"(ff8\Data\{}\field\mapdata)",
                        open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic))),
                    m_root
                      / tl::string::replace_slashes(fmt::format(
                        R"(ff8\Data\{}\FIELD\mapdata)",
                        open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic))),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata)", open_viii::LangCommon::ENG)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata)", open_viii::LangCommon::ENG)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata)", open_viii::LangCommon::FRE)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata)", open_viii::LangCommon::FRE)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata)", open_viii::LangCommon::GER)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata)", open_viii::LangCommon::GER)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata)", open_viii::LangCommon::ITA)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata)", open_viii::LangCommon::ITA)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata)", open_viii::LangCommon::SPA)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata)", open_viii::LangCommon::SPA)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata)", open_viii::LangCommon::JP)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata)", open_viii::LangCommon::JP)),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata)", 'x')),
                    m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata)", 'x'))
               };
          }
          const std::string_view prefix = std::string_view(m_field_name).substr(0, 2);
          return std::array{
               m_root / tl::string::replace_slashes(fmt::format(R"(DEMASTER_EXP\textures\field_bg\{}\{}\)", prefix, m_field_name)),
               m_root / tl::string::replace_slashes(fmt::format(R"(field_bg\{}\{}\)", prefix, m_field_name)),
               m_root / tl::string::replace_slashes(fmt::format(R"(textures\fields\{}\{}\)", prefix, m_field_name)),
               m_root / tl::string::replace_slashes(fmt::format(R"(textures\{}\{}\)", prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(fmt::format(
                   R"(ff8\Data\{}\field\mapdata\{}\{})",
                   open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic),
                   prefix,
                   m_field_name)),
               m_root
                 / tl::string::replace_slashes(fmt::format(
                   R"(ff8\Data\{}\FIELD\mapdata\{}\{})",
                   open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic),
                   prefix,
                   m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})", open_viii::LangCommon::ENG, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})", open_viii::LangCommon::ENG, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})", open_viii::LangCommon::FRE, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})", open_viii::LangCommon::FRE, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})", open_viii::LangCommon::GER, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})", open_viii::LangCommon::GER, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})", open_viii::LangCommon::ITA, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})", open_viii::LangCommon::ITA, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})", open_viii::LangCommon::SPA, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})", open_viii::LangCommon::SPA, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})", open_viii::LangCommon::JP, prefix, m_field_name)),
               m_root
                 / tl::string::replace_slashes(
                   fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})", open_viii::LangCommon::JP, prefix, m_field_name)),
               m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\field\mapdata\{}\{})", 'x', prefix, m_field_name)),
               m_root / tl::string::replace_slashes(fmt::format(R"(ff8\Data\{}\FIELD\mapdata\{}\{})", 'x', prefix, m_field_name))
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
#endif// FIELD_MAP_EDITOR_UPSCALES_HPP
