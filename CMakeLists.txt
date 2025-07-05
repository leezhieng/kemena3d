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

# Optional MinGW flag (set via -G)
if(WIN32)
    option(USE_MINGW "Use MinGW on Windows" OFF)
    if(USE_MINGW)
        message(STATUS "Using MinGW")
        add_definitions(-DUSE_MINGW)
    else()
        message(STATUS "Using MSVC")
        add_definitions(-DUSE_MSVC)
    endif()
endif()

# Collect core source and header files
file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/src/*.cpp)
file(GLOB_RECURSE HEADER_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source/include/*.h)

# Collect AngelScript add_on sources, and remove generateheader.cpp (which defines main)
file(GLOB_RECURSE ADDON_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/add_on/*.cpp)
list(REMOVE_ITEM ADDON_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/add_on/scriptbuilder/generateheader.cpp")

file(GLOB_RECURSE ADDON_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/add_on/*.h)

# Create library target
add_library(kemena3d ${SRC_FILES} ${HEADER_FILES} ${ADDON_SRC} ${ADDON_HEADERS})

set_target_properties(kemena3d PROPERTIES
    VERSION ${PROJECT_VERSION}
    PUBLIC_HEADER "${HEADER_FILES}"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Output/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Output/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/Output/lib"
)

# Include directories
target_include_directories(kemena3d
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/include
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
)

# Link directories if needed
link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/lib/Release/x64
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Build
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast
    ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour
)

# Definitions and static linking behavior
if (NOT BUILD_SHARED_LIBS)
    target_compile_definitions(kemena3d PRIVATE GLEW_STATIC)
    set(GLEW_LIB glew32s)
else()
    set(GLEW_LIB glew32)
endif()

# Platform specific
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

# Link libraries
target_link_libraries(kemena3d
    PUBLIC
        angelscript
        SDL3
        ${GLEW_LIB}
        assimp
        Jolt
        Recast
        Detour
)

# Suppress some noisy warnings
target_compile_options(kemena3d PRIVATE -Wno-deprecated-declarations)

# Visual Studio startup project
if (MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT kemena3d)
endif()

# Install targets and headers
install(TARGETS kemena3d
    EXPORT kemena3dTargets
    ARCHIVE DESTINATION Output/lib
    LIBRARY DESTINATION Output/lib
    RUNTIME DESTINATION Output/lib
    PUBLIC_HEADER DESTINATION Output/include/kemena
)

# Install headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/angelscript/include/
        DESTINATION Output/include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/add_on/
        DESTINATION Output/include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/sdl/include/SDL3/
        DESTINATION Output/include/SDL3 FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/include/GL/
        DESTINATION Output/include/GL FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/include/assimp/
        DESTINATION Output/include/assimp FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glm/glm/
        DESTINATION Output/include/glm FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/stb/
        DESTINATION Output/include/stb FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Jolt/
        DESTINATION Output/include/jolt FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/nlohmann/include/nlohmann/
        DESTINATION Output/include/nlohmann FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast/include/
        DESTINATION Output/include/recast/Recast FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour/include/
        DESTINATION Output/include/recast/Detour FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")

# Install dependency libs
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/angelscript/angelscript/projects/cmake/
        DESTINATION Output/lib FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/sdl/
        DESTINATION Output/lib FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/glew/lib/Release/x64/
        DESTINATION Output/lib FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/assimp/lib/
        DESTINATION Output/lib FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/jolt/Build/
        DESTINATION Output/lib FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Recast/
        DESTINATION Output/lib FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/recast/Detour/
        DESTINATION Output/lib FILES_MATCHING PATTERN "*.lib" PATTERN "*.dll" PATTERN "*.a")
