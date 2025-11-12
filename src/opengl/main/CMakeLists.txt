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
include(${CMAKE_SOURCE_DIR}/cmake/ImGuizmo.cmake)
set(MAIN_SOURCES
    gui/batch.cpp
    gui/collapsing_tile_info.cpp
    gui/create_tile_button.cpp
    gui/custom_paths_window.cpp
    gui/draw_window.cpp
    gui/field_file_window.cpp
    gui/filter_window.cpp
    gui/gui.cpp
    gui/history_window.cpp
    gui/key_value_data.cpp
    gui/keyboard_shortcuts_window.cpp
    gui/image_compare_window.cpp
    gui/import.cpp
    gui/main_menu_paths.cpp
    gui/mouse_positions.cpp
    gui/Selections.cpp
    gui/textures_window.cpp
    gui/tool_tip.cpp
    formatters.cpp
    future_operations.cpp
    generic_combo.cpp
    main.cpp
    map_operation.cpp
    map_sprite.cpp
    mim_sprite.cpp
    open_file_explorer.cpp
    path_search.cpp
    save_image_pbo.cpp
    tile_sizes.cpp
)

if(WIN32 AND NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_executable(${PROJECT_NAME}_MAIN WIN32 ${MAIN_SOURCES})
else()
  add_executable(${PROJECT_NAME}_MAIN ${MAIN_SOURCES})
endif()

# set_property(TARGET ${PROJECT_NAME}_MAIN PROPERTY CXX_STANDARD 20)
find_package(spdlog REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ctre REQUIRED)
find_package(iconfontcppheaders REQUIRED CONFIG)
find_package(clip CONFIG REQUIRED)

add_library(${PROJECT_NAME}_MAIN_COMMON INTERFACE)
target_include_directories(${PROJECT_NAME}_MAIN_COMMON
                           INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

string(TOLOWER "${PROJECT_NAME}" main_exec)
set_target_properties(${PROJECT_NAME}_MAIN PROPERTIES OUTPUT_NAME
                                                      "${main_exec}")
target_compile_definitions(
  ${PROJECT_NAME}_MAIN
  PUBLIC NOMINMAX # imgui uses unguarded min and max.
  PUBLIC DEFAULT_CONFIG_PATH="${DEFAULT_CONFIG_ROOT_PATH}/${main_exec}.toml"
  PUBLIC DEFAULT_IMGUI_INI_PATH="${DEFAULT_CONFIG_ROOT_PATH}/${main_exec}_imgui.ini"
  PUBLIC DEFAULT_LOG_PATH="${DEFAULT_CONFIG_ROOT_PATH}/${main_exec}.log"
  )
target_link_libraries(
  ${PROJECT_NAME}_MAIN
  PRIVATE project_warnings
  PRIVATE project_options
  PRIVATE imgui::imgui
  PRIVATE GLEW::GLEW
  PRIVATE glfw
  PRIVATE OpenGL::GL
  PRIVATE glengine
  PRIVATE ${PROJECT_NAME}_ff_8
  PRIVATE ${PROJECT_NAME}_imgui_utils
  PRIVATE OpenVIII_CPP_WIP_VIIIPaths
  PRIVATE OpenVIII_CPP_WIP_VIIIArchive
  PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
  PRIVATE Threads::Threads
  PRIVATE imgui_filebrowser
  PRIVATE spdlog::spdlog
  PRIVATE iconfontcppheaders
  PRIVATE ctre::ctre
  PRIVATE ImGuizmo
  PRIVATE clip::clip
  PRIVATE ${PROJECT_NAME}_MAIN_COMMON)


target_compile_features(
  ${PROJECT_NAME}_MAIN
  PUBLIC cxx_std_23
  PUBLIC cxx_std_20)
if(MSVC)
  target_compile_definitions(
    ${PROJECT_NAME}_MAIN PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
  target_compile_options(
    ${PROJECT_NAME}_MAIN
    PUBLIC "/bigobj"
    PUBLIC "/await"
    PUBLIC "/Qpar"
    PUBLIC "/MP"
  ) # PUBLIC "/Zc:char8_t-"
  target_link_options(${PROJECT_NAME}_MAIN PUBLIC "/PROFILE")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(
    ${PROJECT_NAME}_MAIN PUBLIC "-fconcepts-diagnostics-depth=2"
  )# PUBLIC "-fno-char8_t"
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(
    ${PROJECT_NAME}_MAIN
    PUBLIC "-stdlib=libc++"
    # PUBLIC "-march=native"
    PUBLIC "-fexperimental-library" # new in clang15 for things like std::ranges
                                    # and std::format
    # PUBLIC "-fno-char8_t"
  )
  target_link_options(
    ${PROJECT_NAME}_MAIN
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
enable_sanitizers(${PROJECT_NAME}_MAIN)
set_target_paths(${PROJECT_NAME}_MAIN)

add_custom_command(
  TARGET ${PROJECT_NAME}_MAIN
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E echo "Copying fonts to target directory..."
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/fonts"
          "$<TARGET_FILE_DIR:${PROJECT_NAME}_MAIN>/fonts"
  COMMAND
    ${CMAKE_COMMAND} -E echo
    "Copying ${CMAKE_SOURCE_DIR}/res to $<TARGET_FILE_DIR:${PROJECT_NAME}_MAIN>/res"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res
          $<TARGET_FILE_DIR:${PROJECT_NAME}_MAIN>/res
  VERBATIM)
