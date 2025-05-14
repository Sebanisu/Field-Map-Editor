#include "custom_paths_window.hpp"
#include "as_string.hpp"
#include "formatters.hpp"

[[nodiscard]] std::array<std::string *, fme::custom_paths_window::s_options_size_value>
  fme::custom_paths_window::get_selections_values() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          return {};
     }
     return { &selections->output_swizzle_pattern,
              &selections->output_deswizzle_pattern,
              &selections->output_map_pattern_for_swizzle,
              &selections->output_map_pattern_for_deswizzle };
}

[[nodiscard]] std::string &fme::custom_paths_window::get_current_string_value_mutable() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          throw;
     }
     return *m_selections_values[std::to_underlying(selections->current_pattern)];
}

[[nodiscard]] const std::string &fme::custom_paths_window::get_current_string_value() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          static std::string dummy{};
          return dummy;
     }
     return *m_selections_values[std::to_underlying(selections->current_pattern)];
}

void fme::custom_paths_window::populate_input_pattern() const
{

     auto it = fmt::vformat_to_n(
       std::ranges::begin(m_input_pattern_string),
       std::ranges::size(m_input_pattern_string) - 1U,
       "{}",
       fmt::make_format_args(get_current_string_value()));
     *it.out = '\0';
}

void fme::custom_paths_window::populate_test_output() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          return;
     }
     m_output_tests.clear();
     for (const auto test_data : m_tests)
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
     const auto pop_table = scope_guard{ &ImGui::EndTable };
     using namespace std::string_view_literals;

     static const auto       values = std::array{ PatternSelector::OutputSwizzlePattern,
                                            PatternSelector::OutputDeswizzlePattern,
                                            PatternSelector::OutputMapPatternForSwizzle,
                                            PatternSelector::OutputMapPatternForDeswizzle };


     const GenericComboClass gcc    = {
          ""sv, []() { return values; }, []() { return values | std::views::transform(AsString{}); }, selections->current_pattern
     };
     if (gcc.render())
     {
          populate_input_pattern();
          save_pattern();
          return true;
     }
     return false;
}

void fme::custom_paths_window::save_pattern() const
{
     auto selections = m_selections.lock();
     if (!selections)
     {
          return;
     }
     selections->update_configuration_key(ConfigKey::CurrentPattern);
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
     const auto pop_table = scope_guard{ &ImGui::EndTable };
     if (ImGui::InputText("##test input", m_input_pattern_string.data(), m_input_pattern_string.size()))
     {
          save_pattern();
          return true;
     }
     if (ImGui::BeginPopupContextItem())// <-- use last item id as popup id
     {
          if (ImGui::Selectable("Copy Pattern"))
          {
               const auto trim = [](const std::string &str) -> std::string {
                    auto start = str.find_first_not_of(" \t\n\r\f\v");
                    if (start == std::string::npos)
                         return "";// Empty or all spaces

                    auto end = str.find_last_not_of(" \t\n\r\f\v");
                    return str.substr(start, end - start + 1);
               };
               const auto test_str = trim(std::string{ m_input_pattern_string.data(), m_input_pattern_string.size() });
               ImGui::SetClipboardText(test_str.data());
          }
          if (ImGui::Button("Close"))
               ImGui::CloseCurrentPopup();
          ImGui::EndPopup();
     }
     return false;
}

[[nodiscard]] bool fme::custom_paths_window::button_add_seperator() const
{
     static constexpr auto seperator = std::array<char, 2>{ std::filesystem::path::preferred_separator, '\0' };
     if (ImGui::Button("Add Separator"))
     {
          size_t current_length = std::strlen(m_input_pattern_string.data());
          if (current_length + 1 < m_input_pattern_string.size())
          {
               std::strncat(m_input_pattern_string.data(), seperator.data(), 1);
               save_pattern();
               return true;
          }
     }
     else
     {
          tool_tip(std::string_view{ seperator });
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

     format_imgui_wrapped_text(
       "{}",
       "Click a {key} to add it to the pattern text box.\n"
       "You can use extra braces like {{key}} to create prefixes or suffixes, e.g., {prefix{key}suffix}.\n"
       "These will only appear if the {key} has a value.\n\n"
       "Right-click a {key} to access a context menu for copying key values.");

     {
          const auto pop_child = scope_guard{ &ImGui::EndChild };
          if (!ImGui::BeginChild("##scrollingKeys", m_scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
          {
               return false;
          }
          static constexpr int cols = 3;
          if (!ImGui::BeginTable("##scrollingKeys", cols, ImGuiTableFlags_BordersInnerV))
          {
               return false;
          }
          const auto pop_table = scope_guard{ &ImGui::EndTable };
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

     format_imgui_wrapped_text(
       "{}",
       "This second section displays what the pattern will look like with various test inputs.\n"
       "{ff8_path}/Textures could output something like c:\\ff8\\Textures.\n"
       "Right-click a key to access a context menu for copying test output.");

     const auto pop_child = scope_guard{ &ImGui::EndChild };
     if (!ImGui::BeginChild("##scrollingTest", m_scrolling_child_size, ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar))
     {
          return false;
     }
     static constexpr int cols = 2;
     if (!ImGui::BeginTable("##scrollingTest", cols, ImGuiTableFlags_BordersInnerV))
     {
          return false;
     }
     const auto pop_table = scope_guard{ &ImGui::EndTable };
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
  : m_selections(input_selections)
{
     populate_input_pattern();
     populate_test_output();
}

fme::custom_paths_window &fme::custom_paths_window::refresh(std::weak_ptr<Selections> input_selections)
{
     m_selections        = input_selections;
     m_selections_values = get_selections_values();
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
     const auto pop_end = scope_guard(&ImGui::End);
     if (!ImGui::Begin(gui_labels::custom_paths_window.data()))
     {
          return;
     }
     const auto pop_changed = scope_guard([this, &override_changed]() { m_changed = override_changed; });


     if (std::ranges::any_of(std::array{ combo_selected_pattern(), textbox_pattern() }, std::identity{}))
     {
          override_changed = true;
     }


     ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 3.F);
     ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2.F, 2.F));
     ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(4.F, 4.F));
     const auto color = ImVec4(0.5F, 0.5F, 0.5F, 1.F);
     ImGui::PushStyleColor(ImGuiCol_TableBorderLight, color);
     ImGui::PushStyleColor(ImGuiCol_TableBorderStrong, color);
     const auto pop_styles = scope_guard{ []() {
          ImGui::PopStyleColor(2);
          ImGui::PopStyleVar(3);
     } };
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