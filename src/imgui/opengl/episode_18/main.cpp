#include "IndexBuffer.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include "Shader.hpp"
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
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  const char *glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  int window_width  = 1280;
  int window_height = 720;
  window =
    glfwCreateWindow(window_width, window_height, "Hello World", NULL, NULL);
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
  glfwMakeContextCurrent(window);

  // This function runs on exit.
  const auto terminate = scope_guard_expensive(
    [window]()
    {
      // Cleanup
      ImGui_ImplOpenGL3_Shutdown();
      ImGui_ImplGlfw_Shutdown();
      ImGui::DestroyContext();

      glfwDestroyWindow(window);
      glfwTerminate();
    });

  glfwSwapInterval(1);

  /* Init GLEW after context */
  if (glewInit() != GLEW_OK)
  {
    fmt::print(stderr, "Error! {}:{} GLEW NOT OKAY\n", __FILE__, __LINE__);
  }

  fmt::print("{}\n", GLCall{ glGetString, GL_VERSION }());

  GLCall{ glEnable, GL_BLEND };
  GLCall{ glBlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA };

  auto               va = VertexArray{};
  const auto         vb = VertexBuffer{ std::array{
    // clang-format off
   -50.0F,-50.0F, 0.F, 0.F, // 0
    50.0F,-50.0F, 1.F, 0.F, // 1
    50.0F, 50.0F, 1.F, 1.F, // 2
   -50.0F, 50.0F, 0.F, 1.F, // 3
    // clang-format on
  } };
  const auto ib = IndexBuffer{ std::array{ 0U, 1U, 2U, 2U, 3U, 0U } };
  VertexBufferLayout layout{};
  layout.push_back<float>(2U);
  layout.push_back<float>(2U);
  va.push_back(vb, layout);

  auto proj = glm::ortho(
    0.F,
    static_cast<float>(window_width),
    0.F,
    static_cast<float>(window_height),
    -1.F,
    1.F);

  auto       view_offset   = glm::vec3(0.F, 0.F, 0.F);
  auto       view          = glm::translate(glm::mat4{ 1.F }, view_offset);
  auto       model_offset  = glm::vec3(200.F, 200.F, 0.F);
  auto       model2_offset = glm::vec3(400.F, 200.F, 0.F);
  auto       model         = glm::translate(glm::mat4{ 1.F }, model_offset);
  auto       mvp           = proj * view * model;

  const auto s = Shader{ std::filesystem::current_path() / "res" / "shader"
                         / "basic.shader" };
  s.Bind();
  s.SetUniform("u_Color", 0.8F, 0.3F, 0.8F, 1.0F);
  s.SetUniform("u_MVP", mvp);

  const auto t =
    Texture(std::filesystem::current_path() / "res" / "textures" / "logo.png");
  t.Bind(0);
  s.SetUniform("u_Texture", 0);

  // Unbind
  va.UnBind();
  s.UnBind();
  vb.UnBind();
  ib.UnBind();
  t.UnBind();

  float    r         = 0.F;
  float    increment = 0.05F;
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
  ImGui_ImplGlfw_InitForOpenGL(window, true);
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

  while (!glfwWindowShouldClose(window))
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
      int        id   = 0;
      const auto pop  = scope_guard(&ImGui::PopID);
      const auto pop2 = pop;
      const auto pop3 = pop;
      ImGui::PushID(++id);
      if (ImGui::SliderFloat3(
            "View Offset",
            &view_offset.x,
            0.F,
            static_cast<float>(window_width)))
      {
      }
      ImGui::PushID(++id);
      if (ImGui::SliderFloat3(
            "Model 1 Offset",
            &model_offset.x,
            0.F,
            static_cast<float>(window_width)))
      {
      }
      ImGui::PushID(++id);
      if (ImGui::SliderFloat3(
            "Model 2 Offset",
            &model2_offset.x,
            0.F,
            static_cast<float>(window_width)))
      {
      }
    }
    ImGui::End();


    ImGui::Render();
    // update window size varibles
    glfwGetFramebufferSize(window, &window_width, &window_height);
    // glViewport(0, 0, display_w, display_h);
    view = glm::translate(glm::mat4{ 1.F }, view_offset);
    /* Render here */
    renderer.Clear();

    s.Bind();
    s.SetUniform("u_Color", r, 0.3F, 0.8F, 1.0F);
    t.Bind(0);
    s.SetUniform("u_Texture", 0);
    s.SetUniform("u_MVP", mvp);

    renderer.Draw(s, va, ib);
    model = glm::translate(glm::mat4{ 1.F }, model_offset);
    mvp   = proj * view * model;
    s.SetUniform("u_MVP", mvp);
    renderer.Draw(s, va, ib);
    model = glm::translate(glm::mat4{ 1.F }, model2_offset);
    mvp   = proj * view * model;
    s.SetUniform("u_MVP", mvp);

    if (r > 1.F)
      increment = -0.05F;
    if (r < 0.F)
      increment = +0.05F;

    r += increment;

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    /* Swap front and back buffers */
    glfwSwapBuffers(window);
  }

  return 0;
}