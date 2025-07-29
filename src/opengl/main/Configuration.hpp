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

     /**
      * @brief Loads an array from a TOML configuration table into a vector.
      *
      * This function reads an array-like value from the given TOML table and
      * converts it into a `std::vector<OutputT>`, using either specialized logic
      * or a general conversion pathway.
      *
      * @tparam OutputT The final type to be stored in the output vector.
      * @tparam InputT The intermediate type to parse from the TOML values. Defaults to `OutputT`.
      *
      * @param table The TOML table to read from.
      * @param key The TOML key corresponding to the array to load.
      * @param output The output vector that will be filled with converted values.
      *               This vector is cleared before writing.
      *
      * @return true if a value was found and successfully loaded; false otherwise.
      *
      * @note For `bool`, both TOML arrays of booleans and strings of `'0'`/`'1'` are supported.
      * @note For `std::filesystem::path`, values are expected as UTF-8 strings and backslashes are converted to slashes.
      * @note For `std::string`, a standard TOML array of strings is parsed.
      * @note For all other types, values are parsed as `InputT`, and optionally cast to `OutputT`.
      * @note This function requires that `InputT` is statically castable to `OutputT`.
      */
     template<typename OutputT, typename InputT = OutputT>
          requires requires { static_cast<OutputT>(std::declval<InputT>()); }
     static bool load_array(const toml::table &table, const std::string_view key, std::vector<OutputT> &output)
     {
          if constexpr (std::same_as<InputT, OutputT> && std::same_as<InputT, bool>)
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
          if constexpr (std::same_as<InputT, OutputT> && std::same_as<InputT, std::filesystem::path>)
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
          if constexpr (std::same_as<InputT, OutputT> && std::same_as<InputT, std::string>)
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
          else
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
     }

     /**
      * @brief Updates the TOML configuration with a typed array.
      *
      * Inserts or replaces the value at the given key in the TOML table
      * with a representation of the input vector. Specialized behaviors
      * are provided for `bool` and `std::filesystem::path`, while the
      * default implementation writes a TOML array using either direct or
      * casted conversion from `InputT` to `OutputT`.
      *
      * @tparam InputT The type of each element in the input vector.
      * @tparam OutputT The type to be written into the TOML array. Defaults to `InputT`.
      *
      * @param table The TOML table where the key/value pair will be inserted or updated.
      * @param key The TOML key under which to store the array.
      * @param input The vector of input values to convert and write to the configuration.
      *
      * @note For `bool`, the values are encoded into a compact string of '0' and '1'.
      * @note For `std::filesystem::path`, paths are stored as forward-slashed UTF-8 strings.
      * @note For other types, values are either directly inserted or cast to `OutputT`.
      */
     template<typename InputT, typename OutputT = InputT>
     static void update_array(toml::table &table, const std::string_view key, const std::vector<InputT> &input)
     {
          if constexpr (std::same_as<InputT, OutputT> && std::same_as<InputT, bool>)
          {
               std::string encoded =
                 input | std::ranges::views::transform([](const auto &b) { return b ? '1' : '0'; }) | std::ranges::to<std::string>();
               table.insert_or_assign(key, std::move(encoded));
          }
          if constexpr (std::same_as<InputT, OutputT> && std::same_as<InputT, std::filesystem::path>)
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
          else
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
