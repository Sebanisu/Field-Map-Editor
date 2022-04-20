//
// Created by pcvii on 3/2/2022.
//

#ifndef FIELD_MAP_EDITOR_MAPBLENDS_HPP
#define FIELD_MAP_EDITOR_MAPBLENDS_HPP
class MapBlends
{
public:
  [[nodiscard]] bool OnImGuiUpdate() const
  {
    bool changed = false;
    if (ImGui::CollapsingHeader("Add Blend"))
    {
      const auto un_indent = glengine::ImGuiIndent();
      if (ImGui::Checkbox("Percent Blends (50%,25%)", &m_enable_percent_blend))
      {
        changed = true;
      }
      const auto pop = glengine::ImGuiPushID();
      if (Blend_Combos(m_add_parameter_selections, m_add_equation_selections))
      {
        changed = true;
      }
    }
    if (ImGui::CollapsingHeader("Subtract Blend"))
    {
      const auto un_indent = glengine::ImGuiIndent();
      const auto pop       = glengine::ImGuiPushID();
      if (Blend_Combos(
            m_subtract_parameter_selections, m_subtract_equation_selections))
      {
        changed = true;
      }
    }
    return changed;
  }
  bool PercentBlendEnabled() const noexcept
  {
    return m_enable_percent_blend;
  }
  void SetAddBlend() const noexcept
  {
    SetBlendModeSelections(
      m_add_parameter_selections, m_add_equation_selections);
  }
  void SetSubtractBlend() const noexcept
  {
    SetBlendModeSelections(
      m_subtract_parameter_selections, m_subtract_equation_selections);
  }

private:
  static void SetBlendModeSelections(
    const glengine::BlendModeParameters &parameters_selections,
    const glengine::BlendModeEquations  &equation_selections)
  {
    GLCall{}(
      glBlendFuncSeparate,
      parameters_selections[0].current_value(),
      parameters_selections[1].current_value(),
      parameters_selections[2].current_value(),
      parameters_selections[3].current_value());
    GLCall{}(
      glBlendEquationSeparate,
      equation_selections[0].current_value(),
      equation_selections[1].current_value());
  }
  [[nodiscard]] static bool Blend_Combos(
    glengine::BlendModeParameters &parameters_selections,
    glengine::BlendModeEquations  &equation_selections)
  {
    if (parameters_selections.OnImGuiUpdate())
    {
      return true;
    }
    ImGui::Separator();
    if (equation_selections.OnImGuiUpdate())
    {
      return true;
    }
    return false;
  }

  mutable bool                          m_enable_percent_blend     = { true };
  mutable glengine::BlendModeParameters m_add_parameter_selections = {
    +glengine::BlendModeParameterEnum::SRC_COLOR,
    +glengine::BlendModeParameterEnum::ONE,
    +glengine::BlendModeParameterEnum::SRC_ALPHA,
    +glengine::BlendModeParameterEnum::ONE_MINUS_SRC_ALPHA
  };
  mutable glengine::BlendModeEquations m_add_equation_selections = {
    +glengine::BlendModeEquationEnum::FUNC_ADD,
    +glengine::BlendModeEquationEnum::FUNC_ADD
  };
  mutable glengine::BlendModeParameters m_subtract_parameter_selections = {
    +glengine::BlendModeParameterEnum::DST_COLOR,
    +glengine::BlendModeParameterEnum::ONE,
    +glengine::BlendModeParameterEnum::SRC_ALPHA,
    +glengine::BlendModeParameterEnum::ONE_MINUS_SRC_ALPHA
  };
  mutable glengine::BlendModeEquations m_subtract_equation_selections = {
    +glengine::BlendModeEquationEnum::FUNC_REVERSE_SUBTRACT,
    +glengine::BlendModeEquationEnum::FUNC_ADD };
};
#endif// FIELD_MAP_EDITOR_MAPBLENDS_HPP