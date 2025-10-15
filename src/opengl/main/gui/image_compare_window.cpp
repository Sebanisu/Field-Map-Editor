#include "image_compare_window.hpp"
#include "format_imgui_text.hpp"
#include "safedir.hpp"
#include <execution>
#include <numeric>
#include <ScopeGuard.hpp>
#include <stb_image.h>
namespace fme
{

ImageCompareWindow::ImageCompareWindow(std::weak_ptr<Selections> selections)
  : m_selections(std::move(selections))
{
}

void ImageCompareWindow::render()
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     bool show_window = selections->get<ConfigKey::DisplayImageCompareWindow>();
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

     if (ImGui::InputText("Path A", m_path1.data(), m_path1.size()))
     {
          const auto tmp = safedir(m_path1.data());
          m_path1_valid  = tmp.is_dir() && tmp.is_exists();
     }
     if (ImGui::InputText("Path B", m_path2.data(), m_path2.size()))
     {
          const auto tmp = safedir(m_path2.data());
          m_path2_valid  = tmp.is_dir() && tmp.is_exists();
     }
     if (ImGui::Button("Compare Directories"))
     {
          m_diff_results.clear();
          CompareDirectories();
     }
     ImGui::Separator();
     if (!m_diff_results.empty())
     {
          if (ImGui::CollapsingHeader(
                "Results", ImGuiTreeNodeFlags_DefaultOpen))
          {
               for (const auto
                      &[path1, path2, total_pixels1, total_pixels2,
                        differing_pixels, difference_percentage] :
                    m_diff_results)
               {
                    format_imgui_wrapped_text(
                      "{} vs {}: {} differing pixels out of {} ({}%)",
                      path1.string(),
                      path2.string(),
                      differing_pixels,
                      (std::max)(total_pixels1, total_pixels2),
                      difference_percentage);
               }
          }
     }
}

void ImageCompareWindow::CompareDirectories()
{
     if (!m_path1_valid || !m_path2_valid)
     {
          spdlog::error("Invalid paths provided for comparison.");
          return;
     }

     for (auto &entryA :
          std::filesystem::recursive_directory_iterator(m_path1.data()))
     {
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
                    m_diff_results.push_back(
                      CompareImage(entryA.path(), pathB));
               }
          }
     }
}

ImageCompareWindow::DiffResult ImageCompareWindow::CompareImage(
  std::filesystem::path fileA,
  std::filesystem::path fileB)
{
     DiffResult result{ .path1 = std::move(fileA), .path2 = std::move(fileB) };
     auto
       &[path1, path2, total_pixels1, total_pixels2, differing_pixels,
         difference_percentage]
       = result;
     int         width1 = 0, height1 = 0, channels1 = 0;
     int         width2 = 0, height2 = 0, channels2 = 0;
     const auto *img1
       = stbi_load(path1.string().c_str(), &width1, &height1, &channels1, 4);
     const auto *img2
       = stbi_load(path2.string().c_str(), &width2, &height2, &channels2, 4);
     const auto cleanup = glengine::ScopeGuard(
       [img1, img2]()
       {
            if (img1)
            {
                 stbi_image_free((void *)img1);
            }
            if (img2)
            {
                 stbi_image_free((void *)img2);
            }
       });
     total_pixels1 = static_cast<std::size_t>(width1) * height1;
     total_pixels2 = static_cast<std::size_t>(width2) * height2;
     if (width1 != width2 || height1 != height2 || channels1 != channels2)
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
     return result;
}

}// namespace fme
