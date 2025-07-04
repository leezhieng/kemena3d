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

# Optional flag for MinGW-specific logic (actual compiler selection must be done via -G)
if(WIN32)
    option(USE_MINGW "Use MinGW on Windows" OFF)

    if(USE_MINGW)
        message(STATUS "Kemena3D SDK is being configured with MinGW")
        add_definitions(-DUSE_MINGW)
    else()
        message(STATUS "Kemena3D SDK is being configured with Visual Studio")
        add_definitions(-DUSE_MSVC)
    endif()
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Debug)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG     ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Debug)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE   ${CMAKE_CURRENT_SOURCE_DIR}/Source/lib/Release)

# Collect source files
file(GLOB_RECURSE SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/src/kemena/*.cpp
)

file(GLOB_RECURSE HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Source/include/kemena/*.h
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
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/include/kemena
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/angelscript/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/add_on
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/sdl/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glm
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stb
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Jolt
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/nlohmann/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour/include
        # ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourCrowd/include
        # ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourTileCache/include
)

# Link directories (if using prebuilt libraries)
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/angelscript/projects/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/sdl
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/lib/Release/x64
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Build
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour
    # ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourCrowd
    # ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/DetourTileCache
)

# Conditional linking and definitions
if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(kemena3d PRIVATE GLEW_STATIC)
    set(GLEW_LIB glew32s)
else()
    set(GLEW_LIB glew32)
endif()

# Platform-specific settings
if (WIN32)
    target_compile_definitions(kemena3d PRIVATE _WINDOWS)
elseif (APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(kemena3d PUBLIC ${COCOA_LIBRARY} ${OpenGL_LIBRARY})
    target_compile_definitions(kemena3d PRIVATE _MACOS)
elseif (UNIX)
    find_package(OpenGL REQUIRED)
    find_package(X11 REQUIRED)
    target_link_libraries(kemena3d PUBLIC ${OPENGL_LIBRARIES} ${X11_LIBRARIES})
    target_compile_definitions(kemena3d PRIVATE _LINUX)
endif()

# Link all libraries
target_link_libraries(kemena3d
    PUBLIC
        libangelscript
        libSDL3
        ${GLEW_LIB}
        libassimp
        libJolt
        libRecast
        libDetour
        # libDetourCrowd
        # libDetourTileCache
)

# Suppress deprecated warnings from system headers (like sprintf)
target_compile_options(kemena3d PRIVATE -Wno-deprecated-declarations)

# Visual Studio startup project
if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT kemena3d)
endif()

# Install
install(TARGETS kemena3d
    EXPORT kemena3dTargets
    ARCHIVE DESTINATION Source/lib/kemena
    LIBRARY DESTINATION Source/lib/kemena
    RUNTIME DESTINATION Source/lib/kemena
    PUBLIC_HEADER DESTINATION Source/include/kemena
)

# Copy headers and libs of each dependency manually
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/angelscript/include/
        DESTINATION Source/include/angelscript)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/add_on/
        DESTINATION Source/include/angelscript/add_on)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/sdl/include/
        DESTINATION Source/include/sdl)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/include/
        DESTINATION Source/include/glew)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/include/
        DESTINATION Source/include/assimp)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glm/
        DESTINATION Source/include/glm)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stb/
        DESTINATION Source/include/stb)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Jolt/
        DESTINATION Source/include/jolt)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/nlohmann/include/
        DESTINATION Source/include/nlohmann)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast/include/
        DESTINATION Source/include/recast/Recast)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour/include/
        DESTINATION Source/include/recast/Detour)

# Optional: install static or shared lib files from Dependencies
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/lib/Release/x64/
        DESTINATION Source/lib/glew FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/lib/
        DESTINATION Source/lib/assimp FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Build/
        DESTINATION Source/lib/jolt FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast/
        DESTINATION Source/lib/recast FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour/
        DESTINATION Source/lib/recast FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
