#include "IndexBuffer.hpp"
#include "Renderer.hpp"
#include "VertexArray.hpp"
#include "scope_guard.hpp"
#include "VertexBuffer.hpp"
#include <cassert>
#include <filesystem>
#include <fmt/format.h>
#include <fstream>
//#include <array>
//#include <concepts>
//#include <functional>
//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <source_location>
//#include <sstream>
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
  const std::uint32_t id  = GLCall{ glCreateShader, type }();
  const char         *src = std::data(source);
  GLCall{ glShaderSource, id, 1, &src, nullptr };
  GLCall{ glCompileShader, id };

  int result{};
  GLCall{ glGetShaderiv, id, GL_COMPILE_STATUS, &result };
  if (result == GL_FALSE)
  {
    int length{};
    GLCall{ glGetShaderiv, id, GL_INFO_LOG_LENGTH, &length };
    std::string message(static_cast<std::string::size_type>(length), '\0');
    GLCall{ glGetShaderInfoLog, id, length, &length, std::data(message) };
    fmt::print(
      stderr,
      "Error {}:{} - Failed to compile shader {} - {}\n",
      __FILE__,
      __LINE__,
      (type == GL_VERTEX_SHADER ? "GL_VERTEX_SHADER"sv
                                : "GL_FRAGMENT_SHADER"sv),
      message);
    GLCall{ glDeleteShader, id };
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
  const std::uint32_t program = GLCall{ glCreateProgram }();
  GLCall{ glAttachShader, program, vs };
  GLCall{ glAttachShader, program, fs };
  GLCall{ glLinkProgram, program };
  GLCall{ glValidateProgram, program };
  GLCall{ glDeleteShader, vs };
  GLCall{ glDeleteShader, fs };
  return program;
}

int
  main(void)
{
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

  std::array positions{ -0.5F, -0.5F, 0.5F, -0.5F, 0.5F, 0.5F, -0.5F, 0.5F };
  std::array indices{ 0U, 1U, 2U, 2U, 3U, 0U };

  auto va = VertexArray{};
  const auto vb = VertexBuffer{ positions };
  const auto ib = IndexBuffer{ indices };
  VertexBufferLayout layout{};
  layout.push_back<float>(2U);
  va.push_back(vb,layout);

  // end VertexArray

  ShaderProgramSource source = ParseShader(
    std::filesystem::current_path() / "res" / "shader" / "basic.shader");

  std::uint32_t shader =
    CreateShader(source.vertex_shader, source.fragment_shader);

  GLCall{ glUseProgram, shader };


  int location = GLCall{ glGetUniformLocation, shader, "u_Color" }();
  assert(location != -1);
  GLCall{ glUniform4f, location, 0.8F, 0.3F, 0.8F, 1.0F };


  // Unbind
  GLCall{ glBindVertexArray, 0 };
  GLCall{ glUseProgram, 0 };
  GLCall{ glBindBuffer, GL_ARRAY_BUFFER, 0 };
  GLCall{ glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0 };

  float r         = 0.F;
  float increment = 0.05F;
  /* Loop until the user closes the window */
  while (!glfwWindowShouldClose(window))
  {
    /* Render here */
    GLCall{ glClear, GL_COLOR_BUFFER_BIT };
    GLCall{ glUseProgram, shader };
    GLCall{ glUniform4f, location, r, 0.3F, 0.8F, 1.0F };
    va.Bind();
    ib.Bind();

    /* Draw bound vertices */
    GLCall{ glDrawElements,
            GL_TRIANGLES,
            static_cast<std::int32_t>(std::size(indices)),
            GL_UNSIGNED_INT,
            nullptr };
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

  GLCall{ glDeleteProgram, shader };
  // GLCall{ glDeleteVertexArrays, 1, &vao };

  return 0;
}