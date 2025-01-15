# set(BOOST_UT_DISABLE_MODULE TRUE) find_package(ut CONFIG REQUIRED)
# function(add_test_common file_name)
# add_executable(${PROJECT_NAME}_UT_${file_name} ${file_name}.cpp)
# target_link_libraries( ${PROJECT_NAME}_UT_${file_name} PRIVATE Boost::ut
# PRIVATE ${PROJECT_NAME}_project_warnings)
# target_include_directories(${PROJECT_NAME}_UT_${file_name} PRIVATE
# ${CMAKE_CURRENT_SOURCE_DIR})
# set_target_properties(${PROJECT_NAME}_UT_${file_name} PROPERTIES OUTPUT_NAME
# "${file_name}") target_compile_definitions(${PROJECT_NAME}_UT_${file_name}
# PRIVATE BOOST_UT_DISABLE_MODULE=1) add_test(NAME
# ${PROJECT_NAME}_UT_${file_name} COMMAND ${PROJECT_NAME}_UT_${file_name})
# target_compile_features( ${PROJECT_NAME}_UT_${file_name} PUBLIC cxx_std_23
# PUBLIC cxx_std_20) if(MSVC) target_compile_definitions(
# ${PROJECT_NAME}_UT_${file_name} PUBLIC _CRT_SECURE_NO_WARNINGS )# required by
# libpng target_compile_options(${PROJECT_NAME}_UT_${file_name} PUBLIC
# "/bigobj") target_link_options(${PROJECT_NAME}_UT_${file_name} PUBLIC
# "/PROFILE") endif() #
# target_compile_definitions(${PROJECT_NAME}_UT_${file_name} PRIVATE #
# BOOST_UT_DISABLE_MODULE ) endfunction() add_test_common(source_tile_conflicts)

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