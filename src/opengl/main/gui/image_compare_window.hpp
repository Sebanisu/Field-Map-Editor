#ifndef C126DFEB_2E60_4799_8C5D_8ECE89625649
#define C126DFEB_2E60_4799_8C5D_8ECE89625649
#include "filebrowser.hpp"
#include "RangeConsumer.hpp"
#include "Selections.hpp"
#include <filesystem>
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
     ImageCompareWindow(const std::shared_ptr<Selections> &selections);
     void render();

   private:
     static constexpr const std::size_t PopThreshold = 1000U;
     enum struct directory_mode
     {
          input_mode,
          output_mode,
     };
     static const constexpr std::size_t        max_path_length  = { 1024U };
     std::array<char, max_path_length>         m_path1          = {};
     bool                                      m_path1_valid    = { false };
     std::array<char, max_path_length>         m_path2          = {};
     bool                                      m_path2_valid    = false;
     std::weak_ptr<Selections>                 m_selections     = {};
     std::vector<DiffResult>                   m_diff_results   = {};
     std::unordered_set<std::filesystem::path> m_files_in_path2 = {};

     std::vector<std::future<ImageCompareWindow::DiffResult>>
                        m_diff_result_futures    = {};
     bool               m_auto_scroll            = true;
     directory_mode     m_directory_browser_mode = {};
     ImGui::FileBrowser m_directory_browser{
          ImGuiFileBrowserFlags_SelectDirectory
          | ImGuiFileBrowserFlags_CreateNewDir
          | ImGuiFileBrowserFlags_EditPathString
          | ImGuiFileBrowserFlags_SkipItemsCausingError
     };
     RangeConsumer<std::filesystem::recursive_directory_iterator> m_consumer;
     FutureConsumer<std::vector<std::future<ImageCompareWindow::DiffResult>>>
                       m_future_consumer;
     void              diff_results_table();
     void              handle_table_sorting();
     void              open_directory_browser();
     void              button_input_browse();
     void              button_output_browse();
     void              CompareDirectoriesStart();
     void              CompareDirectoriesStep();
     void              CompareDirectoriesStop();
     static DiffResult CompareImage(
       std::filesystem::path fileA,
       std::filesystem::path fileB);
     static std::future<ImageCompareWindow::DiffResult> CompareImageAsync(
       std::filesystem::path fileA,
       std::filesystem::path fileB);
};
}// namespace fme
#endif /* C126DFEB_2E60_4799_8C5D_8ECE89625649 */
