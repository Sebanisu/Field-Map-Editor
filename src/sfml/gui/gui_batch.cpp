//
// Created by pcvii on 11/15/2021.
//

#include "gui_batch.hpp"
#include "format_imgui_text.hpp"
#include "formatters.hpp"
#include "generic_combo.hpp"
#include "safedir.hpp"
#include "scope_guard.hpp"
#include <array>
#include <cppcoro/generator.hpp>
#include <cppcoro/task.hpp>
#include <filesystem>
#include <ranges>
#include <string_view>
namespace fme
{

static constexpr auto BatchOperationSourceStrings = std::array{ std::string_view{ "None" },
                                                                std::string_view{ "Fields Archive" },
                                                                std::string_view{ "Upscales or Swizzles" },
                                                                std::string_view{ "Deswizzles" } };


static constexpr auto BatchOperationTransformationStrings =
  std::array{ std::string_view{ "None" }, std::string_view{ "Deswizzle" }, std::string_view{ "Swizzle" } };

static constexpr auto BatchOperationTransformationCompactStrings =
  std::array{ std::string_view{ "Rows" }, std::string_view{ "All" }, std::string_view{ "Map Order" } };
}// namespace fme
bool fme::gui_batch::ask() const
{
     const auto end = scope_guard(&ImGui::End);
     if (ImGui::Begin("Batch Operations", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
     {
          if (ask_archive_path() && ask_source() && ask_transformation() && ask_output() && ask_post_operation())
          {
               const auto sg = push_pop();
               if (ImGui::Button("Start"))
               {
                    puts("Pushed Start");
                    return true;
               }
          }
     }
     return false;
}
bool fme::gui_batch::ask_post_operation() const
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
     if (!std::empty(paths) && ImGui::CollapsingHeader("Embed .map(s) into archives", ImGuiTreeNodeFlags_DefaultOpen))
     {
          format_imgui_text("{}", "Choose directory to load .maps from");

          if (
            generic_combo(
              *m_id,
              "Embed Path",
              [&]() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(paths))); },
              [&]() { return paths; },
              selected_embed_path)
            || !m_embed_path.has_value() || m_embed_path != paths.at(static_cast<std::size_t>(selected_embed_path)))
          {
               m_embed_path = paths.at(static_cast<std::size_t>(selected_embed_path));
          }
          {
               const auto sg = push_pop();
               if (ImGui::Checkbox("Enable ", &m_embed_maps))
               {
               }
          }
          if (m_embed_maps)
          {
               ImGui::SameLine();
               {
                    const auto sg = push_pop();
                    if (ImGui::Checkbox("Reload after?", &m_reload_after))
                    {
                    }
               }
          }
     }
     return (m_embed_maps && m_embed_path.has_value()
          && !m_embed_path.value().empty())
         || (!m_embed_maps && (static_cast<std::uint32_t>(m_transformation_type) & 0b11U) != 0U && m_output_path.has_value()
             && !m_output_path.value().empty());
}
bool fme::gui_batch::ask_output() const
{
     const bool requires_output = (static_cast<std::uint32_t>(m_transformation_type) & 0b11U) != 0U;
     if (requires_output && ImGui::CollapsingHeader("Output", ImGuiTreeNodeFlags_DefaultOpen))
     {
          if (m_output_path.has_value() && !m_output_path.value().empty())
          {
               format_imgui_text("Directory: \"{}\"", m_output_path.value());
          }
          {
               const auto sg = push_pop();
               if (ImGui::Button("Browse"))
               {
                    m_output_browser.Open();
               }
          }
          if (auto path = ask_for_path(m_output_browser); path.has_value())
          {
               m_output_path = path;
          }
     }
     return !requires_output || (m_output_path.has_value() && !m_output_path.value().empty());
}
bool fme::gui_batch::ask_archive_path() const
{
     static int selected_src_archive_int = {};
     if (generic_combo(
           *m_id,
           "Archive Path",
           [&]() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(m_archive_paths))); },
           [&]() { return m_archive_paths; },
           selected_src_archive_int))
     {
          m_archive_group = m_archive_group.with_path(m_archive_paths.at(static_cast<std::size_t>(selected_src_archive_int)));
     }
     {
          const auto sg = push_pop();
          if (ImGui::Button("Browse"))
          {
               m_archive_browser.Open();
          }
     }
     if (auto path = ask_for_path(m_archive_browser); path.has_value())
     {
          selected_src_archive_int = static_cast<int>(m_archive_paths.size());
          m_archive_group          = m_archive_group.with_path(m_archive_paths.emplace_back(path.value().string()));
     }
     return !std::empty(m_archive_paths) && !m_archive_group.failed();
}
bool fme::gui_batch::ask_source() const
{
     if (ImGui::CollapsingHeader("Source", ImGuiTreeNodeFlags_DefaultOpen))
     {
          static int selected_src_type_int = {};
          if (generic_combo(
                *m_id,
                "Type",
                []() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(BatchOperationSourceStrings))); },
                []() { return BatchOperationSourceStrings; },
                selected_src_type_int))
          {
               m_source_type = static_cast<batch_operation_source>(selected_src_type_int);
          }
          if (m_source_type == batch_operation_source::swizzles || m_source_type == batch_operation_source::deswizzles)
          {
               if (m_source_path.has_value() && !m_source_path.value().empty())
               {
                    format_imgui_text("Directory: \"{}\"", m_source_path.value());
               }
               {
                    const auto sg = push_pop();
                    if (ImGui::Button("Browse"))
                    {
                         m_source_browser.Open();
                    }
               }
               if (auto path = ask_for_path(m_source_browser); path.has_value())
               {
                    m_source_path = path;
               }
          }
     }
     return m_source_type == batch_operation_source::fields_archive
         || ((m_source_type == batch_operation_source::swizzles
              || m_source_type == batch_operation_source::deswizzles)
             && m_source_path.has_value() && !m_source_path.value().empty());
}
bool fme::gui_batch::ask_transformation() const
{
     if (!ImGui::CollapsingHeader("Transformation", ImGuiTreeNodeFlags_DefaultOpen))
     {
          const auto old = m_transformation_type;
          using namespace std::string_view_literals;
          static int task_type{};
          if (generic_combo(
                *m_id,
                "Task"sv,
                []() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(BatchOperationTransformationStrings))); },
                []() { return BatchOperationTransformationStrings; },
                task_type))
          {
               if (task_type > 0)
               {
                    m_transformation_type = static_cast<batch_operation_transformation>(1U << (static_cast<std::uint32_t>(task_type - 1)));
               }
               else
               {
                    m_transformation_type = batch_operation_transformation::None;
               }
          }
          else
          {
               m_transformation_type =
                 static_cast<batch_operation_transformation>(static_cast<std::uint32_t>(m_transformation_type) & 0b11U);
          }
          if ((static_cast<std::uint32_t>(m_transformation_type) & 0b11U) != 0U)
          {
               static ff_8::filter_old<int> compact{};
               generic_combo(
                 *m_id,
                 "Compact"sv,
                 []() { return std::views::iota(0, static_cast<int>(std::ranges::ssize(BatchOperationTransformationCompactStrings))); },
                 []() { return BatchOperationTransformationCompactStrings; },
                 [&]() -> ff_8::filter_old<int> & { return compact; });

               if (compact.enabled())
               {
                    if (compact.value() == 0)
                    {
                         m_transformation_type = static_cast<batch_operation_transformation>(
                           static_cast<std::uint32_t>(m_transformation_type)
                           | static_cast<std::uint32_t>(batch_operation_transformation::compact_rows));
                    }
                    else if (compact.value() == 1)
                    {
                         m_transformation_type = static_cast<batch_operation_transformation>(
                           static_cast<std::uint32_t>(m_transformation_type)
                           | static_cast<std::uint32_t>(batch_operation_transformation::compact_all));
                    }
                    else if (compact.value() == 2)
                    {
                         m_transformation_type = static_cast<batch_operation_transformation>(
                           static_cast<std::uint32_t>(m_transformation_type)
                           | static_cast<std::uint32_t>(batch_operation_transformation::compact_map_order));
                    }
               }
               format_imgui_text("Flatten: ");
               static bool flatten_bpp{};
               ImGui::SameLine();
               if (compact.enabled() && compact.value() == 2)
               {
                    ImGui::BeginDisabled();
                    bool skip   = true;
                    flatten_bpp = false;
                    ImGui::Checkbox("BPP", &skip);
                    ImGui::EndDisabled();
               }
               else
               {
                    ImGui::Checkbox("BPP", &flatten_bpp);
               }
               if (flatten_bpp)
               {
                    m_transformation_type = static_cast<batch_operation_transformation>(
                      static_cast<std::uint32_t>(m_transformation_type)
                      | static_cast<std::uint32_t>(batch_operation_transformation::flatten_bpp));
               }
               static bool flatten_palette{};
               ImGui::SameLine();
               ImGui::Checkbox("Palette", &flatten_palette);
               if (flatten_palette)
               {
                    m_transformation_type = static_cast<batch_operation_transformation>(
                      static_cast<std::uint32_t>(m_transformation_type)
                      | static_cast<std::uint32_t>(batch_operation_transformation::flatten_palette));
               }
          }
          if (old != m_transformation_type)
          {
               spdlog::info("0b{:0>6b}U\n", static_cast<std::uint32_t>(m_transformation_type));
          }
     }
     return m_transformation_type != batch_operation_transformation::None || m_source_type != batch_operation_source::fields_archive;
}
ImGui::FileBrowser fme::gui_batch::create_directory_browser(std::string title, const std::vector<std::string> &filetypes)
{
     ImGui::FileBrowser directory_browser{ static_cast<ImGuiFileBrowserFlags>(
       static_cast<std::uint32_t>(ImGuiFileBrowserFlags_SelectDirectory)
       | static_cast<std::uint32_t>(ImGuiFileBrowserFlags_CreateNewDir)) };
     directory_browser.SetTitle(std::move(title));
     directory_browser.SetTypeFilters(filetypes);
     return directory_browser;
}
std::optional<std::filesystem::path> fme::gui_batch::ask_for_path(ImGui::FileBrowser &file_browser)
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
void fme::gui_batch::compact_and_flatten(map_sprite &current_map_sprite) const
{
     const auto compact = [&] {
          if ((static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::compact_rows)) != 0)
          {
               current_map_sprite.compact_rows();
          }
          if ((static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::compact_all)) != 0)
          {
               current_map_sprite.compact_all();
          }
          if (
            (static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::compact_map_order)) != 0)
          {
               current_map_sprite.compact_map_order();
          }
     };
     compact();
     const auto flatten = [&]() -> bool {
          if (
            (static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::compact_map_order)) == 0)
          {
               if ((static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::flatten_bpp)) != 0)
               {
                    current_map_sprite.flatten_bpp();
                    return true;
               }
          }
          if ((static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::flatten_palette)) != 0)
          {
               current_map_sprite.flatten_palette();
               return true;
          }
          return false;
     };
     if (flatten())
     {
          compact();
     }
}
ff_8::filters fme::gui_batch::get_filters()
{
     ff_8::filters filters{};
     if (m_source_type == batch_operation_source::swizzles)
     {
          filters.upscale.update(m_source_path.value()).enable();
     }
     else if (m_source_type == batch_operation_source::deswizzles)
     {
          filters.deswizzle.update(m_source_path.value()).enable();
     }
     return filters;
}
cppcoro::task<void> fme::gui_batch::save_output(map_sprite current_map_sprite) const
{
     if (m_output_path.has_value())
     {
          std::string      base_name     = current_map_sprite.get_base_name();
          std::string_view prefix        = std::string_view{ base_name }.substr(0, 2);
          auto             selected_path = m_output_path.value() / prefix / base_name;
          if ((static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::deswizzle)) != 0)
          {
               // ms.save_pupu_textures(selected_path);
               auto task = current_map_sprite.gen_pupu_textures(selected_path);
               while (!task.is_ready())
               {
                    co_await cppcoro::suspend_always{};
               }
          }
          else if ((static_cast<uint32_t>(m_transformation_type) & static_cast<uint32_t>(batch_operation_transformation::swizzle)) != 0)
          {
               auto task = current_map_sprite.gen_new_textures(selected_path);
               while (!task.is_ready())
               {
                    co_await cppcoro::suspend_always{};
               }
          }
          const std::filesystem::path map_path = selected_path / current_map_sprite.map_filename();
          current_map_sprite.save_modified_map(map_path);
          co_await cppcoro::suspend_always{};
     }
}
cppcoro::task<void> fme::gui_batch::source()
{
     const auto filters   = get_filters();
     auto       gen_field = get_field(m_archive_group);
     for (const auto &field : gen_field)
     {
          co_await cppcoro::suspend_always{};
          auto gen_map_sprite = get_map_sprite(field, filters);
          co_await cppcoro::suspend_always{};
          for (::map_sprite &current_map_sprite_ref : gen_map_sprite)
          {
               auto current_map_sprite = std::move(current_map_sprite_ref);
               co_await cppcoro::suspend_always{};
               compact_and_flatten(current_map_sprite);
               co_await cppcoro::suspend_always{};
               auto task = save_output(std::move(current_map_sprite));
               while (!task.is_ready())
               {
                    co_await cppcoro::suspend_always{};
               }
          }
     }
}
void fme::gui_batch::popup_batch_common_filter_start(ff_8::filter_old<std::filesystem::path> &filter, std::string_view base_name)
{
     std::string_view const prefix = base_name.substr(0, 2);
     if (filter.enabled())
     {
          filter.update(filter.value() / prefix / base_name);
          if (safedir const path = filter.value(); !path.is_exists() || !path.is_dir())
          {
               filter.disable();
          }
     }
}
void fme::gui_batch::popup_batch_common_filter_start(ff_8::filters &filters, std::string &base_name)
{
     if (filters.upscale.enabled())
     {
          popup_batch_common_filter_start(filters.upscale, base_name);
     }
     else if (filters.deswizzle.enabled())
     {
          popup_batch_common_filter_start(filters.deswizzle, base_name);
     }
}
cppcoro::generator<::map_sprite>
  fme::gui_batch::get_map_sprite(const std::shared_ptr<open_viii::archive::FIFLFS<false>> &field, const ff_8::filters &in_filters)
{
     std::string base_name     = ::map_sprite::str_to_lower(field->get_base_name());
     auto        local_filters = in_filters;

     popup_batch_common_filter_start(local_filters, base_name);

     spdlog::info("base name: {}", base_name);
     auto       sprite        = map_sprite{ field, open_viii::LangT::generic, {}, local_filters, {} };
     const auto load_map_file = [&](const ff_8::filter_old<std::filesystem::path> &filter) {
          if (!filter.enabled())
               return;
          auto    map_path      = filter.value() / sprite.map_filename();
          safedir safe_map_path = map_path;
          if (safe_map_path.is_exists())
          {
               sprite.load_map(map_path);
          }
     };
     if (!sprite.fail())
     {
          load_map_file(local_filters.upscale);
          load_map_file(local_filters.deswizzle);
          spdlog::info("language code: {}", open_viii::LangCommon::to_string(open_viii::LangT::generic));
          co_yield std::move(sprite);
     }
     auto gen_field_coo = get_field_coos(field);
     for (const open_viii::LangT coo : gen_field_coo)
     {
          sprite = map_sprite{ field, coo, {}, local_filters, {} };
          if (!sprite.fail())
          {
               spdlog::info("language code: {}", open_viii::LangCommon::to_string(coo));
               load_map_file(local_filters.upscale);
               load_map_file(local_filters.deswizzle);
               co_yield std::move(sprite);
          }
     }
}
cppcoro::generator<open_viii::LangT> fme::gui_batch::get_field_coos(const std::shared_ptr<open_viii::archive::FIFLFS<false>> field)
{
     auto map_pairs = field->get_vector_of_indexes_and_files({ open_viii::graphics::background::Map::EXT });
     for (auto &map_pair : map_pairs)
     {
          const auto &[i, file_path]     = map_pair;
          const auto       filename      = std::filesystem::path(file_path).filename().stem().string();
          std::string      base_name     = ::map_sprite::str_to_lower(field->get_base_name());
          std::string_view filename_view = { filename };
          std::string_view basename_view = { base_name };
          if (
            std::size(filename_view) > std::size(basename_view)
            && filename_view.substr(0, std::min(std::size(filename_view), std::size(basename_view)))
                 == basename_view.substr(0, std::min(std::size(filename_view), std::size(basename_view))))
          {
               const auto coo_view = filename_view.substr(std::size(basename_view) + 1U, 2U);
               co_yield open_viii::LangCommon::from_string(coo_view);
          }
     }
}
cppcoro::generator<std::shared_ptr<open_viii::archive::FIFLFS<false>>> fme::gui_batch::get_field(::archives_group archives_group)
{
     auto gen_fields = get_field_id_and_name(archives_group.mapdata());
     for (const auto &[i, name] : gen_fields)
     {
          spdlog::info("field id: {:>3}\tname: {}", i, name);
          if (auto f = archives_group.field(i); f)
          {
               co_yield std::move(f);
          }
     }
}
cppcoro::generator<std::tuple<int, std::string>> fme::gui_batch::get_field_id_and_name(std::vector<std::string> maps)
{
     for (int i{}; auto &name : maps)
     {
          co_yield std::make_tuple(i, std::move(name));
          ++i;
     }
}
void fme::gui_batch::operator()(int *id)
{
     m_id              = std::move(id);
     static bool asked = { false };
     if (!asked)
     {
          if (ask())
          {
               asked = true;
          }
          return;
     }
     if (asked)
     {
          static std::optional<decltype(source())> task{};
          if (!task.has_value())
          {
               task = source();
          }
          else if (task->is_ready())
          {
               task  = std::nullopt;
               asked = false;
          }
     }
}
