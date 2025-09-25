include(FetchContent)
# Declare the content without using FetchContent_Declare
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps") # Optional: Customize where sources are stored
FetchContent_Populate(
    imgui_filebrowser_fetch
    QUIET
    #GIT_REPOSITORY https://github.com/AirGuanZ/imgui-filebrowser.git
    GIT_REPOSITORY https://github.com/Sebanisu/imgui-filebrowser.git
    GIT_TAG origin/master
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
    SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/imgui_filebrowser_src"
    BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/imgui_filebrowser_bin"
)
add_library(imgui_filebrowser INTERFACE)
target_include_directories(imgui_filebrowser INTERFACE "${CMAKE_BINARY_DIR}/_deps/imgui_filebrowser_src")