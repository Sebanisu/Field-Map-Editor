find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui REQUIRED)
# Access the variable directly
message(STATUS "IMGUI_IMPL_DIR is: ${IMGUI_IMPL_DIR}")

file(GLOB_RECURSE IMGUI_BINDINGS ${IMGUI_IMPL_DIR}/imgui_impl_glfw.*
     ${IMGUI_IMPL_DIR}/imgui_impl_opengl3.*)
foreach(file_path ${IMGUI_BINDINGS})
  message(${file_path})
endforeach()
# Add an executable
add_library(imgui_glfw STATIC ${IMGUI_BINDINGS})
target_link_libraries(
  imgui_glfw
  PUBLIC imgui::imgui
  PUBLIC GLEW::GLEW
  PUBLIC glfw
  PUBLIC OpenGL::GL)

target_compile_features(
  imgui_glfw
  PUBLIC cxx_std_23
  PUBLIC cxx_std_20)

target_include_directories(imgui_glfw PUBLIC ${IMGUI_IMPL_DIR})
target_compile_definitions(
  imgui_glfw
  PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW
  PUBLIC GLEW_STATIC)
add_subdirectory(stbi)
add_subdirectory(glengine)
add_subdirectory(experimental)
add_subdirectory(main)
