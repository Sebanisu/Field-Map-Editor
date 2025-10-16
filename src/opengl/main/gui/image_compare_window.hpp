#ifndef C126DFEB_2E60_4799_8C5D_8ECE89625649
#define C126DFEB_2E60_4799_8C5D_8ECE89625649
#include "filebrowser.hpp"
#include "RangeConsumer.hpp"
#include "Selections.hpp"
#include <filesystem>
#include <string>
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
          std::size_t           total_pixels1         = {};
          std::size_t           total_pixels2         = {};
          std::size_t           differing_pixels      = {};
          double                difference_percentage = {};
     };
     ImageCompareWindow(const std::shared_ptr<Selections> &selections);
     void render();

   private:
     static const constexpr std::size_t max_path_length = 1024U;
     std::array<char, max_path_length>  m_path1{};
     bool                               m_path1_valid = false;
     std::array<char, max_path_length>  m_path2{};
     bool                               m_path2_valid = false;
     std::weak_ptr<Selections>          m_selections;
     std::vector<DiffResult>            m_diff_results{};
     bool                               m_auto_scroll = true;
     ImGui::FileBrowser                 m_directory_browser;
     RangeConsumer<std::filesystem::recursive_directory_iterator> m_consumer;
     void       diff_results_table();
     void       handle_table_sorting();
     void       CompareDirectoriesStart();
     void       CompareDirectoriesStep();
     void       CompareDirectoriesStop();
     DiffResult CompareImage(
       std::filesystem::path fileA,
       std::filesystem::path fileB);
};
}// namespace fme
#endif /* C126DFEB_2E60_4799_8C5D_8ECE89625649 */
