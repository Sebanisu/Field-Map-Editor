#include "custom_paths_window.hpp"
#include "as_string.hpp"
#include "fa_icons.hpp"
#include "formatters.hpp"

static const auto trim = [](const std::string &str) -> std::string {
     auto start = str.find_first_not_of(" \t\n\r\f\v");
     if (start == std::string::npos)
          return "";// Empty or all spaces

     auto end = str.find_last_not_of(" \t\n\r\f\v");
     return str.substr(start, end - start + 1);
};


static const auto                                          m_tests = std::to_array<fme::key_value_data>({
  { .field_name = "ecmall1", .ext = ".ca" },// Basic field_name + ext match
  { .field_name = "ecmall1", .ext = ".jsm", .language_code = open_viii::LangT::en, .pupu_id = 987654U },// Field with language suffix
  { .field_name = "ecmall1", .ext = ".msd", .language_code = open_viii::LangT::jp, .pupu_id = 543210U },// Another language case
  { .field_name = "ecmall1", .ext = ".map", .language_code = open_viii::LangT::de, .pupu_id = 234567U },// Different language, different ext
  { .field_name = "ecmall1", .ext = ".mim", .language_code = open_viii::LangT::fr, .pupu_id = 890123U },// Another unique case
  { .field_name = "ecmall1", .ext = ".inf", .language_code = open_viii::LangT::it, .pupu_id = 456789U },// Italian match
  { .field_name = "ecmall1", .ext = ".sfx", .pupu_id = 678901U },// No language, unique ext
  { .field_name = "ecmall1", .ext = ".tdw", .language_code = open_viii::LangT::es, .pupu_id = 321098U },// Spanish case
  { .field_name = "cwwood2", .ext = ".one" },// `chara.one` match
  { .field_name = "cwwood2", .ext = ".one", .language_code = open_viii::LangT::jp, .pupu_id = 765432U },// `chara_{2_letter_lang}.one` match
  { .field_name = "cdfield1", .ext = ".pmd", .pupu_id = 210987U },// Another general field match
  { .field_name = "cdfield2", .ext = ".pvp", .palette = std::uint8_t{ 2 }, .pupu_id = 210987U },// Field with palette
  { .field_name    = "bgkote1a",
                                             .ext           = ".tiff",
                                             .language_code = open_viii::LangT::es,
                                             .texture_page  = std::uint8_t{ 5 } },// With texture_page
  { .field_name = "bggate_1", .ext = ".gif", .language_code = open_viii::LangT::it, .pupu_id = 78901U },// With pupu_id
  { .field_name    = "bgeat1a",
                                             .ext           = ".bmp",
                                             .language_code = open_viii::LangT::de,
                                             .palette       = std::uint8_t{ 4 },
                                             .texture_page  = std::uint8_t{ 3 },
                                             .pupu_id       = 123456U }// Full case
});

