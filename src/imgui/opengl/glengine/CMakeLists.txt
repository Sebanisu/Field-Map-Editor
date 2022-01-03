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
add_library(glengine STATIC
        BatchRenderer.cpp
        FrameBuffer.cpp
        FrameBufferBackup.cpp
        FrameBufferRenderer.cpp
        GLCheck.cpp
        IndexBuffer.cpp
        IndexBufferDynamic.cpp
        Input.cpp
        Layer/LayerItem.cpp
        Layer/LayerStack.cpp
        Menu.cpp
        MenuItem.cpp
        OrthographicCameraController.cpp
        PixelBuffer.cpp
        Renderer.cpp
        Shader.cpp
        SubTexture.cpp
        Texture.cpp
        TimeStep.cpp
        VertexArray.cpp
        VertexBuffer.cpp
        VertexBufferDynamic.cpp
        Window.cpp
        scope_guard.cpp
        )
target_link_libraries(glengine
        PUBLIC project_warnings
        PUBLIC fmt::fmt
        PUBLIC Threads::Threads
        PUBLIC glfw
        PUBLIC OpenGL::GL
        PUBLIC GLEW::GLEW
        PUBLIC glm::glm
        PUBLIC stbi_wrapper
        PUBLIC imgui::imgui
        PUBLIC imgui_glfw
        )
target_compile_definitions(glengine
        PUBLIC GLEW_STATIC
        )
target_include_directories(glengine
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        )

if (MSVC)
    target_compile_definitions(glengine
            PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
    target_link_options(glengine PRIVATE "/PROFILE")
else ()
    target_compile_options(glengine PUBLIC "-fconcepts-diagnostics-depth=2")
endif ()

target_precompile_headers(glengine
        PUBLIC
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
        <numeric>
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
        <stb_image_write.h>
        <glm/glm.hpp>
        <glm/gtc/matrix_transform.hpp>
        <imgui.h>
        <imgui_impl_glfw.h>
        <imgui_impl_opengl3.h>
        [["concepts.hpp"]]
        [["Renderable.hpp"]]
        )
#target_include_directories(glengine PRIVATE
#        $<$<CONFIG:Debug>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/glengine.dir/Debug>
#        $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/glengine.dir/RelWithDebInfo>
#        $<$<CONFIG:Release>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/glengine.dir/Release>
#        #$<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/glengine.dir/MinSizeRel>
#        ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/glengine.dir)