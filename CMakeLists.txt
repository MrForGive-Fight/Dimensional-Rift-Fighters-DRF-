cmake_minimum_required(VERSION 3.20)
project(DFR VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable folder structure in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Find DirectX (Windows only)
if(WIN32)
    # DirectX paths
    set(DirectX_INCLUDE_DIR "" CACHE PATH "Path to DirectX SDK include directory")
    set(DirectX_LIBRARY_DIR "" CACHE PATH "Path to DirectX SDK library directory")
    
    # Try to find DirectX automatically
    if(NOT DirectX_INCLUDE_DIR)
        # Common paths for Windows SDK
        set(WINDOWS_SDK_PATHS
            "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22621.0"
            "C:/Program Files (x86)/Windows Kits/10/Include/10.0.22000.0"
            "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0"
        )
        
        foreach(SDK_PATH ${WINDOWS_SDK_PATHS})
            if(EXISTS "${SDK_PATH}/um/d3d11.h")
                set(DirectX_INCLUDE_DIR "${SDK_PATH}/um")
                message(STATUS "Found DirectX headers: ${DirectX_INCLUDE_DIR}")
                break()
            endif()
        endforeach()
    endif()
endif()

# Global compile options
if(MSVC)
    add_compile_options(
        /W4                 # Warning level 4
        /permissive-        # Conformance mode
        /Zi                 # Debug information
        /MP                 # Multi-processor compilation
        /std:c++20          # C++20 standard
    )
    
    # Disable specific warnings
    add_compile_options(
        /wd4996             # Deprecated functions
        /wd4251             # DLL interface
        /wd4275             # Non-DLL interface base class
    )
    
    # Release optimizations
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /GL")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
endif()

# Add source directory
add_subdirectory(src)

# Add documentation target
add_custom_target(docs
    SOURCES
    CLAUDE.md
    DFR_PROJECT_SUMMARY.md
    INTEGRATION_SUMMARY.md
    BUILD_INSTRUCTIONS.md
)

# Add examples directory if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/examples)
    add_subdirectory(examples)
endif()

# Add tests directory if it exists
if(EXISTS ${CMAKE_SOURCE_DIR}/tests)
    enable_testing()
    add_subdirectory(tests)
endif()

# Installation rules
install(DIRECTORY assets/ DESTINATION assets)
install(FILES CLAUDE.md README.md DESTINATION .)

# CPack configuration for packaging
set(CPACK_PACKAGE_NAME "DFR")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "ArenaFighter")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Anime 3D Arena Fighter")
include(CPack)
# External dependencies (added for ozz-animation)
set(ozz_build_samples OFF CACHE BOOL "Disable ozz samples" FORCE)
set(ozz_build_howtos OFF CACHE BOOL "Disable ozz howtos" FORCE)
set(ozz_build_tests OFF CACHE BOOL "Disable ozz tests" FORCE)
set(ozz_build_tools ON CACHE BOOL "Enable ozz tools" FORCE)
add_subdirectory(external/ozz-animation)

# DirectXTK - DirectX Tool Kit for particle effects and utilities
set(BUILD_TOOLS OFF CACHE BOOL "Disable DirectXTK tools" FORCE)
set(BUILD_XAUDIO_WIN10 OFF CACHE BOOL "Disable XAudio for Win10" FORCE)
add_subdirectory(external/DirectXTK)
