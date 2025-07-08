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
     std::string                                                                  filters_swizzle_value_string        = {};
     std::string                                                                  filters_swizzle_as_one_image_string = {};
     std::string                                                                  filters_deswizzle_map_value_string  = {};
     std::string                                                                  filters_swizzle_map_value_string    = {};
     std::string                                                                  filters_swizzle_as_one_image_map_string = {};
     std::vector<ff_8::PupuID>                                                    working_unique_pupu                     = {};
     std::map<open_viii::graphics::BPPT, unique_values_and_strings<std::uint8_t>> bpp_palette                             = {};
     unique_values_and_strings<std::uint8_t>                                      texture_page_id                         = {};

     [[nodiscard]] std::vector<std::filesystem::path>                             generate_deswizzle_paths(const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path>                             generate_swizzle_paths(const std::string &ext) const;

     [[nodiscard]] std::vector<std::filesystem::path>
       generate_deswizzle_paths(const std::filesystem::path &path, const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_swizzle_paths(const std::filesystem::path &path, const std::string &ext) const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_deswizzle_map_paths(const std::string &ext = ".map") const;
     [[nodiscard]] std::vector<std::filesystem::path> generate_swizzle_map_paths(const std::string &ext = ".map") const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_deswizzle_map_paths(const std::filesystem::path &path, const std::string &ext) const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_swizzle_map_paths(const std::filesystem::path &path, const std::string &ext) const;

     [[nodiscard]] std::vector<std::filesystem::path>
       generate_deswizzle_paths(const ff_8::PupuID pupu, const std::string &ext = ".png") const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_swizzle_paths(const std::uint8_t texture_page, std::uint8_t palette, const std::string &ext = ".png") const;
     [[nodiscard]] std::vector<std::filesystem::path>
       generate_swizzle_paths(const std::uint8_t texture_page, const std::string &ext = ".png") const;

     [[nodiscard]] std::vector<std::filesystem::path>
                        generate_swizzle_as_one_image_paths(const std::optional<std::uint8_t> &palette, const std::string &ext) const;


     [[nodiscard]] bool has_map_path(
       const std::filesystem::path &filter_path,
       const std::string           &ext                      = ".map",
       const std::string           &secondary_output_pattern = "") const;
     [[nodiscard]] bool has_deswizzle_path(const ff_8::PupuID pupu, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(const std::uint8_t texture_page, std::uint8_t palette, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(const std::uint8_t texture_page, const std::string &ext = ".png") const;

     [[nodiscard]] bool has_deswizzle_path(const std::filesystem::path &filter_path, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(const std::filesystem::path &filter_path, const std::string &ext = ".png") const;
     [[nodiscard]] bool
       has_deswizzle_path(const std::filesystem::path &filter_path, const ff_8::PupuID pupu, const std::string &ext = ".png") const;
     [[nodiscard]] bool has_swizzle_path(
       const std::filesystem::path &filter_path,
       const std::uint8_t           texture_page,
       std::uint8_t                 palette,
       const std::string           &ext = ".png") const;
     bool
       has_swizzle_path(const std::filesystem::path &filter_path, const std::uint8_t texture_page, const std::string &ext = ".png") const;

     [[nodiscard]] std::vector<std::filesystem::path> generate_paths(
       const std::string         &filter_path,
       const fme::key_value_data &cpm,
       const std::string         &output_pattern           = "",
       const std::string         &secondary_output_pattern = "") const;

     /**
      * @brief Generates a list of paths for upscale output based on the provided data.
      *
      * @param field_root The root directory for field assets.
      * @param copy_data Data used to determine which keys and values to process.
      * @return Vector of generated paths for upscale output.
      */
     [[nodiscard]] static std::vector<std::filesystem::path> generate_upscale_paths(
       const std::filesystem::path           &field_root,
       fme::key_value_data                    copy_data,
       std::shared_ptr<const fme::Selections> selections);


     [[nodiscard]] static bool has_upscale_path(
       const std::filesystem::path           &field_root,
       fme::key_value_data                    copy_data,
       std::shared_ptr<const fme::Selections> selections);

     /**
      * @brief Gets a list of base paths for upscaling.
      *
      * These paths are determined based on the root and selections provided.
      *
      * @return Vector of filesystem paths to be used for upscaling.
      */
     [[nodiscard]] static std::vector<std::filesystem::path> get_paths(
       std::shared_ptr<const fme::Selections> selections,
       std::optional<open_viii::LangT>        coo,
       std::string                            root = "{current_path}");

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
       std::string                            root = "{current_path}");
};
}// namespace ff_8

#endif /* D32D9952_25B7_4925_A5EB_B8672C65F70C */
