//
// Created by pcvii on 2/7/2022.
//

#ifndef FIELD_MAP_EDITOR_UNIQUEVALUES_HPP
#define FIELD_MAP_EDITOR_UNIQUEVALUES_HPP
template<typename T>
class UniqueValues
{
     static constexpr auto default_transform_function = [](const auto &value) -> std::string { return fmt::format("{}", value); };

   public:
     UniqueValues() = default;
     template<typename transform_to_stringT = decltype(default_transform_function)>
     UniqueValues(
       std::vector<T>         values,
       transform_to_stringT &&transform_to_string = {})
       : m_values(std::move(values))
       , m_strings([&]() {
            auto transform = m_values | std::views::transform(std::forward<transform_to_stringT>(transform_to_string));
            return std::vector<std::string>{ transform.begin(), transform.end() };
       }())
       , m_enable([&]() { return std::vector<std::uint8_t>(std::ranges::size(m_values), std::uint8_t{ true }); }())
     {
          //    for (const auto &string : m_strings)
          //    {
          //      spdlog::debug("\t{}\r", string);
          //    }
     }
     template<
       std::ranges::range rangeT,
       typename transform_to_stringT = decltype(default_transform_function)>
     UniqueValues(
       rangeT               &&values,
       transform_to_stringT &&transform_to_string = {})
       : UniqueValues(
           std::vector<T>(
             std::ranges::cbegin(values),
             std::ranges::cend(values)),
           std::forward<transform_to_stringT>(transform_to_string))
     {
     }
     std::pair<
       const T &,
       const std::string_view>
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
     /**
      * This just takes the values of old sees what matches this and copies the
      * bool values over
      * @param old
      */
     void update(const UniqueValues<T> &old)
     {
          for (const auto i : std::views::iota(size_t{}, std::ranges::size(m_values)))
          {
               if (const auto location = std::ranges::find(old.values(), m_values[i]); location != old.values().end())
               {
                    m_enable[i] = old.enable()[static_cast<size_t>(std::distance(old.values().begin(), location))];
               }
          }
     }

   private:
     std::vector<T>                    m_values  = {};
     std::vector<std::string>          m_strings = {};
     mutable std::vector<std::uint8_t> m_enable  = {};
};
template<std::ranges::range rangeT>
UniqueValues(rangeT &&a) -> UniqueValues<std::remove_cvref_t<std::ranges::range_value_t<rangeT>>>;
template<
  std::ranges::range rangeT,
  typename funcT>
UniqueValues(
  rangeT &&a,
  funcT &&) -> UniqueValues<std::remove_cvref_t<std::ranges::range_value_t<rangeT>>>;
#endif// FIELD_MAP_EDITOR_UNIQUEVALUES_HPP
