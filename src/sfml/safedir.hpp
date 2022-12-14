//
// Created by pcvii on 12/14/2022.
//

#ifndef FIELD_MAP_EDITOR_SAFEDIR_HPP
#define FIELD_MAP_EDITOR_SAFEDIR_HPP
#include <filesystem>
#include <spdlog/spdlog.h>
#include <system_error>
class safedir
{
public:
  safedir(const std::filesystem::path &path)
    : m_path(path)
    , m_is_dir(set_is_dir())
    , m_is_exist(set_is_exists())
  {
  }
  //  operator bool() const
  //  {
  //    return m_is_dir && m_is_exist;
  //  }
  explicit operator const std::filesystem::path &() const
  {
    return m_path;
  }
  bool is_exists() const
  {
    return m_is_exist;
  }
  bool is_dir() const
  {
    return m_is_dir;
  }

private:
  bool set_is_exists()
  {
    if (m_path.empty())
    {
      return false;
    }
    std::error_code ec         = {};
    bool            return_val = std::filesystem::exists(m_path, ec);
    if (ec)
    {
      warn(ec, __FILE__, __LINE__);
      return false;
    }
    return return_val;
  }
  bool set_is_dir()
  {
    if (m_path.empty())
    {
      return false;
    }
    std::error_code ec         = {};
    bool            return_val = std::filesystem::is_directory(m_path, ec);
    if (ec)
    {
      warn(ec, __FILE__, __LINE__);
      return false;
    }
    return return_val;
  }
  void warn(std::error_code &ec, std::string_view file, int line)
  {
    spdlog::warn(
      "{}:{} - {}: {} path: \"{}\"",
      file,
      line,
      ec.value(),
      ec.message(),
      m_path.string());
    ec.clear();
  }
  const std::filesystem::path &m_path     = {};
  bool                         m_is_dir   = { false };
  bool                         m_is_exist = { false };
};
#endif// FIELD_MAP_EDITOR_SAFEDIR_HPP
