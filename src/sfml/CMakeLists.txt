cmake_minimum_required(VERSION 3.18)
if (MSVC AND
        CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND
        CMAKE_VERSION VERSION_GREATER 3.21.0)
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
elseif (MSVC AND
        CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND
        CMAKE_VERSION VERSION_GREATER 3.20.3 AND
        CMAKE_PATCH_VERSION LESS 21000000)
    # this change happened in CMake 3.20.4
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
elseif (MSVC AND
        CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.29.30129 AND
        CMAKE_VERSION VERSION_GREATER 3.20.3 AND
        CMAKE_PATCH_VERSION GREATER 21032501) #update this when msvc does support 23 flag.
    # msvc uses custom patch version example 21032501 it's older than 3.20.4
    set(CMAKE_CXX_STANDARD 23) # /std:c++latest - unlocks the non stable cpp20 features. For new 16.11 versions
else ()
    set(CMAKE_CXX_STANDARD 20) # /std:c++latest for msvc and -std=c++20 for everyone else.
endif ()
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
        grid.cpp
        upscales.cpp
        Configuration.cpp
        safedir.cpp
        gui/gui.cpp
        gui/gui_batch.cpp
        gui/batch_deswizzle.cpp gui/batch_deswizzle.hpp gui/map_dialog_mode.hpp gui/map_directory_mode.hpp gui/batch_embed.cpp gui/batch_embed.hpp gui/compact_type.hpp)
#set_property(TARGET ${PROJECT_NAME}_SFML PROPERTY CXX_STANDARD 20)
#find_package(ImGui-SFML REQUIRED)
find_package(cppcoro) # todo will probably remove at some point.
find_package(tomlplusplus REQUIRED)
find_package(spdlog REQUIRED)
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
        PRIVATE cppcoro::cppcoro
        PRIVATE tomlplusplus::tomlplusplus
        PRIVATE spdlog::spdlog
        )
target_include_directories(${PROJECT_NAME}_SFML
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_definitions(${PROJECT_NAME}_SFML
        PUBLIC NOMINMAX # imgui uses unguarded min and max.
        PUBLIC TOML_EXCEPTIONS=0)
if (MSVC)
    target_compile_definitions(${PROJECT_NAME}_SFML
            PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
    target_link_options(${PROJECT_NAME}_SFML PRIVATE "/PROFILE")
else ()
target_compile_options(${PROJECT_NAME}_SFML PUBLIC "-fconcepts-diagnostics-depth=2")
endif ()
enable_sanitizers(${PROJECT_NAME}_SFML)
SET_TARGET_PATHS(${PROJECT_NAME}_SFML)