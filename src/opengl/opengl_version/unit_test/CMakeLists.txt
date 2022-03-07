add_executable(${PROJECT_NAME}_OPENGL_UNIT_TEST
        unit_test.cpp
        )
target_link_libraries(${PROJECT_NAME}_OPENGL_UNIT_TEST
        PRIVATE glengine
        PRIVATE project_warnings
        PRIVATE Threads::Threads
        PRIVATE OpenVIII_CPP_WIP_VIIIGraphics
        PRIVATE OpenVIII_CPP_WIP_VIIIArchive
        )
target_include_directories(${PROJECT_NAME}_OPENGL_UNIT_TEST
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        )
SET_TARGET_PATHS(${PROJECT_NAME}_OPENGL)


add_custom_command(TARGET ${PROJECT_NAME}_OPENGL_UNIT_TEST POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
        "Copying ${CMAKE_CURRENT_SOURCE_DIR}/res to $<TARGET_FILE_DIR:${PROJECT_NAME}_OPENGL>/res")
add_custom_command(TARGET ${PROJECT_NAME}_OPENGL_UNIT_TEST POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}_OPENGL>/res)

if (MSVC)
    target_compile_definitions(${PROJECT_NAME}_OPENGL_UNIT_TEST
            PUBLIC _CRT_SECURE_NO_WARNINGS) # required by libpng
    target_link_options(${PROJECT_NAME}_OPENGL_UNIT_TEST PRIVATE "/PROFILE")
endif ()

target_precompile_headers(${PROJECT_NAME}_OPENGL_UNIT_TEST
        PRIVATE
        <open_viii/archive/Archives.hpp>
        <open_viii/graphics/BPPT.hpp>
        <open_viii/graphics/Rectangle.hpp>
        <open_viii/graphics/background/Map.hpp>
        <open_viii/graphics/background/Mim.hpp>
        <open_viii/graphics/color/Color32.hpp>
        <open_viii/paths/Paths.hpp>
        <open_viii/strings/LangCommon.hpp>
        <boost/ht.hpp>
        )