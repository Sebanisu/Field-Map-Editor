#include "image_compare_window.hpp"
#include "format_imgui_text.hpp"
#include "safedir.hpp"
#include "tool_tip.hpp"
#include <execution>
#include <IconsFontAwesome6.h>
#include <numeric>
#include <open_file_explorer.hpp>
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
ImageCompareWindow::DiffResult::operator toml::table() const
{
     return toml::table{ { "path1", path1.string() },
                         { "path2", path2.string() },
                         { "total_pixels1", total_pixels1 },
                         { "total_pixels2", total_pixels2 },
                         { "differing_pixels", differing_pixels },
                         { "difference_percentage", difference_percentage } };
}
toml::array ImageCompareWindow::to_toml_array(
  const std::span<const DiffResult> diffs) noexcept
{
     toml::array arr;
     arr.reserve(diffs.size());
     for (const auto &diff : diffs)
          arr.push_back(static_cast<toml::table>(diff));
     return arr;
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


void ImageCompareWindow::button_input_browse()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Path A");
     m_directory_browser.SetPwd(m_path1.data());
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_directory_browser_mode = directory_mode::input_mode;
};

void ImageCompareWindow::button_output_browse()
{
     m_directory_browser.Open();
     m_directory_browser.SetTitle("Path B");
     m_directory_browser.SetPwd(m_path2.data());
     m_directory_browser.SetTypeFilters({ ".map", ".png" });
     m_directory_browser_mode = directory_mode::output_mode;
};

