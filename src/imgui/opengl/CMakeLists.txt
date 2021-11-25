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
#add_subdirectory(episode_1)
#add_subdirectory(episode_2)
#add_subdirectory(episode_3)
#add_subdirectory(episode_4)
#add_subdirectory(episode_5)
#add_subdirectory(episode_6)
#add_subdirectory(episode_7)
#add_subdirectory(episode_8)
#add_subdirectory(episode_9)
#add_subdirectory(episode_10)
#add_subdirectory(episode_11)
#add_subdirectory(episode_12)
#add_subdirectory(episode_13)
#add_subdirectory(episode_14)
#add_subdirectory(episode_15)
#add_subdirectory(episode_17)
#add_subdirectory(episode_18)
#add_subdirectory(episode_19)
add_subdirectory(episode_20)