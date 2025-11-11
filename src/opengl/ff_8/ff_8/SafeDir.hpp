//
// Created by pcvii on 12/14/2022.
//

#ifndef FIELD_MAP_EDITOR_SAFEDIR_HPP
#define FIELD_MAP_EDITOR_SAFEDIR_HPP
#include <filesystem>
#include <open_viii/tools/Compare.hpp>
#include <spdlog/spdlog.h>
#include <system_error>
namespace ff_8
{
class SafeDir
{
   public:
     SafeDir(const std::filesystem::path &path);
     explicit               operator const std::filesystem::path &() const;
     bool                   is_exists() const noexcept;
     bool                   is_dir() const noexcept;
     std::filesystem::path  case_insensitive_exists() const;
     bool                   is_file() const noexcept;
     bool                   is_symlink() const noexcept;
     bool                   is_block() const noexcept;
     bool                   is_char() const noexcept;
     bool                   is_fifo() const noexcept;
     bool                   is_socket() const noexcept;
     bool                   is_other() const noexcept;
     std::filesystem::perms permissions() const noexcept;
     const std::filesystem::path &path() const noexcept;

   private:
     void debug(
       std::error_code &ec,
       std::string_view file,
       int              line) const;
     const std::filesystem::path &m_path   = {};
     std::filesystem::file_status m_status = {};
};
}// namespace ff_8
#endif// FIELD_MAP_EDITOR_SAFEDIR_HPP
