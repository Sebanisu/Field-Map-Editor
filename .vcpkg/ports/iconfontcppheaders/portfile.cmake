vcpkg_from_github(
  OUT_SOURCE_PATH SOURCE_PATH
  REPO juliettef/IconFontCppHeaders
  REF b087fe1439a31af510cdcdab16b765741d7b4f47
  SHA512 7e41862d65392a734adfafb1cf418d395e2887bc153009b2328a2962a58657fb528dc81e3015204c678719b40966babed531265474e74e783e267d707b9c1b50
  HEAD_REF main
)

# Install all headers from the repo root into include/
file(GLOB HEADER_FILES
  "${SOURCE_PATH}/*.h"
  "${SOURCE_PATH}/*.hpp"
)

file(INSTALL
  DESTINATION ${CURRENT_PACKAGES_DIR}/include
  TYPE FILE
  FILES ${HEADER_FILES}
)

# Install license file
file(INSTALL ${SOURCE_PATH}/licence.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/${PORT} RENAME copyright)

# Create a minimal CMake package config
set(config_file ${CURRENT_PACKAGES_DIR}/share/${PORT}/${PORT}Config.cmake)
file(WRITE ${config_file} "add_library(${PORT} INTERFACE)\n")
file(APPEND ${config_file} "target_include_directories(${PORT} INTERFACE \"\${CMAKE_CURRENT_LIST_DIR}/../../include\")\n")
