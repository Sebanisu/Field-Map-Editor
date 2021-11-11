from conans import ConanFile


class CppStarterProject(ConanFile):
    # Note: options are copied from CMake boolean options.
    # When turned off, CMake sometimes passes them as empty strings.
    options = {
        "cpp_starter_use_imgui": ["ON", "OFF", ""],
        "cpp_starter_use_sdl": ["ON", "OFF", ""]
    }
    name = "CppStarterProject"
    version = "0.1"
    requires = (
        "catch2/2.13.7",
        "docopt.cpp/0.6.3",
        "fmt/8.0.1",
        "spdlog/1.9.2",
    )
    generators = "cmake", "gcc", "txt", "cmake_find_package"

    def requirements(self):
        if self.options.cpp_starter_use_imgui == "ON":
            #self.requires("imgui-sfml/2.1@bincrafters/stable")
            self.requires("imgui/1.85")
            self.requires("sfml/2.5.1")
        if self.options.cpp_starter_use_sdl == "ON":
            self.requires("sdl/2.0.16")
# required for linux? sudo apt-get install -y pkg-config
# required for WSL? error when ever conan tried to run sudo.
# sudo -A apt-get update
# sudo -A apt-get install -y --no-install-recommends libx11-xcb-dev libxcb-dri3-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-render0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xkb-dev xorg-dev libudev-dev
