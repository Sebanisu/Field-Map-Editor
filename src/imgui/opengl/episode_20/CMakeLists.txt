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
add_executable(opengl_series_episode_20
        main.cpp
        Renderer.cpp
        VertexBuffer.cpp
        IndexBuffer.cpp
        VertexArray.cpp
        VertexBufferLayout.cpp
        Shader.cpp
        Texture.cpp
        tests/Test.h
        tests/TestClearColor.cpp
        tests/TestMenu.cpp
        tests/TestTexture2D.cpp
        tests/TestBatchRendering.cpp
        )
target_link_libraries(opengl_series_episode_20
        PRIVATE project_warnings
        PRIVATE fmt::fmt
        PRIVATE Threads::Threads
        PRIVATE glfw
        PRIVATE OpenGL::GL
        PRIVATE GLEW::GLEW
        PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
        PRIVATE glm::glm
        PRIVATE imgui::imgui
        PRIVATE imgui_glfw
        )
target_compile_definitions(opengl_series_episode_20
        PRIVATE GLEW_STATIC
        )
target_include_directories(opengl_series_episode_20
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )
SET_TARGET_PATHS(opengl_series_episode_20)


add_custom_command(TARGET opengl_series_episode_20 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
        "Copying ${CMAKE_CURRENT_SOURCE_DIR}/res to $<TARGET_FILE_DIR:opengl_series_episode_20>/res")
add_custom_command(TARGET opengl_series_episode_20 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:opengl_series_episode_20>/res)

if (MSVC)
    target_compile_definitions(opengl_series_episode_20
            PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
    target_link_options(opengl_series_episode_20 PRIVATE "/PROFILE")
endif ()