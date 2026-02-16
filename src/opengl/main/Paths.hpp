#include "open_viii/paths/Paths.hpp"
#include <filesystem>
#include <spdlog/spdlog.h>

namespace fme
{
inline static const std::filesystem::path app_name = "Field-Map-Editor";


// Example: Config path (inside dataHome or configHome)
inline std::filesystem::path              getAppDataDir()
{
     return open_viii::Paths::dataHome() / app_name;
}

inline std::filesystem::path getAppConfigDir()
{
     return open_viii::Paths::configHome() / app_name;
}

// Logs path (inside stateHome)
inline std::filesystem::path getAppLogsDir()
{
     return open_viii::Paths::stateHome() / app_name / "Logs";
}

inline void createDirs()
{
     const auto create_log
       = [](
           const spdlog::format_string_t<std::string> format,
           const std::filesystem::path               &path)
     {
          std::error_code ec;
          std::filesystem::create_directories(path, ec);

          if (ec)
          {
               spdlog::error("create_directories failed: {}", ec.message());
          }

          spdlog::info(format, path.string());
     };
     create_log("Data Dir:   \"{}\"", getAppDataDir());
     create_log("Config Dir: \"{}\"", getAppConfigDir());
     create_log("Logs Dir:   \"{}\"", getAppLogsDir());
}

}// namespace fme
