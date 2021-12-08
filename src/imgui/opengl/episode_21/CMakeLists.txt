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
find_package(stb REQUIRED)
add_executable(opengl_series_episode_21
        main.cpp
        Renderer.cpp
        VertexBuffer.cpp
        VertexBufferDynamic.cpp
        IndexBuffer.cpp
        IndexBufferDynamic.cpp
        VertexArray.cpp
        VertexBufferLayout.cpp
        Shader.cpp
        Texture.cpp
        tests/Test.hpp
        tests/TestTexture2D.cpp
        tests/TestClearColor.cpp
        tests/TestBatchRendering.cpp
        scope_guard.cpp
        tests/TestBatchRenderingTexture2D.cpp
        tests/TestBatchRenderingTexture2DDynamic.cpp
        tests/TestMenu.cpp
        tests/TestMenuItem.cpp
        tests/TestBatchQuads.cpp
        BatchRenderer.cpp
        tests/TestBatchRenderer.cpp
        SubTexture.cpp
        ff8/Archive.cpp
        ff8/Paths.cpp
        ff8/Coos.cpp
        ff8/Fields.cpp
        tests/TestFF8Archive.cpp
        ff8/Map.cpp
        ff8/Mim.cpp
        ff8/FF8MenuItem.cpp
        ff8/FF8Menu.cpp
        ff8/Palettes.cpp
        ff8/BPPs.cpp DelayedTextures.cpp DelayedTextures.hpp ff8/FF8LoadTextures.cpp ff8/FF8LoadTextures.hpp OrthographicCameraController.cpp OrthographicCameraController.hpp Input.cpp Input.hpp unique_value.hpp Window.cpp Window.hpp Event.hpp)
target_link_libraries(opengl_series_episode_21
        PRIVATE project_warnings
        PRIVATE fmt::fmt
        PRIVATE Threads::Threads
        PRIVATE glfw
        PRIVATE OpenGL::GL
        PRIVATE GLEW::GLEW
        PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
        PRIVATE OpenVIII_CPP_WIP_VIIIArchive
        PRIVATE stb::stb
        PRIVATE glm::glm
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