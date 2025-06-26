//
// Created by pcvii on 9/5/2022.
//

#include "Configuration.hpp"
#include "safedir.hpp"
#include <iostream>
#include <stacktrace>
fme::Configuration::Configuration(const std::filesystem::path &in_path)


{
     if (!s_table)
     {
          s_table = std::make_unique<toml::table>([&]() {
               toml::parse_result result = toml::parse_file(in_path.string());
               if (!result)
               {
                    spdlog::warn("TOML Parsing failed: {}\n\t{}", result.error().description(), in_path.string());
                    return toml::table{};
               }
               return std::move(result).table();
          }());
     }
}

fme::Configuration::Configuration()
  : Configuration([&]() -> const std::filesystem::path & {
       if (s_table)
       {
            static const std::filesystem::path blank{};
            return blank;
       }
       const auto sd = safedir(s_path);
       if (sd.is_exists())
       {
            return s_path;
       }
       std::error_code error_code = {};
       s_path                     = std::filesystem::current_path(error_code) / "Field-Map-Editor_SFML.toml";
       if (error_code)
       {
            spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), s_path);
            error_code.clear();
       }
       return s_path;
  }())
{
}
toml::table *fme::Configuration::operator->() &
{
     return s_table.get();
}
const toml::table *fme::Configuration::operator->() const &&
{
     return s_table.get();
}
const toml::table *fme::Configuration::operator->() const &
{
     return s_table.get();
}
toml::node_view<const toml::node> fme::Configuration::operator[](std::string_view i) const
{
     return std::as_const(*s_table)[i];
}

void fme::Configuration::save() const
{
     std::error_code error_code = {};
     (void)std::filesystem::create_directories(s_path.parent_path(), error_code);
     if (error_code)
     {
          spdlog::warn(
            "create directories error: \"{}\" - \"{}\" - \"{}\"", s_path.parent_path().string(), error_code.message(), error_code.value());
          error_code.clear();
     }
     auto fs = std::ofstream(s_path, std::ios::out | std::ios::binary | std::ios::trunc);
     if (!fs)
     {
          spdlog::error("ofstream: failed to open \"{}\" for writing", s_path.string());
          return;
     }
     fs << *s_table;
     const auto st = std::stacktrace::current();
     std::cerr << st << std::endl;
     spdlog::info("ofstream: saved config \"{}\"", s_path.string());
}