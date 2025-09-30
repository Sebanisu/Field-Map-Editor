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
find_package(imgui CONFIG REQUIRED)
find_package(tomlplusplus CONFIG REQUIRED)
add_executable(
  ${PROJECT_NAME}_EXPERIMENTAL
  Application.cpp
  ff8/Archive.cpp
  ff8/BPPs.cpp
  ff8/Coos.cpp
  ff8/FF8LoadTextures.cpp
  ff8/FF8Menu.cpp
  ff8/FF8UpscaleMenu.cpp
  ff8/Fields.cpp
  ff8/Mim.cpp
  ff8/Palettes.cpp
  ff8/Paths.cpp
  ff8/Upscales.cpp
  ff8/ImGuiTileDisplayWindow.cpp
  ff8/MapHistory.cpp
  ff8/MapTileAdjustments.hpp
  ff8/SimilarAdjustments.cpp
  ff8/Configuration.cpp
  ff8/MenuSaveToggles.hpp
  ff8/PupuID.hpp
  ImGuiDisabled.cpp
  ImGuiPushItemWidth.cpp
  ImGuiPushID.cpp
  ImGuiPushStyleVar.cpp
  ImGuiViewPortWindow.cpp
  main.cpp
  Menu.cpp
  OrthographicCameraController.cpp
  test/LayerTests.cpp
  test/TestBatchQuads.cpp
  test/TestBatchRenderer.cpp
  test/TestBatchRendering.cpp
  test/TestBatchRenderingTexture2D.cpp
  test/TestBatchRenderingTexture2DDynamic.cpp
  test/TestMenu.cpp
  test/TestTexture2D.cpp
  ff8/UniquifyPupu.hpp
  Window.cpp)
target_link_libraries(
  ${PROJECT_NAME}_EXPERIMENTAL
  PRIVATE glengine
  PRIVATE project_warnings
  PRIVATE Threads::Threads
  PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
  PRIVATE OpenVIII_CPP_WIP_VIIIArchive
  PRIVATE OpenVIII_CPP_WIP_VIIIPaths
  PRIVATE imgui::imgui
  PRIVATE GLEW::GLEW
  PRIVATE glfw
  PRIVATE OpenGL::GL)
target_include_directories(${PROJECT_NAME}_EXPERIMENTAL
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_paths(${PROJECT_NAME}_EXPERIMENTAL)

add_custom_command(
  TARGET ${PROJECT_NAME}_EXPERIMENTAL
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E echo
    "Copying ${CMAKE_SOURCE_DIR}/res to $<TARGET_FILE_DIR:${PROJECT_NAME}_EXPERIMENTAL>/res"
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res
          $<TARGET_FILE_DIR:${PROJECT_NAME}_EXPERIMENTAL>/res
  VERBATIM)

target_compile_definitions(${PROJECT_NAME}_EXPERIMENTAL
                           PUBLIC TOML_EXCEPTIONS=0)
if(MSVC)
  target_compile_definitions(
    ${PROJECT_NAME}_EXPERIMENTAL PUBLIC _CRT_SECURE_NO_WARNINGS) # required by
                                                                 # libpng
  target_compile_options(
    ${PROJECT_NAME}_EXPERIMENTAL
    PUBLIC "/Qpar"
    PUBLIC "/MP"
  )
  # target_link_options(${PROJECT_NAME}_EXPERIMENTAL PRIVATE "/PROFILE")
endif()
string(TOLOWER "${PROJECT_NAME}_EXPERIMENTAL" experimental_exec)
set_target_properties(${PROJECT_NAME}_EXPERIMENTAL PROPERTIES
    OUTPUT_NAME "${experimental_exec}"
)
if(supported)
  message(STATUS "IPO / LTO enabled")
  set_target_properties(${PROJECT_NAME}_EXPERIMENTAL
                        PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
  message(STATUS "IPO / LTO not supported: <${error}>")
endif()
# target_precompile_headers(
#   ${PROJECT_NAME}_EXPERIMENTAL
#   PRIVATE
#   <open_viii/archive/Archives.hpp>
#   <open_viii/graphics/BPPT.hpp>
#   <open_viii/graphics/Rectangle.hpp>
#   <open_viii/graphics/background/Map.hpp>
#   <open_viii/graphics/background/Mim.hpp>
#   <open_viii/graphics/color/Color32.hpp>
#   <open_viii/paths/Paths.hpp>
#   <open_viii/strings/LangCommon.hpp>)
target_compile_features(
  ${PROJECT_NAME}_EXPERIMENTAL
  PUBLIC cxx_std_23
  PUBLIC cxx_std_20)
# target_include_directories(${PROJECT_NAME}_EXPERIMENTAL PRIVATE
# $<$<CONFIG:Debug>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_EXPERIMENTAL.dir/Debug>
# $<$<CONFIG:RelWithDebInfo>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_EXPERIMENTAL.dir/RelWithDebInfo>
# $<$<CONFIG:Release>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_EXPERIMENTAL.dir/Release>
# #$<$<CONFIG:MinSizeRel>:${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_EXPERIMENTAL.dir/MinSizeRel>
# ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${PROJECT_NAME}_EXPERIMENTAL.dir)
