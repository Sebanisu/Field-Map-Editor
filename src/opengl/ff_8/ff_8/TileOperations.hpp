#ifndef DC423E74_7AF3_459A_A708_F6C46A9776EA
#define DC423E74_7AF3_459A_A708_F6C46A9776EA
#include "Formatters.hpp"
#include <bit>
#include <compare>
#include <concepts>
#include <limits>
#include <open_viii/graphics/background/Map.hpp>
#include <spdlog/spdlog.h>
#include <type_traits>
#include <utility>

namespace ff_8::TileOperations
{

template<open_viii::graphics::background::is_tile TileT>
static constexpr TileT MaxTile = []()
{
     std::array<std::uint8_t, sizeof(TileT)> tmp{};
     tmp.fill(std::numeric_limits<std::uint8_t>::max());
     return std::bit_cast<TileT>(tmp);
}();

// === TileOperation Base ===
template<auto GetFn, auto SetFn = nullptr>
struct TileOperation
{
     using get_fn_t = decltype(GetFn);
     using set_fn_t = decltype(SetFn);

     // Deduce value_type from any tile type
     template<open_viii::graphics::background::is_tile TileT>
     using value_type = std::invoke_result_t<get_fn_t, TileT>;


     template<typename ValueT>
     struct nested_type
     {
          using local_value_type = ValueT;
     };

     // Partial specialization for value_type<TileT>
     template<open_viii::graphics::background::is_tile TileT>
     struct nested_type<TileT>
     {
          using local_value_type = value_type<TileT>;
     };

     template<typename T>
     using nested_type_t = nested_type<T>::local_value_type;

     // === Get ===
     template<open_viii::graphics::background::is_tile TileT>
     [[nodiscard]] constexpr value_type<TileT>
       operator()(const TileT &tile) const noexcept
     {
          return GetFn(tile);
     }

     // === DefaultValue ===
     struct DefaultValue
     {
          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr value_type<TileT>
            operator()(const TileT &) const noexcept
          {
               return {};
          }
     };

     // === Match ===

     template<typename InputT>
     struct Match
     {

          using ValueT = nested_type_t<InputT>;
          ValueT m_value{};

          constexpr explicit Match(const InputT &tile)
               requires(open_viii::graphics::background::is_tile<InputT>)
            : m_value(GetFn(tile))
          {
          }

          constexpr explicit Match(InputT v)
               requires(!open_viii::graphics::background::is_tile<InputT>)
            : m_value(std::move(v))
          {
          }


          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr bool
            operator()(const TileT &tile) const noexcept
          {
               return GetFn(tile) == m_value;
          }

          [[nodiscard]] constexpr auto operator<=>(const Match &) const
            = default;

          template<typename T>
               requires std::equality_comparable_with<
                 ValueT,
                 T>
          [[nodiscard]] constexpr bool operator==(const T &v) const
          {
               return m_value == v;
          }

          template<typename T>
               requires std::three_way_comparable_with<
                 ValueT,
                 T>
          [[nodiscard]] constexpr auto operator<=>(const T &v) const
          {
               return m_value <=> v;
          }

          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr bool operator==(const TileT &tile) const
          {
               return GetFn(tile) == m_value;
          }
          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr auto operator<=>(const TileT &tile) const
          {
               return m_value <=> GetFn(tile);
          }
     };

     // === With ===
     template<typename InputT>
     struct With
     {
          using ValueT = nested_type_t<InputT>;
          ValueT m_value{};


          constexpr explicit With(const InputT &tile)
               requires(open_viii::graphics::background::is_tile<InputT>)
            : m_value(GetFn(tile))
          {
          }

