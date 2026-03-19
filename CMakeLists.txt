cmake_minimum_required(VERSION 3.10)

project(FeatureEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for Clangd/Intellisense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories("Header Files")

# Find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Specific compiler flags for dxerr
set_source_files_properties("src/dxerr.cpp" PROPERTIES
    COMPILE_FLAGS "-Wno-c++11-narrowing"
)

# Create the executable
add_executable(app WIN32 ${SOURCES})

# Link libraries
target_link_libraries(app PRIVATE user32 d3d11 dxguid d3dcompiler)

# ==============================================================================
# AUTOMATIC SHADER COMPILATION LOGIC
# ==============================================================================

# 1. Find the FXC shader compiler in Windows SDK
find_program(FXC_EXECUTABLE fxc PATHS 
    "C:/Program Files (x86)/Windows Kits/10/bin/*/x64"
    "C:/Program Files (x86)/Windows Kits/8.1/bin/x64"
)

if(NOT FXC_EXECUTABLE)
    message(FATAL_ERROR "fxc.exe not found. Please ensure Windows SDK is installed.")
endif()

# 2. Gather all HLSL files from the Shader directory
file(GLOB SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Shader/*.hlsl")

set(COMPILED_CSO_FILES "")

foreach(SHADER_PATH ${SHADER_SOURCES})
    # Get the filename without extension (e.g., VertexShader)
    get_filename_component(SHADER_NAME ${SHADER_PATH} NAME_WE)
    
    # Define the output path (Place it in the same directory as the executable)
    set(OUTPUT_CSO "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.cso")

    # Determine Shader Profile based on filename
    # If filename contains 'Vertex', use vs_5_0. If 'Pixel', use ps_5_0.
    if(${SHADER_NAME} MATCHES "Vertex")
        set(TARGET_PROFILE "vs_5_0")
    elseif(${SHADER_NAME} MATCHES "Pixel")
        set(TARGET_PROFILE "ps_5_0")
    else()
        # Fallback/Default profile
        set(TARGET_PROFILE "vs_5_0")
    endif()

    # 3. Create a custom command to compile the shader
    add_custom_command(
        OUTPUT ${OUTPUT_CSO}
        COMMAND ${FXC_EXECUTABLE} /T ${TARGET_PROFILE} /Fo "${OUTPUT_CSO}" "${SHADER_PATH}"
        DEPENDS "${SHADER_PATH}"
        COMMENT "Compiling HLSL: ${SHADER_PATH} -> ${OUTPUT_CSO} (${TARGET_PROFILE})"
    )

    list(APPEND COMPILED_CSO_FILES ${OUTPUT_CSO})
endforeach()

# 4. Create a custom target for the shaders and make the app depend on it
add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_CSO_FILES})
add_dependencies(app CompileShaders)

# ==============================================================================

# Copy Resources directory to build folder
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:app>/Resources
)