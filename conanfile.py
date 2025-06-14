from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps, cmake_layout
from conan.tools.files import copy
import platform
import platform

import os

class FieldMapEditorConan(ConanFile):
    name = "Field-Map-Editor"
    version = "1.0"
    package_type = "application"
    
    default_options  = {"fmt/*:shared": True}
    
    settings = "os", "compiler", "build_type", "arch"
    test_type = "explicit"
    
    def requirements(self):
        self.requires("lz4/1.10.0")
        self.requires("spdlog/1.15.0")
        self.requires("glfw/3.4")
        self.requires("glew/2.2.0")
        self.requires("glm/0.9.9.8")
        self.requires("imgui/1.91.8-docking")
        self.requires("sfml/2.6.2")
        self.requires("zlib/1.3.1")
        self.requires("openal-soft/1.22.2")
        self.requires("boost-ext-ut/2.1.0") 
        self.requires("tomlplusplus/3.0.1")
        self.requires("libpng/1.6.44")
        self.requires("stb/cci.20230920")        
        self.requires("iconfontcppheaders/cci.20240620")
        self.requires("ctre/3.9.0")      
    
    def build_requirements(self):
        self.tool_requires("cmake/3.31.6")    
        if platform.system() != "Windows":
            self.tool_requires("ninja/1.12.1")                    

        
    def configure(self):
        self.options["fmt"].header_only = True
        self.options["boost-ext-ut"].disable_module = True
    
    def generate(self):
        tc = CMakeToolchain(self)
        if platform.system() == "Windows":
            tc.generator = "Visual Studio 17 2022"
        else:
            tc.generator = "Ninja"
        tc.presets_prefix = f"conan_{self.settings.os}".lower() + "_" + str(self.settings.build_type).lower()
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