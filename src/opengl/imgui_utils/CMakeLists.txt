cmake_minimum_required(VERSION 3.18)
include(${CMAKE_SOURCE_DIR}/cmake/OpenVIII_CPP_WIP.cmake)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
find_package(tomlplusplus CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(clip CONFIG REQUIRED)
add_library(
  ${PROJECT_NAME}_imgui_utils STATIC
  #... tbd
  imgui_utils/ImGuiPushID.cpp
  )

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(${PROJECT_NAME}_imgui_utils PUBLIC stdc++exp)
endif()
target_compile_features(
  ${PROJECT_NAME}_imgui_utils
  PUBLIC cxx_std_23
  PUBLIC cxx_std_20)
target_link_libraries(
  ${PROJECT_NAME}_imgui_utils
  PUBLIC project_warnings
  PUBLIC imgui_filebrowser
  PUBLIC imgui::imgui
  PUBLIC clip::clip
  PUBLIC ImGuizmo
  PUBLIC glengine
  PUBLIC spdlog::spdlog
  #PUBLIC tomlplusplus::tomlplusplus
  PUBLIC Threads::Threads)
target_include_directories(${PROJECT_NAME}_imgui_utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
  target_compile_definitions(${PROJECT_NAME}_imgui_utils PUBLIC _CRT_SECURE_NO_WARNINGS
 )# required by libpng  
  target_compile_options(${PROJECT_NAME}_imgui_utils
    PUBLIC "/bigobj"
    PUBLIC "/Qpar"
    PUBLIC "/MP"
  )
  # target_link_options(${PROJECT_NAME}_imgui_utils PUBLIC "/PROFILE")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(${PROJECT_NAME}_imgui_utils PUBLIC "-fconcepts-diagnostics-depth=2")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(
    ${PROJECT_NAME}_imgui_utils
    PUBLIC "-stdlib=libc++"
    # PUBLIC "-march=native"
    PUBLIC "-fexperimental-library" # new in clang15 for things like std::ranges
                                    # and std::format
  )
  target_link_options(
    ${PROJECT_NAME}_imgui_utils
    PUBLIC
    "-stdlib=libc++"
    # PUBLIC "-fuse-ld=lld" PUBLIC "-Wl"
    PUBLIC
    "-fexperimental-library" # new in clang15 for things like std::ranges and
                             # std::format
    PUBLIC
    "-v") # ,--gdb-index
endif()

target_compile_definitions(
  ${PROJECT_NAME}_imgui_utils
  PUBLIC NOMINMAX # imgui uses unguarded min and max.
  PUBLIC TOML_EXCEPTIONS=0
  PUBLIC TOML_HEADER_ONLY=1
)

if(supported)
  message(STATUS "IPO / LTO enabled")
  set_target_properties(${PROJECT_NAME}_imgui_utils PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()
