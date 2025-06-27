#!/bin/bash

# Enable strict mode
set -e

DEP_FOLDER="$(cd "$(dirname "$0")" && pwd)"
TEMP_FOLDER="$DEP_FOLDER/temp"

# ASCII Banner
cat << "EOF"
 _  __   ___   __  __    ___    _  _     ___     ____    ___   
| |/ /  | __| |  \/  |  | __|  | \| |   /   \   |__ /   |   \  
| ' <   | _|  | |\/| |  | _|   | .` |   | - |    |_ \   | |) | 
|_|\_\  |___| |_|  |_|  |___|  |_|\_|   |_|_|   |___/   |___/ 
                      www.kemena3d.com
------------------------------------------------------------------------
Automatically download and compile dependencies for Kemena3D...
------------------------------------------------------------------------
EOF

# Validate compiler input
echo
echo "Please choose a compiler:"
echo "1: Build with Xcode (Clang/LLVM from Command Line Tools)"
echo "2: Build with GCC (via Homebrew or custom install)"
echo
read -rp "Enter your choice (1 or 2): " compiler

if [[ "$compiler" != "1" && "$compiler" != "2" ]]; then
    echo "Invalid choice: $compiler"
    exit 1
fi

# Validate linking input
echo
echo "Please choose static linking or dynamic linking:"
echo "1: Static linking (library code built into executable)"
echo "2: Dynamic linking (shared library dylibs)"
echo
read -rp "Enter your choice (1 or 2): " linking

if [[ "$linking" != "1" && "$linking" != "2" ]]; then
    echo "Invalid choice: $linking"
    exit 1
fi

# Validate Assimp format input
echo
echo "Please choose Assimp setting:"
echo "1: All formats (read and write)"
echo "2: GLTF only (read only)"
echo
read -rp "Enter your choice (1 or 2): " modelformat

if [[ "$modelformat" != "1" && "$modelformat" != "2" ]]; then
    echo "Invalid choice: $modelformat"
    exit 1
fi

# After user inputs

# Set compiler paths if using GCC
if [[ "$compiler" == "2" ]]; then
  export CC=$(which gcc-13 || which gcc)
  export CXX=$(which g++-13 || which g++)
else
  export CC=clang
  export CXX=clang++
fi

# Set build flags based on user linking choice
LINK_STATIC_FLAGS="-DBUILD_SHARED_LIBS=OFF"
LINK_DYNAMIC_FLAGS="-DBUILD_SHARED_LIBS=ON"

if [[ "$linking" == "1" ]]; then
  LINK_FLAGS="$LINK_STATIC_FLAGS"
else
  LINK_FLAGS="$LINK_DYNAMIC_FLAGS"
fi

# SDL: Set SDL-specific build flags based on user linking choice
SDL_FLAGS=""
if [[ "$linking" == "1" ]]; then
  SDL_FLAGS="-DSDL_STATIC=ON -DSDL_SHARED=OFF"
else
  SDL_FLAGS="-DSDL_STATIC=OFF -DSDL_SHARED=ON"
fi

# Set Assimp model format flags
if [[ "$modelformat" == "1" ]]; then
  MODEL_FORMAT="-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=ON"
else
  MODEL_FORMAT="-DASSIMP_BUILD_GLTF_IMPORTER=ON -DASSIMP_NO_EXPORT=ON"
fi

# ------------------------------------------------------------------------
# SUCCESS
# ------------------------------------------------------------------------
build_success() {
  echo
  echo "------------------------------------------------------------------------"
  echo "All dependencies have been downloaded and compiled successfully."
  echo "------------------------------------------------------------------------"
}

# ------------------------------------------------------------------------
# FAILED
# ------------------------------------------------------------------------
build_failed() {
  echo
  echo "------------------------------------------------------------------------"
  echo "Failed to download or compile dependencies, please try again."
  echo "------------------------------------------------------------------------"
  exit 1
}

