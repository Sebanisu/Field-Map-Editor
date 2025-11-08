cmake_minimum_required(VERSION 3.18)
include(${CMAKE_SOURCE_DIR}/cmake/OpenVIII_CPP_WIP.cmake)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
add_library(
  ${PROJECT_NAME}_ff_8 STATIC
  ff_8/ArchivesGroup.cpp
  ff_8/PupuID.cpp
  ff_8/UniquifyPupu.cpp
  )

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME}_ff_8 PUBLIC stdc++exp)
endif()
target_compile_features(
  ${PROJECT_NAME}_ff_8
  PUBLIC cxx_std_23
  PUBLIC cxx_std_20)
target_link_libraries(
  ${PROJECT_NAME}_ff_8
  PUBLIC project_warnings
  PUBLIC spdlog::spdlog
  PUBLIC OpenVIII_CPP_WIP_VIIIPaths
  PUBLIC OpenVIII_CPP_WIP_VIIIArchive
  PUBLIC OpenVIII_CPP_WIP_VIIIGraphics
  PUBLIC Threads::Threads)
target_include_directories(${PROJECT_NAME}_ff_8 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
  target_compile_definitions(${PROJECT_NAME}_ff_8 PUBLIC _CRT_SECURE_NO_WARNINGS
  )# required by libpng
  target_compile_options(${PROJECT_NAME}_ff_8
    PUBLIC "/bigobj"
    PUBLIC "/Qpar"
    PUBLIC "/MP"
  )
  # target_link_options(${PROJECT_NAME}_ff_8 PUBLIC "/PROFILE")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME}_ff_8 PUBLIC "-fconcepts-diagnostics-depth=2")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(
    ${PROJECT_NAME}_ff_8
    PUBLIC "-stdlib=libc++"
    # PUBLIC "-march=native"
    PUBLIC "-fexperimental-library" # new in clang15 for things like std::ranges
                                    # and std::format
  )
  target_link_options(
    ${PROJECT_NAME}_ff_8
    PUBLIC
    "-stdlib=libc++"
    # PUBLIC "-fuse-ld=lld" PUBLIC "-Wl"
    PUBLIC
    "-fexperimental-library" # new in clang15 for things like std::ranges and
                             # std::format
    PUBLIC
    "-v") # ,--gdb-index
endif()

if(supported)
  message(STATUS "IPO / LTO enabled")
  set_target_properties(${PROJECT_NAME}_ff_8 PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()
