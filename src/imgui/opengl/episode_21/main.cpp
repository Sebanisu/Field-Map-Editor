//#include <cmake_pch.hxx>
#include "Application.hpp"
int main(void)
{
  // anything created in the window must be destroyed before the window.
  const scope_guard_expensive end = { []() {
    //    ImGui_ImplOpenGL3_Shutdown();
    //    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
  } };
  Application("OpenGL Test Application", 1280, 720).Run();
}