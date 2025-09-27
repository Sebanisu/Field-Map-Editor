# include(FetchContent)
# #options for FetchContent are at https://cmake.org/cmake/help/latest/module/ExternalProject.html
# FetchContent_Declare(
#         icon_font_cpp_headers_fetch
#         GIT_REPOSITORY https://github.com/juliettef/IconFontCppHeaders.git
#         GIT_TAG origin/main
#         GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
# )
# #FetchContent_MakeAvailable(icon_font_cpp_headers_fetch)
# FetchContent_GetProperties(icon_font_cpp_headers_fetch)
# if (NOT icon_font_cpp_headers_fetch_POPULATED)
#     FetchContent_Populate(icon_font_cpp_headers_fetch)
#     add_library(IconFontCppHeaders INTERFACE ${icon_font_cpp_headers_fetch_SOURCE_DIR}/IconsFontAwesome6.h)
#     target_include_directories(IconFontCppHeaders INTERFACE ${icon_font_cpp_headers_fetch_SOURCE_DIR})
#     message("IconFontCppHeaders fetch directory: ${icon_font_cpp_headers_fetch_SOURCE_DIR}")
# endif ()

if (NOT EXISTS "${CMAKE_BINARY_DIR}/fonts/fa-solid-900.ttf")
    message(STATUS "Downloading Font Awesome solid font (fa-solid-900.ttf)...")
    file(
        DOWNLOAD "https://github.com/FortAwesome/Font-Awesome/raw/refs/heads/6.x/webfonts/fa-solid-900.ttf"
        "${CMAKE_BINARY_DIR}/fonts/fa-solid-900.ttf"
        EXPECTED_HASH SHA256=af19d135d3a935b3ebfbd80320716ffe1202052c5f68dc2c5f1abc57005ac605
        TLS_VERIFY ON
    )
endif()