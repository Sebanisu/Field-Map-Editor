#include "MapOperations.hpp"

namespace ff_8
{
TileInputFunctionVariant MakeTileInputFunction(TileInputStrategy s)
{
     std::size_t i = static_cast<std::size_t>(s);

     if (i >= MapOperationSettings::input_values.size()) [[unlikely]]
          return {};// monostate

     return MapOperationSettings::input_values[i];
}

TileOutputFunctionVariant MakeTileOutputFunction(TileOutputStrategy s)
{
     std::size_t i = static_cast<std::size_t>(s);

     if (i >= MapOperationSettings::output_values.size()) [[unlikely]]
          return {};// monostate

     return MapOperationSettings::output_values[i];
}
}// namespace ff_8