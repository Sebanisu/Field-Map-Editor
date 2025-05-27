#ifndef A68AC487_FAA2_4B0F_9C15_BB16CD240B56
#define A68AC487_FAA2_4B0F_9C15_BB16CD240B56

#include "format_imgui_text.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"
#include "key_value_data.hpp"
#include "push_pop_id.hpp"
#include "scope_guard.hpp"
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
#include <string_view>
#include <tl/string.hpp>

namespace fme
{
struct custom_paths_window
{
   private:
     std::weak_ptr<Selections>                                           m_selections              = {};
     mutable bool                                                        m_changed                 = {};

     static constexpr std::uint32_t                                      s_options_size_value      = { 4U };
     static constexpr std::size_t                                        s_input_string_size_value = { 256U };
     mutable std::array<char, s_input_string_size_value>                 m_input_pattern_string    = {};
     
     static inline const auto         m_tests                = std::to_array<key_value_data>({
       { .field_name = "ecmall1", .ext = ".ca" },// Basic field_name + ext match
       { .field_name = "ecmall1", .ext = ".jsm", .language_code = open_viii::LangT::en, .pupu_id = 987654U },// Field with language suffix
       { .field_name = "ecmall1", .ext = ".msd", .language_code = open_viii::LangT::jp, .pupu_id = 543210U },// Another language case
       { .field_name    = "ecmall1",
                                .ext           = ".map",
                                .language_code = open_viii::LangT::de,
                                .pupu_id       = 234567U },// Different language, different ext
       { .field_name = "ecmall1", .ext = ".mim", .language_code = open_viii::LangT::fr, .pupu_id = 890123U },// Another unique case
       { .field_name = "ecmall1", .ext = ".inf", .language_code = open_viii::LangT::it, .pupu_id = 456789U },// Italian match
       { .field_name = "ecmall1", .ext = ".sfx", .pupu_id = 678901U },// No language, unique ext
       { .field_name = "ecmall1", .ext = ".tdw", .language_code = open_viii::LangT::es, .pupu_id = 321098U },// Spanish case
       { .field_name = "cwwood2", .ext = ".one" },// `chara.one` match
       { .field_name    = "cwwood2",
                                .ext           = ".one",
                                .language_code = open_viii::LangT::jp,
                                .pupu_id       = 765432U },// `chara_{2_letter_lang}.one` match
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


     mutable std::vector<std::string> m_output_tests         = std::vector<std::string>{ m_tests.size() };
     static constexpr auto            m_index_values         = std::ranges::views::iota(std::uint32_t{}, s_options_size_value);
     mutable ImVec2                   m_scrolling_child_size = {};

     [[nodiscard]] std::string                                    *get_current_string_value_mutable() const;
     [[nodiscard]] const std::string                              *get_current_string_value() const;

     void                                                          populate_input_pattern() const;
     void                                                          populate_test_output() const;

     [[nodiscard]] bool                                            combo_selected_pattern() const;

     void                                                          save_pattern() const;

     [[nodiscard]] bool                                            textbox_pattern() const;
     [[nodiscard]] bool                                            button_add_seperator() const;
     [[nodiscard]] bool                                            button_remove_last_key() const;
     [[nodiscard]] bool                                            child_keys() const;
     [[nodiscard]] bool                                            child_test_output() const;

   public:
     custom_paths_window(std::weak_ptr<Selections> input_selections);
     custom_paths_window &refresh(std::weak_ptr<Selections> input_selections);
     custom_paths_window &refresh();
     void                 render() const;
};
}// namespace fme

#endif /* A68AC487_FAA2_4B0F_9C15_BB16CD240B56 */
