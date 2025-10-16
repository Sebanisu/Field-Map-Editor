#include "image_compare_window.hpp"
#include "format_imgui_text.hpp"
#include "safedir.hpp"
#include <execution>
#include <numeric>
#include <ScopeGuard.hpp>
#include <stb_image.h>

namespace fme
{
/**
 * @brief Safely copies a null-terminated string from one contiguous range to
 * another.
 *
 * This function copies characters from the source range into the destination
 * range, ensuring that no buffer overflows occur. The destination is
 * null-terminated.
 *
 * @tparam Src Source contiguous range type (e.g., std::string,
 * std::array<char>)
 * @tparam Dst Destination contiguous range type (e.g., std::array<char>)
 * @param src The source string-like range.
 * @param dst The destination character buffer.
 * @return true if the copy succeeded and the resulting path exists and is a
 * directory.
 * @return false if the destination was too small or the path is not a valid
 * directory.
 */
template<
  std::ranges::contiguous_range Src,
  std::ranges::contiguous_range Dst>
     requires std::indirectly_copyable<
       std::ranges::iterator_t<Src>,
       std::ranges::iterator_t<Dst>>
static bool safe_copy_string(
  const Src &src,
  Dst       &dst)
{
     const auto src_size = static_cast<std::ranges::range_difference_t<Src>>(
       std::ranges::ssize(src));
     const auto dst_size = std::ranges::ssize(dst);

     if (src_size + 1 > dst_size)
     {
          spdlog::error(
            "safe_copy_string: destination buffer too small ({} < {}).",
            dst_size,
            src_size + 1);
          return false;
     }

     std::ranges::copy_n(
       std::ranges::begin(src), src_size, std::ranges::data(dst));
     dst[static_cast<size_t>(src_size)] = '\0';

     const auto tmp                     = safedir(std::ranges::data(dst));
     return tmp.is_dir() && tmp.is_exists();
}

ImageCompareWindow::ImageCompareWindow(
  const std::shared_ptr<Selections> &selections)
  : m_selections(selections)
{

     m_path1_valid = safe_copy_string(
       selections->get<ConfigKey::ImageCompareWindowPath1>().string(), m_path1);
     m_path2_valid = safe_copy_string(
       selections->get<ConfigKey::ImageCompareWindowPath2>().string(), m_path2);
}

void ImageCompareWindow::render()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     bool show_window = false;
     try
     {
          show_window = selections->get<ConfigKey::DisplayImageCompareWindow>();
     }
     catch (const std::exception &e)
     {
          spdlog::error(
            "Exception getting DisplayImageCompareWindow: {}", e.what());
          return;
     }
     const auto pop_show_window = glengine::ScopeGuard(
       [&show_window, selections]()
       {
            if (
              selections->get<ConfigKey::DisplayImageCompareWindow>()
              != show_window)
            {
                 selections->get<ConfigKey::DisplayImageCompareWindow>()
                   = show_window;
                 selections->update<ConfigKey::DisplayImageCompareWindow>();
            }
       });


     if (!ImGui::Begin("Image Comparison Tool", &show_window))
     {
          ImGui::End();
          return;
     }
     const auto pop_end = glengine::ScopeGuard([]() { ImGui::End(); });
     ImGui::BeginDisabled(!m_consumer.done());
     if (ImGui::InputText("Path A", m_path1.data(), m_path1.size()))
     {
          const auto tmp = safedir(m_path1.data());
          m_path1_valid  = tmp.is_dir() && tmp.is_exists();
          if (m_path1_valid)
          {
               selections->get<ConfigKey::ImageCompareWindowPath1>()
                 = m_path1.data();
               selections->update<ConfigKey::ImageCompareWindowPath1>();
          }
     }
     if (ImGui::InputText("Path B", m_path2.data(), m_path2.size()))
     {
          const auto tmp = safedir(m_path2.data());
          m_path2_valid  = tmp.is_dir() && tmp.is_exists();
          if (m_path2_valid)
          {
               selections->get<ConfigKey::ImageCompareWindowPath2>()
                 = m_path2.data();
               selections->update<ConfigKey::ImageCompareWindowPath2>();
          }
     }
     if (ImGui::Button("Start Compare"))
     {
          CompareDirectoriesStart();
     }
     ImGui::SameLine();
     ImGui::EndDisabled();
     ImGui::BeginDisabled(m_consumer.done());
     if (ImGui::Button("Stop Compare"))
     {
          CompareDirectoriesStop();
     }
     ImGui::EndDisabled();
     ImGui::Separator();
     if (!m_diff_results.empty())
     {
          if (ImGui::CollapsingHeader(
                "Results", ImGuiTreeNodeFlags_DefaultOpen))
          {
               // Begin a child window with a fixed height and scrollbars
               // Adjust height as needed to fit your layout
               if (ImGui::BeginChild(
                     "ResultsChild", ImVec2(0, 0), true,
                     ImGuiWindowFlags_HorizontalScrollbar))
               {
                    if (ImGui::BeginTable(
                          "ResultsTable", 4,
                          ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
                            | ImGuiTableFlags_ScrollY))
                    {
                         // Set up columns
                         ImGui::TableSetupColumn(
                           "File Path", ImGuiTableColumnFlags_WidthStretch);
                         ImGui::TableSetupColumn(
                           "D Pixels", ImGuiTableColumnFlags_WidthFixed);
                         ImGui::TableSetupColumn(
                           "T Pixels", ImGuiTableColumnFlags_WidthFixed);
                         ImGui::TableSetupColumn(
                           "%", ImGuiTableColumnFlags_WidthFixed);
                         ImGui::TableHeadersRow();

                         for (const auto
                                &[path1, path2, total_pixels1, total_pixels2,
                                  differing_pixels, difference_percentage] :
                              m_diff_results)
                         {
                              // First row: path1
                              ImGui::TableNextRow();
                              ImGui::TableNextColumn();// File Path
                              format_imgui_text("{}", path1);
                              ImGui::TableNextColumn();// Difference
                              format_imgui_text("{}", differing_pixels);
                              ImGui::TableNextColumn();// Difference
                              format_imgui_text("{}", total_pixels1);
                              ImGui::TableNextColumn();// Difference
                              format_imgui_text(
                                "{:0.2f}", difference_percentage);

                              // Second row: path2
                              ImGui::TableNextRow();
                              ImGui::TableNextColumn();// File Path
                              format_imgui_text("{}", path2);
                              ImGui::TableNextColumn();// Difference
                              format_imgui_text("{}", differing_pixels);
                              ImGui::TableNextColumn();// Difference
                              format_imgui_text("{}", total_pixels2);
                              ImGui::TableNextColumn();// Difference
                              format_imgui_text(
                                "{:0.2f}", difference_percentage);
                              if (m_auto_scroll && !m_consumer.done())
                              {
                                   ImGui::SetScrollHereY(1.0f);
                              }
                         }

                         ImGui::EndTable();
                    }
               }
               ImGui::EndChild();
          }
     }
     CompareDirectoriesStep();
}

