//
// Created by pcvii on 9/5/2022.
//

#include "Configuration.hpp"
fme::Configuration::Configuration(std::filesystem::path in_path)
  : m_path(in_path)
  , m_table([this]() {
       toml::parse_result result = toml::parse_file(m_path.string());
       if (!result)
       {
            spdlog::warn("TOML Parsing failed: {}\n\t{}", result.error().description(), m_path.string());
            return toml::table{};
       }
       return std::move(result).table();
  }())
{
}

fme::Configuration::Configuration()
  : Configuration([]() {
       std::error_code error_code = {};
       auto            str        = std::filesystem::current_path(error_code) / "Field-Map-Editor_SFML.toml";
       if (error_code)
       {
            spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), str);
            error_code.clear();
       }
       return str;
  }())
{
}
toml::table *fme::Configuration::operator->() &
{
     return &m_table;
}
const toml::table *fme::Configuration::operator->() const &&
{
     return &m_table;
}
const toml::table *fme::Configuration::operator->() const &
{
     return &m_table;
}
toml::node_view<const toml::node> fme::Configuration::operator[](std::string_view i) const
{
     return m_table[i];
}

void fme::Configuration::save() const
{
     std::error_code error_code = {};
     (void)std::filesystem::create_directories(m_path.parent_path(), error_code);
     if (error_code)
     {
          spdlog::warn(
            "create directories error: \"{}\" - \"{}\" - \"{}\"", m_path.parent_path().string(), error_code.message(), error_code.value());
          error_code.clear();
     }
     auto fs = std::ofstream(m_path, std::ios::out | std::ios::binary | std::ios::trunc);
     if (!fs)
     {
          spdlog::error("ofstream: failed to open \"{}\" for writing", m_path.string());
          return;
     }
     fs << m_table;
}