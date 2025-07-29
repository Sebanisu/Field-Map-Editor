//
// Created by pcvii on 9/5/2022.
//

#ifndef FIELD_MAP_EDITOR_CONFIGURATION_HPP
#define FIELD_MAP_EDITOR_CONFIGURATION_HPP
#include <filesystem>
#include <fmt/std.h>
#include <ranges>
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
      * The configuration file is expected to be located at `field-map-editor.toml`
      * relative to the current working directory.
      */
     Configuration();


     Configuration(std::filesystem::path);

     operator toml::table &() &;
     operator const toml::table &() const &;
     toml::table                      &operator*() &;
     toml::table const                &operator*() const &;

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

     toml::node_view<const toml::node> operator()(std::string_view root, std::string_view child) const;

     /**
      * @brief Saves the current state of the configuration back to the file.
      *
      * Writes the in-memory TOML table to the configuration file located at `m_path`.
      */
     void                              save(const bool remove_from_cache = false) const;


     template<typename OutputT, typename InputT = OutputT>
          requires requires { static_cast<OutputT>(std::declval<InputT>()); }
     static bool load_array(const toml::table &table, const std::string_view key, std::vector<OutputT> &output)
     {
          if (!table.contains(key))
          {
               return false;
          }

          if (const auto *array = table[key].as_array(); array)
          {
               output.clear();
               output.reserve(array->size());
               for (auto &&val : *array)
               {
                    if (auto str = val.value<InputT>(); str.has_value())
                    {
                         if constexpr (std::same_as<std::remove_cvref_t<InputT>, std::remove_cvref_t<OutputT>>)
                         {
                              output.emplace_back(std::move(str.value()));
                         }
                         else
                         {
                              output.push_back(static_cast<OutputT>(str.value()));
                         }
                    }
               }
               return true;
          }
          return false;
     }


     /**
      * @brief Updates the TOML configuration with a string array.
      *
      * Replaces or creates the value at the given key with a TOML array
      * containing the provided strings.
      *
      * @param key The TOML key under which to store the array.
      * @param input The vector of strings to write into the config.
      *
      * @note This function expects that the caller is accessing a wrapper
      *       that uses `operator->()` for TOML table access.
      */

     template<typename InputT, typename OutputT = InputT>
     static void update_array(toml::table &table, const std::string_view key, const std::vector<InputT> &input)
     {
          toml::array array;
          array.reserve(input.size());

          for (const auto &str : input)
          {
               if constexpr (std::same_as<InputT, OutputT>)
               {
                    array.push_back(str);
               }
               else
               {
                    array.push_back(static_cast<OutputT>(str));
               }
          }

          table.insert_or_assign(key, std::move(array));
     }

   private:
     /**
      * @brief The path to the TOML configuration file.
      *
      * This is a static inline constant that resolves to `field-map-editor.toml`
      * in the current working directory.
      */
     mutable std::filesystem::path                              m_path = {};

     /**
      * @brief The in-memory representation of the TOML configuration file.
      */
     static inline std::map<std::filesystem::path, toml::table> s_tables{};
     mutable toml::table                                       *m_table = {};
};



}// namespace fme
#endif// FIELD_MAP_EDITOR_CONFIGURATION_HPP
