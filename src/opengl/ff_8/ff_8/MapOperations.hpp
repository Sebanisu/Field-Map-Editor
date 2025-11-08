#ifndef B2E1701E_178B_463A_A598_6CB97968CE39
#define B2E1701E_178B_463A_A598_6CB97968CE39
#include <ff_8/TileOperations.hpp>
namespace ff_8
{
enum struct TileFunctionType : std::uint8_t
{
     Swizzle,
     SwizzleAsOneImage,
     Deswizzle,
     ExternalSwizzle,
     ExternalSwizzleAsOneImage,
     ExternalDeswizzle
};

enum struct TileInputStrategy : std::uint8_t
{
     MimSwizzle,
     ExternalSwizzle,
     ExternalDeswizzle,
     ExternalSwizzleAsOneImage
};

enum struct TileOutputStrategy : std::uint8_t
{
     Swizzle,
     Deswizzle,
     SwizzleAsOneImage
};

template<TileInputStrategy>
struct TileInputFunctions
{
};

template<TileOutputStrategy>
struct TileOutputFunctions
{
};

template<typename T>
concept TileInput = requires {
     typename T::X;
     typename T::Y;
     typename T::TexturePage;
     typename T::UseTexturePage;
     typename T::Lossless;
     { T::label } -> std::convertible_to<const char *>;
};

template<typename T>
concept TileOutput = requires {
     typename T::X;
     typename T::Y;
     typename T::TexturePage;
     typename T::UseTexturePage;
     typename T::UseBlendingOnRender;
     { T::label } -> std::convertible_to<const char *>;
};

template<>
struct TileInputFunctions<TileInputStrategy::MimSwizzle>
{
     using X                            = TileOperations::SourceX;
     using Y                            = TileOperations::SourceY;
     using TexturePage                  = TileOperations::TextureId;
     using UseTexturePage               = std::true_type;
     static constexpr const char *label = "Input (MIM Swizzle)";
};

template<>
struct TileInputFunctions<TileInputStrategy::ExternalSwizzle>
{
     using X                            = TileOperations::SourceX;
     using Y                            = TileOperations::SourceY;
     using TexturePage                  = TileOperations::TextureId;
     using UseTexturePage               = std::true_type;
     using Lossless                     = std::false_type;
     static constexpr const char *label = "Input (External Swizzle)";
};

template<>
struct TileInputFunctions<TileInputStrategy::ExternalDeswizzle>
{
     using X              = TileOperations::X;
     using Y              = TileOperations::Y;
     using TexturePage    = TileOperations::TextureId::DefaultValue;
     using UseTexturePage = std::false_type;
     using Lossless       = std::true_type;
     static constexpr const char *label = "Input (External Deswizzle)";
};

template<>
struct TileInputFunctions<TileInputStrategy::ExternalSwizzleAsOneImage>
{
     using X              = TileOperations::SwizzleAsOneImage::X;
     using Y              = TileOperations::SwizzleAsOneImage::Y;
     using TexturePage    = TileOperations::SwizzleAsOneImage::TextureId;
     using UseTexturePage = std::true_type;
     using Lossless       = std::true_type;
     static constexpr const char *label
       = "Input (External Swizzle As One Image)";
};

template<>
struct TileOutputFunctions<TileOutputStrategy::Swizzle>
{
     using X                            = TileOperations::SourceX;
     using Y                            = TileOperations::SourceY;
     using TexturePage                  = TileOperations::TextureId;
     using UseTexturePage               = std::true_type;
     using UseBlendingOnRender          = std::false_type;
     static constexpr const char *label = "Output (Swizzle)";
};

template<>
struct TileOutputFunctions<TileOutputStrategy::Deswizzle>
{
     using X                   = TileOperations::X;
     using Y                   = TileOperations::Y;
     using TexturePage         = TileOperations::TextureId::DefaultValue;
     using UseTexturePage      = std::false_type;
     using UseBlendingOnRender = std::true_type;
     static constexpr const char *label = "Output (Deswizzle)";
};

template<>
struct TileOutputFunctions<TileOutputStrategy::SwizzleAsOneImage>
{
     using X                   = TileOperations::SwizzleAsOneImage::X;
     using Y                   = TileOperations::SwizzleAsOneImage::Y;
     using TexturePage         = TileOperations::SwizzleAsOneImage::TextureId;
     using UseTexturePage      = std::true_type;
     using UseBlendingOnRender = std::false_type;
     static constexpr const char *label = "Output (Swizzle As One Image)";
};

}// namespace ff_8
#endif /* B2E1701E_178B_463A_A598_6CB97968CE39 */
