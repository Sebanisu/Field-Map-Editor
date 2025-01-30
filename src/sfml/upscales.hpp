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
     static constexpr std::string_view m_ext        = { ".png" };
     bool                              m_no_prefix  = { false };

   public:
     upscales() = default;
     upscales(std::filesystem::path root, std::string field_name, std::optional<open_viii::LangT> coo, bool no_prefix = false)
       : m_root(std::move(root))
       , m_field_name(std::move(field_name))
       , m_coo(coo)
       , m_no_prefix(no_prefix)
     {
     }
     upscales(std::string field_name, std::optional<open_viii::LangT> coo, bool no_prefix = false)
       : m_root(std::filesystem::current_path())
       , m_field_name(std::move(field_name))
       , m_coo(coo)
       , m_no_prefix(no_prefix)
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
                      / fmt::format("{}_{}_{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), texture_page, palette, ext),
                    field_root / m_field_name / fmt::format("{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), ext)
               };
          }
          return std::array{ field_root / fmt::format("{}_{}_{}{}", m_field_name, texture_page, palette, ext),
                             field_root / prefix / m_field_name / fmt::format("{}_{}_{}{}", m_field_name, texture_page, palette, ext),
                             field_root / m_field_name / fmt::format("{}{}", m_field_name, ext) };
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
                      / fmt::format("{}_{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), texture_page, ext),
                    field_root / m_field_name
                      / fmt::format("{}_{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), texture_page, ext),
                    field_root / m_field_name / fmt::format("{}_{}{}", m_field_name, open_viii::LangCommon::to_string(*m_coo), ext)
               };
          }
          return std::array{ field_root / fmt::format("{}_{}{}", m_field_name, texture_page, ext),
                             field_root / prefix / m_field_name / fmt::format("{}_{}{}", m_field_name, texture_page, ext),
                             field_root / m_field_name / fmt::format("{}_{}{}", m_field_name, texture_page, ext),
                             field_root / m_field_name / fmt::format("{}{}", m_field_name, ext) };
     }

     [[nodiscard]] auto get_paths(bool no_prefix = false) const
     {
          if (m_field_name.empty())
          {
               return std::array{ m_root / "mods" / "Textures",
                                  m_root / "DEMASTER_EXP" / "textures" / "field_bg",
                                  m_root / "field_bg",
                                  m_root / "textures" / "fields",
                                  m_root / "textures",
                                  m_root / "ff8" / "Data"
                                    / open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic) / "field"
                                    / "mapdata",
                                  m_root / "ff8" / "Data"
                                    / open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic) / "FIELD"
                                    / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ENG / "field" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ENG / "FIELD" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::FRE / "field" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::FRE / "FIELD" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::GER / "field" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::GER / "FIELD" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ITA / "field" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ITA / "FIELD" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::SPA / "field" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::SPA / "FIELD" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::JP / "field" / "mapdata",
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::JP / "FIELD" / "mapdata",
                                  m_root / "ff8" / "Data" / "x" / "field" / "mapdata",
                                  m_root / "ff8" / "Data" / "x" / "FIELD" / "mapdata" };
          }
          if (m_no_prefix || no_prefix)
          {
               return std::array{ m_root / "mods" / "Textures" / m_field_name,
                                  m_root / "DEMASTER_EXP" / "textures" / "field_bg" / m_field_name,
                                  m_root / "field_bg" / m_field_name,
                                  m_root / "textures" / "fields" / m_field_name,
                                  m_root / "textures" / m_field_name,
                                  m_root / "ff8" / "Data"
                                    / open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic) / "field"
                                    / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data"
                                    / open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic) / "FIELD"
                                    / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ENG / "field" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ENG / "FIELD" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::FRE / "field" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::FRE / "FIELD" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::GER / "field" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::GER / "FIELD" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ITA / "field" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::ITA / "FIELD" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::SPA / "field" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::SPA / "FIELD" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::JP / "field" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / open_viii::LangCommon::JP / "FIELD" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / "x" / "field" / "mapdata" / m_field_name,
                                  m_root / "ff8" / "Data" / "x" / "FIELD" / "mapdata" / m_field_name };
          }
          const std::string_view prefix = std::string_view(m_field_name).substr(0, 2);
          return std::array{ m_root / "mods" / "Textures" / prefix / m_field_name,
                             m_root / "DEMASTER_EXP" / "textures" / "field_bg" / prefix / m_field_name,
                             m_root / "field_bg" / prefix / m_field_name,
                             m_root / "textures" / "fields" / prefix / m_field_name,
                             m_root / "textures" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic)
                               / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::to_string_3_char(m_coo ? *m_coo : open_viii::LangT::generic)
                               / "FIELD" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::ENG / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::ENG / "FIELD" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::FRE / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::FRE / "FIELD" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::GER / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::GER / "FIELD" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::ITA / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::ITA / "FIELD" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::SPA / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::SPA / "FIELD" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::JP / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / open_viii::LangCommon::JP / "FIELD" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / "x" / "field" / "mapdata" / prefix / m_field_name,
                             m_root / "ff8" / "Data" / "x" / "FIELD" / "mapdata" / prefix / m_field_name };
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
