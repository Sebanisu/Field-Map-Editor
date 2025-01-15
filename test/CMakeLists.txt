find_package(ut REQUIRED CONFIG)

set(boost_namespace Boost)
if(ut_VERSION VERSION_LESS "1.1.9")
  set(boost_namespace boost)
endif()

add_executable(${PROJECT_NAME}_UT_source_tile_conflicts_test
               UT_source_tile_conflicts_test.cpp)
target_link_libraries(${PROJECT_NAME}_UT_source_tile_conflicts_test
                      PRIVATE "${boost_namespace}::ut")
target_compile_features(
  ${PROJECT_NAME}_UT_source_tile_conflicts_test
  PRIVATE cxx_std_23
  PRIVATE cxx_std_20)
target_link_libraries(
  ${PROJECT_NAME}_UT_source_tile_conflicts_test
  PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
  PRIVATE ${PROJECT_NAME}_SFML_Common)
target_compile_definitions(
  ${PROJECT_NAME}_UT_source_tile_conflicts_test
  PRIVATE UT_source_tile_conflicts_test # PRIVATE NDEBUG
)

# Register the test with CTest
add_test(NAME ${PROJECT_NAME}_UT_source_tile_conflicts_test
         COMMAND ${PROJECT_NAME}_UT_source_tile_conflicts_test)
