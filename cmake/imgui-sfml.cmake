include(FetchContent)

# Find Conan-managed ImGui
find_package(imgui REQUIRED)

# Find SFML with configurable components
set(SFML_COMPONENTS system)
set(SFML_TARGETS sfml-system)
if(SFML_WITH_WINDOW)
    list(APPEND SFML_COMPONENTS window)
    list(APPEND SFML_TARGETS sfml-window)
endif()
if(SFML_WITH_GRAPHICS)
    list(APPEND SFML_COMPONENTS graphics)
    list(APPEND SFML_TARGETS sfml-graphics)
endif()
if(SFML_WITH_NETWORK)
    list(APPEND SFML_COMPONENTS network)
    list(APPEND SFML_TARGETS sfml-network)
endif()
if(SFML_WITH_AUDIO)
    list(APPEND SFML_COMPONENTS audio)
    list(APPEND SFML_TARGETS sfml-audio)
endif()
find_package(SFML 2 REQUIRED ${SFML_COMPONENTS} CONFIG)

# Fetch ImGui-SFML directly
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps")
FetchContent_Populate(
    imgui_sfml_fetch
    QUIET
    GIT_REPOSITORY https://github.com/eliasdaler/imgui-sfml.git
    GIT_TAG 2.6.x
    GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
    SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/imgui_sfml_src"
    BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/imgui_sfml_bin"
)

# Create ImGui-SFML static library
add_library(ImGui-SFML STATIC
    "${CMAKE_BINARY_DIR}/_deps/imgui_sfml_src/imgui-SFML.cpp"
    "${CMAKE_BINARY_DIR}/_deps/imgui_sfml_src/imgui-SFML.h"
)
target_include_directories(ImGui-SFML
    PUBLIC "${CMAKE_BINARY_DIR}/_deps/imgui_sfml_src"
)
target_link_libraries(ImGui-SFML
    PUBLIC imgui::imgui
    PUBLIC ${SFML_TARGETS}
)