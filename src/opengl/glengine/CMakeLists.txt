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
add_library(
  glengine STATIC
  glengine/BatchRenderer.cpp
  glengine/BlendModeSettings.cpp
  glengine/CompShader.cpp
  glengine/Counter.cpp
  glengine/DistanceBuffer.cpp
  glengine/FrameBuffer.cpp
  glengine/FrameBufferBackup.cpp
  glengine/GLCheck.cpp
  glengine/HistogramBuffer.cpp
  glengine/Image.cpp
  glengine/IndexBuffer.cpp
  glengine/IndexBufferDynamic.cpp
  glengine/Input.cpp
  glengine/Layer/LayerItem.cpp
  glengine/Layer/LayerStack.cpp
  glengine/MenuItem.cpp
  glengine/PaletteBuffer.cpp
  glengine/Shader.cpp
  glengine/SubTexture.cpp
  glengine/Texture.cpp
  glengine/TimeStep.cpp
  glengine/Vertex.cpp
  glengine/VertexArray.cpp
  glengine/VertexBuffer.cpp
  glengine/VertexBufferDynamic.cpp)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_link_libraries(glengine PUBLIC stdc++exp)
endif()
target_compile_features(
  glengine
  PUBLIC cxx_std_23
  PUBLIC cxx_std_20)
target_link_libraries(
  glengine
  PUBLIC project_warnings
  PUBLIC spdlog::spdlog
  PUBLIC Threads::Threads
  PUBLIC glfw
  PUBLIC OpenGL::GL
  PUBLIC GLEW::GLEW
  PUBLIC glm::glm
  PUBLIC stbi_wrapper)
target_include_directories(glengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(MSVC)
  target_compile_definitions(glengine PUBLIC _CRT_SECURE_NO_WARNINGS
  )# required by libpng
  target_compile_options(glengine
    PUBLIC "/bigobj"
    PUBLIC "/Qpar"
    PUBLIC "/MP"
  )
  # target_link_options(glengine PUBLIC "/PROFILE")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  target_compile_options(glengine PUBLIC "-fconcepts-diagnostics-depth=2")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  target_compile_options(
    glengine
    PUBLIC "-stdlib=libc++"
    # PUBLIC "-march=native"
    PUBLIC "-fexperimental-library" # new in clang15 for things like std::ranges
                                    # and std::format
  )
  target_link_options(
    glengine
    PUBLIC
    "-stdlib=libc++"
    # PUBLIC "-fuse-ld=lld" PUBLIC "-Wl"
    PUBLIC
    "-fexperimental-library" # new in clang15 for things like std::ranges and
                             # std::format
    PUBLIC
    "-v") # ,--gdb-index
endif()

if(supported)
  message(STATUS "IPO / LTO enabled")
  set_target_properties(glengine PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()
