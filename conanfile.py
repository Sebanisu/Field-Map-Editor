from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy

import os

class MyPackage(ConanFile):
    name = "my_package"
    version = "1.0"
    
    default_options  = {"fmt/*:shared": True}
    
    settings = "os", "compiler", "build_type", "arch"
    
    def requirements(self):
        self.requires("spdlog/1.11.0")
        self.requires("glfw/3.3.5")
        self.requires("glew/2.2.0")
        self.requires("glm/0.9.9.8")
        self.requires("imgui/cci.20230105+1.89.2.docking")
        self.requires("sfml/2.6.2")
        self.requires("zlib/1.3.1")
        self.requires("openal-soft/1.22.2")
        self.requires("boost-ext-ut/1.1.9")
        self.requires("tomlplusplus/3.0.1")
        self.requires("libpng/1.6.44")
        self.requires("stb/cci.20230920")

    
    def build_requirements(self):
        self.tool_requires("cmake/[>=3.22.6]")        
        # if self.settings.os != "Windows":
        #     self.tool_requires("pkg-config/[>=0.29.2]")

        
    def configure(self):
        self.options["fmt"].header_only = True
    
    def generate(self):
        #tc = CMakeToolchain(self, generator="Ninja")
        tc = CMakeToolchain(self)
        tc.presets_prefix = f"conan_{self.settings.os}".lower()
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
        
        
