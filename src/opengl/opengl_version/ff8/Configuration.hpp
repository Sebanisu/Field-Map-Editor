//
// Created by pcvii on 9/5/2022.
//

#ifndef FIELD_MAP_EDITOR_CONFIGURATION_HPP
#define FIELD_MAP_EDITOR_CONFIGURATION_HPP
#include <toml++/toml.h>
class Configuration
{
public:
  Configuration()
    : m_table([]() {
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
  toml::table *operator->() &
  {
    return &m_table;
  }
  toml::table       *operator->()       && = delete;
  const toml::table *operator->() const &&
  {
    return &m_table;
  }
  const toml::table *operator->() const &
  {
    return &m_table;
  }
  auto operator[](std::string_view i) const
  {
    return m_table[i];
  }

  void save() const
  {
    std::error_code error_code{};
    std::filesystem::create_directories(m_path.parent_path(), error_code);
    if (error_code)
    {
         spdlog::error(
           "{}:{} - {}: {} - path: {}", __FILE__, __LINE__, error_code.value(), error_code.message(), m_path.parent_path().string());
         error_code.clear();
    }
    auto fs =
      std::ofstream(m_path, std::ios::out | std::ios::binary | std::ios::trunc);
    if (!fs)
    {
      spdlog::error(
        "ofstream: failed to open \"{}\" for writing", m_path.string());
      return;
    }
    fs << m_table;
  }

private:
  static inline const auto m_path =
    std::filesystem::current_path() / "res" / "ff8_config.toml";
  toml::table m_table{};
};
#endif// FIELD_MAP_EDITOR_CONFIGURATION_HPP
