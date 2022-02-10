//
// Created by pcvii on 2/7/2022.
//

#ifndef FIELD_MAP_EDITOR_UNIQUEVALUES_HPP
#define FIELD_MAP_EDITOR_UNIQUEVALUES_HPP
template<typename T>
class UniqueValues
{
  static constexpr auto default_conversion_function =
    [](const auto &value) -> std::string { return fmt::format("{}", value); };

public:
  UniqueValues() = default;
  template<
    typename conversion_to_stringT = decltype(default_conversion_function)>
  UniqueValues(
    std::vector<T>          values,
    conversion_to_stringT &&transform_to_string = {})
    : m_values(std::move(values))
    , m_strings([&]() {
      auto transform = m_values | std::views::transform(transform_to_string);
      return std::vector<std::string>{ transform.begin(), transform.end() };
    }())
    , m_enable([&]() {
      return std::vector<std::uint8_t>(
        std::ranges::size(m_values), std::uint8_t{ true });
    }())
  {

    for (const auto &string : m_strings)
    {
      fmt::print("\t{}\r", string);
    }
  }
  template<
    std::ranges::range rangeT,
    typename conversion_to_stringT = decltype(default_conversion_function)>
  UniqueValues(
    rangeT                &&values,
    conversion_to_stringT &&transform_to_string = {})
    : m_values(std::ranges::cbegin(values), std::ranges::cend(values))
    , m_strings([&]() {
      auto transform = m_values | std::views::transform(transform_to_string);
      return std::vector<std::string>{ transform.begin(), transform.end() };
    }())
  {
    for (const auto &string : m_strings)
    {
      fmt::print("\t{}\r", string);
    }
  }
  std::pair<const T &, const std::string_view>
    operator[](std::size_t i) const noexcept
  {
    assert(std::ranges::size(m_values) == std::ranges::size(m_strings));
    assert(i < std::ranges::size(m_strings));
    return { m_values[i], m_strings[i] };
  }
  std::size_t size() const noexcept
  {
    assert(std::ranges::size(m_values) == std::ranges::size(m_strings));
    return std::ranges::size(m_strings);
  }
  const std::vector<T> &values() const noexcept
  {
    return m_values;
  }
  const std::vector<std::string> &strings() const noexcept
  {
    return m_strings;
  }
  std::vector<std::uint8_t> &enable() const noexcept
  {
    return m_enable;
  }

private:
  std::vector<T>                    m_values  = {};
  std::vector<std::string>          m_strings = {};
  mutable std::vector<std::uint8_t> m_enable  = {};
};
template<std::ranges::range rangeT>
UniqueValues(rangeT &&a)
  -> UniqueValues<std::decay_t<std::ranges::range_value_t<rangeT>>>;
template<std::ranges::range rangeT, typename funcT>
UniqueValues(rangeT &&a, funcT &&)
  -> UniqueValues<std::decay_t<std::ranges::range_value_t<rangeT>>>;
#endif// FIELD_MAP_EDITOR_UNIQUEVALUES_HPP
