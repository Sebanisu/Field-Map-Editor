#ifndef A68AC487_FAA2_4B0F_9C15_BB16CD240B56
#define A68AC487_FAA2_4B0F_9C15_BB16CD240B56

#include "format_imgui_text.hpp"
#include "generic_combo.hpp"
#include "gui_labels.hpp"
#include "key_value_data.hpp"
#include "Selections.hpp"
#include "tool_tip.hpp"
#include <array>
#include <cstdint>
#include <ctre.hpp>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <functional>
#include <glengine/Renderable.hpp>
#include <glengine/ScopeGuard.hpp>
#include <imgui.h>
#include <imgui_utils/ImGuiPushID.hpp>
#include <open_viii/strings/LangCommon.hpp>
#include <optional>
#include <ranges>
#include <string_view>
#include <tl/string.hpp>

namespace fme
{
struct custom_paths_window
{

     using Callback = std::move_only_function<void(ConfigKey)>;

   private:
     static constexpr std::uint32_t s_options_size_value      = { 4U };
     static constexpr std::size_t   s_input_string_size_value = { 256U };
     static constexpr auto          m_index_values
       = std::ranges::views::iota(std::uint32_t{}, s_options_size_value);

     std::weak_ptr<Selections>                           m_selections = {};
     mutable bool                                        m_changed    = {};
     mutable std::array<char, s_input_string_size_value> m_input_pattern_string
       = {};
     mutable std::vector<std::string>  m_output_tests         = {};
     mutable ImVec2                    m_scrolling_child_size = {};
     mutable std::vector<Callback>     callbacks              = {};


     /**
      * @brief Retrieves a pointer to a string at the given index, with clamping
      * and selection handling.
      *
      * This function returns a pointer to the string at the specified index,
      * clamping the index into the valid range `[0, size-1]`. If the index is
      * `-1` (indicating that nothing is currently selected) or if the vector is
      * empty, the function returns `nullptr`.
      *
      * @param strings Reference to the vector of strings to access.
      * @param index   The index of the string to retrieve. A value of `-1`
      * indicates no selection.
      *
      * @return Pointer to the string at the clamped index, or `nullptr` if
      *         `index == -1` or `strings` is empty.
      *
      * @note The returned pointer remains valid only as long as the underlying
      *       vector is not modified in a way that invalidates
      * references/pointers (e.g., reallocation).
      */
     [[nodiscard]] static std::string *get_current_string_value_from_index(
       std::vector<std::string> &strings,
       const int                 index);

     /**
      * @brief Retrieves the currently selected string (mutable).
      *
      * Returns a pointer to the currently selected string, allowing
      * modification. The returned pointer may either reference a string stored
      * in an internal vector or a standalone string.
      *
      * @return Pointer to the selected string, or `nullptr` if nothing is
      * selected.
      *
      * @note The pointer remains valid only as long as the owning container
      *       (vector or standalone storage) is not modified in a way that
      *       invalidates references/pointers.
      */
     [[nodiscard]] std::string       *get_current_string_value_mutable() const;

     /**
      * @brief Retrieves the currently selected string (read-only).
      *
      * Returns a const pointer to the currently selected string, preventing
      * modification. The returned pointer may either reference a string stored
      * in an internal vector or a standalone string.
      *
      * @return Const pointer to the selected string, or `nullptr` if nothing is
      * selected.
      *
      * @note The pointer remains valid only as long as the owning container
      *       (vector or standalone storage) is not modified in a way that
      *       invalidates references/pointers.
      */
     [[nodiscard]] const std::string *get_current_string_value() const;

     /**
      * @brief Retrieves the currently selected vector of strings.
      *
      * Returns a pointer to the underlying vector of strings if the current
      * selection represents a vector. Otherwise, if the current selection
      * does not correspond to a vector, this function returns `nullptr`.
      *
      * @return Const pointer to the current vector of strings, or `nullptr`
      *         if the selection is not a vector.
      *
      * @note The returned pointer remains valid only as long as the owning
      *       container is not modified in a way that invalidates references
      *       or pointers.
      */
     [[nodiscard]] const std::vector<std::string> *
       get_current_string_vector() const;

     /**
      * @brief Retrieves the currently selected vector of strings (mutable).
      *
      * Returns a pointer to the underlying vector of strings if the current
      * selection represents a vector. Otherwise, if the current selection
      * does not correspond to a vector, this function returns `nullptr`.
      *
      * @return Pointer to the current vector of strings, or `nullptr`
      *         if the selection is not a vector.
      *
      * @note The returned pointer allows modification of the underlying
      *       vector, and remains valid only as long as the owning container
      *       is not modified in a way that invalidates references or pointers.
      */
     [[nodiscard]] std::vector<std::string>                            *
       get_current_string_vector_mutable() const;

     /**
      * @brief Determines whether the current selection is a vector or a single
      * string.
      *
      * This function inspects the current selection (via the associated
      * PatternSelector) and classifies it as either a vector of strings
      * or a single string. The result is returned as a value of the
      * VectorOrString enumeration.
      *
      * @return A VectorOrString value indicating the type of the current
      *         selection:
      *         - VectorOrString::vector  if the selection is a vector of
      * strings
      *         - VectorOrString::string  if the selection is a single string
      *
      * @throws std::runtime_error If the selection cannot be classified
      *         (e.g., due to an unhandled PatternSelector value). This case
      *         indicates a coding error and should not occur in normal usage.
      *
      * @note In future, this could be made to return VectorOrString::unknown
      *       instead of throwing, if graceful handling of unexpected values
      *       is desired.
      */
     [[nodiscard]] VectorOrString vector_or_string() const;

