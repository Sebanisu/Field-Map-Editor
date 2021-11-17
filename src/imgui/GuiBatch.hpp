//
// Created by pcvii on 11/15/2021.
//

#ifndef MYPROJECT_GUIBATCH_HPP
#define MYPROJECT_GUIBATCH_HPP
#include "archives_group.hpp"
#include "filebrowser.hpp"
#include "open_viii/paths/Paths.hpp"
#include <cppcoro/generator.hpp>
namespace fme
{
struct GuiBatch
{
  enum class BatchOperationSource
  {
    None,
    FieldsArchive,
    Swizzles,
    Deswizzles,
  };
  enum class BatchOperationTransformation
  {
    None,
    Deswizzle [[maybe_unused]] = 1U,
    Swizzle [[maybe_unused]]   = 1U << 1U,
    CompactRows                = 1U << 2U,
    CompactAll                 = 1U << 3U,
    FlattenBPP                 = 1U << 4U,
    FlattenPalette             = 1U << 5U,
  };
  GuiBatch()
    : m_archive_paths(open_viii::Paths::get())
    , m_archive_group(
        m_archive_paths.empty() ? archives_group{}
                                : archives_group{ {}, m_archive_paths.front() })
  {
  }
  void
    operator()(int *id)
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

private:
  static cppcoro::generator<std::tuple<int, std::string>>
    get_field_id_and_name(std::vector<std::string> maps)
  {
    for (int i{}; auto &name : maps)
    {
      co_yield std::make_tuple(i, std::move(name));
      ++i;
    }
  }
  static cppcoro::generator<std::shared_ptr<open_viii::archive::FIFLFS<false>>>
    get_field(::archives_group archives_group)
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
  static cppcoro::generator<open_viii::LangT>
    get_field_coos(
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
  static cppcoro::generator<::map_sprite>
    get_map_sprite(auto gen_field, const ::filters in_filters)
  {
    for (const std::shared_ptr<open_viii::archive::FIFLFS<false>> &field :
         gen_field)
    {
      std::string base_name     = str_to_lower(field->get_base_name());
      auto        local_filters = in_filters;

      if (local_filters.upscale.enabled())
      {
        popup_batch_common_filter_start(local_filters.upscale, base_name);
      }
      else if (local_filters.deswizzle.enabled())
      {
        popup_batch_common_filter_start(local_filters.deswizzle, base_name);
      }

      fmt::print("{}\n", base_name);
      auto sprite =
        map_sprite{ field, open_viii::LangT::generic, {}, local_filters, {} };
      const auto load_map_file =
        [&](const ::filter<std::filesystem::path> &filter)
      {
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
      for (const open_viii::LangT coo : get_field_coos(field))
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
  }
  static void
    popup_batch_common_filter_start(
      ::filter<std::filesystem::path> &filter,
      std::string_view                 base_name)
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
  [[nodiscard]] cppcoro::generator<map_sprite>
    source()
  {
    ::filters filters{};
    if (m_source_type == BatchOperationSource::Swizzles)
    {
      filters.upscale.update(m_source_path.value()).enable();
    }
    else if (m_source_type == BatchOperationSource::Deswizzles)
    {
      filters.deswizzle.update(m_source_path.value()).enable();
    }
    auto gen_map_sprite = get_map_sprite(get_field(m_archive_group), filters);
    for (::map_sprite &ms : gen_map_sprite)
    {
      compact_and_flatten(ms);
      co_yield std::move(ms);
    }
  }
  void
    compact_and_flatten(map_sprite &ms) const
  {
    const auto c = [&]
    {
      if (
        (static_cast<uint32_t>(m_transformation_type)
         & static_cast<uint32_t>(
           BatchOperationTransformation::CompactRows))
        != 0)
      {
        ms.compact();
      }
      if (
        (static_cast<uint32_t>(m_transformation_type)
         & static_cast<uint32_t>(
           BatchOperationTransformation::CompactAll))
        != 0)
      {
        ms.compact2();
      }
    };
    c();
    const auto f = [&]() -> bool
    {
      bool r = false;
      if (
        (static_cast<uint32_t>(m_transformation_type)
         & static_cast<uint32_t>(
           BatchOperationTransformation::FlattenBPP))
        != 0)
      {
        ms.flatten_bpp();
        r = true;
      }
      if (
        (static_cast<uint32_t>(m_transformation_type)
         & static_cast<uint32_t>(
           BatchOperationTransformation::FlattenPalette))
        != 0)
      {
        ms.flatten_palette();
        r = true;
      }
      return r;
    };
    if (f())
    {
      c();
    }
  }
  [[nodiscard]] bool
    ask() const;
  [[nodiscard]] bool
    ask_post_operation() const;
  [[nodiscard]] bool
    ask_output() const;
  [[nodiscard]] bool
    ask_archive_path() const;
  [[nodiscard]] bool
    ask_source() const;
  [[nodiscard]] bool
    ask_transformation() const;
  [[nodiscard]] static ImGui::FileBrowser
    create_directory_browser(
      std::string              title,
      std::vector<std::string> filetypes = {});
  [[nodiscard]] static std::optional<std::filesystem::path>
    ask_for_path(ImGui::FileBrowser &file_browser);

  mutable std::vector<std::string>             m_archive_paths       = {};
  mutable archives_group                       m_archive_group       = {};
  mutable BatchOperationSource                 m_source_type         = {};
  mutable std::optional<std::filesystem::path> m_source_path         = {};
  mutable BatchOperationTransformation         m_transformation_type = {};
  mutable std::optional<std::filesystem::path> m_output_path         = {};
  mutable int                                 *m_id                  = {};
  mutable bool                                 m_embed_maps          = {};
  mutable bool                                 m_reload_after        = { true };
  mutable std::optional<std::filesystem::path> m_embed_path          = {};
};
}// namespace fme
#endif// MYPROJECT_GUIBATCH_HPP
