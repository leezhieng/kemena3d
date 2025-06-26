cmake_minimum_required(VERSION 3.16)
project(Kemena3dSdk
    VERSION 0.0.1
    DESCRIPTION "Kemena3D SDK"
    LANGUAGES CXX
)

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to build shared or static library
option(BUILD_SHARED_LIBS "Build as shared library" OFF)

# Detect platform and select appropriate toolchain
if(WIN32)
    option(USE_MINGW "Use MinGW on Windows" OFF)
    if(USE_MINGW)
        message(STATUS "Using MinGW")
        set(CMAKE_SYSTEM_NAME Windows)
    else()
        message(STATUS "Using Visual Studio 2022")
        set(CMAKE_GENERATOR_PLATFORM x64)
    endif()
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Debug)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Release)

# Collect source files
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/angelscript/source/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/angelscript/add_on/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/angelscript/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/angelscript/add_on/*.h
)

# Create library target
add_library(kemena3d ${SRC_FILES} ${HEADER_FILES})

set_target_properties(kemena3d PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${HEADER_FILES}"
)

# Include directories
target_include_directories(kemena3d
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/angelscript/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/angelscript/add_on
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/sdl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Jolt
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/nlohmann/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour/include
        #${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourCrowd/include
        #${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourTileCache/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stb
)

# Link directories (if using prebuilt libraries)
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/lib/Release/x64
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Build
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour
    #${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourCrowd
    #${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourTileCache
)

# Link libraries
target_link_libraries(kemena3d
    PUBLIC
        libSDL3
        #glew32
        libassimp
        libJolt
        libRecast
        libDetour
        #libDetourCrowd
        #libDetourTileCache
)

# Conditional linking and definitions for GLEW
if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(kemena3d PRIVATE GLEW_STATIC)
    target_link_libraries(kemena3d PUBLIC glew32s)
else()
    target_link_libraries(kemena3d PUBLIC glew32)
endif()

# Platform-specific settings
if (WIN32)
    target_compile_definitions(kemena3d PRIVATE _WINDOWS)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(kemena3d ${COCOA_LIBRARY} ${OpenGL_LIBRARY})
    target_compile_definitions(kemena3d PRIVATE _MACOS)
elseif(UNIX)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(kemena3d ${OPENGL_LIBRARIES} ${X11_LIBRARIES})
    target_compile_definitions(kemena3d PRIVATE _LINUX)
endif()

# Set Visual Studio startup project (for MSVC users)
if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT kemena3d)
endif()

# Install
install(TARGETS kemena3d
    EXPORT kemena3dTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
    PUBLIC_HEADER DESTINATION include
)