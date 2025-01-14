include(FetchContent)
#options for FetchContent are at https://cmake.org/cmake/help/latest/module/ExternalProject.html
FetchContent_Declare(
        openviii_cpp_wip_fetch
        GIT_REPOSITORY https://github.com/Sebanisu/OpenVIII_CPP_WIP.git
        GIT_TAG origin/main
        GIT_REMOTE_UPDATE_STRATEGY CHECKOUT
)
FetchContent_MakeAvailable(openviii_cpp_wip_fetch)