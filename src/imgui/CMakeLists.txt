# imgui test executable, with full warnings enabled
find_package(Threads REQUIRED)
include(${CMAKE_SOURCE_DIR}/cmake/OpenVIII_CPP_WIP.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/ImGuiFileBrowser.cmake)
add_executable(test_imgui test.cpp gui.cpp)
set_property(TARGET test_imgui PROPERTY CXX_STANDARD 20)
find_package(ImGui-SFML REQUIRED)
find_package(fmt)
target_link_libraries(test_imgui
        PRIVATE project_warnings
        PRIVATE project_options
        PRIVATE ImGui-SFML::ImGui-SFML
        PRIVATE fmt::fmt
        PRIVATE OpenVIII_CPP_WIP_VIIIPaths
        PRIVATE OpenVIII_CPP_WIP_VIIIArchive
        PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
        PRIVATE Threads::Threads
        PRIVATE imgui_filebrowser)

target_compile_definitions(test_imgui
        PUBLIC NOMINMAX) # imgui uses unguarded min and max.
if (MSVC)
    target_compile_definitions(test_imgui
            PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
endif ()