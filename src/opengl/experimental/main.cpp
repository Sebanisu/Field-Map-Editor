// #include <cmake_pch.hxx>
#include "Application.hpp"
int main()
{
     // todo attach this scope gaurd to the api somewhere
     //  anything created in the window must be destroyed before the window.
     spdlog::set_level(spdlog::level::debug);// Set global log level to debug
     const auto end = glengine::ScopeGuard{ []()
                                            {
                                                 ImGui::DestroyContext(nullptr);
                                                 glfwTerminate();
                                            } };
     Application("Field-Map-Editor - EXPERIMENTAL", 1280, 720).run();
}