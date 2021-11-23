#include "IndexBuffer.hpp"
#include "Renderer.hpp"
#include "scope_guard.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include "VertexBuffer.hpp"
#include <glm/glm.hpp>

int
  main(void)
{
  using namespace std::string_view_literals;
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
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
  const auto terminate = scope_guard(&glfwTerminate);
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
    -0.5F, -0.5F, 0.F, 0.F, // 0
     0.5F, -0.5F, 1.F, 0.F, // 1
     0.5F,  0.5F, 1.F, 1.F, // 2
    -0.5F,  0.5F, 0.F, 1.F, // 3
    // clang-format on
  } };
  const auto ib = IndexBuffer{ std::array{ 0U, 1U, 2U, 2U, 3U, 0U } };
  VertexBufferLayout layout{};
  layout.push_back<float>(2U);
  layout.push_back<float>(2U);
  va.push_back(vb, layout);

  const auto s = Shader{ std::filesystem::current_path() / "res" / "shader"
                         / "basic.shader" };

  s.Bind();
  s.SetUniform("u_Color", 0.8F, 0.3F, 0.8F, 1.0F);

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
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    renderer.Clear();

    s.Bind();
    s.SetUniform("u_Color", r, 0.3F, 0.8F, 1.0F);
    t.Bind(0);
    s.SetUniform("u_Texture", 0);

    renderer.Draw(s, va, ib);

    if (r > 1.F)
      increment = -0.05F;
    if (r < 0.F)
      increment = +0.05F;

    r += increment;

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  return 0;
}