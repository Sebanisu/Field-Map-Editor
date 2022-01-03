find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)
message(${CONAN_SRC_DIRS_IMGUI})
file(GLOB_RECURSE IMGUI_BINDINGS
        ${CONAN_SRC_DIRS_IMGUI}/imgui_impl_glfw.*
        ${CONAN_SRC_DIRS_IMGUI}/imgui_impl_opengl3.*)
foreach (file_path ${IMGUI_BINDINGS})
    message(${file_path})
endforeach ()
# Add an executable
add_library(imgui_glfw STATIC ${IMGUI_BINDINGS})
target_link_libraries(imgui_glfw
        PUBLIC imgui::imgui
        PUBLIC glfw
        PUBLIC OpenGL::GL
        )

target_include_directories(imgui_glfw PUBLIC ${CONAN_SRC_DIRS_IMGUI})
add_subdirectory(stbi)
add_subdirectory(glengine)
add_subdirectory(episode_21)