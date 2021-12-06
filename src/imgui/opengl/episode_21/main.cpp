#include "IndexBuffer.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include "Shader.hpp"
#include "tests/TestMenu.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <numeric>

int
  main(void)
{
  using namespace std::string_view_literals;
  const auto end_program_function = [](GLFWwindow *window)
  {
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
  };


  /* Initialize the library */
  if (!glfwInit())
    return -1;

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int                         window_width  = 1280;
  int                         window_height = 720;
  std::shared_ptr<GLFWwindow> window(
    glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL),
    end_program_function);

  if (!window)
  {
    fmt::print(
      stderr,
      "Error! {}:{} GLFW Failed to create Window\n",
      __FILE__,
      __LINE__);
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window.get());

  glfwSwapInterval(1);

  /* Init GLEW after context */
  if (glewInit() != GLEW_OK)
  {
    fmt::print(stderr, "Error! {}:{} GLEW NOT OKAY\n", __FILE__, __LINE__);
  }

  /* Update Viewport when glfw detects window resize */
  glfwSetFramebufferSizeCallback(
    window.get(),
    [](GLFWwindow *, int width, int height) {
      GLCall{ glViewport, 0, 0, width, height };
    });

  fmt::print("{}\n", GLCall{ glGetString, GL_VERSION }());

  GLCall{ glEnable, GL_BLEND };
  GLCall{ glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };


  /* Loop until the user closes the window */
  Renderer renderer{};

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable
  // Keyboard Controls io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //
  // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsClassic();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window.get(), true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load Fonts
  // - If no fonts are loaded, dear imgui will use the default font. You can
  // also load multiple fonts and use ImGui::PushFont()/PopFont() to select
  // them.
  // - AddFontFromFileTTF() will return the ImFont* so you can store it if you
  // need to select the font among multiple.
  // - If the file cannot be loaded, the function will return NULL. Please
  // handle those errors in your application (e.g. use an assertion, or display
  // an error and quit).
  // - The fonts will be rasterized at a given size (w/ oversampling) and stored
  // into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which
  // ImGui_ImplXXXX_NewFrame below will call.
  // - Read 'docs/FONTS.md' for more instructions and details.
  // - Remember that in C/C++ if you want to include a backslash \ in a string
  // literal you need to write a double backslash \\ !
  // io.Fonts->AddFontDefault();
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
  // io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
  // ImFont* font =
  // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f,
  // NULL, io.Fonts->GetGlyphRangesJapanese()); IM_ASSERT(font != NULL);

  const auto test_menu = test::TestMenu{};
  while (!glfwWindowShouldClose(window.get()))
  {
    /* Poll for and process events */
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // place imgui here draws here.
    if (ImGui::Begin("Test Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
      test::OnImGuiRender(test_menu);
      ImGui::Text(
        "%s",
        fmt::format(
          "Application average {:.3f} ms/frame ({:.3f} FPS)",
          1000.0f / ImGui::GetIO().Framerate,
          ImGui::GetIO().Framerate)
          .c_str());
    }
    ImGui::End();
    ImGui::Render();
    renderer.Clear();
    // update window size variables
    glfwGetFramebufferSize(window.get(), &window_width, &window_height);
    // glViewport(0, 0, display_w, display_h);
    /* Render here */
    // renderer.Clear();
    test::OnUpdate(test_menu, float{});
    test::OnRender(test_menu);


    BeginErrorCallBack();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    EndErrorCallback();
    GLClearError();

    /* Swap front and back buffers */
    glfwSwapBuffers(window.get());
  }
  return 0;
}