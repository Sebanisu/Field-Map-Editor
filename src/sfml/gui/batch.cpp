//
// Created by pcvii on 3/7/2023.
//

#include "batch.hpp"
struct as_string
{
     template<typename T>
     std::string operator()(const T &value) const
     {
          return fmt::format("{}", value);
     }
};
void batch::combo_input_type(int &imgui_id)
{
     static const auto values = std::array{ input_types::mim, input_types::deswizzle, input_types::swizzle };
     if (fme::generic_combo(
           imgui_id, "Input Type", []() { return values; }, []() { return values | std::views::transform(as_string{}); }, m_input_type))
     {
     }
}
void batch::combo_output_type(int &imgui_id)
{
     static const auto values = std::array{ output_types::deswizzle, output_types::swizzle };
     if (fme::generic_combo(
           imgui_id, "Output Type", []() { return values; }, []() { return values | std::views::transform(as_string{}); }, m_output_type))
     {
     }
}
void batch::combo_compact_type(int &imgui_id)
{
     static const auto values = std::array{ compact_type::rows, compact_type::all, compact_type::map_order };
     if (fme::generic_combo(
           imgui_id,
           gui_labels::compact,
           []() { return values; },
           []() { return values | std::views::transform(as_string{}); },
           [&]() -> auto & { return m_compact_type; }))
     {
     }
}
void batch::combo_flatten_type(int &imgui_id)
{
     static const auto values = std::array{ flatten_type::bpp, flatten_type::palette, flatten_type::both };
     if (fme::generic_combo(
           imgui_id,
           gui_labels::flatten,
           []() { return values; },
           []() { return values | std::views::transform(as_string{}); },
           [&]() -> auto & { return m_flatten_type; }))
     {
     }
}
