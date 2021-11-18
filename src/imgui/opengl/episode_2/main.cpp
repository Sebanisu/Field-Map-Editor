#include <fmt/format.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int
  main(void)
{
  GLFWwindow *window;

  /* Initialize the library */
  if (!glfwInit())
    return -1;

  /* Create a windowed mode window and its OpenGL context */
  window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
  if (!window)
  {
    fmt::print(
      stderr,
      "Error! {}:{} GLFW Window Failed to create\n",
      __FILE__,
      __LINE__);
    glfwTerminate();
    return -1;
  }

  /* Make the window's context current */
  glfwMakeContextCurrent(window);

  /* Init GLEW after context */
  if (glewInit() != GLEW_OK)
  {
    fmt::print(stderr, "Error! {}:{} GLEW NOT OKAY\n", __FILE__, __LINE__);
  }

  fmt::print("{}\n", glGetString(GL_VERSION));

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glVertex2f(-0.5F, -.5F);
    glVertex2f(0.F, 0.5F);
    glVertex2f(0.5F, -0.5F);
    glEnd();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}