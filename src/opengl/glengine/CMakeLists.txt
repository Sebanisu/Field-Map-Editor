cmake_minimum_required(VERSION 3.18)
include(${CMAKE_SOURCE_DIR}/cmake/OpenVIII_CPP_WIP.cmake)
include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)
find_package(Threads REQUIRED)
find_package(fmt REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(imgui REQUIRED)
find_package(spdlog REQUIRED)
add_library(glengine STATIC
        BatchRenderer.cpp
        Counter.cpp
        FrameBuffer.cpp
        FrameBufferBackup.cpp
        GLCheck.cpp
        ImGuiDisabled.cpp
        ImGuiPushItemWidth.cpp
        ImGuiPushID.cpp
        ImGuiPushStyleVar.cpp
        ImGuiViewPortWindow.cpp
        IndexBuffer.cpp
        IndexBufferDynamic.cpp
        Input.cpp
        Layer/LayerItem.cpp
        Layer/LayerStack.cpp
        Menu.cpp
        MenuItem.cpp
        OrthographicCameraController.cpp
        Shader.cpp
        SubTexture.cpp
        Texture.cpp
        TimeStep.cpp
        VertexArray.cpp
        VertexBuffer.cpp
        VertexBufferDynamic.cpp
        Window.cpp
        )
target_compile_features(glengine
        PUBLIC cxx_std_23
        PUBLIC cxx_std_20)
target_link_libraries(glengine
        PUBLIC project_warnings
        PUBLIC spdlog::spdlog
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
    target_link_options(glengine PUBLIC "/PROFILE")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(glengine PUBLIC "-fconcepts-diagnostics-depth=2")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME}_SFML
            PUBLIC "-stdlib=libc++"
            PUBLIC "-march=native"
            PUBLIC "-fexperimental-library" ##new in clang15 for things like std::ranges and std::format
            )
    target_link_options(${PROJECT_NAME}_SFML
            PUBLIC "-stdlib=libc++"
            PUBLIC "-fuse-ld=lld"
            PUBLIC "-Wl") #,--gdb-index
endif ()

if (supported)
    message(STATUS "IPO / LTO enabled")
    set_target_properties(glengine PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else ()
    message(STATUS "IPO / LTO not supported: <${error}>")
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
        <spdlog/spdlog.h>
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