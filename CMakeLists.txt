cmake_minimum_required(VERSION 3.25)

cmake_policy(SET CMP0010 NEW)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0074 NEW)
cmake_policy(SET CMP0091 NEW)

set(VCPKG_DISABLE_COMPILER_TRACKING 1)
set(VCPKG_INSTALL_OPTIONS "--clean-after-build")
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)

set(CMAKE_SHARED_LINKER_FLAGS
  "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRT /NODEFAULTLIB:MSVCRTD /DEBUG:FULL /FORCE:MULTIPLE /IGNORE:4006,4075,4099,4217"
)

option(CI_BUILD "Set to ON when building on CI" OFF)
if(NOT CI_BUILD AND DEFINED ENV{CI_BUILD} AND "$ENV{CI_BUILD}" STREQUAL "ON")
    set(CI_BUILD ON)
endif()
# Construct path depending on CI mode
if (CI_BUILD)
  set(DEFAULT_CONFIG_ROOT_PATH "res")
else()
  # Use full path when developing locally
  set(DEFAULT_CONFIG_ROOT_PATH "${CMAKE_SOURCE_DIR}/res")
endif()

if(CMAKE_BUILD_TYPE MATCHES Debug)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDebug")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
else()
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMTD")
endif()

project(
  Field-Map-Editor
  VERSION 1.0.4
  LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD "23")

# Set the project name to your project name, my project isn't very descriptive
set(MY_PROJECT_USER "sebanisu")
set(MY_PROJECT_CHANNEL "development")
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  option(ENABLE_BUILD_WITH_TIME_TRACE
         "Enable -ftime-trace to generate time tracing .json files on clang"
         OFF)
  if(ENABLE_BUILD_WITH_TIME_TRACE)
    target_compile_options(project_options INTERFACE -ftime-trace)
  endif()
endif()

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# Add linker configuration
include(cmake/Linker.cmake)
configure_linker(project_options)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)
add_subdirectory(src)

if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME))
  include(CTest)
  add_subdirectory(test)
endif()
