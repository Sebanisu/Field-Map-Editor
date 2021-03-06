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
bool fme::GuiBatch::ask() const
{
  const auto end = scope_guard(&ImGui::End);
  if (ImGui::Begin(
        "Batch Operations", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
  {
    if (
      ask_archive_path() && ask_source() && ask_transformation() && ask_output()
      && ask_post_operation())
    {
      const auto sg = PushPop();
      if (ImGui::Button("Start"))
      {
        puts("Pushed Start");
        return true;
      }
    }
  }
  return false;
}
bool fme::GuiBatch::ask_post_operation() const
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
    {
      const auto sg = PushPop();
      if (ImGui::Checkbox("Enable ", &m_embed_maps))
      {
      }
    }
    if (m_embed_maps)
    {
      ImGui::SameLine();
      {
        const auto sg = PushPop();
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
bool fme::GuiBatch::ask_output() const
{
  const bool requires_output =
    (static_cast<std::uint32_t>(m_transformation_type) & 0b11U) != 0U;
  if (
    requires_output
    && ImGui::CollapsingHeader("Output", ImGuiTreeNodeFlags_DefaultOpen))
  {
    if (m_output_path.has_value() && !m_output_path.value().empty())
    {
      format_imgui_text("Directory: \"{}\"", m_output_path.value());
    }
    {
      const auto sg = PushPop();
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
  return !requires_output
         || (m_output_path.has_value() && !m_output_path.value().empty());
}
bool fme::GuiBatch::ask_archive_path() const
{
  static int selected_src_archive_int = {};
  if (generic_combo(
        *m_id,
        "Archive Path",
        [&]() {
          return std::views::iota(
            0, static_cast<int>(std::ranges::ssize(m_archive_paths)));
        },
        [&]() { return m_archive_paths; },
        selected_src_archive_int))
  {
    m_archive_group = m_archive_group.with_path(
      m_archive_paths.at(static_cast<std::size_t>(selected_src_archive_int)));
  }
  {
    const auto sg = PushPop();
    if (ImGui::Button("Browse"))
    {
      m_archive_browser.Open();
    }
  }
  if (auto path = ask_for_path(m_archive_browser); path.has_value())
  {
    selected_src_archive_int = static_cast<int>(m_archive_paths.size());
    m_archive_group          = m_archive_group.with_path(
      m_archive_paths.emplace_back(path.value().string()));
  }
  return !std::empty(m_archive_paths) && !m_archive_group.failed();
}
bool fme::GuiBatch::ask_source() const
{
  if (ImGui::CollapsingHeader("Source", ImGuiTreeNodeFlags_DefaultOpen))
  {
    static int selected_src_type_int = {};
    if (generic_combo(
          *m_id,
          "Type",
          []() {
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
      if (m_source_path.has_value() && !m_source_path.value().empty())
      {
        format_imgui_text("Directory: \"{}\"", m_source_path.value());
      }
      {
        const auto sg = PushPop();
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
  return m_source_type == BatchOperationSource::FieldsArchive
         || ((m_source_type == BatchOperationSource::Swizzles
              || m_source_type == BatchOperationSource::Deswizzles)
             && m_source_path.has_value() && !m_source_path.value().empty());
}
scope_guard fme::GuiBatch::PushPop() const
{
  ImGui::PushID(++*m_id);
  return scope_guard{ &ImGui::PopID };
}
bool fme::GuiBatch::ask_transformation() const
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
          []() {
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
        []() {
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
ImGui::FileBrowser fme::GuiBatch::create_directory_browser(
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
void fme::GuiBatch::compact_and_flatten(map_sprite &ms) const
{
  const auto c = [&] {
    if (
      (static_cast<uint32_t>(m_transformation_type)
       & static_cast<uint32_t>(BatchOperationTransformation::CompactRows))
      != 0)
    {
      ms.compact();
    }
    if (
      (static_cast<uint32_t>(m_transformation_type)
       & static_cast<uint32_t>(BatchOperationTransformation::CompactAll))
      != 0)
    {
      ms.compact2();
    }
  };
  c();
  const auto f = [&]() -> bool {
    if (
      (static_cast<uint32_t>(m_transformation_type)
       & static_cast<uint32_t>(BatchOperationTransformation::FlattenBPP))
      != 0)
    {
      ms.flatten_bpp();
      return true;
    }
    if (
      (static_cast<uint32_t>(m_transformation_type)
       & static_cast<uint32_t>(BatchOperationTransformation::FlattenPalette))
      != 0)
    {
      ms.flatten_palette();
      return true;
    }
    return false;
  };
  if (f())
  {
    c();
  }
}
filters fme::GuiBatch::get_filters()
{
  filters filters{};
  if (m_source_type == BatchOperationSource::Swizzles)
  {
    filters.upscale.update(m_source_path.value()).enable();
  }
  else if (m_source_type == BatchOperationSource::Deswizzles)
  {
    filters.deswizzle.update(m_source_path.value()).enable();
  }
  return filters;
}
cppcoro::generator<bool> fme::GuiBatch::save_output(const map_sprite ms) const
{
  if (m_output_path.has_value())
  {
    std::string      base_name     = ms.get_base_name();
    std::string_view prefix        = std::string_view{ base_name }.substr(0, 2);
    auto             selected_path = m_output_path.value() / prefix / base_name;
    if (
      (static_cast<uint32_t>(m_transformation_type)
       & static_cast<uint32_t>(BatchOperationTransformation::Deswizzle))
      != 0)
    {
      // ms.save_pupu_textures(selected_path);
      auto gen_pupu_textures = ms.gen_pupu_textures(selected_path);
      for (bool b : gen_pupu_textures)
      {
        co_yield b;
      }
    }
    else if (
      (static_cast<uint32_t>(m_transformation_type)
       & static_cast<uint32_t>(BatchOperationTransformation::Swizzle))
      != 0)
    {
      auto gen_new_textures = ms.gen_new_textures(selected_path);
      for (bool b : gen_new_textures)
      {
        co_yield b;
      }
    }
    const std::filesystem::path map_path = selected_path / ms.map_filename();
    ms.save_modified_map(map_path);
  }
}
cppcoro::generator<bool> fme::GuiBatch::source()
{
  const auto filters   = get_filters();
  auto       gen_field = get_field(m_archive_group);
  for (auto field : gen_field)
  {
    co_yield true;
    auto gen_map_sprite = get_map_sprite(field, filters);
    co_yield true;
    for (::map_sprite &ms_ref : gen_map_sprite)
    {
      auto ms = std::move(ms_ref);
      co_yield true;
      compact_and_flatten(ms);
      co_yield true;
      auto gen_save_output = save_output(std::move(ms));
      co_yield true;
      for (bool b : gen_save_output)
      {
        co_yield b;
      }
    }
  }
}
void fme::GuiBatch::popup_batch_common_filter_start(
  filter<std::filesystem::path> &filter,
  std::string_view               base_name)
{
  std::string_view prefix = base_name.substr(0, 2);
  if (filter.enabled())
  {
    filter.update(filter.value() / prefix / base_name);
    if (
      !std::filesystem::exists(filter.value())
      || !std::filesystem::is_directory(filter.value()))
    {
      filter.disable();
    }
  }
}
void fme::GuiBatch::popup_batch_common_filter_start(
  filters     &filters,
  std::string &base_name)
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
cppcoro::generator<::map_sprite> fme::GuiBatch::get_map_sprite(
  const std::shared_ptr<open_viii::archive::FIFLFS<false>> &field,
  const filters                                            &in_filters)
{
  std::string base_name     = str_to_lower(field->get_base_name());
  auto        local_filters = in_filters;

  popup_batch_common_filter_start(local_filters, base_name);

  fmt::print("{}\n", base_name);
  auto sprite =
    map_sprite{ field, open_viii::LangT::generic, {}, local_filters, {} };
  const auto load_map_file =
    [&](const ::filter<std::filesystem::path> &filter) {
      if (!filter.enabled())
        return;
      auto map_path = filter.value() / sprite.map_filename();
      if (std::filesystem::exists(map_path))
      {
        sprite.load_map(map_path);
      }
    };
  if (!sprite.fail())
  {
    load_map_file(local_filters.upscale);
    load_map_file(local_filters.deswizzle);
    fmt::print(
      "{}\n", open_viii::LangCommon::to_string(open_viii::LangT::generic));
    co_yield std::move(sprite);
  }
  auto gen_field_coo = get_field_coos(field);
  for (const open_viii::LangT coo : gen_field_coo)
  {
    sprite = map_sprite{ field, coo, {}, local_filters, {} };
    if (!sprite.fail())
    {
      fmt::print("{}\n", open_viii::LangCommon::to_string(coo));
      load_map_file(local_filters.upscale);
      load_map_file(local_filters.deswizzle);
      co_yield std::move(sprite);
    }
  }
}
cppcoro::generator<open_viii::LangT> fme::GuiBatch::get_field_coos(
  const std::shared_ptr<open_viii::archive::FIFLFS<false>> field)
{
  auto map_pairs = field->get_vector_of_indexes_and_files(
    { open_viii::graphics::background::Map::EXT });
  for (auto &map_pair : map_pairs)
  {
    const auto &[i, file_path] = map_pair;
    const auto filename =
      std::filesystem::path(file_path).filename().stem().string();
    std::string      base_name     = str_to_lower(field->get_base_name());
    std::string_view filename_view = { filename };
    std::string_view basename_view = { base_name };
    if (
      std::size(filename_view) > std::size(basename_view)
      && filename_view.substr(
           0, std::min(std::size(filename_view), std::size(basename_view)))
           == basename_view.substr(
             0, std::min(std::size(filename_view), std::size(basename_view))))
    {
      const auto coo_view =
        filename_view.substr(std::size(basename_view) + 1U, 2U);
      co_yield open_viii::LangCommon::from_string(coo_view);
    }
  }
}
cppcoro::generator<std::shared_ptr<open_viii::archive::FIFLFS<false>>>
  fme::GuiBatch::get_field(::archives_group archives_group)
{
  auto gen_fields = get_field_id_and_name(archives_group.mapdata());
  for (const auto &[i, name] : gen_fields)
  {
    fmt::print("{:>3}: {}\n", i, name);
    if (auto f = archives_group.field(i); f)
    {
      co_yield std::move(f);
    }
  }
}
cppcoro::generator<std::tuple<int, std::string>>
  fme::GuiBatch::get_field_id_and_name(std::vector<std::string> maps)
{
  for (int i{}; auto &name : maps)
  {
    co_yield std::make_tuple(i, std::move(name));
    ++i;
  }
}
void fme::GuiBatch::operator()(int *id)
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
    static std::optional<decltype(source())> gen_source{};
    static decltype(decltype(source()){}.begin()) gen_current{};
    if (!gen_source.has_value())
    {
      gen_source  = source();
      gen_current = gen_source->begin();
    }
    else if (gen_current != gen_source->end())
    {
      std::ignore = *gen_current;
      ++gen_current;
    }
    else
    {
      gen_source = std::nullopt;
      asked      = false;
    }
  }
}