     /**
      * @brief Saves the current pattern.
      *
      * Persists the currently selected pattern based on the active
      * PatternSelector. The exact storage mechanism depends on the
      * configuration of the class.
      *
      * @throws std::runtime_error If an unhandled PatternSelector value is
      *         encountered. This indicates a coding error and should be
      *         highly unlikely during normal execution.
      */
     void                         save_pattern() const;

     /**
      * @brief Populates ImGui controls for pattern input.
      *
      * Sets up the input-related ImGui controls based on the current
      * pattern state. This prepares the UI for editing or selecting patterns.
      */
     void                         populate_input_pattern() const;

     /**
      * @brief Populates ImGui controls with test or sample output data.
      *
      * Uses the current pattern to generate example/test data and updates
      * the ImGui UI to display it.
      */
     void                         populate_test_output() const;

     /**
      * @brief Renders a combo box to select the type of pattern.
      *
      * Uses PatternSelector to determine available options and updates
      * the selection if the user chooses a different pattern type.
      *
      * @return True if the user changed the selection, false otherwise.
      */
     [[nodiscard]] bool           combo_selected_pattern() const;

     /**
      * @brief Renders a text box for editing a single string pattern.
      *
      * Shown only when the current selection represents a single string.
      *
      * @return True if the text box value was modified, false otherwise.
      */
     [[nodiscard]] bool           textbox_pattern() const;

     /**
      * @brief Renders a table for editing a vector of strings pattern.
      *
      * Shown only when the current selection represents a vector of strings.
      *
      * @return True if any value in the table was modified, false otherwise.
      */
     [[nodiscard]] bool           vector_pattern() const;

     /**
      * @brief Adds a separator character to the pattern.
      *
      * Adds either a backslash (\) or forward slash (/). These are
      * internally normalized to a forward slash (/).
      *
      * @return True if the separator button was clicked, false otherwise.
      */
     [[nodiscard]] bool           button_add_seperator() const;

     /**
      * @brief Removes the last key from the pattern.
      *
      * Searches for a key enclosed in {} in the current pattern and
      * removes it.
      *
      * @return True if a key was removed, false otherwise.
      */
     [[nodiscard]] bool           button_remove_last_key() const;

     /**
      * @brief Shows a scrollable ImGui child containing available keys.
      *
      * Keys can be clicked to add them to the current pattern.
      *
      * @return True if the user clicked any key to modify the pattern,
      *         false otherwise.
      */
     [[nodiscard]] bool           child_keys() const;

     /**
      * @brief Shows a scrollable ImGui child containing generated test output.
      *
      * Displays the data generated by populate_test_output() for reference.
      *
      * @return True if any interaction modifies the UI state, false otherwise.
      */
     [[nodiscard]] bool           child_test_output() const;

   public:
     /**
      * @brief Registers a callback to be notified when configuration keys
      * change.
      *
      * This function stores the provided callback so that it will be invoked
      * whenever a configuration change is notified through @ref notify.
      *
      * The callback receives the @ref ConfigKey of the configuration that
      * changed. It can then decide whether to act on that key.
      *
      * @param cb The callback to register. Ownership of the callback is
      *           transferred and stored internally. Callbacks must be
      *           move-constructible.
      *
      * @note Callbacks are invoked in the order they are registered.
      *       Currently there is no way to unregister a callback.
      */
     void register_callback(Callback cb);

     /**
      * @brief Notifies all registered callbacks of a configuration change.
      *
      * This function calls every registered callback with the given
      * @ref ConfigKey. Each callback may choose to ignore the key or
      * perform some action in response.
      *
      * @param key The configuration key that changed.
      *
      * @note All callbacks are invoked synchronously in the order they
      *       were registered.
      */
     void notify(ConfigKey key) const;

     /**
      * @brief Constructs a CustomPathsWindow with the given selections.
      *
      * @param input_selections A weak pointer to a Selections object, which
      *        acts as a configuration wrapper. Typically set once and used
      *        throughout the lifetime of the window.
      */
     custom_paths_window(std::weak_ptr<Selections> input_selections);

     /**
      * @brief Refreshes the window state using a new Selections pointer.
      *
      * Updates the internal selections pointer, marks the window as changed,
      * and triggers any dependent state updates such as regenerating test
      * output via populate_test_output().
      *
      * @param input_selections A weak pointer to a Selections object.
      * @return Reference to this CustomPathsWindow for chaining.
      */
     custom_paths_window &refresh(std::weak_ptr<Selections> input_selections);

     /**
      * @brief Refreshes the window state using the current selections.
      *
      * Marks the window as changed and triggers any dependent state updates
      * such as regenerating test output via populate_test_output().
      *
      * @return Reference to this CustomPathsWindow for chaining.
      */
     custom_paths_window &refresh();

     /**
      * @brief Renders the window using ImGui controls.
      *
      * Draws all input, combo boxes, tables, and children as configured
      * by the current pattern state. Typically called every frame.
      */
     void                 render() const;
};
}// namespace fme
// static_assert(glengine::Renderable<fme::custom_paths_window>);
#endif /* A68AC487_FAA2_4B0F_9C15_BB16CD240B56 */
