// #include <cmake_pch.hxx>
#ifndef DEFAULT_LOG_PATH
#define DEFAULT_LOG_PATH "res/field-map-editor.log"// fallback, just in case
#endif
#include "Application.hpp"
#include <spdlog/sinks/basic_file_sink.h>

#ifdef _WIN32
#include <windows.h>

int main(
  int    argc,
  char **argv);// your existing main

int WINAPI WinMain(
  [[maybe_unused]] HINSTANCE hInstance,
  [[maybe_unused]] HINSTANCE hPrevInstance,
  [[maybe_unused]] LPSTR     lpCmdLine,
  [[maybe_unused]] int       nCmdShow)
{
     // Convert lpCmdLine to argc/argv if needed, or just call main(0,nullptr)
     return main(__argc, __argv);
}
#endif


int main(
  [[maybe_unused]] int    argc,
  [[maybe_unused]] char **argv)
{
     // todo attach this scope gaurd to the api somewhere
     //  anything created in the window must be destroyed before the window.
     std::error_code error_code = {};

     auto            temp_path  = std::filesystem::path(DEFAULT_LOG_PATH);
     std::string     path;
     try
     {
          if (temp_path.is_relative())
          {
               temp_path = (std::filesystem::current_path(error_code) / path);
          }
          temp_path.make_preferred();
          spdlog::info("log path: {}", temp_path);
          path = temp_path.string();
     }
     catch (const std::filesystem::filesystem_error &e)
     {
          spdlog::error(
            "Filesystem error while constructing config path: {}", e.what());
     }
     catch (const std::exception &e)
     {
          spdlog::error(
            "Unexpected error while constructing config path: {}", e.what());
     }
     if (error_code)
     {
          spdlog::warn(
            "{}:{} - {}: {} path: \"{}\"",
            __FILE__,
            __LINE__,
            error_code.value(),
            error_code.message(),
            path);
          error_code.clear();
     }

     try
     {
          // Create file logger and set as default
          auto file_logger = spdlog::basic_logger_mt("file_logger", path, true);

          // Remove logger name from output pattern
          file_logger->set_pattern(R"([%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v)");

          spdlog::set_default_logger(file_logger);

          // Set log level based on build type
          // #ifndef NDEBUG
          spdlog::set_level(spdlog::level::debug);// Debug build
          // #else
          //           spdlog::set_level(spdlog::level::info);// Release build
          // #endif

          // Optional: control flush policy
          spdlog::flush_on(spdlog::level::info);

          // Now log anywhere
          spdlog::info("App started");
     }
     catch (const spdlog::spdlog_ex &ex)
     {
          std::cerr << "Log init failed: " << ex.what() << std::endl;
     }
     const auto end = glengine::ScopeGuard{ []()
                                            {
                                                 ImGui::DestroyContext(nullptr);
                                                 glfwTerminate();
                                            } };
     Application("Field-Map-Editor - EXPERIMENTAL", 1280, 720).run();
}