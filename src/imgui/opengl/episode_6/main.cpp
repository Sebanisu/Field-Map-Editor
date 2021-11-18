#include <array>
#include <fmt/format.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
using namespace std::string_view_literals;
inline std::uint32_t
  CompileShader(const std::uint32_t type, const std::string_view source)
{
  const std::uint32_t id  = glCreateShader(type);
  const char         *src = std::data(source);
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);

  int result{};
  glGetShaderiv(id, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE)
  {
    int length{};
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::string message(length, '\0');
    glGetShaderInfoLog(id, length, &length, std::data(message));
    fmt::print(
      stderr,
      "Error {}:{} - Failed to compile sharder {} - {}\n",
      __FILE__,
      __LINE__,
      (type == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER"sv
                                : "GL_FRAGMENT_SHADER"sv),
      message);
    glDeleteShader(id);
    return 0U;
  }

  return id;
}
inline std::uint32_t
  CreateShader(
    const std::string_view vertexShader,
    const std::string_view fragmentShader)
{
  const std::uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
  const std::uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
  const std::uint32_t program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glValidateProgram(program);
  glDeleteShader(vs);
  glDeleteShader(fs);
  return program;
}

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
      "Error! {}:{} GLFW Failed to create Window\n",
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

  std::array    positions{ -0.5F, -.5F, 0.F, 0.5F, 0.5F, -0.5F };

  std::uint32_t buffer{};
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(
    GL_ARRAY_BUFFER,
    std::size(positions) * sizeof(float),
    std::data(positions),
    GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

  std::uint32_t shader = CreateShader(
    R"(#version 330 core

layout(location = 0) in vec4 position;

void main()
{
  gl_Position = position;
}
)"sv,
    R"(#version 330 core

layout(location = 0) out vec4 color;

void main()
{
  color = vec4(1.0, 0.0, 0.0, 1.0);
}
)"sv);
  glUseProgram(shader);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw bound vertices */
    glDrawArrays(
      GL_TRIANGLES, 0, static_cast<GLsizei>(std::size(positions) / 2U));

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}