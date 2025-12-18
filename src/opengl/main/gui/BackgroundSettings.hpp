#ifndef A123DC46_3563_4C3C_8852_7D600E671125
#define A123DC46_3563_4C3C_8852_7D600E671125
#include "spdlog/spdlog.h"
namespace fme
{

enum class BackgroundSettings : uint8_t
{
     // Presets
     Default      = 0,

     // Color count bit
     OneColor     = Default,
     TwoColors    = 1 << 0,

     // Pattern bit
     Checkerboard = Default,
     Solid        = 1 << 1,


};


// Bitwise operators
inline constexpr BackgroundSettings operator|(
  BackgroundSettings lhs,
  BackgroundSettings rhs)
{
     return static_cast<BackgroundSettings>(
       std::to_underlying(lhs) | std::to_underlying(rhs));
}

inline constexpr BackgroundSettings operator&(
  BackgroundSettings lhs,
  BackgroundSettings rhs)
{
     return static_cast<BackgroundSettings>(
       std::to_underlying(lhs) & std::to_underlying(rhs));
}

inline constexpr BackgroundSettings &operator|=(
  BackgroundSettings &lhs,
  BackgroundSettings  rhs)
{
     lhs = lhs | rhs;
     return lhs;
}

inline constexpr bool ValidateBackgroundSettings(BackgroundSettings setting)
{
     // Must only contain valid bits
     if (
       std::to_underlying(setting)
       & ~std::to_underlying(
         BackgroundSettings::Solid | BackgroundSettings::TwoColors))
     {
          if (!std::is_constant_evaluated())
          {
               spdlog::debug(
                 "Invalid BackgroundSettings: contains unknown bits ({:#X})",
                 std::to_underlying(setting));
          }
          return false;
     }

     // Disallow Solid + TwoColors
     if (
       (setting & (BackgroundSettings::Solid | BackgroundSettings::TwoColors))
       == (BackgroundSettings::Solid | BackgroundSettings::TwoColors))
     {
          if (!std::is_constant_evaluated())
          {
               spdlog::debug(
                 "Invalid BackgroundSettings: Solid and TwoColors cannot be "
                 "combined ({:#X})",
                 std::to_underlying(setting));
          }
          return false;
     }

     return true;
}

inline constexpr bool HasFlag(
  BackgroundSettings value,
  BackgroundSettings flag)
{
     if (flag == BackgroundSettings::Default)
          return value == BackgroundSettings::Default;
     else
          return (value & flag) != BackgroundSettings::Default;
}


static_assert(ValidateBackgroundSettings(BackgroundSettings::Default));
static_assert(!ValidateBackgroundSettings(
  BackgroundSettings::TwoColors | BackgroundSettings::Solid));

// Static asserts to validate HasFlag logic

static_assert(
  HasFlag(
    BackgroundSettings::Default,
    BackgroundSettings::Default),
  "Default has Default");
static_assert(
  HasFlag(
    BackgroundSettings::Default,
    BackgroundSettings::OneColor),
  "Default has OneColor (alias of Default)");
static_assert(
  !HasFlag(
    BackgroundSettings::Default,
    BackgroundSettings::TwoColors),
  "Default does not have TwoColors");
static_assert(
  HasFlag(
    BackgroundSettings::TwoColors,
    BackgroundSettings::TwoColors),
  "TwoColors has TwoColors");
static_assert(
  !HasFlag(
    BackgroundSettings::TwoColors,
    BackgroundSettings::Default),
  "TwoColors does not have Default");
static_assert(
  !HasFlag(
    BackgroundSettings::TwoColors,
    BackgroundSettings::OneColor),
  "TwoColors does not have OneColor (Default)");
static_assert(
  !HasFlag(
    BackgroundSettings::OneColor,
    BackgroundSettings::TwoColors),
  "OneColor (Default) does not have TwoColors");

// Also test pattern bits
static_assert(
  HasFlag(
    BackgroundSettings::Default,
    BackgroundSettings::Checkerboard),
  "Default has Checkerboard");
static_assert(
  !HasFlag(
    BackgroundSettings::Default,
    BackgroundSettings::Solid),
  "Default does not have Solid");
static_assert(
  HasFlag(
    BackgroundSettings::Solid,
    BackgroundSettings::Solid),
  "Solid has Solid");
static_assert(
  !HasFlag(
    BackgroundSettings::Solid,
    BackgroundSettings::Checkerboard),
  "Solid does not have Checkerboard");

constexpr void SetFlag(
  BackgroundSettings &current,
  BackgroundSettings  flag,
  bool                enabled)
{
     if (flag == BackgroundSettings::Default)
     {
          // No operation if flag is Default
          return;
     }

     auto value   = std::to_underlying(current);
     auto flagBit = std::to_underlying(flag);

     if (enabled)
     {
          value |= flagBit;// Set the bit
     }
     else
     {
          value &= ~flagBit;// Clear the bit
     }

     current = static_cast<BackgroundSettings>(value);

     return;
}

[[nodiscard]] constexpr BackgroundSettings SetFlag(
  BackgroundSettings &&initial,
  BackgroundSettings   flag,
  bool                 enabled)
{
     SetFlag(initial, flag, enabled);
     return initial;
}

static_assert(
  SetFlag(
    BackgroundSettings::OneColor,
    BackgroundSettings::TwoColors,
    true)
    == BackgroundSettings::TwoColors,
  "Enable TwoColors failed");
static_assert(
  SetFlag(
    BackgroundSettings::TwoColors,
    BackgroundSettings::TwoColors,
    false)
    == BackgroundSettings::OneColor,
  "Disable TwoColors failed");

static_assert(
  SetFlag(
    BackgroundSettings::Checkerboard,
    BackgroundSettings::Solid,
    true)
    == BackgroundSettings::Solid,
  "Enable Solid failed");
static_assert(
  SetFlag(
    BackgroundSettings::Solid,
    BackgroundSettings::Solid,
    false)
    == BackgroundSettings::Checkerboard,
  "Disable Solid failed");

// Test no-op when flag == Default
static_assert(
  SetFlag(
    BackgroundSettings::Solid,
    BackgroundSettings::Default,
    true)
    == BackgroundSettings::Solid,
  "No-op with Default flag failed");
static_assert(
  SetFlag(
    BackgroundSettings::TwoColors,
    BackgroundSettings::Default,
    false)
    == BackgroundSettings::TwoColors,
  "No-op with Default flag failed");
}// namespace fme

#endif /* A123DC46_3563_4C3C_8852_7D600E671125 */
