cmake_minimum_required(VERSION 3.10)
project(FeatureEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ==============================================================================
# 1. TÜM SOURCE (.cpp) DOSYALARINI REKÜRSİF BUL
# ==============================================================================
# GLOB_RECURSE src içindeki tüm alt klasörleri tarar
file(GLOB_RECURSE SOURCES "src/*.cpp")

# ImGui dosyalarını ekle
file(GLOB IMGUI_SOURCES "imgui/*.cpp")
list(APPEND SOURCES ${IMGUI_SOURCES})

# ==============================================================================
# 2. TÜM HEADER (.h) KLASÖRLERİNİ OTOMATİK BUL VE EKLE
# ==============================================================================
# Tüm .h dosyalarını bulup bulundukları klasörleri bir listeye ekliyoruz
file(GLOB_RECURSE ALL_HEADERS "Header Files/*.h" "Header Files/*.hpp")
set(HEADER_DIRS "")
foreach(HEADER_PATH ${ALL_HEADERS})
    get_filename_component(HEADER_DIR ${HEADER_PATH} DIRECTORY)
    list(APPEND HEADER_DIRS ${HEADER_DIR})
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS) # Aynı klasörden birden fazla header varsa temizle

# Bulunan tüm alt klasörleri include listesine ekle
include_directories("Header Files") # Ana dizin
include_directories(${HEADER_DIRS})  # Tüm alt dizinler
include_directories("imgui")

# ==============================================================================
# 3. EXECUTABLE OLUŞTURMA VE LİNKLEME
# ==============================================================================
add_executable(app WIN32 ${SOURCES})

# Özel compiler flagları
set_source_files_properties("src/dxerr.cpp" "imgui/imgui_impl_dx11.cpp" PROPERTIES
    COMPILE_FLAGS "-Wno-c++11-narrowing"
)

target_link_libraries(app PRIVATE user32 d3d11 dxguid d3dcompiler)

# ==============================================================================
# 4. OTOMATİK SHADER COMPILATION (REKÜRSİF)
# ==============================================================================
find_program(FXC_EXECUTABLE fxc PATHS 
    "C:/Program Files (x86)/Windows Kits/10/bin/*/x64"
    "C:/Program Files (x86)/Windows Kits/8.1/bin/x64"
)

# Burada da GLOB_RECURSE kullanarak alt klasörlerdeki shaderları buluyoruz
file(GLOB_RECURSE SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Shader/*.hlsl")

set(COMPILED_CSO_FILES "")
foreach(SHADER_PATH ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_PATH} NAME_WE)
    set(OUTPUT_CSO "${CMAKE_CURRENT_BINARY_DIR}/${SHADER_NAME}.cso")

    if(${SHADER_NAME} MATCHES "Vertex")
        set(TARGET_PROFILE "vs_5_0")
    elseif(${SHADER_NAME} MATCHES "Pixel")
        set(TARGET_PROFILE "ps_5_0")
    else()
        set(TARGET_PROFILE "vs_5_0")
    endif()

    add_custom_command(
        OUTPUT ${OUTPUT_CSO}
        COMMAND ${FXC_EXECUTABLE} /T ${TARGET_PROFILE} /Fo "${OUTPUT_CSO}" "${SHADER_PATH}"
        DEPENDS "${SHADER_PATH}"
        COMMENT "Compiling HLSL: ${SHADER_PATH} -> ${OUTPUT_CSO}"
    )
    list(APPEND COMPILED_CSO_FILES ${OUTPUT_CSO})
endforeach()

add_custom_target(CompileShaders ALL DEPENDS ${COMPILED_CSO_FILES})
add_dependencies(app CompileShaders)

# Resources kopyalama
add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Resources
    $<TARGET_FILE_DIR:app>/Resources
)

target_compile_definitions(app PRIVATE
    $<$<CONFIG:Debug>:IS_DEBUG=1>
    $<$<NOT:$<CONFIG:Debug>>:IS_DEBUG=0>
)