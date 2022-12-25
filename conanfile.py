from conans import ConanFile


class FieldMapEditor(ConanFile):
    # Note: options are copied from CMake boolean options.
    # When turned off, CMake sometimes passes them as empty strings.
    name = "Field-Map-Editor"
    version = "0.1"
    requires = (
        # "catch2/2.13.7", another unit testing library.
        # "docopt.cpp/0.6.3", command line library.
        #"fmt/8.1.1",  # formatting library, included with spdlog
        "spdlog/1.10.0",  # logging library
        "glfw/3.3.5",  # opengl version opengl library.
        "glew/2.2.0",  # opengl version opengl extension library
        "glm/0.9.9.8",  # opengl version opengl math library
        "imgui/cci.20220207+1.87.docking", #docking UI for opengl
        #"imgui/1.87",  # UI used for both opengl and sfml
        "sfml/2.5.1",  # sfml version simplified c++98 gaming library
        #"libunwind/1.6.2", # umm test program is looking for this?
        "zlib/1.2.13", # resolve conflict by using it here.
        "andreasbuhr-cppcoro/cci.20210113",  # sfml version uses coroutines. Thought this would help with async.
        "stb/cci.20210713",  # image loading and saving
        "boost-ext-ut/1.1.9",  # unit testing library / wip
        #"tomlplusplus/3.1.0"  #toml++  c++17 configuration library
        "tomlplusplus/3.0.1"
    )
    generators = "cmake", "gcc", "txt", "cmake_find_package"
    def configure(self):
        self.options['boost-ext-ut'].disable_module = True
# required for linux? sudo apt-get install -y pkg-config
# required for WSL? error when ever conan tried to run sudo.
# sudo -A apt-get update
# sudo -A apt-get install -y --no-install-recommends libx11-xcb-dev libxcb-dri3-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev libxcb-render0-dev libxcb-shape0-dev libxcb-sync-dev libxcb-util-dev libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xkb-dev xorg-dev libudev-dev
