//
// Created by pcvii on 11/15/2021.
//

#include "GuiBatch.hpp"
#include "format_imgui_text.hpp"
#include "formatters.hpp"
#include "generic_combo.hpp"
#include "scope_guard.hpp"
#include <array>
#include <filesystem>
#include <ranges>
#include <string_view>
namespace fme
{

static constexpr auto BatchOperationSourceStrings =
  std::array{ std::string_view{ "None" },
              std::string_view{ "Fields Archive" },
              std::string_view{ "Upscales or Swizzles" },
              std::string_view{ "Deswizzles" } };


static constexpr auto BatchOperationTransformationStrings =
  std::array{ std::string_view{ "None" },
              std::string_view{ "Deswizzle" },
              std::string_view{ "Swizzle" } };

static constexpr auto BatchOperationTransformationCompactStrings =
  std::array{ std::string_view{ "Rows" }, std::string_view{ "All" } };
}// namespace fme
bool
  fme::GuiBatch::ask() const
{
  const auto end = scope_guard(&ImGui::End);
  if (ImGui::Begin(
        "Batch Operations", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    if (
      ask_archive_path() && ask_source() && ask_transformation() && ask_output()
      && ask_post_operation())
    {
      PupuID(++*m_id);
      const auto pop = scope_guard(&ImGui::PopID);
      if (ImGui::Button("Start"))
      {
        puts("Pushed Start");
        return true;
      }
    }
  }
  return false;
}
bool
  fme::GuiBatch::ask_post_operation() const
{
  static int               selected_embed_path = {};
  std::vector<std::string> paths               = {};
  paths.reserve(2U);
  if (m_output_path.has_value())
  {
    paths.push_back(m_output_path.value().string());
  }
  if (m_source_path.has_value())
  {
    paths.push_back(m_source_path.value().string());
  }
  if (
    !std::empty(paths)
    && ImGui::CollapsingHeader(
      "Embed .map(s) into archives", ImGuiTreeNodeFlags_DefaultOpen))
  {
    format_imgui_text("{}", "Choose directory to load .maps from");

    if (
      generic_combo(
        *m_id,
        "Embed Path",
        [&]() {
          return std::views::iota(
            0, static_cast<int>(std::ranges::ssize(paths)));
        },
        [&]() { return paths; },
        selected_embed_path)
      || !m_embed_path.has_value()
      || m_embed_path
           != paths.at(static_cast<std::size_t>(selected_embed_path)))
    {
      m_embed_path = paths.at(static_cast<std::size_t>(selected_embed_path));
    }
    ImGui::PushID(++m_id);
    if (ImGui::Checkbox("Enable ", &m_embed_maps))
    {
    }
    if (m_embed_maps)
    {
      ImGui::SameLine();
      ImGui::PopID();
      if (ImGui::Checkbox("Reload after?", &m_reload_after))
      {
      }
    }
  }
  return (m_embed_maps && m_embed_path.has_value()
          && !m_embed_path.value().empty())
         || (!m_embed_maps && (static_cast<std::uint32_t>(m_transformation_type) & 0b11U) != 0U && m_output_path.has_value()
             && !m_output_path.value().empty());
}
bool
  fme::GuiBatch::ask_output() const
{
  const bool requires_output =
    (static_cast<std::uint32_t>(m_transformation_type) & 0b11U) != 0U;
  if (
    requires_output
    && ImGui::CollapsingHeader("Output", ImGuiTreeNodeFlags_DefaultOpen))
  {
    static auto db = create_directory_browser("Select Directory");
    ImGui::PushID(++*m_id);
    if (m_output_path.has_value() && !m_output_path.value().empty())
    {
      format_imgui_text("Directory: \"{}\"", m_output_path.value());
    }
    if (ImGui::Button("Browse"))
    {
      db.Open();
    }
    ImGui::PopID();
    if (auto path = ask_for_path(db); path.has_value())
    {
      m_output_path = path;
    }
  }
  return !requires_output
         || (m_output_path.has_value() && !m_output_path.value().empty());
}
bool
  fme::GuiBatch::ask_archive_path() const
{
  static auto db = create_directory_browser("Select Directory");
  static int  selected_src_archive_int = {};
  if (generic_combo(
        *m_id,
        "Archive Path",
        [&]()
        {
          return std::views::iota(
            0, static_cast<int>(std::ranges::ssize(m_archive_paths)));
        },
        [&]() { return m_archive_paths; },
        selected_src_archive_int))
  {
    m_archive_group = m_archive_group.with_path(
      m_archive_paths.at(static_cast<std::size_t>(selected_src_archive_int)));
  }
  ImGui::PushID(++*m_id);
  if (ImGui::Button("Browse"))
  {
    db.Open();
  }
  ImGui::PopID();
  if (auto path = ask_for_path(db); path.has_value())
  {
    selected_src_archive_int = static_cast<int>(m_archive_paths.size());
    m_archive_group          = m_archive_group.with_path(
               m_archive_paths.emplace_back(path.value().string()));
  }
  return !std::empty(m_archive_paths) && !m_archive_group.failed();
}
bool
  fme::GuiBatch::ask_source() const
{
  if (ImGui::CollapsingHeader("Source", ImGuiTreeNodeFlags_DefaultOpen))
  {
    static int selected_src_type_int = {};
    if (generic_combo(
          *m_id,
          "Type",
          []()
          {
            return std::views::iota(
              0,
              static_cast<int>(
                std::ranges::ssize(BatchOperationSourceStrings)));
          },
          []() { return BatchOperationSourceStrings; },
          selected_src_type_int))
    {
      m_source_type = static_cast<BatchOperationSource>(selected_src_type_int);
    }
    if (
      m_source_type == BatchOperationSource::Swizzles
      || m_source_type == BatchOperationSource::Deswizzles)
    {
      static auto db = create_directory_browser("Select Directory");
      ImGui::PushID(++*m_id);
      if (m_source_path.has_value() && !m_source_path.value().empty())
      {
        format_imgui_text("Directory: \"{}\"", m_source_path.value());
      }
      if (ImGui::Button("Browse"))
      {
        db.Open();
      }
      ImGui::PopID();
      if (auto path = ask_for_path(db); path.has_value())
      {
        m_source_path = path;
      }
    }
  }
  return m_source_type == BatchOperationSource::FieldsArchive
         || ((m_source_type == BatchOperationSource::Swizzles
              || m_source_type == BatchOperationSource::Deswizzles)
             && m_source_path.has_value() && !m_source_path.value().empty());
}
bool
  fme::GuiBatch::ask_transformation() const
{
  if (!ImGui::CollapsingHeader(
        "Transformation", ImGuiTreeNodeFlags_DefaultOpen))
  {
    const auto old = m_transformation_type;
    using namespace std::string_view_literals;
    static int task_type{};
    if (generic_combo(
          *m_id,
          "Task"sv,
          []()
          {
            return std::views::iota(
              0,
              static_cast<int>(
                std::ranges::ssize(BatchOperationTransformationStrings)));
          },
          []() { return BatchOperationTransformationStrings; },
          task_type))
    {
      if (task_type > 0)
      {
        m_transformation_type = static_cast<BatchOperationTransformation>(
          1U << (static_cast<std::uint32_t>(task_type - 1)));
      }
      else
      {
        m_transformation_type = BatchOperationTransformation::None;
      }
    }
    else
    {
      m_transformation_type = static_cast<BatchOperationTransformation>(
        static_cast<std::uint32_t>(m_transformation_type) & 0b11U);
    }
    if ((static_cast<std::uint32_t>(m_transformation_type) & 0b11U) != 0U)
    {
      static ::filter<int> compact{};
      generic_combo(
        *m_id,
        "Compact"sv,
        []()
        {
          return std::views::iota(
            0,
            static_cast<int>(
              std::ranges::ssize(BatchOperationTransformationCompactStrings)));
        },
        []() { return BatchOperationTransformationCompactStrings; },
        [&]() -> ::filter<int> & { return compact; });

      if (compact.enabled())
      {
        if (compact.value() == 0)
        {
          m_transformation_type = static_cast<BatchOperationTransformation>(
            static_cast<std::uint32_t>(m_transformation_type)
            | static_cast<std::uint32_t>(
              BatchOperationTransformation::CompactRows));
        }
        else if (compact.value() == 1)
        {
          m_transformation_type = static_cast<BatchOperationTransformation>(
            static_cast<std::uint32_t>(m_transformation_type)
            | static_cast<std::uint32_t>(
              BatchOperationTransformation::CompactAll));
        }
      }
      format_imgui_text("Flatten: ");
      static bool flatten_bpp{};
      ImGui::SameLine();
      ImGui::Checkbox("BPP", &flatten_bpp);
      if (flatten_bpp)
        m_transformation_type = static_cast<BatchOperationTransformation>(
          static_cast<std::uint32_t>(m_transformation_type)
          | static_cast<std::uint32_t>(
            BatchOperationTransformation::FlattenBPP));
      static bool flatten_palette{};
      ImGui::SameLine();
      ImGui::Checkbox("Palette", &flatten_palette);
      if (flatten_palette)
        m_transformation_type = static_cast<BatchOperationTransformation>(
          static_cast<std::uint32_t>(m_transformation_type)
          | static_cast<std::uint32_t>(
            BatchOperationTransformation::FlattenPalette));
    }
    if (old != m_transformation_type)
      fmt::print(
        "0b{:0>6b}U\n", static_cast<std::uint32_t>(m_transformation_type));
  }
  return !(
    m_transformation_type == BatchOperationTransformation::None
    && m_source_type == BatchOperationSource::FieldsArchive);
}
ImGui::FileBrowser
  fme::GuiBatch::create_directory_browser(
    std::string              title,
    std::vector<std::string> filetypes)
{
  ImGui::FileBrowser directory_browser{ static_cast<ImGuiFileBrowserFlags>(
    static_cast<std::uint32_t>(ImGuiFileBrowserFlags_SelectDirectory)
    | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir)) };
  directory_browser.SetTitle(std::move(title));
  directory_browser.SetTypeFilters(std::move(filetypes));
  return directory_browser;
}
std::optional<std::filesystem::path>
  fme::GuiBatch::ask_for_path(ImGui::FileBrowser &file_browser)
{
  std::optional<std::filesystem::path> path{};
  file_browser.Display();
  if (file_browser.HasSelected())
  {
    path = file_browser.GetSelected();
    file_browser.ClearSelected();
  }
  return path;
}
