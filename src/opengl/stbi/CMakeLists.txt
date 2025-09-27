find_package(Stb REQUIRED)
add_library(stbi_wrapper STATIC
        stbi.cpp
        )
target_include_directories(stbi_wrapper PRIVATE ${Stb_INCLUDE_DIR})
target_compile_definitions(stbi_wrapper
        PRIVATE STB_IMAGE_IMPLEMENTATION
        PRIVATE STB_IMAGE_WRITE_IMPLEMENTATION
        )