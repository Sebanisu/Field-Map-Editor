//
// Created by pcvii on 9/5/2022.
//

#include "Configuration.hpp"
#include "safedir.hpp"
#include <iostream>
#include <stacktrace>

fme::Configuration::Configuration(std::filesystem::path in_path)
  : m_path(std::move(in_path))
  , m_table([&]() {
       auto [it, inserted] = s_tables.try_emplace(m_path, [&]() {
            toml::parse_result result = toml::parse_file(m_path.string());
            if (!result)
            {
                 spdlog::warn("TOML Parsing failed: {}\n\t{}", result.error().description(), m_path.string());
                 return toml::table{};
            }
            return std::move(result).table();
       }());

       return &it->second;
  }())
{
}

fme::Configuration::Configuration()
  : fme::Configuration([]() -> const std::filesystem::path & {
       static std::filesystem::path path;// path is cached across calls
       static bool                  initialized = false;

       if (!initialized)
       {
            std::error_code error_code{};
            path = std::filesystem::current_path(error_code) / "res" / "field-map-editor.toml";
            if (error_code)
            {
                 spdlog::warn("{}:{} - {}: {} path: \"{}\"", __FILE__, __LINE__, error_code.value(), error_code.message(), path.string());
            }
            initialized = true;
       }

       return path;
  }())
{
}


fme::Configuration::operator toml::table &() &
{
     return *m_table;
}
fme::Configuration::operator const toml::table &() const &
{
     return *m_table;
}
toml::table &fme::Configuration::operator*() &
{
     return *m_table;
}
toml::table const &fme::Configuration::operator*() const &
{
     return *m_table;
}

toml::table *fme::Configuration::operator->() &
{
     return m_table;
}
const toml::table *fme::Configuration::operator->() const &&
{
     return m_table;
}
const toml::table *fme::Configuration::operator->() const &
{
     return m_table;
}
toml::node_view<const toml::node> fme::Configuration::operator[](std::string_view i) const
{
     return std::as_const(*m_table)[i];
}

toml::node_view<const toml::node> fme::Configuration::operator()(std::string_view root, std::string_view child) const
{
     auto root_node = std::as_const(*m_table)[root];
     if (!root_node)
          return {};

     return root_node[child];
}

void fme::Configuration::save(const bool remove_from_cache) const
{
     std::error_code error_code = {};
     (void)std::filesystem::create_directories(m_path.parent_path(), error_code);
     if (error_code)
     {
          spdlog::warn(
            "create directories error: \"{}\" - \"{}\" - \"{}\"", m_path.parent_path().string(), error_code.message(), error_code.value());
          error_code.clear();
     }
     auto fs = std::ofstream(m_path, std::ios::out | std::ios::binary | std::ios::trunc);
     if (!fs)
     {
          spdlog::error("ofstream: failed to open \"{}\" for writing", m_path.string());
          return;
     }
     fs << *m_table;
     // const auto st = std::stacktrace::current();
     // std::cerr << st << std::endl;
     spdlog::info("ofstream: saved config \"{}\"", m_path.string());

     if (remove_from_cache)
     {
          s_tables.erase(m_path);
          m_table = nullptr;
          m_path  = std::filesystem::path{};
     }
}


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
bool fme::Configuration::load_array<std::string, std::string>(
  const toml::table        &table,
  const std::string_view    key,
  std::vector<std::string> &output)
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
bool fme::Configuration::load_array<bool, bool>(const toml::table &table, const std::string_view key, std::vector<bool> &output)
{
     if (!table.contains(key))
          return false;

     if (const auto *array = table[key].as_array(); array)
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
     else if (const auto *str = table[key].as_string(); str)
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
bool fme::Configuration::load_array<std::filesystem::path, std::filesystem::path>(
  const toml::table                  &table,
  const std::string_view              key,
  std::vector<std::filesystem::path> &output)
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
void fme::Configuration::update_array<bool, bool>(toml::table &table, const std::string_view key, const std::vector<bool> &input)
{
     std::string encoded =
       input | std::ranges::views::transform([](const auto &b) { return b ? '1' : '0'; }) | std::ranges::to<std::string>();
     table.insert_or_assign(key, std::move(encoded));
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
void fme::Configuration::update_array<std::filesystem::path, std::filesystem::path>(
  toml::table                              &table,
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

     table.insert_or_assign(key, std::move(array));
}