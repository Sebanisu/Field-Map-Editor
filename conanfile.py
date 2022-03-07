from conans import ConanFile


class FieldMapEditor(ConanFile):
    # Note: options are copied from CMake boolean options.
    # When turned off, CMake sometimes passes them as empty strings.
    name = "Field-Map-Editor"
    version = "0.1"
    requires = (
        # "catch2/2.13.7",
        # "docopt.cpp/0.6.3",
        "fmt/8.1.1",  # formatting library
        "spdlog/1.9.2",  # logging library
        "glfw/3.3.5",  # opengl version
        "glew/2.2.0",  # opengl version
        "glm/0.9.9.8",  # opengl version
        "imgui/1.87",  # UI used for both opengl and sfml
        "sfml/2.5.1",  # sfml version
        "andreasbuhr-cppcoro/cci.20210113",  # sfml version
        "stb/cci.20210713",  # image loading and saving
        "boost-ext-ut/1.1.8"  # unit testing library
    )
    generators = "cmake", "gcc", "txt", "cmake_find_package"
# required for linux? sudo apt-get install -y pkg-config
# required for WSL? error when ever conan tried to run sudo.
# sudo -A apt-get update
# sudo -A apt-get install -y --no-install-recommends libx11-xcb-dev libxcb-dri3-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-render0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xkb-dev xorg-dev libudev-dev
