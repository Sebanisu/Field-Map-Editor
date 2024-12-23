from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy

import os

class MyPackage(ConanFile):
    name = "my_package"
    version = "1.0"
    
    default_options  = {"fmt/*:shared": True}
    
    settings = "os", "compiler", "build_type", "arch"

    requires = "spdlog/1.11.0", "glfw/3.3.5","glew/2.2.0", "glm/0.9.9.8","imgui/cci.20230105+1.89.2.docking", "sfml/2.6.2","zlib/1.3.1", "openal-soft/1.22.2", "boost-ext-ut/1.1.9", "tomlplusplus/3.0.1", "libpng/1.6.44", "stb/cci.20230920"
    
    def generate(self):
        #tc = CMakeToolchain(self, generator="Ninja")
        tc = CMakeToolchain(self)
        imgui = self.dependencies["imgui"].cpp_info
        tc.variables["IMGUI_IMPL_DIR"] = os.path.normpath(imgui.srcdirs[0]).replace("\\", "/")        
        tc.variables["SFML_WITH_WINDOW"] = self.dependencies["sfml"].options.window
        tc.variables["SFML_WITH_GRAPHICS"] = self.dependencies["sfml"].options.graphics
        tc.generate()
        cmd = CMakeDeps(self)
        cmd.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        
        
    def layout(self):
        cmake_layout(self)
