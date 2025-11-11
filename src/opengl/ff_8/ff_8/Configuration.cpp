//
// Created by pcvii on 9/5/2022.
//
#ifndef DEFAULT_CONFIG_PATH
#define DEFAULT_CONFIG_PATH "res/field-map-editor.toml"// fallback, just in case
#endif

#include "SafeDir.hpp"
#include <ff_8/Configuration.hpp>
#include <iostream>
#include <stacktrace>

ff_8::Configuration::Configuration(std::filesystem::path in_path)
  : m_path(std::move(in_path))
  , m_table(
      [&]()
      {
           auto it = s_tables.find(m_path);
           if (it == s_tables.end())
           {
                toml::parse_result result = toml::parse_file(m_path.string());
                if (!result)
                {
                     spdlog::warn(
                       "TOML Parsing failed: {}\n\t{}",
                       result.error().description(),
                       m_path.string());
                     it = s_tables.emplace(m_path, toml::table{}).first;
                }
                else
                {
                     it = s_tables.emplace(m_path, std::move(result).table())
                            .first;
                }
           }
           return &it->second;
      }())
{
}

ff_8::Configuration::Configuration()
  : ff_8::Configuration(
      []() -> const std::filesystem::path &
      {
           static std::filesystem::path path;// path is cached across calls
           static bool                  initialized = false;

           if (!initialized)
           {
                std::error_code error_code{};


                try
                {
                     path = std::filesystem::path(DEFAULT_CONFIG_PATH);

                     if (path.is_relative())
                     {
                          auto current
                            = std::filesystem::current_path(error_code);
                          if (error_code)
                          {
                               std::cerr
                                 << "Warning: failed to get current path: "
                                 << error_code.message() << '\n';
                               return path;// fallback to relative path
                          }
                          path = current / path;
                     }
                     path.make_preferred();
                }
                catch (const std::filesystem::filesystem_error &e)
                {
                     std::cerr
                       << "Filesystem error while constructing config path: "
                       << e.what() << '\n';
                }
                catch (const std::exception &e)
                {
                     std::cerr
                       << "Unexpected error while constructing config path: "
                       << e.what() << '\n';
                }
                if (error_code)
                {
                     spdlog::warn(
                       "{}:{} - {}: {} path: \"{}\"",
                       __FILE__,
                       __LINE__,
                       error_code.value(),
                       error_code.message(),
                       path.string());
                }
                initialized = true;
           }

           return path;
      }())
{
}


ff_8::Configuration::operator toml::table &() &
{
     return *m_table;
}
ff_8::Configuration::operator const toml::table &() const &
{
     return *m_table;
}
toml::table &ff_8::Configuration::operator*() &
{
     return *m_table;
}
toml::table const &ff_8::Configuration::operator*() const &
{
     return *m_table;
}

toml::table *ff_8::Configuration::operator->() &
{
     return m_table;
}
const toml::table *ff_8::Configuration::operator->() const &&
{
     return m_table;
}
const toml::table *ff_8::Configuration::operator->() const &
{
     return m_table;
}
toml::node_view<const toml::node>
  ff_8::Configuration::operator[](std::string_view i) const
{
     return std::as_const(*m_table)[i];
}

toml::node_view<const toml::node> ff_8::Configuration::operator()(
  std::string_view root,
  std::string_view child) const
{
     auto root_node = std::as_const(*m_table)[root];
     if (!root_node)
          return {};

     return root_node[child];
}

void ff_8::Configuration::save(const bool remove_from_cache) const
{
     const auto erase = [&]()
     {
          if (remove_from_cache)
          {
               s_tables.erase(m_path);
               m_table = nullptr;
               m_path  = std::filesystem::path{};
          }
     };

     std::error_code error_code = {};
     (void)std::filesystem::create_directories(
       m_path.parent_path(), error_code);
     if (error_code)
     {
          spdlog::warn(
            "create directories error: \"{}\" - \"{}\" - \"{}\"",
            m_path.parent_path().string(),
            error_code.message(),
            error_code.value());
          error_code.clear();
     }
     auto fs = std::ofstream(
       m_path, std::ios::out | std::ios::binary | std::ios::trunc);
     if (!fs)
     {
          spdlog::error(
            "ofstream: failed to open \"{}\" for writing", m_path.string());
          erase();
          return;
     }
     fs << *m_table;
     spdlog::info("ofstream: saved config \"{}\"", m_path.string());
     erase();
}

bool ff_8::Configuration::reload()
{
     toml::parse_result result = toml::parse_file(m_path.string());
     if (!result)
     {
          spdlog::warn(
            "TOML reload failed: {}\n\t{}",
            result.error().description(),
            m_path.string());
          return false;
     }

     s_tables[m_path] = std::move(result).table();
     m_table          = &s_tables[m_path];
     return true;
}
