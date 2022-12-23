#ifndef FIELD_MAP_EDITOR_GUI_BATCH_HPP
#define FIELD_MAP_EDITOR_GUI_BATCH_HPP
#include "archives_group.hpp"
#include "filebrowser.hpp"
#include "open_viii/paths/Paths.hpp"
#include "scope_guard.hpp"
#include <cppcoro/generator.hpp>
#include <cppcoro/task.hpp>
namespace fme
{
struct [[nodiscard]] gui_batch
{
  enum class batch_operation_source
  {
    None,
    fields_archive,
    swizzles,
    deswizzles,
  };
  enum class batch_operation_transformation
  {
    None,
    deswizzle [[maybe_unused]] = 1U,
    swizzle [[maybe_unused]]   = 1U << 1U,
    compact_rows               = 1U << 2U,
    compact_all                = 1U << 3U,
    flatten_bpp                = 1U << 4U,
    flatten_palette            = 1U << 5U,
  };
  gui_batch(archives_group ag)
    : m_archive_paths(open_viii::Paths::get())
    , m_archive_group(std::move(ag))
  // m_archive_paths.empty() ? archives_group{}
  //                         : archives_group{ {}, m_archive_paths.front() })
  {
  }
  void operator()(int *id);

private:
  static cppcoro::generator<std::tuple<int, std::string>>
    get_field_id_and_name(std::vector<std::string> maps);
  static cppcoro::generator<std::shared_ptr<open_viii::archive::FIFLFS<false>>>
    get_field(::archives_group archives_group);
  static cppcoro::generator<open_viii::LangT> get_field_coos(
    const std::shared_ptr<open_viii::archive::FIFLFS<false>> field);
  static cppcoro::generator<::map_sprite> get_map_sprite(
    const std::shared_ptr<open_viii::archive::FIFLFS<false>> &field,
    const ::filters                                          &in_filters);
  static void
    popup_batch_common_filter_start(::filters &filters, std::string &base_name);
  static void popup_batch_common_filter_start(
    ::filter<std::filesystem::path> &filter,
    std::string_view                 base_name);
  [[nodiscard]] cppcoro::generator<bool> source();
  cppcoro::generator<bool>               save_output(const map_sprite ms) const;
  filters                                get_filters();
  void               compact_and_flatten(map_sprite &ms) const;
  [[nodiscard]] bool ask() const;
  [[nodiscard]] bool ask_post_operation() const;
  [[nodiscard]] bool ask_output() const;
  [[nodiscard]] bool ask_archive_path() const;
  [[nodiscard]] bool ask_source() const;
  [[nodiscard]] bool ask_transformation() const;
  [[nodiscard]] static ImGui::FileBrowser create_directory_browser(
    std::string              title,
    std::vector<std::string> filetypes = {});
  [[nodiscard]] static std::optional<std::filesystem::path>
    ask_for_path(ImGui::FileBrowser &file_browser);

  mutable std::vector<std::string>             m_archive_paths       = {};
  mutable archives_group                       m_archive_group       = {};
  mutable batch_operation_source               m_source_type         = {};
  mutable std::optional<std::filesystem::path> m_source_path         = {};
  mutable batch_operation_transformation       m_transformation_type = {};
  mutable std::optional<std::filesystem::path> m_output_path         = {};
  mutable int                                 *m_id                  = {};
  mutable bool                                 m_embed_maps          = {};
  mutable bool                                 m_reload_after        = { true };
  mutable std::optional<std::filesystem::path> m_embed_path          = {};
  mutable ImGui::FileBrowser                   m_archive_browser =
    create_directory_browser("Select Directory");
  mutable ImGui::FileBrowser m_source_browser =
    create_directory_browser("Select Directory");
  mutable ImGui::FileBrowser m_output_browser =
    create_directory_browser("Select Directory");
  [[nodiscard]] auto push_pop() const
  {
    ImGui::PushID(++*m_id);
    return scope_guard{ &ImGui::PopID };
  }
};
}// namespace fme
#endif// FIELD_MAP_EDITOR_GUI_BATCH_HPP
