/**
 * Based on back_insert_iterator and back_inserter
 * Copyright (c) Microsoft Corporation.
 * SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
 * I did this because SFML custom container doesn't have push_back.
 * I might not need something like this I thought I might.
 */
#ifndef FIELD_MAP_EDITOR_APPEND_INSERTER_HPP
#define FIELD_MAP_EDITOR_APPEND_INSERTER_HPP

#include <cstddef>
#include <iterator>
#include <memory>
#include <utility>
template<typename ContainerT>
requires(requires(ContainerT c, typename ContainerT::value_type v) {
  c.append(v);
}) class append_insert_iterator
{// wrap pushes to append of container as output iterator
public:
  using iterator_category                     = std::output_iterator_tag;
  using value_type                            = void;
  using pointer                               = void;
  using reference                             = void;
  using container_type                        = ContainerT;
  using difference_type                       = std::ptrdiff_t;
  constexpr append_insert_iterator() noexcept = default;
  constexpr explicit append_insert_iterator(
    ContainerT &local_container) noexcept
    : container(std::addressof(local_container))
  {
  }
  constexpr append_insert_iterator &
    operator=(const typename ContainerT::value_type &value)
  {
    container->append(value);
    return *this;
  }
  constexpr append_insert_iterator &
    operator=(typename ContainerT::value_type &&value)
  {
    container->append(std::move(value));
    return *this;
  }
  [[nodiscard]] constexpr append_insert_iterator &operator*() noexcept
  {
    return *this;
  }
  constexpr append_insert_iterator &operator++() noexcept
  {
    return *this;
  }
  constexpr append_insert_iterator operator++(int) noexcept
  {
    return *this;
  }

protected:
  ContainerT *container = nullptr;
};
template<class ContainerT>
[[nodiscard]] constexpr append_insert_iterator<ContainerT>
  append_inserter(ContainerT &container) noexcept
{
  return append_insert_iterator<ContainerT>(container);
}
#endif// FIELD_MAP_EDITOR_APPEND_INSERTER_HPP