[[nodiscard]] fme::custom_paths_window::vector_or_string_t fme::custom_paths_window::vector_or_string() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return fme::custom_paths_window::vector_or_string_t::unknown;
     }
     switch (selections->current_pattern)
     {
          case fme::PatternSelector::OutputSwizzlePattern:
          case fme::PatternSelector::OutputDeswizzlePattern:
          case fme::PatternSelector::OutputMapPatternForSwizzle:
          case fme::PatternSelector::OutputMapPatternForDeswizzle:
               return fme::custom_paths_window::vector_or_string_t::string;
          case fme::PatternSelector::PathPatternsCommonUpscale:
          case fme::PatternSelector::PathPatternsCommonUpscaleForMaps:
          case fme::PatternSelector::PathPatternsNoPaletteAndTexturePage:
          case fme::PatternSelector::PathPatternsWithPaletteAndTexturePage:
          case fme::PatternSelector::PathPatternsWithPupuID:
          case fme::PatternSelector::PathPatternsWithTexturePage:
               return fme::custom_paths_window::vector_or_string_t::vector;
     }
     return fme::custom_paths_window::vector_or_string_t::unknown;
}
[[nodiscard]] std::string *fme::custom_paths_window::get_current_string_value_mutable() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return nullptr;
     }
     switch (selections->current_pattern)
     {
          case fme::PatternSelector::OutputSwizzlePattern:
               return &selections->output_swizzle_pattern;
          case fme::PatternSelector::OutputDeswizzlePattern:
               return &selections->output_deswizzle_pattern;
          case fme::PatternSelector::OutputMapPatternForSwizzle:
               return &selections->output_map_pattern_for_swizzle;
          case fme::PatternSelector::OutputMapPatternForDeswizzle:
               return &selections->output_map_pattern_for_deswizzle;
          case fme::PatternSelector::PathPatternsCommonUpscale:
               return get_current_string_value_from_index(selections->paths_common_upscale, selections->current_pattern_index);
          case fme::PatternSelector::PathPatternsCommonUpscaleForMaps:
               return get_current_string_value_from_index(selections->paths_common_upscale_for_maps, selections->current_pattern_index);
          case fme::PatternSelector::PathPatternsNoPaletteAndTexturePage:
               return get_current_string_value_from_index(selections->paths_no_palette_and_texture_page, selections->current_pattern_index);
          case fme::PatternSelector::PathPatternsWithPaletteAndTexturePage:
               return get_current_string_value_from_index(
                 selections->paths_with_palette_and_texture_page, selections->current_pattern_index);
          case fme::PatternSelector::PathPatternsWithPupuID:
               return get_current_string_value_from_index(selections->paths_with_pupu_id, selections->current_pattern_index);
          case fme::PatternSelector::PathPatternsWithTexturePage:
               return get_current_string_value_from_index(selections->paths_with_texture_page, selections->current_pattern_index);
     }
     return nullptr;
}


[[nodiscard]] std::vector<std::string> *fme::custom_paths_window::get_current_string_vector_mutable() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return nullptr;
     }
     switch (selections->current_pattern)
     {
          case fme::PatternSelector::OutputSwizzlePattern:
          case fme::PatternSelector::OutputDeswizzlePattern:
          case fme::PatternSelector::OutputMapPatternForSwizzle:
          case fme::PatternSelector::OutputMapPatternForDeswizzle:
               return nullptr;
          case fme::PatternSelector::PathPatternsCommonUpscale:
               return &selections->paths_common_upscale;
          case fme::PatternSelector::PathPatternsCommonUpscaleForMaps:
               return &selections->paths_common_upscale_for_maps;
          case fme::PatternSelector::PathPatternsNoPaletteAndTexturePage:
               return &selections->paths_no_palette_and_texture_page;
          case fme::PatternSelector::PathPatternsWithPaletteAndTexturePage:
               return &selections->paths_with_palette_and_texture_page;
          case fme::PatternSelector::PathPatternsWithPupuID:
               return &selections->paths_with_pupu_id;
          case fme::PatternSelector::PathPatternsWithTexturePage:
               return &selections->paths_with_texture_page;
     }
     return nullptr;
}

[[nodiscard]] const std::vector<std::string> *fme::custom_paths_window::get_current_string_vector() const
{
     return get_current_string_vector_mutable();
}


[[nodiscard]] std::string *fme::custom_paths_window::get_current_string_value_from_index(std::vector<std::string> &strings, const int index)
{
     if (index < 0 || index >= std::ranges::ssize(strings))
     {
          return nullptr;
     }
     return &strings[index];
}

[[nodiscard]] const std::string *fme::custom_paths_window::get_current_string_value() const
{
     return get_current_string_value_mutable();
}

void fme::custom_paths_window::populate_input_pattern() const
{

     if (const auto *const strptr = get_current_string_value())
     {
          auto it = fmt::vformat_to_n(
            std::ranges::begin(m_input_pattern_string),
            std::ranges::size(m_input_pattern_string) - 1U,
            "{}",
            fmt::make_format_args(*strptr));
          *it.out = '\0';
     }
     else
     {
          std::ranges::fill(m_input_pattern_string, '\0');
     }
}

void fme::custom_paths_window::populate_test_output() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     m_output_tests.clear();
     for (const auto & test_data : m_tests)
     {
          std::string &output_test = m_output_tests.emplace_back(m_input_pattern_string.begin(), m_input_pattern_string.end());
          output_test              = test_data.replace_tags(output_test, selections);
     }
}

