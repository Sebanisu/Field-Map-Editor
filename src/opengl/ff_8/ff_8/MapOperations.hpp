#ifndef B2E1701E_178B_463A_A598_6CB97968CE39
#define B2E1701E_178B_463A_A598_6CB97968CE39
#include <ff_8/TileOperations.hpp>
namespace ff_8
{
enum struct TileInputStrategy : std::uint8_t
{
     MimSwizzle,
     ExternalSwizzle,
     ExternalDeswizzle,
     ExternalSwizzleAsOneImage,
     All
};

enum struct TileOutputStrategy : std::uint8_t
{
     Swizzle,
     Deswizzle,
     SwizzleAsOneImage,
     All
};

template<TileInputStrategy>
struct TileInputFunction
{
};

template<TileOutputStrategy>
struct TileOutputFunction
{
};

template<>
struct TileInputFunction<TileInputStrategy::MimSwizzle>
{
     using X              = TileOperations::SourceX;
     using Y              = TileOperations::SourceY;
     using TexturePage    = TileOperations::TextureId;
     using UseTexturePage = std::true_type;
     using Lossless       = std::true_type;
     using External       = std::false_type;
     using StrategyType   = TileInputStrategy;
     static constexpr TileInputStrategy strategy
       = TileInputStrategy::MimSwizzle;
     static constexpr std::string_view label = "Input (MIM Swizzle)";
     static constexpr std::string_view description
       = "Inputs all tiles from a MIM swizzled image file. This uses the "
         "original game files so it is the most accurate representation of the "
         "image.";
};

template<>
struct TileInputFunction<TileInputStrategy::ExternalSwizzle>
{
     using X              = TileOperations::SourceX;
     using Y              = TileOperations::SourceY;
     using TexturePage    = TileOperations::TextureId;
     using UseTexturePage = std::true_type;
     using Lossless       = std::false_type;
     using External       = std::true_type;
     using StrategyType   = TileInputStrategy;
     static constexpr TileInputStrategy strategy
       = TileInputStrategy::ExternalSwizzle;
     static constexpr std::string_view label = "Input (External Swizzle)";
     static constexpr std::string_view description
       = "Inputs all tiles from multiple swizzled image files. This is lossy "
         "because tiles may overlap. Due to palettes or bits per pixel "
         "settings. Meaning you might not get the same image in as you put "
         "out.";
};

template<>
struct TileInputFunction<TileInputStrategy::ExternalDeswizzle>
{
     using X              = TileOperations::X;
     using Y              = TileOperations::Y;
     using TexturePage    = TileOperations::TextureId::DefaultValue;
     using UseTexturePage = std::false_type;
     using Lossless       = std::true_type;
     using External       = std::true_type;
     using StrategyType   = TileInputStrategy;
     static constexpr TileInputStrategy strategy
       = TileInputStrategy::ExternalDeswizzle;
     static constexpr std::string_view label = "Input (External Deswizzle)";
     static constexpr std::string_view description
       = "Inputs all tiles from multiple deswizzled image files. Pupu ID is "
         "used in the filename to determine which tile in each file. This "
         "prevents data loss due to overlapping tiles.";
};

template<>
struct TileInputFunction<TileInputStrategy::ExternalSwizzleAsOneImage>
{
     using X              = TileOperations::SwizzleAsOneImage::X;
     using Y              = TileOperations::SwizzleAsOneImage::Y;
     using TexturePage    = TileOperations::SwizzleAsOneImage::TextureId;
     using UseTexturePage = std::true_type;
     using Lossless       = std::true_type;
     using External       = std::true_type;
     using StrategyType   = TileInputStrategy;
     static constexpr TileInputStrategy strategy
       = TileInputStrategy::ExternalSwizzleAsOneImage;
     static constexpr std::string_view label
       = "Input (External Swizzle As One Image)";
     static constexpr std::string_view description
       = "Inputs all tiles from a single swizzled image file. The tiles come "
         "in sequential order, so the texture page and source coordinates are "
         "calculated based on the tile index. This prevents data loss due to "
         "overlapping tiles.";
};

template<>
struct TileOutputFunction<TileOutputStrategy::Swizzle>
{
     using X                                      = TileOperations::SourceX;
     using Y                                      = TileOperations::SourceY;
     using TexturePage                            = TileOperations::TextureId;
     using UseTexturePage                         = std::true_type;
     using UseBlendingOnRender                    = std::false_type;
     using Lossless                               = std::false_type;
     using StrategyType                           = TileOutputStrategy;
     static constexpr TileOutputStrategy strategy = TileOutputStrategy::Swizzle;
     static constexpr std::string_view   label    = "Output (Swizzle)";
     static constexpr std::string_view   description
       = "Standard swizzle output used by MIM. May lose data when multiple "
         "tiles share the same texture page and source coordinates but have "
         "different palettes or blending settings.";
};

template<>
struct TileOutputFunction<TileOutputStrategy::Deswizzle>
{
     using X                   = TileOperations::X;
     using Y                   = TileOperations::Y;
     using TexturePage         = TileOperations::TextureId::DefaultValue;
     using UseTexturePage      = std::false_type;
     using UseBlendingOnRender = std::true_type;
     using Lossless            = std::true_type;
     using StrategyType        = TileOutputStrategy;
     static constexpr TileOutputStrategy strategy
       = TileOutputStrategy::Deswizzle;
     static constexpr std::string_view label = "Output (Deswizzle)";
     static constexpr std::string_view description
       = "Uses Pupu ID to break up the unscrambled image into seperate files. "
         "This avoid losing data. Pupu ID is generated automatically during "
         "the loading of the tiles. It uses various unique properties of the "
         "tile to generate a unique ID where tiles aren't overlapping.";
};

template<>
struct TileOutputFunction<TileOutputStrategy::SwizzleAsOneImage>
{
     using X                   = TileOperations::SwizzleAsOneImage::X;
     using Y                   = TileOperations::SwizzleAsOneImage::Y;
     using TexturePage         = TileOperations::SwizzleAsOneImage::TextureId;
     using UseTexturePage      = std::true_type;
     using UseBlendingOnRender = std::false_type;
     using Lossless            = std::true_type;
     using StrategyType        = TileOutputStrategy;
     static constexpr TileOutputStrategy strategy
       = TileOutputStrategy::SwizzleAsOneImage;
     static constexpr std::string_view label = "Output (Swizzle As One Image)";
     static constexpr std::string_view description
       = "Outputs all tiles into a single swizzled image file. "
         "This also reorderes the tiles to be in sequential order. This will "
         "prevent data loss. Because it's impossible for the tiles to overlap "
         "when using this method.";
};

template<class Strategy>
struct TileFunction
{
     using S                                = std::remove_cvref_t<Strategy>;
     // -------------------------
     // Type aliases for compile-time use
     // -------------------------
     using XType                            = typename S::X;
     using YType                            = typename S::Y;
     using TexturePageType                  = typename S::TexturePage;
     using StrategyType                     = typename S::StrategyType;
     static constexpr StrategyType strategy = S::strategy;

