//#include <cmake_pch.hxx>
#include "Application.hpp"
int main(void)
{
  //todo attach this scope gaurd to the api somewhere
  // anything created in the window must be destroyed before the window.
  const glengine::scope_guard_expensive end = { []() {
    ImGui::DestroyContext(nullptr);
    glfwTerminate();
  } };
  Application("OpenGL Test Application", 1280, 720).Run();
}