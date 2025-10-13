include(FetchContent)

# Require ImGui (for example, via Conan or other find_package methods)
find_package(imgui REQUIRED)

# Set the base directory for fetch content (adjust if needed)
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps")

# Fetch ImGuizmo source repository
FetchContent_Populate(
    imguizmo_fetch
    QUIET
    GIT_REPOSITORY https://github.com/CedricGuillemet/ImGuizmo.git
    GIT_TAG 770ca52940fddc1ed8c9e48829e9525bfedd0ef4
    #GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
    #GIT_SHALLOW FALSE
    SOURCE_DIR "${CMAKE_BINARY_DIR}/_deps/imguizmo_src"
    BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/imguizmo_bin"
)

# Create the static library including all source files
add_library(ImGuizmo STATIC
    "${CMAKE_BINARY_DIR}/_deps/imguizmo_src/GraphEditor.cpp"
    "${CMAKE_BINARY_DIR}/_deps/imguizmo_src/ImCurveEdit.cpp"
    "${CMAKE_BINARY_DIR}/_deps/imguizmo_src/ImGradient.cpp"
    "${CMAKE_BINARY_DIR}/_deps/imguizmo_src/ImGuizmo.cpp"
    "${CMAKE_BINARY_DIR}/_deps/imguizmo_src/ImSequencer.cpp"
)

# Expose the include directory so that the headers (.h and .hpp) can be found.
target_include_directories(ImGuizmo
    PUBLIC "${CMAKE_BINARY_DIR}/_deps/imguizmo_src"
)

# Link with ImGui, which is a dependency for ImGuizmo
target_link_libraries(ImGuizmo
    PUBLIC imgui::imgui
)
