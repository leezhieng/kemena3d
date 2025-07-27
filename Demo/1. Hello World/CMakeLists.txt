cmake_minimum_required(VERSION 3.10)

project(Kemena3dDemo LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output to bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source file
add_executable(demo main.cpp)

# Custom include and lib directories
target_include_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../Output/include)
target_link_directories(demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../Output/lib)

# Hide console window
set_target_properties(demo PROPERTIES WIN32_EXECUTABLE TRUE)

# Link libraries (Windows only)
if (WIN32)
    target_link_libraries(demo PRIVATE
        # Third-party static libraries
        kemena3d
        SDL3
        assimp
        angelscript
        imgui
        glew32s

        # Windows system libraries
        opengl32
        winmm
        ole32
        setupapi
        gdi32
        imm32
        version
        oleaut32
        uuid
        libz
    )
endif()

target_compile_definitions(demo PRIVATE KEMENA3D_STATIC)