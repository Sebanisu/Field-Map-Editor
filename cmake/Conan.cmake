include_guard()
# from https://raw.githubusercontent.com/aminya/project_options/main/src/Conan.cmake
# Run Conan for dependency management
macro(run_conan)
    # Download automatically, you can also just copy the conan.cmake file
    if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(
                #                DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.17.0/conan.cmake"
                DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/release/0.18/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                #                EXPECTED_HASH SHA256=3bef79da16c2e031dc429e1dac87a08b9226418b300ce004cc125a82687baeef
                EXPECTED_HASH SHA256=5cdb3042632da3efff558924eecefd580a0e786863a857ca097c3d1d43df5dcd
                TLS_VERIFY ON)
    endif ()

    set(ENV{CONAN_REVISIONS_ENABLED} 1)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
    list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

    include(${CMAKE_BINARY_DIR}/conan.cmake)

    # Add (or remove) remotes as needed
    # conan_add_remote(NAME conan-center URL https://conan.bintray.com)
    #    conan_add_remote(
    #            NAME
    #            cci
    #            URL
    #            https://center.conan.io
    #            INDEX
    #            0)
    #    conan_add_remote(
    #            NAME
    #            bincrafters
    #            URL
    #            https://bincrafters.jfrog.io/artifactory/api/conan/public-conan)

    if (CONAN_EXPORTED)
        # standard conan installation, in which deps will be defined in conanfile. It is not necessary to call conan again, as it is already running.
        if (EXISTS "${CMAKE_BINARY_DIR}/../conanbuildinfo.cmake")
            include(${CMAKE_BINARY_DIR}/../conanbuildinfo.cmake)
        else ()
            message(
                    FATAL_ERROR "Could not set up conan because \"${CMAKE_BINARY_DIR}/../conanbuildinfo.cmake\" does not exist")
        endif ()
        conan_basic_setup()
    else ()
        # For multi configuration generators, like VS and XCode
        if (NOT CMAKE_CONFIGURATION_TYPES)
            message(STATUS "Single configuration build!")
            set(LIST_OF_BUILD_TYPES ${CMAKE_BUILD_TYPE})
        else ()
            message(STATUS "Multi-configuration build: '${CMAKE_CONFIGURATION_TYPES}'!")
            set(LIST_OF_BUILD_TYPES ${CMAKE_CONFIGURATION_TYPES})
        endif ()

        foreach (TYPE ${LIST_OF_BUILD_TYPES})
            message(STATUS "Running Conan for build type '${TYPE}'")

            if ("${ProjectOptions_CONAN_PROFILE}" STREQUAL "")
                # Detects current build settings to pass into conan
                conan_cmake_autodetect(settings BUILD_TYPE ${TYPE})
                if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                    set(CONAN_SETTINGS SETTINGS ${settings} compiler.cppstd=20)
                    #set(CONAN_ENV ENV "CC=${CMAKE_C_COMPILER}" "CXX=${CMAKE_CXX_COMPILER}" "CXXFLAGS=/D_HAS_AUTO_PTR_ETC") #shouldn't need to enable auto_ptr anymore
                elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
                    set(CONAN_SETTINGS SETTINGS ${settings} compiler.cppstd=20 compiler.libcxx=libstdc++11)
                    set(CONAN_ENV ENV "CC=${CMAKE_C_COMPILER}" "CXX=${CMAKE_CXX_COMPILER}" "CXXFLAGS=-fexperimental-library")# "CXXFLAGS=-stdlib=libc++ -march=native -fexperimental-library" "LDFLAGS=-stdlib=libc++ -fuse-ld=lld -Wl")
                else () #GCC
                    set(CONAN_SETTINGS SETTINGS ${settings} compiler.cppstd=20 compiler.libcxx=libstdc++11)
                    set(CONAN_ENV ENV "CC=${CMAKE_C_COMPILER}" "CXX=${CMAKE_CXX_COMPILER}")
                endif ()
            else ()
                # Derive all conan settings from a conan profile
                set(CONAN_SETTINGS
                        PROFILE
                        ${ProjectOptions_CONAN_PROFILE}
                        SETTINGS
                        "build_type=${TYPE}")
                # CONAN_ENV should be redundant, since the profile can set CC & CXX
            endif ()

            # PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR} is used to tell conan to process
            # the external "conanfile.py" provided with the project
            # Alternatively a conanfile.txt could be used
            conan_cmake_install(
                    PATH_OR_REFERENCE
                    ${CMAKE_SOURCE_DIR}
                    BUILD
                    outdated
                    # Pass compile-time configured options into conan
                    OPTIONS
                    ${ProjectOptions_CONAN_OPTIONS}
                    UPDATE
                    # Pass CMake compilers to Conan
                    ${CONAN_ENV}
                    # Pass either autodetected settings or a conan profile
                    ${CONAN_SETTINGS}
                    ${OUTPUT_QUIET})
        endforeach ()
    endif ()

endmacro()