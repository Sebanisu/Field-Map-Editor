#include <array>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <sstream>
using namespace std::string_view_literals;

struct ShaderProgramSource
{
  std::string vertex_shader{};
  std::string fragment_shader{};
};
inline ShaderProgramSource
  ParseShader(const std::filesystem::path &file_path)
{
  std::ifstream fs(file_path, std::ios::binary | std::ios::in);
  if (!fs.is_open())
  {
    fmt::print(
      stderr,
      "Error {}:{} - Failed to open shader \n\t\"{}\"\n",
      __FILE__,
      __LINE__,
      file_path.string());
    return {};
  }
  enum class ShaderType
  {
    None     = -1,
    Vertex   = 0,
    Fragment = 1,
  };
  std::stringstream ss[2U] = {};
  {
    std::string line{};
    ShaderType  mode = ShaderType::None;
    while (std::getline(fs, line))
    {
      if (line.find("#shader") != std::string::npos)
      {
        if (line.find("vertex") != std::string::npos)
        {
          // set mode to vertex.
          mode = ShaderType::Vertex;
        }
        else if (line.find("fragment") != std::string::npos)
        {
          // set mode to fragment.
          mode = ShaderType::Fragment;
        }
      }
      else
      {
        if (std::string::size_type pos = line.find("#");
            pos != std::string::npos)
        {
          line.erase(0, pos);
        }
        ss[static_cast<std::size_t>(mode)] << line << '\n';
      }
    }
  }

  return { ss[0].str(), ss[1].str() };
}
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
      "Error {}:{} - Failed to compile shader {} - {}\n",
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

  std::array positions{ -0.5F, -0.5F, 0.5F, -0.5F, 0.5F, 0.5F, -0.5F, 0.5F };
  std::array indices{ 0U, 1U, 2U, 2U, 3U, 0U };

  {
    std::uint32_t buffer{};
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(
      GL_ARRAY_BUFFER,
      std::size(positions) * sizeof(float),
      std::data(positions),
      GL_STATIC_DRAW);
  }

  {
    std::uint32_t ibo{};
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER,
      std::size(indices) * sizeof(std::uint32_t),
      std::data(indices),
      GL_STATIC_DRAW);
  }

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

  ShaderProgramSource source = ParseShader(
    std::filesystem::current_path() / "res" / "shader" / "basic.shader");
  std::uint32_t shader =
    CreateShader(source.vertex_shader, source.fragment_shader);
  glUseProgram(shader);

  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Draw bound vertices */
    glDrawElements(
      GL_TRIANGLES,
      static_cast<GLsizei>(std::size(indices)),
      GL_UNSIGNED_INT,
      nullptr);

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
  }

  glDeleteProgram(shader);
  glfwTerminate();
  return 0;
}