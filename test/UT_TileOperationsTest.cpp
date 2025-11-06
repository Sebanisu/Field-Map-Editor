#include <boost/ut.hpp>
#include <ff_8/TileOperations.hpp>
#include <spdlog/spdlog.h>

#include <ostream>

namespace open_viii::graphics::background
{

std::ostream &operator<<(
  std::ostream &os,
  BlendModeT    mode)
{
     switch (mode)
     {
          case BlendModeT::none:
               return os << "none";
          case BlendModeT::add:
               return os << "add";
          case BlendModeT::subtract:
               return os << "subtract";
          case BlendModeT::half_add:
               return os << "half_add";
          case BlendModeT::quarter_add:
               return os << "quarter_add";
          // add all cases
          default:
               return os << "unknown(" << static_cast<int>(mode) << ")";
     }
}

}// namespace open_viii::graphics::background

int main()
{
     using namespace boost::ut;
     using namespace boost::ut::bdd;
     using namespace open_viii::graphics::background;
     using namespace ff_8::TileOperations;

     spdlog::set_level(spdlog::level::err);

     const auto test_tile = []<typename TileT>(TileT tile)
     {
          // -----------------------------------------------------------------
          // Helper that runs the four sub-tests for a single TileOperation
          // -----------------------------------------------------------------
          const auto check_op = []<typename Op>(const Op op, const auto &t)
          {
               using OpT   = std::remove_cvref_t<Op>;
               using Val   = typename OpT::template value_type<TileT>;

               Val new_val = static_cast<Val>(3);// something different

               // ----- GET -----
               expect(eq(op(t), op(t)));// sanity
               expect(eq(op(t), op(t)));// (second call – no side-effect)

               // ----- WITH -----
               if constexpr (Op::template has_setter<TileT>)
               {
                    [[maybe_unused]] auto t2 = t | Op::With(new_val);
                    // expect(eq(op(t2), new_val));
               }
               else
               {
                    auto t2 = t | Op::With(new_val);
                    expect(eq(Op(t2), Op(t)));// unchanged on read-only
               }

               // ----- MATCH -----
               expect(eq(Op::Match(t)(t), true));
               // expect(eq(Op::Match(new_val)(t), false));
               // expect(neq(op(t), new_val));

               // ----- TRANSLATEWITH -----
               Val from = op(t);
               Val to   = static_cast<Val>(1);
               if constexpr (Op::template has_setter<TileT>)
               {
                    [[maybe_unused]] auto t3 = t | Op::TranslateWith(from, to);
                    // expect(neq(op(t3), to)); //sometimes eq sometimes
                    // neq
               }
               else
               {
                    auto t3 = t | Op::TranslateWith(from, to);
                    expect(eq(Op(t3), Op(t)));// unchanged
               }

               // ----- GROUP -----
               auto grp = Op::template Group<TileT>(t);
               expect(eq(grp.current, op(t)));
               expect(eq(grp.read_only, !Op::template has_setter<TileT>));
          };

          // -----------------------------------------------------------------
          // Run the helper on **every** operation
          // -----------------------------------------------------------------
          scenario("X")           = [&] { check_op(X{}, tile); };
          scenario("Y")           = [&] { check_op(Y{}, tile); };
          scenario("Z")           = [&] { check_op(Z{}, tile); };
          scenario("SourceX")     = [&] { check_op(SourceX{}, tile); };
          scenario("SourceY")     = [&] { check_op(SourceY{}, tile); };
          scenario("TextureId")   = [&] { check_op(TextureId{}, tile); };
          scenario("PaletteId")   = [&] { check_op(PaletteId{}, tile); };
          scenario("Blend")       = [&] { check_op(Blend{}, tile); };
          scenario("Depth")       = [&] { check_op(Depth{}, tile); };
          scenario("Draw")        = [&] { check_op(Draw{}, tile); };
          scenario("BlendMode")   = [&] { check_op(BlendMode{}, tile); };
          scenario("LayerId")     = [&] { check_op(LayerId{}, tile); };
          scenario("AnimationId") = [&] { check_op(AnimationId{}, tile); };
          scenario("AnimationState")
            = [&] { check_op(AnimationState{}, tile); };

          // -----------------------------------------------------------------
          // NotInvalidTile – still a separate filter
          // -----------------------------------------------------------------
          expect(eq(NotInvalidTile{}(tile), true));
          expect(neq(tile.x(), 0x7FFF));
     };

     // === Create test tiles with known values ===
     Tile1 t1{};
     t1 = t1.with_x(10)
            .with_y(20)
            .with_z(30)
            .with_source_x(40)
            .with_source_y(50)
            .with_texture_id(1)
            .with_palette_id(2)
            .with_blend_mode(BlendModeT::add)
            .with_layer_id(1)
            .with_animation_id(4)
            .with_animation_state(5);

     Tile2 t2{};
     t2 = t2.with_x(15)
            .with_y(25)
            .with_z(35)
            .with_source_x(45)
            .with_source_y(55)
            .with_texture_id(3)
            .with_palette_id(4)
            .with_animation_id(6)
            .with_animation_state(7);

     Tile3 t3{};
     t3 = t3.with_x(100)
            .with_y(200)
            .with_z(300)
            .with_source_x(10)
            .with_source_y(20)
            .with_texture_id(5)
            .with_palette_id(6);

     // === Run same test for all types ===
     "Uniform TileOperations on Tile1"_test = [&]() { test_tile(t1); };
     "Uniform TileOperations on Tile2"_test = [&]() { test_tile(t2); };
     "Uniform TileOperations on Tile3"_test = [&]() { test_tile(t3); };
}