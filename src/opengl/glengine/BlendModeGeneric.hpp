//
// Created by pcvii on 2/2/2022.
//

#ifndef FIELD_MAP_EDITOR_BLENDMODEGENERIC_HPP
#define FIELD_MAP_EDITOR_BLENDMODEGENERIC_HPP
#include "GenericCombo.hpp"
namespace glengine
{
template<typename SelectionsStringsLambdaT, typename SelectionsValuesLambdaT>
class BlendModeGeneric
{
public:
  static_assert(
    std::ranges::size(SelectionsStringsLambdaT()())
    == std::ranges::size(SelectionsValuesLambdaT()()));
  static constexpr auto selection_strings = SelectionsStringsLambdaT()();
  static constexpr auto selection_values  = SelectionsValuesLambdaT()();
  constexpr BlendModeGeneric()            = default;

  constexpr BlendModeGeneric(int default_selection)
    : selection(default_selection)
  {
  }

  constexpr int current_value() const noexcept
  {
    if constexpr (std::ranges::empty(selection_values))
    {
      return 0;
    }
    return selection_values[static_cast<std::size_t>(std::clamp(
      selection,
      int{},
      static_cast<int>(std::ranges::ssize(selection_values) - 1)))];
  }

  auto &operator=(int new_selection) noexcept
  {
    selection = new_selection;
  }

  operator int &() noexcept
  {
    return selection;
  }

private:
  int selection;
};

template<typename LabelsLambdaT, typename UsedSelectionsT>
class BlendModeGenerics
{
private:
  std::array<UsedSelectionsT, std::ranges::size(LabelsLambdaT()())>
    m_selections{};

public:
  static constexpr auto labels  = LabelsLambdaT()();

  constexpr BlendModeGenerics() = default;
  template<typename... Ts>
  // clang-format off
  requires((std::constructible_from<UsedSelectionsT,Ts>
             &&...) && sizeof...(Ts) == std::ranges::size(LabelsLambdaT()()))
    // clang-format on
    constexpr BlendModeGenerics(Ts &&...ts)
    : m_selections(std::array{ UsedSelectionsT{ std::forward<Ts>(ts) }... })
  {
  }


  [[nodiscard]] bool OnImGuiUpdate()
  {
    bool changed = false;
    for (std::size_t i = 0U; i != std::ranges::size(labels); ++i)
    {
      if (glengine::GenericCombo(
            labels[i].data(), m_selections[i], UsedSelectionsT::selection_strings))
      {
        changed = true;
      }
    }
    return changed;
  }
  const UsedSelectionsT &operator[](std::size_t i) const
  {
    if constexpr (std::ranges::empty(labels))
    {
      return {};
    }
    else
    {
      return m_selections.at(
        std::clamp(i, std::size_t{ 0 }, std::ranges::size(m_selections) - 1));
    }
  }
};
}// namespace glengine
#endif// FIELD_MAP_EDITOR_BLENDMODEGENERIC_HPP