          constexpr explicit With(InputT v)
               requires(!open_viii::graphics::background::is_tile<InputT>)
            : m_value(std::move(v))
          {
          }

          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr TileT
            operator()(const TileT &tile) const noexcept
               requires(!std::is_same_v<
                        set_fn_t,
                        std::nullptr_t>)
          {
               spdlog::trace(
                 "with_... used: {}", static_cast<value_type<TileT>>(m_value));
               return SetFn(tile, static_cast<value_type<TileT>>(m_value));
          }

          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr TileT
            operator()(const TileT &tile) const noexcept
               requires(std::is_same_v<
                        set_fn_t,
                        std::nullptr_t>)
          {
               spdlog::debug("no with_... available");
               return tile;
          }

          template<open_viii::graphics::background::is_tile TileT>
          friend constexpr TileT operator|(
            const TileT &tile,
            const With  &op)
          {
               return op(tile);
          }
     };

     // === TranslateWith ===
     template<typename InputT>
     struct TranslateWith
     {
          using ValueT = nested_type_t<InputT>;
          ValueT m_from{};
          ValueT m_to{};

          template<typename V>
          constexpr explicit TranslateWith(
            const InputT &tile,
            V           &&to)
               requires(open_viii::graphics::background::is_tile<InputT>)
            : m_from(GetFn(tile))
            , m_to(std::forward_like<V>(static_cast<ValueT>(to)))
          {
          }

          constexpr explicit TranslateWith(
            InputT from,
            InputT to)
               requires(!open_viii::graphics::background::is_tile<InputT>)
            : m_from(std::move(from))
            , m_to(std::move(to))
          {
          }

          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr TileT
            operator()(const TileT &tile) const noexcept
               requires(!std::is_same_v<
                        set_fn_t,
                        std::nullptr_t>)
          {
               if constexpr (requires(ValueT a, ValueT b) {
                                  { a + b } -> std::convertible_to<ValueT>;
                                  { a - b } -> std::convertible_to<ValueT>;
                             })
               {
                    const auto current = m_to + GetFn(tile) - m_from;
                    spdlog::trace(
                      "translate with_...: {} -> {}", current, current);
                    return SetFn(tile, static_cast<value_type<TileT>>(current));
               }
               else
               {
                    spdlog::debug(
                      "translate skipped for type without + or - support");
                    return tile;
               }
          }

          template<open_viii::graphics::background::is_tile TileT>
          [[nodiscard]] constexpr TileT
            operator()(const TileT &tile) const noexcept
               requires(std::is_same_v<
                        set_fn_t,
                        std::nullptr_t>)
          {
               spdlog::debug("no translate with_... available");
               return tile;
          }

          template<open_viii::graphics::background::is_tile TileT>
          friend constexpr TileT operator|(
            const TileT         &tile,
            const TranslateWith &op)
          {
               return op(tile);
          }
     };

     // === Group (UI State) ===
     template<open_viii::graphics::background::is_tile TileT>
     struct Group
     {
          using local_value_type = value_type<TileT>;
          local_value_type                  current{};

          static constexpr local_value_type min_value = []()
          {
               if constexpr (std::signed_integral<local_value_type>)
               {
                    return std::numeric_limits<local_value_type>::min();
               }
               else
               {
                    return GetFn(TileT{});
               }
          }();

          static constexpr local_value_type max_value = []()
          {
               if constexpr (std::signed_integral<local_value_type>)
               {
                    return std::numeric_limits<local_value_type>::max();
               }
               else
               {
                    return GetFn(MaxTile<TileT>);
               }
          }();

          static constexpr bool read_only
            = std::is_same_v<set_fn_t, std::nullptr_t>;

          constexpr Group() = default;
          constexpr explicit Group(local_value_type v)
            : current(std::move(v))
          {
          }
          constexpr explicit Group(const TileT &tile)
            : current(GetFn(tile))
          {
          }

          using transform_with = With<TileT>;
          using match_with     = Match<TileT>;
     };

     // === Concepts ===
     template<open_viii::graphics::background::is_tile TileT>
     static constexpr bool has_setter
       = !std::is_same_v<set_fn_t, std::nullptr_t>;

     // === Deduction Guides ===
     template<open_viii::graphics::background::is_tile T>
     Match(T) -> Match<T>;

