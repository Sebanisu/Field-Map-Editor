//
// Created by pcvii on 3/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPBLENDS_HPP
#define FIELD_MAP_EDITOR_MAPBLENDS_HPP
class MapBlends
{
public:
  [[nodiscard]] bool on_im_gui_update() const
  {
    bool changed = false;
    if (ImGui::CollapsingHeader("Add Blend"))
    {
      const auto un_indent = glengine::ImGuiIndent();
      if (ImGui::Checkbox("Percent Blends (50%,25%)", &m_enable_percent_blend))
      {
        changed = true;
      }
      const auto pop = glengine::ImGuiPushId();
      if (blend_combos(m_add_parameter_selections, m_add_equation_selections))
      {
        changed = true;
      }
    }
    if (ImGui::CollapsingHeader("Subtract Blend"))
    {
      const auto un_indent = glengine::ImGuiIndent();
      const auto pop       = glengine::ImGuiPushId();
      if (blend_combos(
            m_subtract_parameter_selections, m_subtract_equation_selections))
      {
        changed = true;
      }
    }
    return changed;
  }
  bool percent_blend_enabled() const noexcept
  {
    return m_enable_percent_blend;
  }
  void set_add_blend() const noexcept
  {
    set_blend_mode_selections(
      m_add_parameter_selections, m_add_equation_selections);
  }
  void set_subtract_blend() const noexcept
  {
    set_blend_mode_selections(
      m_subtract_parameter_selections, m_subtract_equation_selections);
  }

private:
  static void set_blend_mode_selections(
    const glengine::BlendModeParameters &parameters_selections,
    const glengine::BlendModeEquations  &equation_selections)
  {
    GlCall{}(
      glBlendFuncSeparate,
      parameters_selections[0].current_value(),
      parameters_selections[1].current_value(),
      parameters_selections[2].current_value(),
      parameters_selections[3].current_value());
    GlCall{}(
      glBlendEquationSeparate,
      equation_selections[0].current_value(),
      equation_selections[1].current_value());
  }
  [[nodiscard]] static bool blend_combos(
    glengine::BlendModeParameters &parameters_selections,
    glengine::BlendModeEquations  &equation_selections)
  {
    if (parameters_selections.on_im_gui_update())
    {
      return true;
    }
    ImGui::Separator();
    if (equation_selections.on_im_gui_update())
    {
      return true;
    }
    return false;
  }

  mutable bool                          m_enable_percent_blend     = { true };
  mutable glengine::BlendModeParameters m_add_parameter_selections = {
    +glengine::BlendModeParameterEnum::SrcColor,
    +glengine::BlendModeParameterEnum::One,
    +glengine::BlendModeParameterEnum::SrcAlpha,
    +glengine::BlendModeParameterEnum::OneMinusSrcAlpha
  };
  mutable glengine::BlendModeEquations m_add_equation_selections = {
    +glengine::BlendModeEquationEnum::FuncAdd,
    +glengine::BlendModeEquationEnum::FuncAdd
  };
  mutable glengine::BlendModeParameters m_subtract_parameter_selections = {
    +glengine::BlendModeParameterEnum::DstColor,
    +glengine::BlendModeParameterEnum::One,
    +glengine::BlendModeParameterEnum::SrcAlpha,
    +glengine::BlendModeParameterEnum::OneMinusSrcAlpha
  };
  mutable glengine::BlendModeEquations m_subtract_equation_selections = {
    +glengine::BlendModeEquationEnum::FuncReverseSubtract,
    +glengine::BlendModeEquationEnum::FuncAdd
  };
};
#endif// FIELD_MAP_EDITOR_MAPBLENDS_HPP
