//#include <cmake_pch.hxx>
#include "Application.hpp"
int main(void)
{
  // todo attach this scope gaurd to the api somewhere
  //  anything created in the window must be destroyed before the window.
  const auto end = glengine::scope_guard{ []() {
    ImGui::DestroyContext(nullptr);
    glfwTerminate();
  } };
  Application("Field-Map-Editor - OPENGL", 1280, 720).Run();
}