[[nodiscard]] bool fme::custom_paths_window::combo_selected_pattern() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return false;
     }
     if (!ImGui::BeginTable("##test input", 2, ImGuiTableFlags_SizingStretchProp))
     {
          return false;
     }
     ImGui::TableNextRow();
     ImGui::TableNextColumn();
     format_imgui_text("{}", "Selected Pattern: ");
     ImGui::TableNextColumn();
     const auto pop_table = glengine::ScopeGuard{ &ImGui::EndTable };
     using namespace std::string_view_literals;

     static const auto       values = std::array{ PatternSelector::OutputSwizzlePattern,
                                            PatternSelector::OutputDeswizzlePattern,
                                            PatternSelector::OutputMapPatternForSwizzle,
                                            PatternSelector::OutputMapPatternForDeswizzle,
                                            PatternSelector::PathPatternsCommonUpscale,
                                            PatternSelector::PathPatternsCommonUpscaleForMaps,
                                            PatternSelector::PathPatternsNoPaletteAndTexturePage,
                                            PatternSelector::PathPatternsWithPaletteAndTexturePage,
                                            PatternSelector::PathPatternsWithPupuID,
                                            PatternSelector::PathPatternsWithTexturePage };


     const GenericComboClass gcc    = { ""sv,
                                        []() { return values; },
                                     []() { return values | std::views::transform(AsString{}); },
                                     selections->current_pattern,
                                     generic_combo_settings{ .num_columns = 1 } };
     if (gcc.render())
     {
          selections->current_pattern_index = -1;
          populate_input_pattern();
          selections->update_configuration_key(ConfigKey::CurrentPattern);
          selections->update_configuration_key(ConfigKey::CurrentPatternIndex);
          return true;
     }
     return false;
}

void fme::custom_paths_window::save_pattern() const
{
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return;
     }
     if (auto *const strptr = get_current_string_value_mutable(); strptr)
     {
          *strptr = std::string{ m_input_pattern_string.data() };
     }
     switch (selections->current_pattern)
     {
          case PatternSelector::OutputSwizzlePattern:
               selections->update_configuration_key(ConfigKey::OutputSwizzlePattern);
               break;
          case PatternSelector::OutputDeswizzlePattern:
               selections->update_configuration_key(ConfigKey::OutputDeswizzlePattern);
               break;
          case PatternSelector::OutputMapPatternForSwizzle:
               selections->update_configuration_key(ConfigKey::OutputMapPatternForSwizzle);
               break;
          case PatternSelector::OutputMapPatternForDeswizzle:
               selections->update_configuration_key(ConfigKey::OutputMapPatternForDeswizzle);
               break;
          case fme::PatternSelector::PathPatternsCommonUpscale:
               selections->update_configuration_key(ConfigKey::PathPatternsCommonUpscale);
               break;
          case fme::PatternSelector::PathPatternsCommonUpscaleForMaps:
               selections->update_configuration_key(ConfigKey::PathPatternsCommonUpscaleForMaps);
               break;
          case fme::PatternSelector::PathPatternsNoPaletteAndTexturePage:
               selections->update_configuration_key(ConfigKey::PathPatternsNoPaletteAndTexturePage);
               break;
          case fme::PatternSelector::PathPatternsWithPaletteAndTexturePage:
               selections->update_configuration_key(ConfigKey::PathPatternsWithPaletteAndTexturePage);
               break;
          case fme::PatternSelector::PathPatternsWithPupuID:
               selections->update_configuration_key(ConfigKey::PathPatternsWithPupuID);
               break;
          case fme::PatternSelector::PathPatternsWithTexturePage:
               selections->update_configuration_key(ConfigKey::PathPatternsWithTexturePage);
               break;
     }
}

[[nodiscard]] bool fme::custom_paths_window::textbox_pattern() const
{

     if (!ImGui::BeginTable("##test input", 2, ImGuiTableFlags_SizingStretchProp))
     {
          return false;
     }
     ImGui::TableNextRow();
     ImGui::TableNextColumn();
     format_imgui_text("{}", "Pattern: ");
     ImGui::TableNextColumn();
     const auto pop_table = glengine::ScopeGuard{ &ImGui::EndTable };
     if (ImGui::InputText("##test input", m_input_pattern_string.data(), m_input_pattern_string.size()))
     {
          save_pattern();
          return true;
     }
     if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
     {
          if (ImGui::Selectable("Copy Pattern"))
          {
               const auto test_str = trim(std::string{ m_input_pattern_string.data(), m_input_pattern_string.size() });
               ImGui::SetClipboardText(test_str.data());
          }
          if (ImGui::Button("Close"))
               ImGui::CloseCurrentPopup();
          ImGui::EndPopup();
     }
     return false;
}

