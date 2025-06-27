#!/bin/bash

set -e

# Initialize
DEP_FOLDER="$(cd "$(dirname "$0")" && pwd)"
TEMP_FOLDER="$DEP_FOLDER/temp"
BUILD_FOLDER="$DEP_FOLDER/build"

# ASCII Banner
cat << "EOF"
 _  __   ___   __  __    ___    _  _     ___     ____    ___   
| |/ /  | __| |  \/  |  | __|  | \| |   /   \   |__ /   |   \  
| ' <   | _|  | |\/| |  | _|   | .` |   | - |    |_ \   | |) | 
|_|\_\  |___| |_|  |_|  |___|  |_|\_|   |_|_|   |___/   |___/ 
                     www.kemena3d.com
------------------------------------------------------------------------
Automatically compile Kemena3D SDK...
------------------------------------------------------------------------
EOF

# Compiler selection
echo
echo "Please choose a compiler:"
echo "1: Build with Xcode (Clang/LLVM)"
echo "2: Build with GCC (via Homebrew or custom install)"
echo
read -rp "Enter your choice (1 or 2): " compiler

if [[ "$compiler" != "1" && "$compiler" != "2" ]]; then
    echo "Invalid choice: $compiler"
    exit 1
fi

# Linking type
echo
echo "Please choose static linking or dynamic linking:"
echo "1: Static linking (library code built into executable)"
echo "2: Dynamic linking (shared dylib libraries)"
echo
read -rp "Enter your choice (1 or 2): " linking

if [[ "$linking" != "1" && "$linking" != "2" ]]; then
    echo "Invalid choice: $linking"
    exit 1
fi

# Set compiler variables
if [[ "$compiler" == "2" ]]; then
    export CC=$(which gcc-13 || which gcc-12 || which gcc)
    export CXX=$(which g++-13 || which g++-12 || which g++)
    echo "[INFO] Using GCC: $CC, $CXX"
else
    export CC=clang
    export CXX=clang++
fi

# Determine linking flags
if [[ "$linking" == "1" ]]; then
    LINK_FLAG="-DBUILD_SHARED_LIBS=OFF"
else
    LINK_FLAG="-DBUILD_SHARED_LIBS=ON"
fi

# Determine toolchain flag
if [[ "$compiler" == "2" ]]; then
    TOOLCHAIN_FLAG="-DUSE_MINGW=ON"
else
    TOOLCHAIN_FLAG="-DUSE_MINGW=OFF"
fi

# Build modes
build_with_cmake() {
    local BUILD_MODE="$1"
    local ARGS="$2"

    echo "=== Building Kemena3D SDK ($BUILD_MODE) ==="

    if [ ! -f "CMakeLists.txt" ]; then
        echo "[ERROR] CMakeLists.txt not found"
        exit 1
    fi

    echo "[INFO] Configuring with CMake..."
    cmake -S . -B "$BUILD_FOLDER/$BUILD_MODE" -DCMAKE_BUILD_TYPE="$BUILD_MODE" $ARGS
    if [ $? -ne 0 ]; then
        echo "[ERROR] CMake configuration failed."
        exit 1
    fi

    echo "[INFO] Building..."
    cmake --build "$BUILD_FOLDER/$BUILD_MODE"
    if [ $? -ne 0 ]; then
        echo "[ERROR] Build failed."
        exit 1
    fi

    echo "[SUCCESS] Built Kemena3D SDK ($BUILD_MODE)"
}

# Run builds
build_with_cmake "Debug" "$LINK_FLAG $TOOLCHAIN_FLAG"
build_with_cmake "Release" "$LINK_FLAG $TOOLCHAIN_FLAG"

# Success message
echo
echo "------------------------------------------------------------------------"
echo "Kemena3D SDK has been compiled successfully."
echo "------------------------------------------------------------------------"
read -n 1 -s -r -p "Press any key to close..."