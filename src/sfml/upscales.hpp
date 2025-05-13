//
// Created by pcvii on 9/29/2021.
//

#ifndef FIELD_MAP_EDITOR_UPSCALES_HPP
#define FIELD_MAP_EDITOR_UPSCALES_HPP
#include "gui/key_value_data.hpp"
#include "gui/selections.hpp"
#include "open_viii/strings/LangCommon.hpp"
#include "open_viii/strings/LangT.hpp"
#include "safedir.hpp"
#include "tl/string.hpp"
#include <array>
#include <filesystem>
#include <fmt/core.h>
#include <string>
#include <string_view>
#include <utility>
/**
 * @brief Handles upscale path generation and related utilities.
 */
struct upscales
{
   private:
     /**
      * @brief Optional language code.
      *
      * Used for remaster versions (which use single-character codes like 'x')
      * or other versions which use a three-character COO (Country of Origin) code.
      */
     std::optional<open_viii::LangT> m_coo{};

     /**
      * @brief Root directory for upscaling operations.
      *
      * Defaults to "{current_path}" which should be replaced during processing.
      */
     std::string                     m_root       = "{current_path}";

     /**
      * @brief Reference to user selections.
      *
      * Used to influence or filter which paths are generated or processed.
      */
     std::weak_ptr<fme::Selections>  m_selections = {};

   public:
     /**
      * @brief Constructor with selections only.
      *
      * @param selections Weak pointer to the selections used for generating paths.
      */
     upscales(std::weak_ptr<fme::Selections> selections);

     /**
      * @brief Constructor with full parameter set.
      *
      * @param root The root directory for the upscaling process.
      * @param coo Optional language or country code.
      * @param selections Weak pointer to the selections used for generating paths.
      */
     upscales(std::string root, std::optional<open_viii::LangT> coo, std::weak_ptr<fme::Selections> selections);

     /**
      * @brief Generates a list of paths for upscale output based on the provided data.
      *
      * @param field_root The root directory for field assets.
      * @param copy_data Data used to determine which keys and values to process.
      * @return Vector of generated paths for upscale output.
      */
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_upscale_paths(const std::filesystem::path &field_root, fme::key_value_data copy_data) const;

     /**
      * @brief Gets a list of base paths for upscaling.
      *
      * These paths are determined based on the root and selections provided.
      *
      * @return Vector of filesystem paths to be used for upscaling.
      */
     [[nodiscard]] std::vector<std::filesystem::path> get_paths() const;

     /**
      * @brief Gets the list of map paths including common fallback locations.
      *
      * Prepends common map-only paths to the base paths from get_paths().
      * Useful for locating custom maps.
      *
      * @return Vector of filesystem paths including map-specific paths.
      */
     [[nodiscard]] std::vector<std::filesystem::path> get_map_paths() const;
};

#endif// FIELD_MAP_EDITOR_UPSCALES_HPP
