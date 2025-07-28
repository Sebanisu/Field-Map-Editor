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
          requires std::constructible_from<OutputT, InputT>
     bool load_array(const std::string_view key, std::vector<OutputT> &output) const
     {
          if (!operator->()->contains(key))
          {
               return false;
          }

          if (const auto *array = operator[](key).as_array(); array)
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
     void update_array(const std::string_view key, const std::vector<InputT> &input)
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

          operator->()->insert_or_assign(key, std::move(array));
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


/**
 * @brief Loads a string array from the TOML configuration.
 *
 * If the specified key exists and contains a TOML array of strings,
 * the `output` vector is cleared and replaced with its contents.
 * If the key is not found or not an array of strings, `output` is left unchanged.
 *
 * @param key The TOML key to look up.
 * @param output The vector to populate with strings from the config.
 * @returns true if changed
 *
 * @note This function expects that the caller is accessing a wrapper
 *       that uses `operator->()` and `operator[]` for TOML table access.
 */
template<>
inline bool Configuration::load_array<std::string, std::string>(const std::string_view key, std::vector<std::string> &output) const
{
     if (!operator->()->contains(key))
     {
          return false;
     }

     if (const auto *array = operator[](key).as_array(); array)
     {
          output.clear();
          output.reserve(array->size());
          for (auto &&val : *array)
          {
               if (auto str = val.value<std::string>(); str.has_value())
               {
                    output.emplace_back(std::move(str.value()));
               }
          }
          return true;
     }
     return false;
}

/**
 * @brief Loads a TOML array of booleans from the given key into a vector.
 *
 * This function checks whether the specified key exists and corresponds to an array.
 * If it does, it attempts to read each element as a `bool` and appends it to the output vector.
 *
 * @param key The TOML key to look up.
 * @param output A reference to a vector where the boolean values will be stored.
 * @return true if the array was successfully loaded; false otherwise (e.g., key not found or wrong type).
 */
template<>
inline bool Configuration::load_array<bool, bool>(const std::string_view key, std::vector<bool> &output) const
{
     if (!operator->()->contains(key))
          return false;

     if (const auto *array = operator[](key).as_array(); array)
     {
          output.clear();
          output.reserve(array->size());

          for (const auto &val : *array)
          {
               if (auto b = val.value<bool>(); b.has_value())
                    output.push_back(*b);
          }
          return true;
     }
     else if (const auto *str = operator[](key).as_string(); str)
     {
          output = str->get() | std::ranges::views::transform([](const char &c) { return c == '0' ? false : true; })
                   | std::ranges::to<std::vector>();
          return true;
     }
     return false;
}


/**
 * @brief Loads an array of file system paths from the TOML configuration.
 *
 * If the specified key exists and maps to a TOML array of strings, this function
 * clears and replaces the `output` vector with the parsed paths. If the key does not
 * exist or is not an array of strings, no changes are made to `output`.
 *
 * @param key The TOML key to look up.
 * @param output A vector to populate with `std::filesystem::path` values.
 * @returns true if changed
 *
 * @note Assumes the TOML array contains strings representing file system paths.
 * @note Requires access via `operator->()` and `operator[]` to the TOML table.
 */
template<>
inline bool Configuration::load_array<std::filesystem::path, std::filesystem::path>(
  const std::string_view              key,
  std::vector<std::filesystem::path> &output) const
{
     if (!operator->()->contains(key))
     {
          return false;
     }

     if (const auto *array = operator[](key).as_array(); array)
     {
          output.clear();
          output.reserve(array->size());
          for (auto &&val : *array)
          {
               if (auto str_opt = val.value<std::u8string>(); str_opt)
               {
                    auto str = std::move(*str_opt);
                    std::ranges::replace(str, u8'\\', u8'/');
                    output.emplace_back(std::move(str));
               }
          }
          return true;
     }
     return false;
}


/**
 * @brief Writes a vector of booleans into the TOML table under the given key.
 *
 * This function creates a TOML array from the input vector and assigns it to the given key.
 * Any existing value at that key will be overwritten.
 *
 * @param key The TOML key under which the array will be stored.
 * @param input A vector of boolean values to be written into the TOML array.
 */
template<>
inline void Configuration::update_array<bool, bool>(const std::string_view key, const std::vector<bool> &input)
{
     std::string encoded =
       input | std::ranges::views::transform([](const auto &b) { return b ? '1' : '0'; }) | std::ranges::to<std::string>();
     operator->()->insert_or_assign(key, std::move(encoded));
}

/**
 * @brief Writes an array of file system paths to the TOML configuration.
 *
 * Converts the provided `std::filesystem::path` vector to a TOML array of strings
 * and inserts or replaces the value at the specified key in the configuration.
 *
 * @param key The TOML key under which to store the array.
 * @param input The vector of `std::filesystem::path` values to write.
 *
 * @note Paths are converted to strings using `path.string()`.
 * @note Requires access via `operator->()` to the TOML table.
 */
template<>
inline void Configuration::update_array<std::filesystem::path, std::filesystem::path>(
  const std::string_view                    key,
  const std::vector<std::filesystem::path> &input)
{
     toml::array array;
     array.reserve(input.size());

     for (const auto &path : input)
     {
          auto str = path.u8string();
          std::ranges::replace(str, u8'\\', u8'/');
          array.push_back(str);
     }

     operator->()->insert_or_assign(key, std::move(array));
}

}// namespace fme
#endif// FIELD_MAP_EDITOR_CONFIGURATION_HPP
