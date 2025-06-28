#!/bin/sh

# Enable strict mode
set -e

# Initialize
KEMENA3D_FOLDER="$(cd "$(dirname "$0")" && pwd)"
TEMP_FOLDER="$KEMENA3D_FOLDER/temp"
BUILD_FOLDER="$KEMENA3D_FOLDER/build"

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

# Linking type
echo
echo "Please choose static linking or dynamic linking:"
echo "1: Static linking (library code built into executable)"
echo "2: Dynamic linking (shared .so libraries)"
echo
printf "Enter your choice (1 or 2): "
read linking

if [ "$linking" != "1" ] && [ "$linking" != "2" ]; then
    echo "Invalid choice: $linking"
    exit 1
fi

# Determine linking flags
if [ "$linking" = "1" ]; then
    LINK_FLAG="-DBUILD_SHARED_LIBS=OFF"
else
    LINK_FLAG="-DBUILD_SHARED_LIBS=ON"
fi

# Build modes
build_with_cmake() {
    BUILD_MODE="$1"
    ARGS="$2"

    echo "=== Building Kemena3D SDK ($BUILD_MODE) ==="

    if [ ! -f "$KEMENA3D_FOLDER/CMakeLists.txt" ]; then
        echo "[ERROR] CMakeLists.txt not found"
        exit 1
    fi

    echo "[INFO] Configuring with CMake..."
    cmake -S "$KEMENA3D_FOLDER" -B "$BUILD_FOLDER/$BUILD_MODE" -DCMAKE_BUILD_TYPE="$BUILD_MODE" $ARGS
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
build_with_cmake "Debug" "$LINK_FLAG"
build_with_cmake "Release" "$LINK_FLAG"

# Success message
echo
echo "------------------------------------------------------------------------"
echo "Kemena3D SDK has been compiled successfully."
echo "------------------------------------------------------------------------"
echo "Press Enter to close..."
read dummy