[[nodiscard]] bool fme::custom_paths_window::vector_pattern() const
{
     const auto *const vptr = get_current_string_vector();
     if (!vptr)
     {
          return false;
     }
     const auto selections = m_selections.lock();
     if (!selections)
     {
          spdlog::error("Failed to lock m_selections: shared_ptr is expired.");
          return false;
     }
     format_imgui_text("{}", "Pattern: ");
     constexpr static int cols = 2;
     if (!ImGui::BeginTable("##vector of patterns table", cols, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV))
     {
          return false;
     }
     const auto pop_table = glengine::ScopeGuard{ &ImGui::EndTable };

     // Setup columns
     ImGui::TableSetupColumn("Pattern", ImGuiTableColumnFlags_WidthStretch);// First column stretches to content
     ImGui::TableSetupColumn("Actions", ImGuiTableColumnFlags_WidthFixed, 120.0f);// Second column fixed width for buttons
     // ImGui::TableHeadersRow();

     bool                          r_val     = false;
     bool                          bg_color  = true;
     std::optional<std::ptrdiff_t> delete_me = std::nullopt;

     for (const auto &[index, str] : *vptr | std::ranges::views::enumerate)
     {
          ImGui::TableNextRow();
          if (bg_color)
          {
               ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkTeal.fade(-0.4F) });// Dark red
          }
          else
          {
               ImGui::TableSetBgColor(
                 ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkTeal.fade(-0.4F) });// Slightly lighter dark red
          }
          bg_color = !bg_color;
          ImGui::TableNextColumn();
          {
               const float availableWidth = ImGui::GetContentRegionAvail().x;
               ImGui::PushItemWidth(availableWidth);// Set textbox width to fill cell
               const auto pop_width = glengine::ScopeGuard(&ImGui::PopItemWidth);
               if (selections->current_pattern_index == static_cast<int>(index))
               {
                    if (ImGui::InputText("##test input", m_input_pattern_string.data(), m_input_pattern_string.size()))
                    {
                         save_pattern();
                         r_val = true;
                    }
               }
               else
               {
                    if (ImGui::Selectable(str.c_str()))
                    {
                         selections->current_pattern_index = static_cast<int>(index);
                         selections->update_configuration_key(ConfigKey::CurrentPatternIndex);
                         populate_input_pattern();
                         r_val = true;
                    }
                    else
                    {
                         tool_tip(str);
                    }
               }
          }
          ImGui::TableNextColumn();
          // Delete button
          if (ImGui::Button(fmt::format("{}##delete_{}", ICON_FA_TRASH, index).c_str()))
          {
               delete_me = index;
          }
          else
          {
               tool_tip("Delete");
          }
          ImGui::SameLine();// Keep buttons on the same line
          if (selections->current_pattern_index != static_cast<int>(index))
          {
               // Edit button
               if (ImGui::Button(fmt::format("{}##edit_{}", ICON_FA_PEN, index).c_str()))
               {
                    selections->current_pattern_index = static_cast<int>(index);
                    selections->update_configuration_key(ConfigKey::CurrentPatternIndex);
                    populate_input_pattern();
                    r_val = true;
               }
               else
               {
                    tool_tip("Edit");
               }
          }
          else
          {
               if (ImGui::Button(fmt::format("{}##cancel_{}", ICON_FA_XMARK, index).c_str()))
               {
                    selections->current_pattern_index = -1;
                    selections->update_configuration_key(ConfigKey::CurrentPatternIndex);
                    std::ranges::fill(m_input_pattern_string, '\0');
                    r_val = true;
               }
               else
               {
                    tool_tip("Cancel Edit");
               }
          }
          ImGui::SameLine();// Keep buttons on the same line

          // Copy button
          if (ImGui::Button(fmt::format("{}##copy_{}", ICON_FA_CLIPBOARD, index).c_str()))
          {
               if (selections->current_pattern_index != static_cast<int>(index))
               {
                    ImGui::SetClipboardText(str.c_str());
               }
               else
               {
                    const auto test_str = trim(std::string{ m_input_pattern_string.data(), m_input_pattern_string.size() });
                    ImGui::SetClipboardText(test_str.data());
               }
          }
          else
          {
               tool_tip("Copy to Clipboard");
          }
     }
     ImGui::TableNextRow();
     if (bg_color)
     {
          ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkTeal.fade(-0.4F) });// Dark red
     }
     else
     {
          ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkTeal.fade(-0.4F) });// Slightly lighter dark red
     }
     ImGui::TableNextColumn();
     const char *add        = "Add New Pattern";
     const auto  add_action = [&]() {
          if (auto *const mut_vptr = get_current_string_vector_mutable(); mut_vptr)
          {
               mut_vptr->emplace_back();
               selections->current_pattern_index = static_cast<int>(std::ranges::ssize(*mut_vptr) - 1);
               selections->update_configuration_key(ConfigKey::CurrentPatternIndex);
               populate_input_pattern();
               save_pattern();
               r_val = true;
          }
     };
     if (ImGui::Selectable(add))
     {
          add_action();
     }
     ImGui::TableNextColumn();
     if (ImGui::Button(fmt::format("{}##add", ICON_FA_PLUS).c_str()))
     {
          add_action();
     }
     else
     {
          tool_tip(add);
     }

     if (delete_me.has_value())
     {
          if (auto *const mut_vptr = get_current_string_vector_mutable(); mut_vptr)
          {
               if (selections->current_pattern_index == static_cast<int>(delete_me.value()))
               {
                    selections->current_pattern_index = -1;
                    selections->update_configuration_key(ConfigKey::CurrentPatternIndex);
                    std::ranges::fill(m_input_pattern_string, '\0');
               }
               else if (selections->current_pattern_index > static_cast<int>(delete_me.value()))
               {
                    selections->current_pattern_index--;
                    selections->update_configuration_key(ConfigKey::CurrentPatternIndex);
               }
               auto iter = mut_vptr->begin();
               std::ranges::advance(iter, delete_me.value());
               mut_vptr->erase(iter);
               save_pattern();
               r_val = true;
          }
     }
     return r_val;
}

