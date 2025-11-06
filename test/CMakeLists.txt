# --------------------------------------------------------------
#  Boost.UT – tests
# --------------------------------------------------------------
find_package(ut REQUIRED CONFIG)

set(boost_namespace Boost)
if(ut_VERSION VERSION_LESS "1.1.9")
  set(boost_namespace boost)
endif()

# source_tile_conflicts_test -----------------------------------------------

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
  PRIVATE ${PROJECT_NAME}_MAIN_COMMON)
target_compile_definitions(
  ${PROJECT_NAME}_UT_source_tile_conflicts_test
  PRIVATE UT_source_tile_conflicts_test # PRIVATE NDEBUG
)

# Register the test with CTest
add_test(NAME ${PROJECT_NAME}_UT_source_tile_conflicts_test
         COMMAND ${PROJECT_NAME}_UT_source_tile_conflicts_test)

# TileOperationsTest -----------------------------------------------------
         add_executable(${PROJECT_NAME}_UT_TileOperationsTest
    UT_TileOperationsTest.cpp
)

target_link_libraries(${PROJECT_NAME}_UT_TileOperationsTest
    PRIVATE "${boost_namespace}::ut"
    PRIVATE ${PROJECT_NAME}_ff_8
)

target_compile_features(${PROJECT_NAME}_UT_TileOperationsTest
    PRIVATE cxx_std_23
)

target_compile_definitions(${PROJECT_NAME}_UT_TileOperationsTest
    PRIVATE UT_TileOperationsTest
    # PRIVATE NDEBUG   # uncomment if you want release asserts off
)

add_test(
    NAME ${PROJECT_NAME}_UT_TileOperationsTest
    COMMAND ${PROJECT_NAME}_UT_TileOperationsTest
)

# # optional – nicer ctest output
# set_tests_properties(${PROJECT_NAME}_UT_TileOperationsTest PROPERTIES
#     FAIL_REGULAR_EXPRESSION "failed"
#     PASS_REGULAR_EXPRESSION "[Aa]ll tests passed"
# )