     constexpr                     operator StrategyType() const
     {
          return strategy;
     }

     // Mandatory trait:
     // UseTexturePage indicates if the texture page is used in this operation.
     // Texture Page shifts everything by 256 pixels in X direction. Depending
     // on the bits per pixel. Higher bpp uses shortened texture pages: 4bpp =
     // 256, 8bpp = 128, 16bpp = 64. Though for output we just use full 256
     // pixel pages.
     static constexpr bool UseTexturePage          = S::UseTexturePage::value;

     // Mandatory trait:
     // Lossless indicates if the operation is lossless or lossy.
     static constexpr bool Lossless                = S::Lossless::value;

     static constexpr std::string_view label       = S::label;
     static constexpr std::string_view description = S::description;


     // Optional trait:
     // External  (input-only trait) indicates if the tile data comes from
     // external files and not the original MIM files.
     static constexpr bool             External
       = requires { typename S::External; } ? S::External::value : false;

     // Optional trait:
     // UseBlendingOnRender (output-only trait) forced to false for outputting
     // to files. But when true and rendering to screen its used for a default.
     // That can be toggled on and off. When false it cannot be toggled.
     static constexpr bool UseBlendingOnRender = requires {
          typename S::UseBlendingOnRender;
     } ? S::UseBlendingOnRender::value : false;


