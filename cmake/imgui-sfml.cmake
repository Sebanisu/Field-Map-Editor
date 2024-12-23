include(FetchContent)
find_package(imgui REQUIRED) #from conan
find_package(SFML REQUIRED) #from conan
#list(GET imgui_INCLUDE_DIRS 0 IMGUI_DIR)
#message(STATUS "dir='${IMGUI_DIR}'")
#foreach(dir ${imgui_INCLUDE_DIRS})
#    message(STATUS "dir='${dir}'")
#endforeach()
#options for FetchContent are at https://cmake.org/cmake/help/latest/module/ExternalProject.html
FetchContent_Declare(
imgui-sfml_fetch
GIT_REPOSITORY https://github.com/eliasdaler/imgui-sfml.git
GIT_TAG 2.6.x #origin/master
GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
)
#FetchContent_MakeAvailable(imgui-sfml_fetch)
FetchContent_GetProperties(imgui-sfml_fetch)
if (NOT imgui-sfml_fetch_POPULATED)
FetchContent_Populate(imgui-sfml_fetch)
#add_subdirectory(${imgui-sfml_fetch_SOURCE_DIR} ${imgui-sfml_fetch_BINARY_DIR} EXCLUDE_FROM_ALL)
    add_library(ImGui-SFML STATIC
            ${imgui-sfml_fetch_SOURCE_DIR}/imgui-SFML.cpp
            )
    target_include_directories(ImGui-SFML
            PUBLIC ${imgui-sfml_fetch_SOURCE_DIR}
            )
    target_link_libraries(ImGui-SFML
            PUBLIC imgui::imgui
            PUBLIC sfml::graphics
            PUBLIC sfml::system
            PUBLIC sfml::window)
endif ()