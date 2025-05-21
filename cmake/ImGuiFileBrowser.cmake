
#options for FetchContent are at https://cmake.org/cmake/help/latest/module/ExternalProject.html
include(FetchContent)
FetchContent_Declare(
    imgui_filebrowser_fetch
    #GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser.git
    GIT_REPOSITORY https://github.com/sebanisu/imgui-filebrowser.git
    GIT_TAG origin/master
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
)
FetchContent_MakeAvailable(imgui_filebrowser_fetch)

add_library(imgui_filebrowser INTERFACE)
target_include_directories(imgui_filebrowser INTERFACE ${imgui_filebrowser_fetch_SOURCE_DIR})