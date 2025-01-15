#ifndef CF53C804_EE24_4949_8AA8_FE0182BFCCC1
#define CF53C804_EE24_4949_8AA8_FE0182BFCCC1
#include <array>
#include <cassert>
#include <compare>
#include <concepts>
#include <cstdint>
#include <open_viii/graphics/background/Map.hpp>
#include <open_viii/graphics/background/Tile1.hpp>
#include <utility>
namespace ff_8
{
class [[nodiscard]] source_tile_conflicts final
{
#ifdef UT_source_tile_conflicts_test
     // Code specific to UT_source_tile_conflicts_test
   public:
#else
     // Code for other cases
   private:
#endif

     static constexpr auto X_SIZE    = 16U;
     static constexpr auto Y_SIZE    = 16U;
     static constexpr auto T_SIZE    = 13U;
     static constexpr auto GRID_SIZE = X_SIZE * Y_SIZE;

     using GridArray                 = std::array<std::uint16_t, X_SIZE * Y_SIZE * T_SIZE>;

     GridArray m_grid                = {};
     template<std::unsigned_integral X, std::unsigned_integral Y, std::unsigned_integral T>
     [[nodiscard]] static constexpr std::unsigned_integral auto calculate_index(X x, Y y, T t) noexcept
     {
          validate_inputs(x, y, t);
          auto index = t * (GRID_SIZE) + y + (x / X_SIZE);
          assert(std::cmp_less(index, X_SIZE * Y_SIZE * T_SIZE) && "the calculated index is out of range.");

          return index;
     }

     template<std::unsigned_integral Index, std::unsigned_integral X, std::unsigned_integral Y, std::unsigned_integral T>
     static constexpr void reverse_index(Index index, X &x, Y &y, T &t) noexcept
     {
          t                     = index / GRID_SIZE;// Reverse the t calculation
          const Index remaining = index % GRID_SIZE;// Remaining part after extracting t
          y                     = (remaining / X_SIZE) * Y_SIZE;// y is the remainder
          x                     = (remaining % Y_SIZE) * X_SIZE;// Calculate x by reversing the division
     }

     template<std::unsigned_integral X, std::unsigned_integral Y, std::unsigned_integral T>
     static constexpr void validate_inputs([[maybe_unused]] X x, [[maybe_unused]] Y y, [[maybe_unused]] T t) noexcept
     {
          if constexpr (!std::is_same_v<X, std::uint8_t>)
          {
               assert(std::cmp_less(x, GRID_SIZE) && "x must be less than 256");
          }
          if constexpr (!std::is_same_v<Y, std::uint8_t>)
          {
               assert(std::cmp_less(y, GRID_SIZE) && "y must be less than 256");
          }
          assert(std::cmp_equal(x % X_SIZE, 0U) && "x must be a multiple of 16");
          assert(std::cmp_equal(y % Y_SIZE, 0U) && "y must be a multiple of 16");
          assert(std::cmp_less(t, T_SIZE) && "t must be less than the maximum number of texture pages");
     }

   public:
#if defined(__cpp_multidimensional_subscript) && __cpp_multidimensional_subscript >= 202110L
     [[nodiscard]] constexpr auto &operator[](std::uint16_t x, std::uint16_t y, std::uint8_t t) noexcept// C++23 required
     {
          validate_inputs(x, y, t);
          return m_grid[calculate_index(x, y, t)];
     }
#endif
     [[nodiscard]] constexpr auto &operator()(const std::uint8_t x, const std::uint8_t y, const std::uint8_t t) noexcept
     {
          validate_inputs(x, y, t);
          return m_grid[calculate_index(x, y, t)];
     }

#if defined(__cpp_multidimensional_subscript) && __cpp_multidimensional_subscript >= 202110L
     [[nodiscard]] constexpr auto operator[](std::uint16_t x, std::uint16_t y, std::uint8_t t) const noexcept// C++23 required
     {
          validate_inputs(x, y, t);
          return m_grid[calculate_index(x, y, t)];
     }
#endif
     [[nodiscard]] constexpr auto operator()(const std::uint8_t x, const std::uint8_t y, const std::uint8_t t) const noexcept
     {
          validate_inputs(x, y, t);
          return m_grid[calculate_index(x, y, t)];
     }


#if defined(__cpp_multidimensional_subscript) && __cpp_multidimensional_subscript >= 202110L
     template<open_viii::graphics::background::is_tile tile_t>
     [[nodiscard]] constexpr auto &operator[](const tile_t &tile) noexcept// C++23 required
     {
          validate_inputs(tile.source_x(), tile.source_y(), tile.texture_id());
          return m_grid[calculate_index(tile.source_x(), tile.source_y(), tile.texture_id())];
     }
#endif
     template<open_viii::graphics::background::is_tile tile_t>
     [[nodiscard]] constexpr auto &operator()(const tile_t &tile) noexcept
     {
          validate_inputs(tile.source_x(), tile.source_y(), tile.texture_id());
          return m_grid[calculate_index(tile.source_x(), tile.source_y(), tile.texture_id())];
     }

#if defined(__cpp_multidimensional_subscript) && __cpp_multidimensional_subscript >= 202110L
     template<open_viii::graphics::background::is_tile tile_t>
     [[nodiscard]] constexpr auto operator[](const tile_t &tile) const noexcept// C++23 required
     {
          validate_inputs(tile.source_x(), tile.source_y(), tile.texture_id());
          return m_grid[calculate_index(tile.source_x(), tile.source_y(), tile.texture_id())];
     }
#endif
     template<open_viii::graphics::background::is_tile tile_t>
     [[nodiscard]] constexpr auto operator()(const tile_t &tile) const noexcept
     {
          validate_inputs(tile.source_x(), tile.source_y(), tile.texture_id());
          return m_grid[calculate_index(tile.source_x(), tile.source_y(), tile.texture_id())];
     }

     constexpr auto operator<=>(const source_tile_conflicts &) const noexcept = default;
};
}// namespace ff_8

#endif /* CF53C804_EE24_4949_8AA8_FE0182BFCCC1 */
