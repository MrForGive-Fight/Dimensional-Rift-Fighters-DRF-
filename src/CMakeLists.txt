# DFR Source CMake Configuration

# Collect all source files
file(GLOB_RECURSE DFR_SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# Group files for IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${DFR_SOURCES})

# Create executable
add_executable(DFRGame WIN32 ${DFR_SOURCES})

# Set properties
set_target_properties(DFRGame PROPERTIES
    FOLDER "Game"
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Include directories
target_include_directories(DFRGame PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# DirectX include directories
if(DirectX_INCLUDE_DIR)
    target_include_directories(DFRGame PRIVATE ${DirectX_INCLUDE_DIR})
endif()

# Link libraries
target_link_libraries(DFRGame PRIVATE
    d3d11
    dxgi
    dxguid
    d3dcompiler
    winmm
    shlwapi
    comctl32
)

# Precompiled headers (optional)
# target_precompile_headers(DFRGame PRIVATE pch.h)

# Copy assets to output directory
add_custom_command(TARGET DFRGame POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:DFRGame>/assets
)

# Set startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT DFRGame)