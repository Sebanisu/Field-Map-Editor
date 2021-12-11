find_package(stb REQUIRED)
add_library(stbi_wrapper STATIC
        stbi.cpp)
target_link_libraries(stbi_wrapper
        PUBLIC stb::stb
        )
target_compile_definitions(stbi_wrapper
        PRIVATE STB_IMAGE_IMPLEMENTATION)