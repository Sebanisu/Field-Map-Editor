# imgui test executable, with full warnings enabled
add_executable(test_imgui test.cpp)

find_package(ImGui-SFML REQUIRED)
find_package(fmt)
target_link_libraries(test_imgui PRIVATE project_warnings project_options ImGui-SFML::ImGui-SFML fmt::fmt)
