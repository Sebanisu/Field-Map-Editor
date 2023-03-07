cmake_minimum_required(VERSION 3.18)
## Supported values are 98, 11, 14, 17, 20, 23.
#To use all of c++20 on MVSC you must set cmake to 23
#https://github.com/microsoft/STL/issues/1814#issuecomment-845572895
#https://gitlab.kitware.com/cmake/cmake/-/issues/22606
#https://github.com/Kitware/CMake/blob/71bf838cf35fabc27ff009f2901eeffc786fa753/Modules/Compiler/MSVC-CXX.cmake#L32


set(CMAKE_CXX_STANDARD_REQUIRED ON) ## the value of the CXX_STANDARD target property is treated as a requirement
set(CMAKE_CXX_EXTENSIONS OFF) ## on g++ this ensures: -std=c++11 and not -std=gnu++11
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) ## automatically create a module definition (.def) file with all global
## symbols found in the input .obj files for a SHARED library on Windows.
# imgui test executable, with full warnings enabled
find_package(Threads REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/OpenVIII_CPP_WIP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/ImGuiFileBrowser.cmake)
add_executable(${PROJECT_NAME}_SFML
        test.cpp
        archives_group.cpp
        mim_sprite.cpp
        map_sprite.cpp
        MapHistory.cpp
        UniquifyPupu.hpp
        grid.cpp
        upscales.cpp
        Configuration.cpp
        safedir.cpp
        tile_operations.hpp
        gui/gui.cpp
        gui/file_dialog_mode.hpp gui/map_directory_mode.hpp gui/compact_type.hpp gui/mouse_positions.cpp gui/mouse_positions.hpp gui/scrolling.cpp gui/scrolling.hpp gui/Selections.cpp gui/Selections.hpp square.cpp square.hpp tile_sizes.hpp save_image_pbo.cpp save_image_pbo.hpp settings_backup.hpp map_group.cpp map_group.hpp map_operation.cpp map_operation.hpp open_file_explorer.cpp open_file_explorer.hpp RangeConsumer.hpp future_operations.cpp future_operations.hpp)
#set_property(TARGET ${PROJECT_NAME}_SFML PROPERTY CXX_STANDARD 20)
#find_package(ImGui-SFML REQUIRED)
find_package(tomlplusplus REQUIRED)
find_package(spdlog REQUIRED)
find_package(GLEW REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/imgui-sfml.cmake)
target_link_libraries(${PROJECT_NAME}_SFML
        PRIVATE project_warnings
        PRIVATE project_options
        PRIVATE ImGui-SFML
        PRIVATE OpenVIII_CPP_WIP_VIIIPaths
        PRIVATE OpenVIII_CPP_WIP_VIIIArchive
        PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
        PRIVATE Threads::Threads
        PRIVATE imgui_filebrowser
        PRIVATE tomlplusplus::tomlplusplus
        PRIVATE spdlog::spdlog
        PRIVATE GLEW::GLEW
        )
target_include_directories(${PROJECT_NAME}_SFML
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_definitions(${PROJECT_NAME}_SFML
        PUBLIC NOMINMAX # imgui uses unguarded min and max.
        PUBLIC TOML_EXCEPTIONS=0
        PUBLIC GLEW_STATIC
        )

target_compile_features(${PROJECT_NAME}_SFML
        PUBLIC cxx_std_23
        PUBLIC cxx_std_20)
if (MSVC)
    target_compile_definitions(${PROJECT_NAME}_SFML
            PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
    target_compile_options(${PROJECT_NAME}_SFML PUBLIC "/bigobj" PUBLIC "/await")
    target_link_options(${PROJECT_NAME}_SFML PRIVATE "/PROFILE")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME}_SFML PUBLIC "-fconcepts-diagnostics-depth=2")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME}_SFML
            PRIVATE "-stdlib=libc++"
            PRIVATE "-march=native"
            PRIVATE "-fexperimental-library" ##new in clang15 for things like std::ranges and std::format
            )
    target_link_options(${PROJECT_NAME}_SFML
            PRIVATE "-stdlib=libc++"
            PRIVATE "-fuse-ld=lld"
            PRIVATE "-Wl") #,--gdb-index
endif ()
enable_sanitizers(${PROJECT_NAME}_SFML)
SET_TARGET_PATHS(${PROJECT_NAME}_SFML)