[[nodiscard]] bool fme::custom_paths_window::button_add_seperator() const
{
     static constexpr char seperator = std::filesystem::path::preferred_separator;
     if (ImGui::Button("Add Separator"))
     {
          size_t current_length = std::strlen(m_input_pattern_string.data());
          if (current_length + 1 < m_input_pattern_string.size())
          {
               std::size_t len = std::strlen(m_input_pattern_string.data());
               if (len + 1 < m_input_pattern_string.size())
               {
                    m_input_pattern_string[len]     = seperator;
                    m_input_pattern_string[len + 1] = '\0';
               }
               else
               {
                    spdlog::warn("m_input_pattern_string is full");
               }
               save_pattern();
               return true;
          }
     }
     else
     {
          tool_tip(std::string{ seperator });
     }
     return false;
}

[[nodiscard]] bool fme::custom_paths_window::button_remove_last_key() const
{
     if (ImGui::Button("Remove Last {Key}"))
     {
          std::string_view path_view(m_input_pattern_string.data());

          // Find the last occurrence of '{' and '}'
          size_t           last_open  = path_view.rfind('{');
          size_t           last_close = path_view.rfind('}');

          // Ensure they are a valid pair at the end
          if (
            last_open != std::string_view::npos && last_close != std::string_view::npos && last_open < last_close
            && last_close == path_view.size() - 1)
          {
               // Remove the last {key_value}
               m_input_pattern_string[last_open] = '\0';
               save_pattern();
               return true;
          }
     }
     return false;
}

