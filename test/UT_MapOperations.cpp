// UT_MapOperations.cpp — robust, zero-magic-number version
#include <boost/ut.hpp>
#include <ff_8/MapOperations.hpp>
#include <string_view>
#include <type_traits>
int main()
{
     using namespace boost::ut;
     using namespace ff_8;
     using namespace std::string_view_literals;
     using namespace MapOperationSettings;


     "MakeTileInputFunction returns correct type for each strategy"_test = []
     {
          expect(
            std::holds_alternative<
              TileFunction<TileInputFunction<TileInputStrategy::MimSwizzle>>>(
              MakeTileInputFunction(TileInputStrategy::MimSwizzle)));

          expect(
            std::holds_alternative<TileFunction<
              TileInputFunction<TileInputStrategy::ExternalSwizzle>>>(
              MakeTileInputFunction(TileInputStrategy::ExternalSwizzle)));

          expect(
            std::holds_alternative<TileFunction<
              TileInputFunction<TileInputStrategy::ExternalDeswizzle>>>(
              MakeTileInputFunction(TileInputStrategy::ExternalDeswizzle)));

          expect(
            std::holds_alternative<TileFunction<
              TileInputFunction<TileInputStrategy::ExternalSwizzleAsOneImage>>>(
              MakeTileInputFunction(
                TileInputStrategy::ExternalSwizzleAsOneImage)));

          // Invalid / All → monostate
          expect(
            std::holds_alternative<std::monostate>(
              MakeTileInputFunction(TileInputStrategy::All)));
          expect(
            std::holds_alternative<std::monostate>(
              MakeTileInputFunction(static_cast<TileInputStrategy>(99))));
     };

     "MakeTileOutputFunction returns correct type"_test = []
     {
          expect(
            std::holds_alternative<
              TileFunction<TileOutputFunction<TileOutputStrategy::Swizzle>>>(
              MakeTileOutputFunction(TileOutputStrategy::Swizzle)));

          expect(
            std::holds_alternative<
              TileFunction<TileOutputFunction<TileOutputStrategy::Deswizzle>>>(
              MakeTileOutputFunction(TileOutputStrategy::Deswizzle)));

          expect(
            std::holds_alternative<TileFunction<
              TileOutputFunction<TileOutputStrategy::SwizzleAsOneImage>>>(
              MakeTileOutputFunction(TileOutputStrategy::SwizzleAsOneImage)));
     };

     "Traits are correct without assuming order"_test = []
     {
          auto check_strategy = [](const auto &strategy)
          {
               using Func = std::remove_cvref_t<decltype(strategy)>;

               if constexpr (std::is_same_v<Func, std::monostate>)
               {
                    return;
               }
               else
               {
                    using ExpectedX = typename Func::XType;
                    using ExpectedY = typename Func::YType;

                    // Example: ExternalSwizzleAsOneImage should use
                    // SwizzleAsOneImage ops
                    if constexpr (
                      std::is_same_v<
                        Func, TileFunction<TileInputFunction<
                                TileInputStrategy::ExternalSwizzleAsOneImage>>>)
                    {
                         expect(
                           std::is_same_v<
                             ExpectedX, TileOperations::SwizzleAsOneImage::X>);
                         expect(
                           std::is_same_v<
                             ExpectedY, TileOperations::SwizzleAsOneImage::Y>);
                    }
               }
          };

          // Visit every valid input strategy
          for (std::size_t i = 1; i < MapOperationSettings::input_values.size();
               ++i)
          {
               std::visit(
                 check_strategy, MapOperationSettings::input_values[i]);
          }
     };

     // ==================================================================
     // NEW: Exhaustive compile-time array correctness
     // ==================================================================

     "MapOperationSettings arrays have exact expected size and content"_test
       = []
     {
          static_assert(
            input_values.size()
            == std::variant_size_v<TileInputFunctionVariant>);
          static_assert(
            output_values.size()
            == std::variant_size_v<TileOutputFunctionVariant>);

          expect(eq(input_labels.size(), input_values.size()));
          expect(eq(input_descriptions.size(), input_values.size()));
          expect(eq(output_labels.size(), output_values.size()));
          expect(eq(output_descriptions.size(), output_values.size()));

          // First entry is always monostate → empty label
          expect(eq(input_labels[0], ""sv));
          expect(eq(input_descriptions[0], ""sv));
          expect(std::holds_alternative<std::monostate>(input_values[0]));
     };
}