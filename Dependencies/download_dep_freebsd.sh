#!/bin/sh

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

# Linking selection
printf "\nPlease choose static linking or dynamic linking:\n"
echo "1: Static linking (library code built into executable)"
echo "2: Dynamic linking (shared library .so)"
printf "\nEnter your choice (1 or 2): "
read linking

if [ "$linking" != "1" ] && [ "$linking" != "2" ]; then
    echo "Invalid choice: $linking"
    exit 1
fi

# Assimp format selection
printf "\nPlease choose Assimp setting:\n"
echo "1: All formats (read and write)"
echo "2: GLTF only (read only)"
printf "\nEnter your choice (1 or 2): "
read modelformat

if [ "$modelformat" != "1" ] && [ "$modelformat" != "2" ]; then
    echo "Invalid choice: $modelformat"
    exit 1
fi

# Set compiler
export CC=$(which gcc || which clang)
export CXX=$(which g++ || which clang++)

# Link flags
LINK_STATIC_FLAGS="-DBUILD_SHARED_LIBS=OFF"
LINK_DYNAMIC_FLAGS="-DBUILD_SHARED_LIBS=ON"
LINK_FLAGS="$LINK_DYNAMIC_FLAGS"
[ "$linking" = "1" ] && LINK_FLAGS="$LINK_STATIC_FLAGS"

# SDL flags
SDL_FLAGS="-DSDL_STATIC=OFF -DSDL_SHARED=ON"
[ "$linking" = "1" ] && SDL_FLAGS="-DSDL_STATIC=ON -DSDL_SHARED=OFF"

# Assimp model format
MODEL_FORMAT="-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=ON"
if [ "$modelformat" = "2" ]; then
  MODEL_FORMAT="-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF \
-DASSIMP_BUILD_GLTF_IMPORTER=ON -DASSIMP_BUILD_GLTF_EXPORTER=OFF -DASSIMP_NO_EXPORT=ON \
-DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_SAMPLES=OFF \
-DASSIMP_BUILD_FBX_IMPORTER=OFF -DASSIMP_BUILD_OBJ_IMPORTER=OFF -DASSIMP_BUILD_COLLADA_IMPORTER=OFF"
fi

# Utility Functions
build_success() {
  echo "\n------------------------------------------------------------------------"
  echo "All dependencies have been downloaded and compiled successfully."
  echo "------------------------------------------------------------------------"
}