[[nodiscard]] bool fme::custom_paths_window::child_keys() const
{
     bool override_changed = false;
     if (!ImGui::CollapsingHeader("Keys", ImGuiTreeNodeFlags_DefaultOpen))
     {
          return false;
     }

     ImGui::BeginDisabled(get_current_string_value() == nullptr);
     const auto pop_disabled = glengine::ScopeGuard(&ImGui::EndDisabled);
     format_imgui_wrapped_text(
       "{}",
       "Click a {key} to add it to the pattern text box.\n"
       "You can use extra braces like {{key}} to create prefixes or suffixes, e.g., {prefix{key}suffix}.\n"
       "These will only appear if the {key} has a value.\n\n"
       "Right-click a {key} to access a context menu for copying key values.");
     {
          const auto pop_child = glengine::ScopeGuard{ &ImGui::EndChild };
          if (!ImGui::BeginChild("##scrollingKeys", m_scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
          {
               return false;
          }
          static constexpr int cols = 3;
          if (!ImGui::BeginTable("##scrollingKeys", cols, ImGuiTableFlags_BordersInnerV))
          {
               return false;
          }
          const auto pop_table = glengine::ScopeGuard{ &ImGui::EndTable };
          bool       bg_color  = true;
          for (const auto &[index, pair] :
               std::ranges::views::enumerate(std::ranges::views::zip(key_value_data::keys::all_keys, key_value_data::keys::all_tooltips)))
          {
               const auto &[key, tooltip] = pair;

               if (index % cols == 0)
               {
                    ImGui::TableNextRow();
                    if (bg_color)
                    {
                         ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkRed.fade(-0.4F) });// Dark red
                    }
                    else
                    {
                         ImGui::TableSetBgColor(
                           ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkRed.fade(-0.4F) });// Slightly lighter dark red
                    }
                    bg_color = !bg_color;
               }
               ImGui::TableNextColumn();
               const auto  pop_id       = PushPopID();
               std::string key_brackets = fmt::format("{{{}}}", key);
               if (ImGui::Selectable(key_brackets.data()))
               {
                    // Ensure null-termination and safe appending
                    static constexpr size_t max_size       = s_input_string_size_value - 1;// Leave space for null terminator
                    size_t                  current_length = std::strlen(m_input_pattern_string.data());
                    size_t                  append_length  = key_brackets.size();

                    if (current_length + append_length < max_size)
                    {
                         std::strncat(m_input_pattern_string.data(), key_brackets.data(), append_length);
                    }
                    else
                    {
                         std::strncat(m_input_pattern_string.data(), key_brackets.data(), max_size - current_length);
                    }

                    save_pattern();
                    override_changed = true;
               }
               if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
               {
                    if (ImGui::Selectable("Copy Key"))
                    {
                         ImGui::SetClipboardText(key_brackets.data());
                    }
                    if (ImGui::Selectable("Copy All Keys"))
                    {
                         using namespace std::string_view_literals;
                         auto combined_keys = fmt::format("{{{}}}", fmt::join(key_value_data::keys::all_keys, "}\n{"sv));
                         ImGui::SetClipboardText(combined_keys.data());
                    }
                    if (ImGui::Button("Close"))
                         ImGui::CloseCurrentPopup();
                    ImGui::EndPopup();
               }
               const auto current_tooltip = fmt::format("{}\nClick to add {}\nRight-click to open popup", tooltip, key_brackets);
               ImGui::SetItemTooltip("%s", current_tooltip.data());
          }
     }
     const auto same_line = []() -> bool {
          ImGui::SameLine();
          return {};
     };
     if (std::ranges::any_of(std::array{ button_remove_last_key(), same_line(), button_add_seperator() }, std::identity{}))
     {
          override_changed = true;
     }
     return override_changed;
}

