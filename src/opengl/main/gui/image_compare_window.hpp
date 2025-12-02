#ifndef C126DFEB_2E60_4799_8C5D_8ECE89625649
#define C126DFEB_2E60_4799_8C5D_8ECE89625649
#include "filebrowser.hpp"
#include "RangeConsumer.hpp"
#include "Selections.hpp"
#include <filesystem>
#include <glengine/Renderable.hpp>
#include <span>
#include <string>
#include <unordered_set>
#include <vector>
namespace fme
{
class ImageCompareWindow
{
   public:
     struct DiffResult
     {
          std::filesystem::path path1                 = {};
          std::filesystem::path path2                 = {};
          std::int64_t          total_pixels1         = {};
          std::int64_t          total_pixels2         = {};
          std::int64_t          differing_pixels      = {};
          double                difference_percentage = {};

          explicit              operator toml::table() const;
     };
     [[nodiscard]]
     static toml::array
       to_toml_array(const std::span<const DiffResult> diffs) noexcept;
     ImageCompareWindow() = default;
     ImageCompareWindow(const std::shared_ptr<Selections> &selections);
     void on_im_gui_update() const;
     void on_im_gui_window_menu() const;

   private:
     enum struct directory_mode
     {
          input_mode,
          output_mode,
     };
     std::weak_ptr<Selections>                 m_selections    = {};
     static constexpr const std::size_t        PopThreshold    = { 1000U };
     static const constexpr std::size_t        max_path_length = { 1024U };
     mutable std::array<char, max_path_length> m_path1         = {};
     mutable bool                              m_path1_valid   = { false };
     mutable std::array<char, max_path_length> m_path2         = {};
     mutable bool                              m_path2_valid   = { false };
     mutable std::vector<DiffResult>           m_diff_results  = {};
     mutable std::unordered_set<std::filesystem::path> m_files_in_path2 = {};

     mutable std::vector<std::future<ImageCompareWindow::DiffResult>>
                                m_diff_result_futures    = {};
     mutable bool               m_auto_scroll            = true;
     mutable directory_mode     m_directory_browser_mode = {};
     mutable ImGui::FileBrowser m_directory_browser{
          ImGuiFileBrowserFlags_SelectDirectory
          | ImGuiFileBrowserFlags_CreateNewDir
          | ImGuiFileBrowserFlags_EditPathString
          | ImGuiFileBrowserFlags_SkipItemsCausingError
     };

     mutable ImGui::FileBrowser m_save_dialog{
          ImGuiFileBrowserFlags_EditPathString
          | ImGuiFileBrowserFlags_CreateNewDir
          | ImGuiFileBrowserFlags_EnterNewFilename
          | ImGuiFileBrowserFlags_SkipItemsCausingError
     };
     mutable RangeConsumer<std::filesystem::recursive_directory_iterator>
       m_consumer;
     mutable FutureConsumer<
       std::vector<std::future<ImageCompareWindow::DiffResult>>>
                                   m_future_consumer;

     mutable std::filesystem::path m_save_directory = []()
     {
          try
          {
               return std::filesystem::current_path();
          }
          catch (std::filesystem::filesystem_error &e)
          {
               spdlog::error(
                 "{}:{} - Failed to get current path: {}", __FILE__, __LINE__,
                 e.what());
               return std::filesystem::path{};
          }
     }();
     void              export_button() const;
     void              diff_results_table() const;
     void              handle_table_sorting() const;
     void              display_save_browser() const;
     void              open_directory_browser() const;
     void              button_input_browse() const;
     void              button_output_browse() const;
     void              CompareDirectoriesStart() const;
     void              CompareDirectoriesStep() const;
     void              CompareDirectoriesStop() const;
     static DiffResult CompareImage(
       std::filesystem::path fileA,
       std::filesystem::path fileB);
     static std::future<ImageCompareWindow::DiffResult> CompareImageAsync(
       std::filesystem::path fileA,
       std::filesystem::path fileB);
};
}// namespace fme
static_assert(glengine::Renderable<fme::ImageCompareWindow>);
#endif /* C126DFEB_2E60_4799_8C5D_8ECE89625649 */