void ImageCompareWindow::open_directory_browser()
{
     m_directory_browser.Display();
     if (!m_directory_browser.HasSelected())
     {
          return;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     const auto clear_browser = glengine::ScopeGuard(
       [this]() { m_directory_browser.ClearSelected(); });
     const std::string &selected_path
       = m_directory_browser.GetDirectory().string();
     // todo check if the directory is valid.
     // const auto         tmp           = safedir(selected_path);
     switch (m_directory_browser_mode)
     {
          case directory_mode::input_mode:
          {
               m_path1_valid = safe_copy_string(selected_path, m_path1);
               selections->get<ConfigKey::ImageCompareWindowPath1>()
                 = m_path1.data();
               selections->update<ConfigKey::ImageCompareWindowPath1>();
          }
          break;
               break;
          case directory_mode::output_mode:
          {
               m_path2_valid = safe_copy_string(selected_path, m_path2);
               selections->get<ConfigKey::ImageCompareWindowPath2>()
                 = m_path2.data();
               selections->update<ConfigKey::ImageCompareWindowPath2>();
          }
          break;
     }
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

     open_directory_browser();
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


     const auto pop_end = glengine::ScopeGuard(&ImGui::End);
     if (!ImGui::Begin("Image Comparison Tool", &show_window))
     {
          return;
     }

     const ImGuiStyle &style        = ImGui::GetStyle();
     const float       spacing      = style.ItemInnerSpacing.x;
     const float       button_size  = ImGui::GetFrameHeight();
     const float       button_width = button_size * 3.0F;
     ImGui::BeginDisabled(!m_consumer.done() || !m_future_consumer.done());
     {
          const auto  popid = PushPopID();
          const float width = ImGui::CalcItemWidth();
          ImGui::PushItemWidth(width - (spacing * 2.0F) - button_width * 2.0F);
          const auto pop_item_width
            = glengine::ScopeGuard(&ImGui::PopItemWidth);
          if (!m_path1_valid)
          {
               ImGui::PushStyleColor(
                 ImGuiCol_FrameBg,
                 static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
               ImGui::PushStyleColor(
                 ImGuiCol_FrameBgHovered,
                 static_cast<ImVec4>(
                   ImColor::HSV(0.0F, 0.8F, 0.8F)));// lighter red on hover
               ImGui::PushStyleColor(
                 ImGuiCol_FrameBgActive,
                 static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
          }
          const auto pop_color = glengine::ScopeGuard(
            [valid = m_path1_valid]()
            {
                 if (!valid)
                 {
                      ImGui::PopStyleColor(3);
                 }
            });
          if (ImGui::InputText("##Path A", m_path1.data(), m_path1.size()))
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

          ImGui::SameLine(0, spacing);
          if (ImGui::Button(
                gui_labels::browse.data(), ImVec2{ button_width, button_size }))
          {
               button_input_browse();
          }
          ImGui::SameLine(0, spacing);
          if (ImGui::Button(
                gui_labels::explore.data(),
                ImVec2{ button_width, button_size }))
          {
               open_directory(m_path1.data());
          }
          tool_tip(gui_labels::explore_tooltip);
          ImGui::SameLine(0, spacing);

          format_imgui_wrapped_text("{}", "Path A");
     }
     {
          const auto  popid = PushPopID();
          const float width = ImGui::CalcItemWidth();
          ImGui::PushItemWidth(width - (spacing * 2.0F) - button_width * 2.0F);
          const auto pop_item_width
            = glengine::ScopeGuard(&ImGui::PopItemWidth);
          if (!m_path2_valid)
          {
               ImGui::PushStyleColor(
                 ImGuiCol_FrameBg,
                 static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
               ImGui::PushStyleColor(
                 ImGuiCol_FrameBgHovered,
                 static_cast<ImVec4>(
                   ImColor::HSV(0.0F, 0.8F, 0.8F)));// lighter red on hover
               ImGui::PushStyleColor(
                 ImGuiCol_FrameBgActive,
                 static_cast<ImVec4>(ImColor::HSV(0.0F, 0.5F, 0.5F)));
          }
          const auto pop_color = glengine::ScopeGuard(
            [valid = m_path2_valid]()
            {
                 if (!valid)
                 {
                      ImGui::PopStyleColor(3);
                 }
            });
          if (ImGui::InputText("##Path B", m_path2.data(), m_path2.size()))
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

          ImGui::SameLine(0, spacing);
          if (ImGui::Button(
                gui_labels::browse.data(), ImVec2{ button_width, button_size }))
          {
               button_output_browse();
          }
          ImGui::SameLine(0, spacing);
          if (ImGui::Button(
                gui_labels::explore.data(),
                ImVec2{ button_width, button_size }))
          {
               open_directory(m_path2.data());
          }
          tool_tip(gui_labels::explore_tooltip);
          ImGui::SameLine(0, spacing);

          format_imgui_wrapped_text("{}", "Path B");
     }
     if (ImGui::Button("Start Compare"))
     {
          CompareDirectoriesStart();
     }
     ImGui::SameLine();
     ImGui::EndDisabled();
     ImGui::BeginDisabled(m_consumer.done() && m_future_consumer.done());
     if (ImGui::Button("Stop Compare"))
     {
          CompareDirectoriesStop();
     }
     ImGui::EndDisabled();
     ImGui::SameLine();
     if (ImGui::Checkbox("Autoscroll", &m_auto_scroll))
     {
          // noop;
     }
     ImGui::BeginDisabled(
       !m_consumer.done() || !m_future_consumer.done()
       || m_diff_results.empty());
     export_button();
     ImGui::EndDisabled();
     ImGui::Separator();
     if (!m_diff_result_futures.empty())
     {
          format_imgui_text(
            "Async queued up {} operations...", m_diff_result_futures.size());
     }

     if (!m_diff_results.empty())
     {
          format_imgui_text("{} Different Results.", m_diff_results.size());
     }
     diff_results_table();
     CompareDirectoriesStep();
     if (
       (m_consumer.done() && !m_diff_result_futures.empty())
       || m_diff_result_futures.size() >= PopThreshold)
     {
          m_future_consumer += std::move(m_diff_result_futures);
          m_diff_result_futures.clear();
     }
     if (m_future_consumer.done())
     {
          return;
     }
     m_future_consumer.consume_one_with_callback(
       [this](DiffResult result)
       {
            if (result.differing_pixels > 0)
                 m_diff_results.push_back(std::move(result));
       });
}

void ImageCompareWindow::diff_results_table()
{
     if (m_diff_results.empty())
     {
          return;
     }
     if (!ImGui::CollapsingHeader("Results", ImGuiTreeNodeFlags_DefaultOpen))
     {
          return;
     }
     // Begin a child window with a fixed height and scrollbars
     // Adjust height as needed to fit your layout

     const auto end_child = glengine::ScopeGuard(&ImGui::EndChild);
     if (!ImGui::BeginChild(
           "ResultsChild", ImVec2(0, 0), true,
           ImGuiWindowFlags_HorizontalScrollbar))
     {
          return;
     }

     if (!ImGui::BeginTable(
           "ResultsTable", 5,
           ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
             | ImGuiTableFlags_ScrollY | ImGuiTableFlags_Sortable))
     {
          return;
     }
     const auto end_table = glengine::ScopeGuard(&ImGui::EndTable);

     // Set up columns
     ImGui::TableSetupColumn("File Path", ImGuiTableColumnFlags_WidthStretch);
     ImGui::TableSetupColumn("D Pixels", ImGuiTableColumnFlags_WidthFixed);
     ImGui::TableSetupColumn("T Pixels", ImGuiTableColumnFlags_WidthFixed);
     ImGui::TableSetupColumn("%##Header", ImGuiTableColumnFlags_WidthFixed);
     ImGui::TableSetupColumn(
       ICON_FA_COPY "##Header",
       ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoSort);
     ImGui::TableHeadersRow();
     handle_table_sorting();
     ImGuiListClipper clipper{};
     clipper.Begin(
       static_cast<int>(m_diff_results.size()),
       ImGui::GetTextLineHeightWithSpacing() * 2U);
     while (clipper.Step())
     {
          int start_idx = clipper.DisplayStart;
          int end_idx
            = (clipper.DisplayEnd + 1);// include partially visible row
          int count = end_idx - start_idx;
          for (const auto
                 &[path1, path2, total_pixels1, total_pixels2, differing_pixels,
                   difference_percentage] :
               m_diff_results | std::views::drop(start_idx)
                 | std::views::take(count))
          {

               const auto crop_path_to_fit
                 = [](
                     const std::filesystem::path &path,
                     float                        total_width) -> std::string
               {
                    std::string parent          = path.parent_path().string();
                    std::string filename        = path.filename().string();
                    float       separator_width = ImGui::CalcTextSize("/").x;
                    float       filename_width
                      = ImGui::CalcTextSize(filename.c_str()).x;
                    float available_for_parent
                      = total_width - filename_width - separator_width
                        - ImGui::GetStyle().ItemSpacing.x * 2;

                    if (available_for_parent <= 0.0f)
                         return "...";

                    const char *ellipsis = "...";
                    float ellipsis_width = ImGui::CalcTextSize(ellipsis).x;

                    // Trim from the RIGHT (preserve the beginning of the path)
                    while (!parent.empty()
                           && ImGui::CalcTextSize(parent.c_str()).x
                                  + ellipsis_width
                                > available_for_parent)
                    {
                         parent.pop_back();
                    }

                    if (parent.size() < path.parent_path().string().size())
                         parent += ellipsis;

                    return parent;
               };


               // First row: path1
               ImGui::TableNextRow();
               ImGui::TableNextColumn();// File Path
               format_imgui_text(
                 "{}{}",
                 crop_path_to_fit(path1, ImGui::GetContentRegionAvail().x),
                 static_cast<char>(std::filesystem::path::preferred_separator));
               ImGui::SameLine(0, 0);
               ImGui::PushStyleColor(
                 ImGuiCol_Text, IM_COL32(255, 192, 64, 255));// Orange
               format_imgui_text("{}", path1.filename());
               ImGui::PopStyleColor();
               ImGui::TableNextColumn();// Difference
               format_imgui_text("{}", differing_pixels);
               ImGui::TableNextColumn();// Difference
               format_imgui_text("{}", total_pixels1);
               ImGui::TableNextColumn();// Difference
               format_imgui_text("{:0.2f}", difference_percentage * 100.0);
               ImGui::TableNextColumn();// Copy
               {
                    const auto pop_id_buttons = PushPopID();
                    if (ImGui::SmallButton(ICON_FA_COPY))
                    {
                         ImGui::SetClipboardText(path1.string().c_str());
                    }
                    else
                    {
                         tool_tip("Copy full path to clipboard");
                    }
               }

               // Second row: path2
               ImGui::TableNextRow();
               ImGui::TableNextColumn();// File Path
               format_imgui_text(
                 "{}{}",
                 crop_path_to_fit(path2, ImGui::GetContentRegionAvail().x),
                 static_cast<char>(std::filesystem::path::preferred_separator));
               ImGui::SameLine(0, 0);
               ImGui::PushStyleColor(
                 ImGuiCol_Text, IM_COL32(255, 192, 64, 255));// Orange
               format_imgui_text("{}", path2.filename());
               ImGui::PopStyleColor();
               ImGui::TableNextColumn();// Difference
               format_imgui_text("{}", differing_pixels);
               ImGui::TableNextColumn();// Difference
               format_imgui_text("{}", total_pixels2);
               ImGui::TableNextColumn();// Difference
               format_imgui_text("{:0.2f}", difference_percentage * 100.0);
               ImGui::TableNextColumn();// Copy
               {
                    const auto pop_id_buttons = PushPopID();
                    if (ImGui::SmallButton(ICON_FA_COPY))
                    {
                         ImGui::SetClipboardText(path1.string().c_str());
                    }
                    else
                    {
                         tool_tip("Copy full path to clipboard");
                    }
               }
          }
     }
     clipper.End();
     if (m_auto_scroll && (!m_consumer.done() || !m_future_consumer.done()))
     {
          ImGui::SetScrollHereY(1.0f);
     }
}

void ImageCompareWindow::handle_table_sorting()
{
     if (!m_consumer.done() || !m_future_consumer.done())
     {
          return;
     }
     ImGuiTableSortSpecs *sort_specs = ImGui::TableGetSortSpecs();
     if (!(sort_specs && sort_specs->SpecsDirty))
     {
          return;
     }
     // sort_specs->Specs contains the column
     // index and direction
     for (int i = 0; i < sort_specs->SpecsCount; i++)
     {
          const ImGuiTableColumnSortSpecs *col_spec     = &sort_specs->Specs[i];
          int                              column_index = col_spec->ColumnIndex;
          ImGuiSortDirection dir = col_spec->SortDirection;// 0=asc,1=desc

          // Perform your sorting of your data
          // based on column_index and dir
          // Example: sort your vector of rows
          // accordingly
          switch (column_index)
          {
               case 0:// File Path
                    if (dir == ImGuiSortDirection_Ascending)
                         std::ranges::sort(
                           m_diff_results,
                           {},// default operator<
                           [](const auto &diff)
                           { return diff.path1.filename(); });
                    else
                         std::ranges::sort(
                           m_diff_results,
                           std::ranges::greater{},
                           [](const auto &diff)
                           { return diff.path1.filename(); });
                    break;

               case 1:// D Pixels
                    if (dir == ImGuiSortDirection_Ascending)
                         std::ranges::sort(
                           m_diff_results,
                           {},// operator<
                           [](const auto &diff)
                           { return diff.differing_pixels; });
                    else
                         std::ranges::sort(
                           m_diff_results,
                           std::ranges::greater{},
                           [](const auto &diff)
                           { return diff.differing_pixels; });
                    break;

               case 2:// T Pixels
                    if (dir == ImGuiSortDirection_Ascending)
                         std::ranges::sort(
                           m_diff_results,
                           {},
                           [](const auto &diff)
                           {
                                return (std::max)(diff.total_pixels1,
                                                  diff.total_pixels2);
                           });
                    else
                         std::ranges::sort(
                           m_diff_results,
                           std::ranges::greater{},
                           [](const auto &diff)
                           {
                                return (std::max)(diff.total_pixels1,
                                                  diff.total_pixels2);
                           });
                    break;

               case 3:// %
                    if (dir == ImGuiSortDirection_Ascending)
                         std::ranges::sort(
                           m_diff_results,
                           {},
                           [](const auto &diff)
                           { return diff.difference_percentage; });
                    else
                         std::ranges::sort(
                           m_diff_results,
                           std::ranges::greater{},
                           [](const auto &diff)
                           { return diff.difference_percentage; });
                    break;

               default:
                    break;
          }
     }

     // Mark as not dirty
     sort_specs->SpecsDirty = false;
}

void ImageCompareWindow::CompareDirectoriesStart()
{
     if (!m_path1_valid || !m_path2_valid)
     {
          spdlog::error("Invalid paths provided for comparison.");
          return;
     }
     m_diff_results.clear();
     m_diff_result_futures.clear();
     m_consumer = RangeConsumer<std::filesystem::recursive_directory_iterator>(
       std::filesystem::recursive_directory_iterator(m_path1.data()));
     m_future_consumer.clear_detached();
     m_files_in_path2.clear();
     for (auto &entry :
          std::filesystem::recursive_directory_iterator(m_path2.data()))
     {
          try
          {
               if (entry.is_regular_file())
               {
                    m_files_in_path2.insert(
                      std::filesystem::relative(entry.path(), m_path2.data()));
               }
          }
          catch (const std::filesystem::filesystem_error &e)
          {
               spdlog::warn(
                 "Skipping file due to filesystem error: {} ({})", entry.path(),
                 e.what());
          }
          catch (const std::exception &e)
          {
               spdlog::warn(
                 "Skipping file due to unexpected exception: {} ({})",
                 entry.path(), e.what());
          }
          catch (...)
          {
               spdlog::warn(
                 "Skipping file due to unknown exception: {}", entry.path());
          }
     }
}

void ImageCompareWindow::CompareDirectoriesStep()
{

     if (m_consumer.done())
          return;
     static constexpr const int MAX_SKIPPED   = 100;
     static constexpr const int MAX_PROCESSED = 10;
     int                        skipped       = {};
     int                        processed     = {};
     do
     {
          auto &entryA = *m_consumer;
          ++m_consumer;
          try
          {
               if (entryA.is_regular_file())
               {
                    const auto relative_path = std::filesystem::relative(
                      entryA.path(), std::filesystem::path(m_path1.data()));
                    if (m_files_in_path2.contains(relative_path))
                    {
                         const auto pathB
                           = std::filesystem::path(m_path2.data())
                             / relative_path;

                         m_diff_result_futures.push_back(
                           CompareImageAsync(entryA.path(), pathB));
                         ++processed;
                    }
                    else
                    {
                         ++skipped;
                    }
               }
          }
          catch (const std::filesystem::filesystem_error &e)
          {
               spdlog::warn(
                 "Skipping file due to filesystem error: {} ({})",
                 entryA.path(), e.what());
          }
          catch (const std::exception &e)
          {
               spdlog::warn(
                 "Skipping file due to unexpected exception: {} ({})",
                 entryA.path(), e.what());
          }
          catch (...)
          {
               spdlog::warn(
                 "Skipping file due to unknown exception: {}", entryA.path());
          }
     } while (!m_consumer.done() && skipped < MAX_SKIPPED
              && processed < MAX_PROCESSED);
     if (m_consumer.done())
     {
          spdlog::info("Directory comparison completed.");
          spdlog::info("Found {} differing files.", m_diff_results.size());
     }
}

void ImageCompareWindow::export_button()
{
     if (ImGui::Button("Export Diff Results"))
     {
          try
          {
               // Convert your diff results to TOML string/array
               toml::table entries{};

               entries.insert_or_assign(
                 "compare", to_toml_array(m_diff_results));

               // Choose a path — hardcoded or with a dialog
               std::filesystem::path out_path = "diff_results.toml";

               // Write the TOML to file
               std::ofstream         ofs(out_path);
               if (!ofs)
               {
                    spdlog::error(
                      "Failed to open file for writing: {}", out_path.string());
               }
               else
               {
                    ofs << entries;
                    spdlog::info(
                      "Diff results written to {}", out_path.string());
               }
          }
          catch (const std::exception &e)
          {
               spdlog::error("Error exporting diff results: {}", e.what());
          }
     }
}

void ImageCompareWindow::CompareDirectoriesStop()
{
     m_consumer.stop();
     m_future_consumer.stop();
}

struct StbiDeleter
{
     void operator()(std::uint8_t *img) const noexcept
     {
          if (img)
               stbi_image_free(img);
     }
};

std::future<ImageCompareWindow::DiffResult>
  ImageCompareWindow::CompareImageAsync(
    std::filesystem::path fileA,
    std::filesystem::path fileB)
{
     return std::async(
       std::launch::async,
       [fileA = std::move(fileA), fileB = std::move(fileB)]
       { return CompareImage(fileA, fileB); });
}

ImageCompareWindow::DiffResult ImageCompareWindow::CompareImage(
  std::filesystem::path fileA,
  std::filesystem::path fileB)
{
     // Normalize paths using the preferred separator for the current
     // platform
     fileA.make_preferred();
     fileB.make_preferred();

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
       = static_cast<std::int64_t>(width1) * static_cast<std::int64_t>(height1);
     total_pixels2
       = static_cast<std::int64_t>(width2) * static_cast<std::int64_t>(height2);
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
       size_t{ 0 },
       static_cast<std::size_t>((std::min)(total_pixels1, total_pixels2)));
     static const constexpr int error = 0;
     differing_pixels                 = std::transform_reduce(
       std::execution::par_unseq, indices.begin(), indices.end(),
       std::int64_t{ 0 }, std::plus{},
       [&](const size_t idx)
       {
            const size_t i  = idx * 4;
            const int    dr = std::abs(img1[i] - img2[i]);
            const int    dg = std::abs(img1[i + 1] - img2[i + 1]);
            const int    db = std::abs(img1[i + 2] - img2[i + 2]);
            const int    da = std::abs(img1[i + 3] - img2[i + 3]);
            if (dr > error || dg > error || db > error || da > error)
            {
                 return std::int64_t{ 1 };
            }
            return std::int64_t{ 0 };
       });
     difference_percentage
       = static_cast<double>(differing_pixels)
         / static_cast<double>((std::max)(total_pixels1, total_pixels2));
     return result;
}

}// namespace fme
