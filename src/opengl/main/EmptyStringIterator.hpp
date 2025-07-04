//
// Created by pcvii on 3/13/2023.
//

#ifndef FIELD_MAP_EDITOR_EMPTYSTRINGITERATOR_HPP
#define FIELD_MAP_EDITOR_EMPTYSTRINGITERATOR_HPP
#include <ranges>
#include <string>
class EmptyStringIterator
{
   public:
     using difference_type   = std::ptrdiff_t;
     using value_type        = std::string_view;
     using pointer           = std::string_view *;
     using reference         = std::string_view &;
     using iterator_category = std::random_access_iterator_tag;

     constexpr bool operator==(const EmptyStringIterator & /*unused*/) const noexcept
     {
          return true;
     }
     constexpr bool operator!=(const EmptyStringIterator & /*unused*/) const noexcept
     {
          return false;
     }
     constexpr const value_type &operator*() const noexcept
     {
          return empty_string;
     }
     constexpr const value_type *operator->() const noexcept
     {
          return &empty_string;
     }
     constexpr EmptyStringIterator &operator++() noexcept
     {
          return *this;
     }
     constexpr EmptyStringIterator operator++(int) noexcept
     {
          return *this;
     }
     constexpr EmptyStringIterator &operator--() noexcept
     {
          return *this;
     }
     constexpr EmptyStringIterator operator--(int) noexcept
     {
          return *this;
     }
     constexpr EmptyStringIterator &operator+=(difference_type /*unused*/) noexcept
     {
          return *this;
     }
     constexpr EmptyStringIterator &operator-=(difference_type /*unused*/) noexcept
     {
          return *this;
     }
     constexpr const value_type &operator[](difference_type /*unused*/) const noexcept
     {
          return empty_string;
     }

     friend constexpr difference_type operator-(const EmptyStringIterator & /*unused*/, const EmptyStringIterator & /*unused*/) noexcept
     {
          return 0;
     }
     friend constexpr EmptyStringIterator operator+(const EmptyStringIterator & /*unused*/, difference_type /*unused*/) noexcept
     {
          return EmptyStringIterator{};
     }
     friend constexpr EmptyStringIterator operator+(difference_type /*unused*/, const EmptyStringIterator & /*unused*/) noexcept
     {
          return EmptyStringIterator{};
     }
     friend constexpr EmptyStringIterator operator-(const EmptyStringIterator & /*unused*/, difference_type /*unused*/) noexcept
     {
          return EmptyStringIterator{};
     }
     friend constexpr bool operator<(const EmptyStringIterator & /*unused*/, const EmptyStringIterator & /*unused*/) noexcept
     {
          return false;
     }
     friend constexpr bool operator>(const EmptyStringIterator & /*unused*/, const EmptyStringIterator & /*unused*/) noexcept
     {
          return false;
     }
     friend constexpr bool operator<=(const EmptyStringIterator & /*unused*/, const EmptyStringIterator & /*unused*/) noexcept
     {
          return true;
     }
     friend constexpr bool operator>=(const EmptyStringIterator & /*unused*/, const EmptyStringIterator & /*unused*/) noexcept
     {
          return true;
     }

   private:
     static constexpr value_type empty_string = {};
};

class EmptyStringView : public std::ranges::view_interface<EmptyStringView>
{
   public:
     friend constexpr auto begin(EmptyStringView /*unused*/)
     {
          return EmptyStringIterator{};
     }
     friend constexpr auto end(EmptyStringView /*unused*/)
     {
          return EmptyStringIterator{};
     }
     constexpr EmptyStringView operator()() const
     {
          return {};
     }
};
#endif// FIELD_MAP_EDITOR_EMPTYSTRINGITERATOR_HPP
