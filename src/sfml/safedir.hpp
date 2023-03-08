//
// Created by pcvii on 12/14/2022.
//

#ifndef FIELD_MAP_EDITOR_SAFEDIR_HPP
#define FIELD_MAP_EDITOR_SAFEDIR_HPP
#include <filesystem>
#include <open_viii/tools/Compare.hpp>
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
  std::filesystem::path case_insensitive_exists() const
  {
    std::error_code ec{};
    for (const auto &entry :
         std::filesystem::directory_iterator(m_path.parent_path(), ec))
    {
      bool size_match =
        entry.path().filename().string().size() == m_path.filename().string().size();
      if (
        size_match
        && open_viii::tools::i_equals(
          entry.path().filename().string(), m_path.filename().string()))
      {
        return entry.path();
      }
    }
    if (ec)
    {
      debug(ec, __FILE__, __LINE__);
    }
    return {};
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
      debug(ec, __FILE__, __LINE__);
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
    bool            const return_val = std::filesystem::is_directory(m_path, ec);
    if (ec)
    {
      debug(ec, __FILE__, __LINE__);
      return false;
    }
    return return_val;
  }
  void debug(std::error_code &ec, std::string_view file, int line) const
  {
    spdlog::debug(
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
