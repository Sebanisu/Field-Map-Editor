#ifndef A68AC487_FAA2_4B0F_9C15_BB16CD240B56
#define A68AC487_FAA2_4B0F_9C15_BB16CD240B56

#include "format_imgui_text.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"
#include "key_value_data.hpp"
#include "push_pop_id.hpp"
#include "Selections.hpp"
#include "tool_tip.hpp"
#include <array>
#include <cstdint>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <imgui.h>
#include <open_viii/strings/LangCommon.hpp>
#include <optional>
#include <ranges>
#include <ScopeGuard.hpp>
#include <string_view>
#include <tl/string.hpp>

namespace fme
{
struct custom_paths_window
{
     enum struct vector_or_string_t : std::uint8_t
     {
          unknown,
          vector,
          string,
     };

   private:
     static constexpr std::uint32_t                      s_options_size_value      = { 4U };
     static constexpr std::size_t                        s_input_string_size_value = { 256U };
     static constexpr auto                               m_index_values = std::ranges::views::iota(std::uint32_t{}, s_options_size_value);

     std::weak_ptr<Selections>                           m_selections   = {};
     mutable bool                                        m_changed      = {};
     mutable std::array<char, s_input_string_size_value> m_input_pattern_string = {};
     mutable std::vector<std::string>                    m_output_tests         = {};
     mutable ImVec2                                      m_scrolling_child_size = {};

     [[nodiscard]] static std::string             *get_current_string_value_from_index(std::vector<std::string> &strings, const int index);
     [[nodiscard]] const std::string              *get_current_string_value() const;
     [[nodiscard]] const std::vector<std::string> *get_current_string_vector() const;
     [[nodiscard]] std::string                    *get_current_string_value_mutable() const;
     [[nodiscard]] std::vector<std::string>       *get_current_string_vector_mutable() const;
     [[nodiscard]] vector_or_string_t              vector_or_string() const;

     void                                          populate_input_pattern() const;
     void                                          populate_test_output() const;
     void                                          save_pattern() const;

     [[nodiscard]] bool                            combo_selected_pattern() const;
     [[nodiscard]] bool                            textbox_pattern() const;
     [[nodiscard]] bool                            vector_pattern() const;
     [[nodiscard]] bool                            button_add_seperator() const;
     [[nodiscard]] bool                            button_remove_last_key() const;
     [[nodiscard]] bool                            child_keys() const;
     [[nodiscard]] bool                            child_test_output() const;

   public:
     custom_paths_window(std::weak_ptr<Selections> input_selections);
     custom_paths_window &refresh(std::weak_ptr<Selections> input_selections);
     custom_paths_window &refresh();
     void                 render() const;
};
}// namespace fme

#endif /* A68AC487_FAA2_4B0F_9C15_BB16CD240B56 */