build_failed() {
  echo "\n------------------------------------------------------------------------"
  echo "Failed to download or compile dependencies, please try again."
  echo "------------------------------------------------------------------------"
  exit 1
}

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
  fetch -o "$ZIPFILE" "$URL" || build_failed

  echo "Extracting..."
  unzip -q "$ZIPFILE" -d "$TEMP_FOLDER" || build_failed
  rm -f "$ZIPFILE"

  mkdir -p "$DESTFOLDER"

  if [ "$FLATTEN" = "true" ]; then
    mv "$TEMP_FOLDER"/*/* "$DESTFOLDER"/ || build_failed
  else
    mv "$TEMP_FOLDER"/* "$DESTFOLDER"/ || build_failed
  fi

  rm -rf "$TEMP_FOLDER"
  echo "$NAME setup complete."
}

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
  fetch -o "$TARFILE" "$URL" || build_failed

  echo "Extracting..."
  tar -xzf "$TARFILE" -C "$TEMP_FOLDER" || build_failed
  rm -f "$TARFILE"

  mkdir -p "$DESTFOLDER"

  if [ "$FLATTEN" = "true" ]; then
    mv "$TEMP_FOLDER"/*/* "$DESTFOLDER"/ || build_failed
  else
    mv "$TEMP_FOLDER"/* "$DESTFOLDER"/ || build_failed
  fi

  rm -rf "$TEMP_FOLDER"
  echo "$NAME setup complete."
}

clone_git() {
  NAME=$1
  VERSION=$2
  REPO_URL=$3
  REVISION=$4
  DESTFOLDER="$DEP_FOLDER/$5"
  TEMP_CLONE="$TEMP_FOLDER"

  echo "=== Git Clone $NAME ($VERSION) ==="

  if [ -d "$DESTFOLDER" ]; then
    echo "Folder '$DESTFOLDER' already exists. Skipping."
    return
  fi

  rm -rf "$TEMP_CLONE"
  git clone "$REPO_URL" "$TEMP_CLONE" || build_failed
  cd "$TEMP_CLONE" || build_failed

  if [ -n "$REVISION" ]; then
    git checkout "$REVISION" || build_failed
  fi

  git submodule update --init --recursive || build_failed
  cd - > /dev/null || build_failed

  mv "$TEMP_CLONE" "$DESTFOLDER"
  echo "$NAME setup complete."
}

build_with_cmake() {
  NAME=$1
  SOURCE_DIR="$DEP_FOLDER/$2"
  BUILD_MODE=$3
  ARGS=$4

  echo "=== Building $NAME ($BUILD_MODE) ==="

  mkdir -p "$SOURCE_DIR/build_$BUILD_MODE"
  cd "$SOURCE_DIR/build_$BUILD_MODE" || build_failed

  cmake .. -DCMAKE_BUILD_TYPE=$BUILD_MODE $ARGS || build_failed
  cmake --build . || build_failed

  cd - > /dev/null || build_failed
  echo "$NAME ($BUILD_MODE) build complete."
}

build_with_make() {
  NAME=$1
  SOURCE_DIR="$DEP_FOLDER/$2"
  BUILD_MODE=$3

  echo "=== Building $NAME ($BUILD_MODE) ==="

  cd "$SOURCE_DIR" || build_failed
  make clean || true
  make || build_failed
  cd - > /dev/null || build_failed
  echo "$NAME built using Make."
}

# Dependencies
clone_git "SDL" "v3.2.16" "https://github.com/libsdl-org/SDL.git" "release-3.2.16" "sdl"
build_with_cmake "SDL" "sdl" "Debug" "$SDL_FLAGS"
build_with_cmake "SDL" "sdl" "Release" "$SDL_FLAGS"

clone_git "GLM" "v1.0.1" "https://github.com/g-truc/glm.git" "1.0.1" "glm"

download_and_extract_tarball "GLEW" "v2.2.0" "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0.tgz" "glew" "true"
build_with_make "GLEW" "glew" "Debug"
build_with_make "GLEW" "glew" "Release"

download_and_extract_zip "Recast Navigation" "v1.6.0" "https://github.com/recastnavigation/recastnavigation/archive/refs/tags/v1.6.0.zip" "recast" "true"
build_with_cmake "Detour" "recast/Detour" "Debug" "$LINK_FLAGS"
build_with_cmake "Detour" "recast/Detour" "Release" "$LINK_FLAGS"
build_with_cmake "Recast" "recast/Recast" "Debug" "$LINK_FLAGS"
build_with_cmake "Recast" "recast/Recast" "Release" "$LINK_FLAGS"

clone_git "Assimp" "v6.0.2" "https://github.com/assimp/assimp.git" "v6.0.2" "assimp"
build_with_cmake "Assimp" "assimp" "Debug" "$LINK_FLAGS $MODEL_FORMAT"
build_with_cmake "Assimp" "assimp" "Release" "$LINK_FLAGS $MODEL_FORMAT"

clone_git "STB" "f58f558" "https://github.com/nothings/stb.git" "f58f558c120e9b32c217290b80bad1a0729fbb2c" "stb"

clone_git "nlohmann JSON" "v3.12.0" "https://github.com/nlohmann/json.git" "v3.12.0" "nlohmann"

download_and_extract_zip "JoltPhysics" "v5.3.0" "https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.3.0.zip" "jolt" "true"
build_with_cmake "JoltPhysics" "jolt/Build" "Debug" "$LINK_FLAGS"
build_with_cmake "JoltPhysics" "jolt/Build" "Release" "$LINK_FLAGS"

build_success
printf "\nPress Enter to close..."
read dummy