void ImageCompareWindow::CompareDirectoriesStart()
{
     if (!m_path1_valid || !m_path2_valid)
     {
          spdlog::error("Invalid paths provided for comparison.");
          return;
     }
     m_diff_results.clear();
     m_consumer = RangeConsumer<std::filesystem::recursive_directory_iterator>(
       std::filesystem::recursive_directory_iterator(m_path1.data()));
}

void ImageCompareWindow::CompareDirectoriesStep()
{
     if (m_consumer.done())
          return;

     auto &entryA = *m_consumer;

     if (entryA.is_regular_file())
     {
          const auto relative_path = std::filesystem::relative(
            entryA.path(), std::filesystem::path(m_path1.data()));
          const auto pathB
            = std::filesystem::path(m_path2.data()) / relative_path;
          if (
            std::filesystem::exists(pathB)
            && std::filesystem::is_regular_file(pathB))
          {
               auto diff = CompareImage(entryA.path(), pathB);
               if (diff.differing_pixels > 0)
                    m_diff_results.push_back(std::move(diff));
          }
     }
     ++m_consumer;
     if (m_consumer.done())
     {
          spdlog::info("Directory comparison completed.");
          spdlog::info("Found {} differing files.", m_diff_results.size());
     }
}

void ImageCompareWindow::CompareDirectoriesStop()
{
     m_consumer.stop();
}

struct StbiDeleter
{
     void operator()(std::uint8_t *img) const noexcept
     {
          if (img)
               stbi_image_free(img);
     }
};
ImageCompareWindow::DiffResult ImageCompareWindow::CompareImage(
  std::filesystem::path fileA,
  std::filesystem::path fileB)
{
     DiffResult result{ .path1 = std::move(fileA), .path2 = std::move(fileB) };
     auto
       &[path1, path2, total_pixels1, total_pixels2, differing_pixels,
         difference_percentage]
       = result;
     int width1 = 0, height1 = 0, channels1 = 0;
     int width2 = 0, height2 = 0, channels2 = 0;

     std::unique_ptr<std::uint8_t[], StbiDeleter> img1(
       stbi_load(path1.string().c_str(), &width1, &height1, &channels1, 4));
     std::unique_ptr<std::uint8_t[], StbiDeleter> img2(
       stbi_load(path2.string().c_str(), &width2, &height2, &channels2, 4));
     total_pixels1
       = static_cast<std::size_t>(width1) * static_cast<std::size_t>(height1);
     total_pixels2
       = static_cast<std::size_t>(width2) * static_cast<std::size_t>(height2);
     if (
       !img1 || !img2 || width1 != width2 || height1 != height2
       || channels1 != channels2)
     {
          differing_pixels      = (std::max)(total_pixels1, total_pixels2);
          difference_percentage = 100.0;
          return result;// Different dimensions or channels, consider
                        // completely different images
     }

     auto indices = std::views::iota(
       size_t{ 0 }, (std::min)(total_pixels1, total_pixels2));
     static const constexpr int error = 0;
     differing_pixels                 = std::transform_reduce(
       std::execution::par_unseq, indices.begin(), indices.end(), size_t{ 0 },
       std::plus{},
       [&](const size_t idx)
       {
            const size_t i  = idx * 4;
            const int    dr = std::abs(img1[i] - img2[i]);
            const int    dg = std::abs(img1[i + 1] - img2[i + 1]);
            const int    db = std::abs(img1[i + 2] - img2[i + 2]);
            const int    da = std::abs(img1[i + 3] - img2[i + 3]);
            if (dr > error || dg > error || db > error || da > error)
            {
                 return 1;
            }
            return 0;
       });
     difference_percentage
       = static_cast<double>(differing_pixels)
         / static_cast<double>((std::max)(total_pixels1, total_pixels2));
     return result;
}

}// namespace fme