# ------------------------------------------------------------------------
# BELOW ARE THE FUNCTIONS
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# Download and extract zip
# ------------------------------------------------------------------------
download_and_extract_zip() {
  NAME=$1
  VERSION=$2
  URL=$3
  DESTFOLDER="$DEP_FOLDER/$4"
  FLATTEN=$5
  ZIPFILE="$TEMP_FOLDER/${4}_temp.zip"

  echo "=== Downloading $NAME ($VERSION) ==="

  if [ -d "$DESTFOLDER" ]; then
    echo "Folder '$DESTFOLDER' already exists. Skipping."
    return
  fi

  rm -rf "$TEMP_FOLDER"
  mkdir -p "$TEMP_FOLDER"

  echo "Downloading ZIP..."
  curl -L "$URL" -o "$ZIPFILE" || build_failed

  echo "Extracting..."
  unzip -q "$ZIPFILE" -d "$TEMP_FOLDER" || build_failed
  rm -f "$ZIPFILE"

  mkdir -p "$DESTFOLDER"

  if [[ "$FLATTEN" == "true" ]]; then
    echo "Flattening files to $DESTFOLDER..."
    shopt -s dotglob
    mv "$TEMP_FOLDER"/*/* "$DESTFOLDER"/ || build_failed
  else
    echo "Moving extracted folder to $DESTFOLDER..."
    mv "$TEMP_FOLDER"/* "$DESTFOLDER"/ || build_failed
  fi

  rm -rf "$TEMP_FOLDER"
  echo "$NAME setup complete."
}

# ------------------------------------------------------------------------
# Download and extract tarball
# ------------------------------------------------------------------------
download_and_extract_tarball() {
  NAME=$1
  VERSION=$2
  URL=$3
  DESTFOLDER="$DEP_FOLDER/$4"
  FLATTEN=$5
  FILENAME=$(basename "$URL")
  TARFILE="$TEMP_FOLDER/$FILENAME"

  echo "=== Downloading $NAME ($VERSION) ==="

  if [ -d "$DESTFOLDER" ]; then
    echo "Folder '$DESTFOLDER' already exists. Skipping."
    return
  fi

  rm -rf "$TEMP_FOLDER"
  mkdir -p "$TEMP_FOLDER"

  echo "Downloading tarball..."
  curl -L "$URL" -o "$TARFILE" || build_failed

  echo "Extracting..."
  tar -xzf "$TARFILE" -C "$TEMP_FOLDER" || build_failed
  rm -f "$TARFILE"

  mkdir -p "$DESTFOLDER"

  if [[ "$FLATTEN" == "true" ]]; then
    echo "Flattening files to $DESTFOLDER..."
    shopt -s dotglob
    mv "$TEMP_FOLDER"/*/* "$DESTFOLDER"/ || build_failed
  else
    echo "Moving extracted folder to $DESTFOLDER..."
    mv "$TEMP_FOLDER"/* "$DESTFOLDER"/ || build_failed
  fi

  rm -rf "$TEMP_FOLDER"
  echo "$NAME setup complete."
}

# ------------------------------------------------------------------------
# Clone Git
# ------------------------------------------------------------------------
clone_git() {
  NAME=$1
  VERSION=$2
  REPO_URL=$3
  REVISION=$4
  DESTFOLDER="$DEP_FOLDER/$5"
  TEMP_CLONE="$TEMP_FOLDER"

  echo "=== Git Clone $NAME ($VERSION) ==="

  if [ -d "$DESTFOLDER" ]; then
    echo "Folder '$DESTFOLDER' already exists. Skipping $NAME clone."
    return
  fi

  rm -rf "$TEMP_CLONE"
  echo "Cloning $NAME repository..."
  git clone "$REPO_URL" "$TEMP_CLONE" || build_failed

  cd "$TEMP_CLONE" || build_failed
  if [[ -n "$REVISION" ]]; then
    echo "Checking out revision: $REVISION"
    git checkout "$REVISION" || build_failed
  fi

  echo "Initializing submodules..."
  git submodule update --init --recursive
  cd - > /dev/null || build_failed

  if [ -d "$DESTFOLDER" ]; then
    echo "ERROR: Destination folder '$DESTFOLDER' already exists."
    rm -rf "$TEMP_CLONE"
    build_failed
  fi

  mv "$TEMP_CLONE" "$DESTFOLDER"
  echo "$NAME setup complete in folder '$DESTFOLDER'."
}

# ------------------------------------------------------------------------
# Build With CMake (CLANG / GCC)
# ------------------------------------------------------------------------
build_with_cmake() {
  NAME=$1
  SOURCE_DIR="$DEP_FOLDER/$2"
  BUILD_MODE=$3
  ARGS=$4
  GEN=${5:-"Unix Makefiles"}

  echo "=== Building $NAME ($BUILD_MODE) ==="

  CMAKELISTS="$SOURCE_DIR/CMakeLists.txt"
  if [ ! -f "$CMAKELISTS" ]; then
    echo "[ERROR] CMakeLists.txt not found in '$SOURCE_DIR'"
    build_failed
  fi

  # Patch if missing cmake_minimum_required
  if ! grep -q "cmake_minimum_required" "$CMAKELISTS"; then
    echo "[INFO] Patching missing cmake_minimum_required in CMakeLists.txt"
    sed -i.bak '1i\
cmake_minimum_required(VERSION 3.10)
' "$CMAKELISTS"
  fi

  BUILD_DIR="$TEMP_FOLDER/build_$NAME"
  mkdir -p "$BUILD_DIR"
  cd "$BUILD_DIR" || build_failed

  echo "[INFO] Running CMake configuration..."
  cmake -G "$GEN" "$SOURCE_DIR" -DCMAKE_BUILD_TYPE="$BUILD_MODE" $ARGS || build_failed

  echo "[INFO] Building $NAME..."
  cmake --build . || build_failed

  echo "[SUCCESS] $NAME built successfully."
  cd - > /dev/null || build_failed
}

# ------------------------------------------------------------------------
# Build With Make
# ------------------------------------------------------------------------
build_with_make() {
  NAME=$1
  SOURCE_DIR="$DEP_FOLDER/$2"
  BUILD_MODE=$3

  echo "=== Building $NAME ($BUILD_MODE) ==="

  if [ ! -f "$SOURCE_DIR/Makefile" ]; then
    echo "[ERROR] Makefile not found in $SOURCE_DIR."
    build_failed
  fi

  cd "$SOURCE_DIR" || build_failed

  echo "[INFO] Running make clean..."
  make clean || true

  #echo "[INFO] Building extensions (macOS safe)..."
  #make extensions SYSTEM=darwin PYTHON=python3 || build_failed

  echo "[INFO] Building target..."
  make CONFIG="$BUILD_MODE" || build_failed

  BUILD_DIR="$SOURCE_DIR/build_$(echo "$BUILD_MODE" | tr '[:upper:]' '[:lower:]')"
  mkdir -p "$BUILD_DIR"

  #echo "[INFO] Installing to $BUILD_DIR..."
  #make install FINAL_DEST="$BUILD_DIR" CONFIG="$BUILD_MODE" || build_failed

  echo "[SUCCESS] $NAME built and installed to $BUILD_DIR."
  cd - > /dev/null || build_failed
}

# ------------------------------------------------------------------------
# BELOW ARE THE DEPENDENCIES
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# SDL
# ------------------------------------------------------------------------
clone_git "SDL" "v3.2.16" "https://github.com/libsdl-org/SDL.git" "release-3.2.16" "sdl"
build_with_cmake "SDL" "sdl" "Debug" "$SDL_FLAGS"
build_with_cmake "SDL" "sdl" "Release" "$SDL_FLAGS"

# ------------------------------------------------------------------------
# GLM
# ------------------------------------------------------------------------
clone_git "GLM" "v1.0.1" "https://github.com/g-truc/glm.git" "1.0.1" "glm"

# ------------------------------------------------------------------------
# GLEW
# ------------------------------------------------------------------------
download_and_extract_tarball "GLEW" "v2.2.0" "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz" "glew" "true"
build_with_make "GLEW" "glew" "Debug"
build_with_make "GLEW" "glew" "Release"

# ------------------------------------------------------------------------
# Recast Navigation
# ------------------------------------------------------------------------
download_and_extract_zip "Recast Navigation" "v1.6.0" "https://github.com/recastnavigation/recastnavigation/archive/refs/tags/v1.6.0.zip" "recast" "true"
build_with_cmake "Detour" "recast/Detour" "Debug" "$LINK_FLAGS"
build_with_cmake "Detour" "recast/Detour" "Release" "$LINK_FLAGS"
build_with_cmake "Recast" "recast/Recast" "Debug" "$LINK_FLAGS"
build_with_cmake "Recast" "recast/Recast" "Release" "$LINK_FLAGS"

# ------------------------------------------------------------------------
# Assimp
# ------------------------------------------------------------------------
clone_git "Assimp" "v6.0.2" "https://github.com/assimp/assimp.git" "v6.0.2" "assimp"
build_with_cmake "Assimp" "assimp" "Debug" "$LINK_FLAGS $MODEL_FORMAT"
build_with_cmake "Assimp" "assimp" "Release" "$LINK_FLAGS $MODEL_FORMAT"

# ------------------------------------------------------------------------
# STB
# ------------------------------------------------------------------------
clone_git "STB" "f58f558" "https://github.com/nothings/stb.git" "f58f558c120e9b32c217290b80bad1a0729fbb2c" "stb"

# ------------------------------------------------------------------------
# nlohmann JSON
# ------------------------------------------------------------------------
clone_git "nlohmann JSON" "v3.12.0" "https://github.com/nlohmann/json.git" "v3.12.0" "nlohmann"

# ------------------------------------------------------------------------
# JoltPhysics
# ------------------------------------------------------------------------
# Note: Jolt does not have native macOS build support, assumes CMake usage is supported
# If CMake not supported, this section will need a custom script
download_and_extract_zip "JoltPhysics" "v5.3.0" "https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.3.0.zip" "jolt" "true"
build_with_cmake "JoltPhysics" "jolt/Build" "Debug" "$LINK_FLAGS"
build_with_cmake "JoltPhysics" "jolt/Build" "Release" "$LINK_FLAGS"

# ------------------------------------------------------------------------
# End of script
# ------------------------------------------------------------------------
build_success

read -n 1 -s -r -p "Press any key to close..."
