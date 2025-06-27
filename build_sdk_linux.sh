#!/bin/bash

# Prevent exit when failed
set +e
trap 'echo -e "\n[ERROR] Script failed at line $LINENO. Exiting..."; read -n 1 -s -r -p "Press any key to close..."; exit 1' ERR

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

# ------------------------------------------------------------------------
# FAILED
# ------------------------------------------------------------------------
build_failed() {
  echo
  echo "------------------------------------------------------------------------"
  echo "Failed to build Kemena3D SDK, please try again."
  echo "------------------------------------------------------------------------"
  read -n 1 -s -r -p "Press any key to exit..."
  echo
  exit 1
}

# Linking type
echo
echo "Please choose static linking or dynamic linking:"
echo "1: Static linking (library code built into executable)"
echo "2: Dynamic linking (shared .so libraries)"
echo
read -rp "Enter your choice (1 or 2): " linking

if [[ "$linking" != "1" && "$linking" != "2" ]]; then
    echo "Invalid choice: $linking"
    build_failed
fi

# Determine linking flags
if [[ "$linking" == "1" ]]; then
    LINK_FLAG="-DBUILD_SHARED_LIBS=OFF"
else
    LINK_FLAG="-DBUILD_SHARED_LIBS=ON"
fi

# Build modes
build_with_cmake() {
    local BUILD_MODE="$1"
    local ARGS="$2"

    echo "=== Building Kemena3D SDK ($BUILD_MODE) ==="

    if [ ! -f "$KEMENA3D_FOLDER/CMakeLists.txt" ]; then
        echo "[ERROR] CMakeLists.txt not found"
        build_failed
    fi

    echo "[INFO] Configuring with CMake..."
    cmake -S "$KEMENA3D_FOLDER" -B "$BUILD_FOLDER/$BUILD_MODE" -DCMAKE_BUILD_TYPE="$BUILD_MODE" $ARGS
    if [ $? -ne 0 ]; then
        echo "[ERROR] CMake configuration failed."
        build_failed
    fi

    echo "[INFO] Building..."
    cmake --build "$BUILD_FOLDER/$BUILD_MODE"
    if [ $? -ne 0 ]; then
        echo "[ERROR] Build failed."
        build_failed
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
read -n 1 -s -r -p "Press any key to close..."