     // X cordinate getter, use X(tile) to get value
     XType                 X{};
     // Y cordinate getter, use Y(tile) to get value
     YType                 Y{};
     // Texture page getter, use TexturePage(tile) to get value
     TexturePageType       TexturePage{};

     // Path to the images used for this operation (input-only)
     // Though this is currently stored in the filters so we might not use this.
     std::filesystem::path Path{};
};

template<TileInputStrategy Strategy>
TileFunction(TileInputStrategy) -> TileFunction<TileInputFunction<Strategy>>;

template<TileOutputStrategy Strategy>
TileFunction(TileOutputStrategy) -> TileFunction<TileOutputFunction<Strategy>>;


using TileInputFunctionVariant = decltype([] {
    return []<std::size_t... Is>(std::index_sequence<Is...>)
        -> std::variant<std::monostate,TileFunction<TileInputFunction<static_cast<TileInputStrategy>(Is)>>...>
    {
        return std::monostate{}; // default-constructed variant
    }(std::make_index_sequence<static_cast<std::size_t>(TileInputStrategy::All)>{});
}());

using TileOutputFunctionVariant = decltype([] {
    return []<std::size_t... Is>(std::index_sequence<Is...>)
        -> std::variant<std::monostate,TileFunction<TileOutputFunction<static_cast<TileOutputStrategy>(Is)>>...>
    {
        return std::monostate{}; // default-constructed variant
    }(std::make_index_sequence<static_cast<std::size_t>(TileOutputStrategy::All)>{});
}());
struct MapOperationSettings
{
     template<class Variant>
     consteval auto all_labels()
     {
          return []<std::size_t... Is>(std::index_sequence<Is...>)
          {
               return std::array<std::string_view, sizeof...(Is)>{ (
                 []
                 {
                      using T = std::variant_alternative_t<Is, Variant>;
                      if constexpr (std::is_same_v<T, std::monostate>)
                           return std::string_view{ "" };
                      else
                           return T::label;
                 }())... };
          }(std::make_index_sequence<std::variant_size_v<Variant>>{});
     }

     template<class Variant>
     consteval auto all_descriptions()
     {
          return []<std::size_t... Is>(std::index_sequence<Is...>)
          {
               return std::array<std::string_view, sizeof...(Is)>{ (
                 []
                 {
                      using T = std::variant_alternative_t<Is, Variant>;
                      if constexpr (std::is_same_v<T, std::monostate>)
                           return std::string_view{ "" };
                      else
                           return T::description;
                 }())... };
          }(std::make_index_sequence<std::variant_size_v<Variant>>{});
     }

     template<class Variant>
     consteval auto all_values()
     {
          return []<std::size_t... Is>(std::index_sequence<Is...>)
          {
               return std::array<Variant, sizeof...(Is)>{ Variant{
                 std::variant_alternative_t<Is, Variant>{} }... };
          }(std::make_index_sequence<std::variant_size_v<Variant>>{});
     }
     static constexpr inline auto input_labels
       = all_labels<TileInputFunctionVariant>();
     static constexpr inline auto input_descriptions
       = all_descriptions<TileInputFunctionVariant>();
     static constexpr inline auto input_values
       = all_values<TileInputFunctionVariant>();
     static constexpr inline auto output_labels
       = all_labels<TileOutputFunctionVariant>();
     static constexpr inline auto output_descriptions
       = all_descriptions<TileOutputFunctionVariant>();
     static constexpr inline auto output_values
       = all_values<TileOutputFunctionVariant>();
};

}// namespace ff_8
#endif /* B2E1701E_178B_463A_A598_6CB97968CE39 */
