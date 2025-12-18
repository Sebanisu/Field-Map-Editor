//
// Created by pcvii on 12/14/2022.
//

#include "SafeDir.hpp"

namespace ff_8
{
SafeDir::SafeDir(const std::filesystem::path &path)
  : m_path(path)
{
     std::error_code ec{};
     m_status = std::filesystem::status(m_path, ec);
     if (ec)
     {
          debug(ec, __FILE__, __LINE__);
          return;
     }
}

SafeDir::operator const std::filesystem::path &() const
{
     return m_path;
}

bool SafeDir::is_exists() const noexcept
{
     return std::filesystem::exists(m_status);
}

bool SafeDir::is_dir() const noexcept
{
     return std::filesystem::is_directory(m_status);
}

bool SafeDir::is_file() const noexcept
{
     return std::filesystem::is_regular_file(m_status);
}

bool SafeDir::is_symlink() const noexcept
{
     return std::filesystem::is_symlink(m_status);
}

bool SafeDir::is_block() const noexcept
{
     return std::filesystem::is_block_file(m_status);
}

bool SafeDir::is_char() const noexcept
{
     return std::filesystem::is_character_file(m_status);
}

bool SafeDir::is_fifo() const noexcept
{
     return std::filesystem::is_fifo(m_status);
}

bool SafeDir::is_socket() const noexcept
{
     return std::filesystem::is_socket(m_status);
}

bool SafeDir::is_other() const noexcept
{
     return std::filesystem::is_other(m_status);
}

std::filesystem::perms SafeDir::permissions() const noexcept
{
     return m_status.permissions();
}

const std::filesystem::path &SafeDir::path() const noexcept
{
     return m_path;
}

std::filesystem::path SafeDir::case_insensitive_exists() const
{
     std::error_code ec{};
     for (const auto &entry :
          std::filesystem::directory_iterator(m_path.parent_path(), ec))
     {
          bool size_match = entry.path().filename().string().size()
                            == m_path.filename().string().size();
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
void SafeDir::debug(
  std::error_code &ec,
  std::string_view file,
  int              line) const
{
     if (ec == std::errc::no_such_file_or_directory)
     {
          // Not found - common, safe to ignore
          return;
     }
     spdlog::info(
       "{}:{} - {}: {} path: \"{}\"",
       file,
       line,
       ec.value(),
       ec.message(),
       m_path.string());
     ec.clear();
}

}// namespace ff_8