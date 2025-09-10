#ifndef D32D9952_25B7_4925_A5EB_B8672C65F70C
#define D32D9952_25B7_4925_A5EB_B8672C65F70C
#include "gui/key_value_data.hpp"
#include "gui/Selections.hpp"
#include "PupuID.hpp"
#include "unique_values.hpp"
#include <filesystem>
#include <map>
#include <string>
#include <vector>
namespace ff_8
{
struct path_search
{
     using Coo                                                                                                        = open_viii::LangT;
     using OptCoo                                                                                                     = std::optional<Coo>;
     std::shared_ptr<const fme::Selections>                                       selections                          = {};
     OptCoo                                                                       opt_coo                             = {};
     std::string                                                                  field_name                          = {};
     std::string                                                                  filters_deswizzle_value_string      = {};
     std::string                                                                  filters_full_filename_value_string  = {};
     std::string                                                                  filters_swizzle_value_string        = {};
     std::string                                                                  filters_swizzle_as_one_image_string = {};
     std::string                                                                  filters_map_value_string            = {};
     std::vector<ff_8::PupuID>                                                    working_unique_pupu                 = {};
     std::vector<std::string>                                                     current_filenames                   = {};
     std::map<open_viii::graphics::BPPT, unique_values_and_strings<std::uint8_t>> bpp_palette                         = {};
     unique_values_and_strings<std::uint8_t>                                      texture_page_id                     = {};


     [[nodiscard]] std::vector<std::filesystem::path>                             generate_deswizzle_paths(const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path>                             generate_deswizzle_paths(
                                   const std::filesystem::path &path,
                                   const std::string           &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_deswizzle_paths(
       const ff_8::PupuID pupu,
       const std::string &ext = ".png") const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_full_filename_paths(const std::string &) const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_full_filename_paths(
       const std::filesystem::path &,
       const std::string &) const;


     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_paths(const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_paths(
       const std::filesystem::path &path,
       const std::string           &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_paths(
       const std::uint8_t texture_page,
       std::uint8_t       palette,
       const std::string &ext = ".png") const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_paths(
       const std::uint8_t texture_page,
       const std::string &ext = ".png") const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_as_one_image_paths(
       const std::optional<std::uint8_t> &palette,
       const std::string                 &ext) const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_map_paths(const std::string &ext = ".map") const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_map_paths(
       const std::filesystem::path &path,
       const std::string           &ext) const;

     [[nodiscard]] bool has_map_path(
       const std::filesystem::path &filter_path,
       const std::string           &ext                      = ".map",
       const std::string           &secondary_output_pattern = "") const;
     [[nodiscard]] bool has_deswizzle_path(
       const ff_8::PupuID pupu,
       const std::string &ext = ".png") const;
     [[nodiscard]] bool has_deswizzle_path(
       const std::filesystem::path &filter_path,
       const std::string           &ext = ".png") const;

     [[nodiscard]] bool has_full_filename_path() const;
     [[nodiscard]] bool has_full_filename_path(const std::filesystem::path &) const;
     [[nodiscard]] bool has_full_filename_path(
       const std::filesystem::path &,
       const std::string &) const;
     [[nodiscard]] bool has_deswizzle_path(
       const std::filesystem::path &filter_path,
       const ff_8::PupuID           pupu,
       const std::string           &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(
       const std::uint8_t texture_page,
       std::uint8_t       palette,
       const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(
       const std::uint8_t texture_page,
       const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(
       const std::filesystem::path &filter_path,
       const std::string           &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(
       const std::filesystem::path &filter_path,
       const std::uint8_t           texture_page,
       std::uint8_t                 palette,
       const std::string           &ext = ".png") const;
     bool has_swizzle_path(
       const std::filesystem::path &filter_path,
       const std::uint8_t           texture_page,
       const std::string           &ext = ".png") const;


     [[nodiscard]] bool has_swizzle_as_one_image_path(
       const std::filesystem::path &filter_path,
       const std::string           &ext = ".png") const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_paths(
       const std::string                      &filter_path,
       const fme::key_value_data              &cpm,
       const std::vector<const std::string *> &output_patterns = {}) const;

     /**
      * @brief Generates a list of paths for external textures or map output based on the provided data.
      *
      * @param field_root The root directory for field assets.
      * @param copy_data Data used to determine which keys and values to process.
      * @return Vector of generated paths for external textures or map output.
      */
     [[nodiscard]] static std::vector<std::filesystem::path> generate_paths(
       const std::filesystem::path           &field_root,
       fme::key_value_data                    copy_data,
       std::shared_ptr<const fme::Selections> selections);

     /**
      * @brief Generates a bool for paths for external textures or map output based on the provided data.
      *
      * @param field_root The root directory for field assets.
      * @param copy_data Data used to determine which keys and values to process.
      * @return true if found any match for generated paths for textures or map output.
      */
     [[nodiscard]] static bool has_path(
       const std::filesystem::path           &field_root,
       fme::key_value_data                    copy_data,
       std::shared_ptr<const fme::Selections> selections);

     /**
      * @brief Gets a list of base paths for external paths for textures.
      *
      * These paths are determined based on the root and selections provided.
      *
      * @return Vector of filesystem paths to be used for external paths for textures.
      */
     [[nodiscard]] static std::vector<std::filesystem::path> get_paths(
       std::shared_ptr<const fme::Selections> selections,
       std::optional<open_viii::LangT>        coo,
       std::filesystem::path                  root = "{current_path}");

     /**
      * @brief Gets the list of map paths including common fallback locations.
      *
      * Prepends common map-only paths to the base paths from get_paths().
      * Useful for locating custom maps.
      *
      * @return Vector of filesystem paths including map-specific paths.
      */
     [[nodiscard]] static std::vector<std::filesystem::path> get_map_paths(
       std::shared_ptr<const fme::Selections> selections,
       std::optional<open_viii::LangT>        coo,
       std::filesystem::path                  root = "{current_path}");
};
}// namespace ff_8

#endif /* D32D9952_25B7_4925_A5EB_B8672C65F70C */