     template<
       typename U,
       open_viii::graphics::background::is_tile T>
          requires std::constructible_from<
            value_type<T>,
            U>
     Match(U) -> Match<T>;

     template<
       typename U,
       open_viii::graphics::background::is_tile T>
          requires std::constructible_from<
            value_type<T>,
            U>
     With(U) -> With<T>;
};


// Define operations
using X = TileOperation<
  [](auto &&t) { return t.x(); },
  [](auto &&t, auto v) { return t.with_x(v); }>;

using Y = TileOperation<
  [](auto &&t) { return t.y(); },
  [](auto &&t, auto v) { return t.with_y(v); }>;

using XY = TileOperation<
  [](auto &&t) { return t.xy(); },
  [](auto &&t, auto v) { return t.with_xy(v); }>;

using Z = TileOperation<
  [](auto &&t) { return t.z(); },
  [](auto &&t, auto v) { return t.with_z(v); }>;

using SourceX = TileOperation<
  [](auto &&t) { return t.source_x(); },
  [](auto &&t, auto v) { return t.with_source_x(v); }>;

using SourceY = TileOperation<
  [](auto &&t) { return t.source_y(); },
  [](auto &&t, auto v) { return t.with_source_y(v); }>;

using SourceXY = TileOperation<
  [](auto &&t) { return t.source_xy(); },
  [](auto &&t, auto v) { return t.with_source_xy(v); }>;

using TextureId = TileOperation<
  [](auto &&t) { return t.texture_id(); },
  [](auto &&t, auto v) { return t.with_texture_id(v); }>;

using BlendMode = TileOperation<
  [](auto &&t) { return t.blend_mode(); },
  [](open_viii::graphics::background::is_tile auto &&t, auto v)
  {
       if constexpr (requires { t.with_blend_mode(v); })
       {
            return t.with_blend_mode(v);
       }
       else
       {
            return std::forward<decltype(t)>(t);
       }
  }>;

using Blend = TileOperation<
  [](auto &&t) { return t.blend(); },
  [](auto &&t, auto v) { return t.with_blend(v); }>;

using Draw = TileOperation<
  [](auto &&t) { return t.draw(); },
  [](auto &&t, auto v) { return t.with_draw(v); }>;

using Depth = TileOperation<
  [](auto &&t) { return t.depth(); },
  [](auto &&t, auto v) { return t.with_depth(v); }>;

using LayerId = TileOperation<
  [](auto &&t) { return t.layer_id(); },
  [](open_viii::graphics::background::is_tile auto &&t, auto v)
  {
       if constexpr (requires { t.with_layer_id(v); })
       {
            return t.with_layer_id(v);
       }
       else
       {
            return std::forward<decltype(t)>(t);
       }
  }>;

using PaletteId = TileOperation<
  [](auto &&t) { return t.palette_id(); },
  [](auto &&t, auto v) { return t.with_palette_id(v); }>;

using AnimationId = TileOperation<
  [](auto &&t) { return t.animation_id(); },
  [](open_viii::graphics::background::is_tile auto &&t, auto v)
  {
       if constexpr (requires { t.with_animation_id(v); })
       {
            return t.with_animation_id(v);
       }
       else
       {
            return std::forward<decltype(t)>(t);
       }
  }>;

using AnimationState = TileOperation<
  [](auto &&t) { return t.animation_state(); },
  [](open_viii::graphics::background::is_tile auto &&t, auto v)
  {
       if constexpr (requires { t.with_animation_state(v); })
       {
            return t.with_animation_state(v);
       }
       else
       {
            return std::forward<decltype(t)>(t);
       }
  }>;

struct NotInvalidTile
{
     template<open_viii::graphics::background::is_tile T>
     constexpr bool operator()(const T &tile) const noexcept
     {
          return std::cmp_not_equal(tile.x(), 0x7FFFU);
     }
};

}// namespace ff_8::TileOperations
#endif /* DC423E74_7AF3_459A_A708_F6C46A9776EA */