//
// Created by pcvii on 9/5/2022.
//

#ifndef FIELD_MAP_EDITOR_CONFIGURATION_HPP
#define FIELD_MAP_EDITOR_CONFIGURATION_HPP
#include <filesystem>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <toml++/toml.h>
namespace fme
{
/**
 * @brief Manages the configuration settings for the application, using a TOML file.
 *
 * This class provides access to a TOML configuration file and allows for reading and writing
 * settings. The configuration file is loaded into memory and saved when necessary.
 */
class Configuration
{
   public:
     /**
      * @brief Constructs a Configuration object and loads the TOML configuration file.
      *
      * The configuration file is expected to be located at `Field-Map-Editor_SFML.toml`
      * relative to the current working directory.
      */
     Configuration();

     /**
      * @brief Provides mutable access to the underlying TOML table.
      * @return A pointer to the mutable TOML table.
      */
     toml::table                      *operator->() &;

     /**
      * @brief Deleted move-access operator for mutable TOML table.
      *
      * Prevents accessing the TOML table through rvalue references.
      */
     toml::table                      *operator->() && = delete;

     /**
      * @brief Provides read-only access to the underlying TOML table.
      * @return A pointer to the immutable TOML table.
      */
     const toml::table                *operator->() const &&;

     /**
      * @brief Provides read-only access to the underlying TOML table.
      * @return A pointer to the immutable TOML table.
      */
     const toml::table                *operator->() const &;

     /**
      * @brief Accesses a value in the TOML table by key.
      * @param i The key of the configuration entry to retrieve.
      * @return The value associated with the specified key, or an empty value if the key is not found.
      */
     toml::node_view<const toml::node> operator[](std::string_view i) const;

     /**
      * @brief Saves the current state of the configuration back to the file.
      *
      * Writes the in-memory TOML table to the configuration file located at `m_path`.
      */
     void                              save() const;

   private:
     /**
      * @brief The path to the TOML configuration file.
      *
      * This is a static inline constant that resolves to `Field-Map-Editor_SFML.toml`
      * in the current working directory.
      */
     static inline const auto m_path = []() {
          std::error_code error_code = {};
          auto            str        = std::filesystem::current_path(error_code) / "Field-Map-Editor_SFML.toml";
          if (error_code)
          {
               spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), str);
               error_code.clear();
          }
          return str;
     }();

     /**
      * @brief The in-memory representation of the TOML configuration file.
      */
     toml::table m_table{};
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_CONFIGURATION_HPP
