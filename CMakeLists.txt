# Specify the minimum required version of CMake. 3.10 is sufficient for most systems.
cmake_minimum_required(VERSION 3.10)

# Define the project name. This will be the name of the generated solution file.
project(FeatureEngine)

# Enforce the use of the C++17 standard.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# THE MAGIC LINE FOR CLANGD:
# This command tells CMake to automatically generate the "compile_commands.json" file.
# All that complexity in the old build.bat script was just to achieve this.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add the "Header Files" directory to the include search paths.
# This replaces the old -I "Header Files" compiler flag.
include_directories("Header Files")

# Find all .cpp files under the "src" directory and its subdirectories.
# This replaces the old `for /R` loop from the batch script.
file(GLOB_RECURSE SOURCES "src/*.cpp")

set_source_files_properties("src/dxerr.cpp" PROPERTIES
    COMPILE_FLAGS "-Wno-c++11-narrowing"
)

# Create an executable named "app" (which will become app.exe on Windows)
# using the `SOURCES` list we just populated.
add_executable(app WIN32 ${SOURCES})

# Link the required Windows libraries to our executable.
# This replaces the old -luser32 linker flag.
target_link_libraries(app PRIVATE user32 d3d11 dxguid)