[[nodiscard]] bool fme::custom_paths_window::child_test_output() const
{
     bool override_changed = false;
     if (!ImGui::CollapsingHeader("Test Output", ImGuiTreeNodeFlags_DefaultOpen))
     {
          return false;
     }

     ImGui::BeginDisabled(get_current_string_value() == nullptr);
     const auto pop_disabled = glengine::ScopeGuard(&ImGui::EndDisabled);

     format_imgui_wrapped_text(
       "{}",
       "This second section displays what the pattern will look like with various test inputs.\n"
       "{ff8_path}/Textures could output something like c:\\ff8\\Textures.\n"
       "Right-click a key to access a context menu for copying test output.");

     const auto pop_child = glengine::ScopeGuard{ &ImGui::EndChild };
     if (!ImGui::BeginChild("##scrollingTest", m_scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
     {
          return false;
     }
     static constexpr int cols = 2;
     if (!ImGui::BeginTable("##scrollingTest", cols, ImGuiTableFlags_BordersInnerV))
     {
          return false;
     }
     const auto pop_table = glengine::ScopeGuard{ &ImGui::EndTable };
     bool       bg_color  = true;
     for (const auto &[index, test_str] : m_output_tests | std::ranges::views::enumerate)
     {
          if (index % cols == 0)
          {
               ImGui::TableNextRow();
               if (bg_color)
               {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableDarkGreen.fade(-0.4F) });// Dark red
               }
               else
               {
                    ImGui::TableSetBgColor(
                      ImGuiTableBgTarget_RowBg0, ImU32{ colors::TableLightDarkGreen.fade(-0.4F) });// Slightly lighter dark red
               }
               bg_color = !bg_color;
          }
          ImGui::TableNextColumn();
          const auto pop_id = PushPopID();
          (void)ImGui::Selectable(test_str.data());
          if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
          {
               if (ImGui::Selectable("Copy Test Output"))
               {
                    ImGui::SetClipboardText(test_str.data());
               }
               if (ImGui::Selectable("Copy All Test Output"))
               {
                    using namespace std::string_literals;
                    auto combined_paths = m_output_tests | std::ranges::views::join_with("\n"s) | std::ranges::to<std::string>();
                    ImGui::SetClipboardText(combined_paths.data());
               }
               if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
               ImGui::EndPopup();
          }
          ImGui::SetItemTooltip("Right-click to open popup");
     }
     return override_changed;
}


fme::custom_paths_window::custom_paths_window(std::weak_ptr<Selections> input_selections)
  : m_selections{ input_selections }
  , m_output_tests{ m_tests.size() }
{
     populate_input_pattern();
     populate_test_output();
}

fme::custom_paths_window &fme::custom_paths_window::refresh(std::weak_ptr<Selections> input_selections)
{
     m_selections = input_selections;
     populate_input_pattern();

     return refresh();
}

fme::custom_paths_window &fme::custom_paths_window::refresh()
{
     m_changed = true;
     return *this;
}


void fme::custom_paths_window::render() const
{
     m_scrolling_child_size = ImVec2(0, ImGui::GetFrameHeightWithSpacing() * 7 + 30);
     using namespace std::string_view_literals;
     bool override_changed = { false };
     auto selections       = m_selections.lock();
     if (!selections || !selections->display_custom_paths_window)
     {
          return;
     }
     bool      &visible     = selections->display_custom_paths_window;
     const auto pop_visible = glengine::ScopeGuard{ [&selections, &visible, was_visable = visible] {
          if (was_visable != visible)
          {
               selections->update_configuration_key(ConfigKey::DisplayCustomPathsWindow);
          }
     } };
     const auto pop_end = glengine::ScopeGuard(&ImGui::End);
     if (!ImGui::Begin(gui_labels::custom_paths_window.data(), &visible))
     {
          return;
     }
     const auto pop_changed = glengine::ScopeGuard([this, &override_changed]() { m_changed = override_changed; });
     ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.F);
     ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.F, 2.F));
     ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.F, 4.F));
     const auto color = ImVec4(0.5F, 0.5F, 0.5F, 1.F);
     ImGui::PushStyleColor(ImGuiCol_TableBorderLight, color);
     ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, color);
     const auto pop_styles = glengine::ScopeGuard{ []() {
          ImGui::PopStyleColor(2);
          ImGui::PopStyleVar(3);
     } };
     switch (vector_or_string())
     {
          case vector_or_string_t::string: {
               if (std::ranges::any_of(std::array{ combo_selected_pattern(), textbox_pattern() }, std::identity{}))
               {
                    override_changed = true;
               }
          }
          break;
          case vector_or_string_t::vector: {
               if (std::ranges::any_of(std::array{ combo_selected_pattern(), vector_pattern() }, std::identity{}))
               {
                    override_changed = true;
               }
          }
          default:
          case vector_or_string_t::unknown: {
               spdlog::warn("vector_or_string() is unknown type");
          }
          break;
     }

     if (child_keys())
     {
          override_changed = true;
     }
     if (m_changed)
     {
          populate_test_output();
     }
     if (child_test_output())
     {
          override_changed = true;
     }
}