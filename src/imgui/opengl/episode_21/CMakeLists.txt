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


include(${CMAKE_SOURCE_DIR}/cmake/OpenVIII_CPP_WIP.cmake)
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
add_subdirectory(stbi)
add_executable(opengl_series_episode_21
        Application.cpp
        BatchRenderer.cpp
        DelayedTextures.cpp
        IndexBuffer.cpp
        IndexBufferDynamic.cpp
        Input.cpp
        LayerItem.cpp
        LayerStack.cpp
        LayerTests.cpp
        Menu.cpp
        MenuItem.cpp
        OrthographicCameraController.cpp
        Renderer.cpp
        Shader.cpp
        SubTexture.cpp
        Texture.cpp
        VertexArray.cpp
        VertexBuffer.cpp
        VertexBufferDynamic.cpp
        VertexBufferLayout.cpp
        Window.cpp
        ff8/Archive.cpp
        ff8/BPPs.cpp
        ff8/Coos.cpp
        ff8/FF8LoadTextures.cpp
        ff8/FF8Menu.cpp
        ff8/Fields.cpp
        ff8/Map.cpp
        ff8/Mim.cpp
        ff8/Palettes.cpp
        ff8/Paths.cpp
        main.cpp
        scope_guard.cpp
        tests/TestBatchQuads.cpp
        tests/TestBatchRenderer.cpp
        tests/TestBatchRendering.cpp
        tests/TestBatchRenderingTexture2D.cpp
        tests/TestBatchRenderingTexture2DDynamic.cpp
        tests/TestClearColor.cpp
        tests/TestFF8Archive.cpp
        tests/TestMenu.cpp
        tests/TestTexture2D.cpp
        TimeStep.cpp TimeStep.hpp)
target_link_libraries(opengl_series_episode_21
        PRIVATE project_warnings
        PRIVATE fmt::fmt
        PRIVATE Threads::Threads
        PRIVATE glfw
        PRIVATE OpenGL::GL
        PRIVATE GLEW::GLEW
        PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
        PRIVATE OpenVIII_CPP_WIP_VIIIArchive
        PRIVATE glm::glm
        PRIVATE stbi_wrapper
        PRIVATE imgui::imgui
        PRIVATE imgui_glfw
        )
target_compile_definitions(opengl_series_episode_21
        PRIVATE GLEW_STATIC
        )
target_include_directories(opengl_series_episode_21
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )
SET_TARGET_PATHS(opengl_series_episode_21)


add_custom_command(TARGET opengl_series_episode_21 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
        "Copying ${CMAKE_CURRENT_SOURCE_DIR}/res to $<TARGET_FILE_DIR:opengl_series_episode_21>/res")
add_custom_command(TARGET opengl_series_episode_21 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:opengl_series_episode_21>/res)

if (MSVC)
    target_compile_definitions(opengl_series_episode_21
            PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
    target_link_options(opengl_series_episode_21 PRIVATE "/PROFILE")
endif ()

target_precompile_headers(opengl_series_episode_21
        PRIVATE
        <algorithm>
        <array>
        <cassert>
        <concepts>
        <cstdint>
        <cstring>
        <filesystem>
        <fstream>
        <functional>
        <future>
        <iterator>
        <memory>
        <optional>
        <ranges>
        <source_location>
        <span>
        <sstream>
        <string>
        <string_view>
        <thread>
        <type_traits>
        <unordered_map>
        <utility>
        <variant>
        <vector>
        <GL/glew.h>
        <GLFW/glfw3.h>
        <fmt/format.h>
        <fmt/chrono.h>
        <stb_image.h>
        <glm/glm.hpp>
        <glm/gtc/matrix_transform.hpp>
        <imgui.h>
        <imgui_impl_glfw.h>
        <imgui_impl_opengl3.h>
        <open_viii/archive/Archives.hpp>
        <open_viii/graphics/BPPT.hpp>
        <open_viii/graphics/Rectangle.hpp>
        <open_viii/graphics/background/Map.hpp>
        <open_viii/graphics/background/Mim.hpp>
        <open_viii/graphics/color/Color32.hpp>
        <open_viii/paths/Paths.hpp>
        <open_viii/strings/LangCommon.hpp>
        [["concepts.hpp"]]
        [["Renderable.hpp"]]
        )