include(FetchContent)
#options for FetchContent are at https://cmake.org/cmake/help/latest/module/ExternalProject.html
FetchContent_Declare(
        openviii_cpp_wip_fetch
        GIT_REPOSITORY https://github.com/Sebanisu/OpenVIII_CPP_WIP.git
        GIT_TAG origin/main
        GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
)
#FetchContent_MakeAvailable(openviii_cpp_wip_fetch)
FetchContent_GetProperties(openviii_cpp_wip_fetch)
if (NOT openviii_cpp_wip_fetch_POPULATED)
    FetchContent_Populate(openviii_cpp_wip_fetch)
    add_subdirectory(${openviii_cpp_wip_fetch_SOURCE_DIR} ${openviii_cpp_wip_fetch_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()