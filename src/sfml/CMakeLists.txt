cmake_minimum_required(VERSION 3.18)
# Supported values are 98, 11, 14, 17, 20, 23. To use all of c++20 on MVSC you
# must set cmake to 23
# https://github.com/microsoft/STL/issues/1814#issuecomment-845572895
# https://gitlab.kitware.com/cmake/cmake/-/issues/22606
# https://github.com/Kitware/CMake/blob/71bf838cf35fabc27ff009f2901eeffc786fa753/Modules/Compiler/MSVC-CXX.cmake#L32

set(CMAKE_CXX_STANDARD_REQUIRED ON) # the value of the CXX_STANDARD target
                                    # property is treated as a requirement
set(CMAKE_CXX_EXTENSIONS OFF) # on g++ this ensures: -std=c++11 and not
                              # -std=gnu++11
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # automatically create a module
                                         # definition (.def) file with all
                                         # global
# symbols found in the input .obj files for a SHARED library on Windows. imgui
# test executable, with full warnings enabled
find_package(Threads REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/OpenVIII_CPP_WIP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/ImGuiFileBrowser.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/IconFontCppHeaders.cmake)
add_executable(
  ${PROJECT_NAME}_SFML
  gui/batch.cpp
  gui/collapsing_tile_info.cpp
  gui/create_tile_button.cpp
  gui/custom_paths_window.cpp
  gui/field_file_window.cpp
  gui/gui.cpp
  gui/history_window.cpp
  gui/key_value_data.cpp
  gui/import.cpp
  gui/main_menu_paths.cpp
  gui/mouse_positions.cpp
  gui/push_pop_id.cpp
  gui/Selections.cpp
  gui/tool_tip.cpp
  archives_group.cpp
  Configuration.cpp
  events.cpp
  formatters.cpp
  future_operations.cpp
  generic_combo.cpp
  grid.cpp
  main.cpp
  map_group.cpp
  map_operation.cpp
  MapHistory.cpp
  map_sprite.cpp
  mim_sprite.cpp
  open_file_explorer.cpp
  PupuID.cpp
  safedir.cpp
  save_image_pbo.cpp
  source_tile_conflicts.cpp
  square.cpp
  tile_sizes.cpp
  unique_values.cpp
  UniquifyPupu.cpp
  upscales.cpp
  utilities.cpp)
# set_property(TARGET ${PROJECT_NAME}_SFML PROPERTY CXX_STANDARD 20)
find_package(tomlplusplus REQUIRED)
find_package(spdlog REQUIRED)
find_package(GLEW REQUIRED)
find_package(ctre REQUIRED)
find_package(iconfontcppheaders REQUIRED CONFIG)

include(${CMAKE_SOURCE_DIR}/cmake/imgui-sfml.cmake)
#find_package(ImGui-SFML)

add_library(${PROJECT_NAME}_SFML_Common INTERFACE)
target_include_directories(${PROJECT_NAME}_SFML_Common
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(
  ${PROJECT_NAME}_SFML
  PRIVATE project_warnings
  PRIVATE project_options
  #PRIVATE ImGui-SFML::ImGui-SFML
  PRIVATE ImGui-SFML
  PRIVATE OpenVIII_CPP_WIP_VIIIPaths
  PRIVATE OpenVIII_CPP_WIP_VIIIArchive
  PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
  PRIVATE Threads::Threads
  PRIVATE imgui_filebrowser
  PRIVATE tomlplusplus::tomlplusplus
  PRIVATE spdlog::spdlog
  PRIVATE GLEW::GLEW
  PRIVATE iconfontcppheaders::iconfontcppheaders
  PRIVATE ctre::ctre
  PRIVATE ${PROJECT_NAME}_SFML_Common)

target_compile_definitions(
  ${PROJECT_NAME}_SFML
  PUBLIC NOMINMAX # imgui uses unguarded min and max.
  PUBLIC TOML_EXCEPTIONS=0
  PUBLIC GLEW_STATIC)

target_compile_features(
  ${PROJECT_NAME}_SFML
  PUBLIC cxx_std_23
  PUBLIC cxx_std_20)
if(MSVC)
  target_compile_definitions(
    ${PROJECT_NAME}_SFML PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
  target_compile_options(
    ${PROJECT_NAME}_SFML
    PUBLIC "/bigobj"
    PUBLIC "/await") # PUBLIC "/Zc:char8_t-"
  target_link_options(${PROJECT_NAME}_SFML PUBLIC "/PROFILE")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(
    ${PROJECT_NAME}_SFML PUBLIC "-fconcepts-diagnostics-depth=2"
  )# PUBLIC "-fno-char8_t"
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(
    ${PROJECT_NAME}_SFML
    PUBLIC "-stdlib=libc++"
    # PUBLIC "-march=native"
    PUBLIC "-fexperimental-library" # new in clang15 for things like std::ranges
                                    # and std::format
    # PUBLIC "-fno-char8_t"
  )
  target_link_options(
    ${PROJECT_NAME}_SFML
    PUBLIC
    "-stdlib=libc++"
    # PUBLIC "-fuse-ld=lld"
    PUBLIC
    "-fexperimental-library" # new in clang15 for things like std::ranges and
                             # std::format
    # PUBLIC "-Wl" -lstdc++
    PUBLIC
    "-v") # ,--gdb-index
endif()
enable_sanitizers(${PROJECT_NAME}_SFML)
set_target_paths(${PROJECT_NAME}_SFML)

add_custom_command(
  TARGET ${PROJECT_NAME}_SFML
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copying fonts to target directory..."
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/fonts"
          "$<TARGET_FILE_DIR:${PROJECT_NAME}_SFML>/fonts"
  VERBATIM)
