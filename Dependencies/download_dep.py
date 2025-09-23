#!/usr/bin/env python3
# Kemena3D Dependency Setup (pure standard library only)

import os
import sys
import shutil
import zipfile
import urllib.request
import subprocess
from pathlib import Path
import platform

system = platform.system()

# ------------------------------------------------------------------------
# Config / Paths
# ------------------------------------------------------------------------
ROOT = Path(__file__).resolve().parent
TEMP = ROOT / "temp"

# VS and MinGW defaults (edit if your paths differ)
VS2022_VCVARS = Path(os.environ.get("ProgramFiles", "")) / "Microsoft Visual Studio" / "2022" / "Community" / "VC" / "Auxiliary" / "Build" / "vcvars64.bat"

if system == "Windows":
    GCC_PATH = Path(r"C:/msys64/mingw64/bin/gcc.exe")
    GPP_PATH = Path(r"C:/msys64/mingw64/bin/g++.exe")

elif system in ("Linux", "FreeBSD"):
    GCC_PATH = Path("/usr/bin/gcc")
    GPP_PATH = Path("/usr/bin/g++")

elif system == "Darwin":  # macOS
    # Default Clang installed with Xcode Command Line Tools
    GCC_PATH = Path("/usr/bin/clang")
    GPP_PATH = Path("/usr/bin/clang++")

else:
    raise RuntimeError(f"Unsupported platform: {system}")

# Versions / URLs
ANGELSCRIPT_ZIP = "https://www.angelcode.com/angelscript/sdk/files/angelscript_2.37.0.zip"
SDL_GIT = "https://github.com/libsdl-org/SDL.git"
SDL_BRANCH = "release-3.2.16"
IMGUI_GIT = "https://github.com/ocornut/imgui.git"
GLM_GIT = "https://github.com/g-truc/glm.git"
GLM_TAG = "1.0.1"
GLEW_ZIP = "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip"
ASSIMP_GIT = "https://github.com/assimp/assimp.git"
ASSIMP_TAG = "v6.0.2"
STB_GIT = "https://github.com/nothings/stb.git"
STB_REV = "f58f558c120e9b32c217290b80bad1a0729fbb2c"
NLOHMANN_GIT = "https://github.com/nlohmann/json.git"
NLOHMANN_TAG = "v3.12.0"
PFD_GIT = "https://github.com/samhocevar/portable-file-dialogs.git"
PFD_TAG = "main"
IMGUIZMO_GIT = "https://github.com/CedricGuillemet/ImGuizmo.git"
IMGUIZMO_TAG = "master"

# Optional (commented in batch script)
RECAST_ZIP = "https://github.com/recastnavigation/recastnavigation/archive/refs/tags/v1.6.0.zip"
JOLT_ZIP = "https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.3.0.zip"

# ------------------------------------------------------------------------
# Utilities
# ------------------------------------------------------------------------
def die(msg: str, code: int = 1):
    print(f"[ERROR] {msg}")
    sys.exit(code)

def run(cmd: str, cwd: Path | None = None, env: dict | None = None):
    print(f"[RUN] {cmd}")
    result = subprocess.run(cmd, shell=True, cwd=str(cwd) if cwd else None, env=env)
    if result.returncode != 0:
        die(f"Command failed: {cmd}")

def ensure_tools():
    for tool in ("git", "cmake"):
        if shutil.which(tool) is None:
            die(f"'{tool}' not found in PATH. Please install and/or add to PATH.")

def ensure_dir(p: Path):
    p.mkdir(parents=True, exist_ok=True)

def clean_temp():
    if TEMP.exists():
        shutil.rmtree(TEMP)
    ensure_dir(TEMP)

def download_zip(url, out_path):
    print(f"[DOWNLOAD] {url}")
    # Use curl (Windows 10+ has it by default)
    result = subprocess.run(["curl", "-L", "-o", str(out_path), url])
    if result.returncode != 0:
        raise RuntimeError(f"curl failed for {url}")

def extract_zip(zip_path: Path, dest_dir: Path):
    print(f"[EXTRACT] {zip_path.name} -> {dest_dir}")
    with zipfile.ZipFile(zip_path, "r") as z:
        z.extractall(dest_dir)

def download_and_extract_zip(name: str, version: str, url: str, destfolder: Path, flatten: bool = True):
    if destfolder.exists():
        print(f"[SKIP] {name}: '{destfolder}' already exists.")
        return
    clean_temp()
    zip_path = TEMP / f"{destfolder.name}.zip"
    print(f"=== Downloading {name} ({version}) ===")
    download_zip(url, zip_path)
    extract_zip(zip_path, TEMP)
    zip_path.unlink(missing_ok=True)
    ensure_dir(destfolder)

    # Flatten: move contents of the first extracted directory into destfolder
    if flatten:
        # Find first top-level dir in TEMP
        top = None
        for entry in TEMP.iterdir():
            if entry.is_dir():
                top = entry
                break
        if top is None:
            die(f"{name}: No folder found inside archive.")
        for item in top.iterdir():
            target = destfolder / item.name
            if item.is_dir():
                shutil.copytree(item, target, dirs_exist_ok=True)
            else:
                shutil.copy2(item, target)
    else:
        # Move the first folder as-is
        top = None
        for entry in TEMP.iterdir():
            if entry.is_dir():
                top = entry
                break
        if top is None:
            die(f"{name}: No folder found inside archive.")
        shutil.move(str(top), str(destfolder))

    shutil.rmtree(TEMP)
    print(f"[OK] {name} setup complete at '{destfolder}'.")

def clone_git(name: str, version: str, repo: str, destfolder: Path, revision: str | None = None, branch: str | None = None):
    if destfolder.exists():
        print(f"[SKIP] {name}: '{destfolder}' already exists.")
        return
    print(f"=== Git Clone {name} ({version}) ===")
    clean_temp()
    args = ["git", "clone"]
    if branch:
        args += ["--branch", branch, "--single-branch"]
    args += [repo, str(TEMP)]
    run(" ".join(args))
    if revision:
        run(f"git checkout {revision}", cwd=TEMP)
    run("git submodule update --init --recursive", cwd=TEMP)
    shutil.move(str(TEMP), str(destfolder))
    print(f"[OK] {name} setup complete at '{destfolder}'.")

def is_msvc(generator: str) -> bool:
    return generator.startswith("Visual Studio")

def build_with_cmake(name: str, srcdir: Path, build_mode: str, args: str, generator: str):
    cmakelists = srcdir / "CMakeLists.txt"
    if not cmakelists.exists():
        die(f"{name}: CMakeLists.txt not found in '{srcdir}'.")
    print(f"[CMAKE] Building {name} ({build_mode}) with {generator}")
    build_dir = srcdir / f"build_{build_mode}"
    ensure_dir(build_dir)
    # Configure
    run(f'cmake -G "{generator}" -S . -B "{build_dir}" -DCMAKE_BUILD_TYPE={build_mode} {args}', cwd=srcdir)
    # Build
    if is_msvc(generator):
        run(f'cmake --build "{build_dir}" --config {build_mode}', cwd=srcdir)
    else:
        # MinGW (single-config); passing --config is harmless but we omit it
        run(f'cmake --build "{build_dir}"', cwd=srcdir)
    print(f"[OK] {name} built successfully ({build_mode}).")

def write_imgui_cmakelists(imgui_dir: Path, linking: str):
    add_lib = "add_library(imgui STATIC ${IMGUI_SRC})" if linking == "1" else "add_library(imgui SHARED ${IMGUI_SRC})"

    cmake_txt = f"""cmake_minimum_required(VERSION 3.10)
project(ImGuiLib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
file(GLOB IMGUI_SRC
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_tables.cpp
    backends/imgui_impl_sdl3.cpp
    backends/imgui_impl_opengl3.cpp
)
{add_lib}
target_include_directories(imgui PUBLIC ${{CMAKE_CURRENT_SOURCE_DIR}} ${{CMAKE_CURRENT_SOURCE_DIR}}/backends)
if(DEFINED SDL3_INCLUDE_DIR)
    target_include_directories(imgui PUBLIC ${{SDL3_INCLUDE_DIR}})
endif()
# Choose your OpenGL loader macro as needed. Keeping GLEW macro to match batch script.
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)
"""
    (imgui_dir / "CMakeLists.txt").write_text(cmake_txt, encoding="utf-8")
    print(f"[OK] Generated ImGui CMakeLists.txt at '{imgui_dir / 'CMakeLists.txt'}'")

def write_imguizmo_cmakelists(imguizmo_dir: Path, imgui_dir: Path, linking: str):
    add_lib = "add_library(imguizmo STATIC ${IMGUI_SRC})" if linking == "1" else "add_library(imguizmo SHARED ${IMGUI_SRC})"

    cmake_txt = f"""cmake_minimum_required(VERSION 3.10)
project(ImGuiLib LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
file(GLOB IMGUI_SRC
    GraphEditor.cpp
    ImCurveEdit.cpp
    ImGradient.cpp
    ImGuizmo.cpp
    ImSequencer.cpp
)
{add_lib}
target_include_directories(imguizmo PUBLIC ${{CMAKE_CURRENT_SOURCE_DIR}} {imgui_dir.as_posix()})
"""
    (imguizmo_dir / "CMakeLists.txt").write_text(cmake_txt, encoding="utf-8")
    print(f"[OK] Generated ImGuizmo CMakeLists.txt at '{imguizmo_dir / 'CMakeLists.txt'}'")

def choose(prompt: str, options: dict[str, str]) -> str:
    print(prompt)
    for k, v in options.items():
        print(f"{k}: {v}")
    choice = input("Enter your choice: ").strip()
    if choice not in options:
        die(f"Invalid choice: {choice}")
    return choice

def print_banner():
    print(r"  _  __   ___   __  __    ___    _  _     ___     ____    ___   ")
    print(r" | |/ /  | __| |  \/  |  | __|  | \| |   /   \   |__ /   |   \  ")
    print(r" | ' <   | _|  | |\/| |  | _|   | .` |   | - |    |_ \   | |) | ")
    print(r" |_|\_\  |___| |_|  |_|  |___|  |_|\_|   |_|_|   |___/   |___/  ")
    print(r"                       www.kemena3d.com")
    print("------------------------------------------------------------------------")
    print("Automatically download and compile dependencies for Kemena3D...")
    print("------------------------------------------------------------------------\n")

# ------------------------------------------------------------------------
# Main
# ------------------------------------------------------------------------
def main():
    ensure_tools()
    print_banner()

    # Select compiler based on OS
    if system == "Windows":
        compiler = choose(
            "Please choose a compiler:",
            {
                "1": "Build with Visual Studio 2022 (Community Edition)",
                "2": "Build with MinGW (GCC 14 or above)"
            }
        )

    elif system == "Linux":
        print("Linux detected → using GCC (default).")

    elif system == "Darwin":  # macOS
        compiler = choose(
            "Please choose a compiler:",
            {
                "1": "Xcode (Clang/LLVM from Command Line Tools)",
                "2": "GCC (via Homebrew or custom install)"
            }
        )

    elif system == "FreeBSD":
        print("FreeBSD detected → using GCC (default).")

    else:
        print(f"Unsupported platform: {system}")
        exit(1)
    
    # Select static or dynamic linking
    linking = choose(
        "\nPlease choose static linking or dynamic linking:",
        {"1": "Static linking (library code built into executable)",
         "2": "Dynamic linking (library code built into DLL files)"}
    )
    
    # Select supported model formats
    modelformat = choose(
        "\nPlease choose Assimp setting:",
        {"1": "All formats (read and write)",
         "2": "GLTF only (read only)"}
    )

    # Select generator and common flags
    if system == "Windows":
        if compiler == "1":
            generator = "Visual Studio 17 2022"
        elif compiler == "2":
            generator = "MinGW Makefiles"

    elif system in ("Linux", "FreeBSD"):
        generator = "Unix Makefiles"

    elif system == "Darwin":  # macOS
        if compiler == "1":
            generator = "Xcode"
        elif compiler == "2":
            generator = "Unix Makefiles"

    if not generator:
        raise RuntimeError(f"No CMake generator for {system}/{compiler}")

    print(f"[INFO] Using generator: {generator}")

    # --------------------------------------------------------------------
    # AngelScript
    # --------------------------------------------------------------------
    download_and_extract_zip("AngelScript", "v2.37.0", ANGELSCRIPT_ZIP, ROOT / "angelscript", flatten=True)
    as_src = ROOT / "angelscript" / "angelscript" / "projects" / "cmake"
    if compiler == "1":
        # VS
        if linking == "1":
            build_with_cmake("AngelScript", as_src, "Debug",  "-DBUILD_SHARED_LIBS=OFF", generator)
            build_with_cmake("AngelScript", as_src, "Release","-DBUILD_SHARED_LIBS=OFF", generator)
        else:
            build_with_cmake("AngelScript", as_src, "Debug",  "-DBUILD_SHARED_LIBS=ON", generator)
            build_with_cmake("AngelScript", as_src, "Release","-DBUILD_SHARED_LIBS=ON", generator)
    else:
        # MinGW
        if linking == "1":
            build_with_cmake("AngelScript", as_src, "Debug",  "-DBUILD_SHARED_LIBS=OFF", generator)
            build_with_cmake("AngelScript", as_src, "Release","-DBUILD_SHARED_LIBS=OFF", generator)
        else:
            build_with_cmake("AngelScript", as_src, "Debug",  "-DBUILD_SHARED_LIBS=ON", generator)
            build_with_cmake("AngelScript", as_src, "Release","-DBUILD_SHARED_LIBS=ON", generator)

    # --------------------------------------------------------------------
    # SDL
    # --------------------------------------------------------------------
    clone_git("SDL", "v3.2.16", SDL_GIT, ROOT / "sdl", branch=SDL_BRANCH)
    if compiler == "1":
        if linking == "1":
            build_with_cmake("SDL", ROOT / "sdl", "Debug",  "-DSDL_STATIC=ON -DSDL_SHARED=OFF", generator)
            build_with_cmake("SDL", ROOT / "sdl", "Release","-DSDL_STATIC=ON -DSDL_SHARED=OFF", generator)
        else:
            build_with_cmake("SDL", ROOT / "sdl", "Debug",  "-DSDL_STATIC=OFF -DSDL_SHARED=ON", generator)
            build_with_cmake("SDL", ROOT / "sdl", "Release","-DSDL_STATIC=OFF -DSDL_SHARED=ON", generator)
    else:
        cc = f'-DCMAKE_C_COMPILER="{GCC_PATH}" -DCMAKE_CXX_COMPILER="{GPP_PATH}"'
        if linking == "1":
            build_with_cmake("SDL", ROOT / "sdl", "Debug",  f"{cc} -DSDL_STATIC=ON -DSDL_SHARED=OFF", generator)
            build_with_cmake("SDL", ROOT / "sdl", "Release",f"{cc} -DSDL_STATIC=ON -DSDL_SHARED=OFF", generator)
        else:
            build_with_cmake("SDL", ROOT / "sdl", "Debug",  f"{cc} -DSDL_STATIC=OFF -DSDL_SHARED=ON", generator)
            build_with_cmake("SDL", ROOT / "sdl", "Release",f"{cc} -DSDL_STATIC=OFF -DSDL_SHARED=ON", generator)

    # --------------------------------------------------------------------
    # ImGui (generate CMakeLists.txt and build)
    # --------------------------------------------------------------------
    clone_git("imgui", "docking branch", IMGUI_GIT, ROOT / "imgui", branch="docking")
    write_imgui_cmakelists(ROOT / "imgui", linking)
    sdl_include = ROOT / "sdl" / "include"
    sdl_include_arg = f'-DSDL3_INCLUDE_DIR="{sdl_include}"'
    if compiler == "1":
        if linking == "1":
            build_with_cmake("imgui", ROOT / "imgui", "Debug",  f"-DBUILD_SHARED_LIBS=OFF {sdl_include_arg}", generator)
            build_with_cmake("imgui", ROOT / "imgui", "Release",f"-DBUILD_SHARED_LIBS=OFF {sdl_include_arg}", generator)
        else:
            build_with_cmake("imgui", ROOT / "imgui", "Debug",  f"-DBUILD_SHARED_LIBS=ON {sdl_include_arg}", generator)
            build_with_cmake("imgui", ROOT / "imgui", "Release",f"-DBUILD_SHARED_LIBS=ON {sdl_include_arg}", generator)
    else:
        if linking == "1":
            build_with_cmake("imgui", ROOT / "imgui", "Debug",  f"-DBUILD_SHARED_LIBS=OFF {sdl_include_arg}", generator)
            build_with_cmake("imgui", ROOT / "imgui", "Release",f"-DBUILD_SHARED_LIBS=OFF {sdl_include_arg}", generator)
        else:
            build_with_cmake("imgui", ROOT / "imgui", "Debug",  f"-DBUILD_SHARED_LIBS=ON {sdl_include_arg}", generator)
            build_with_cmake("imgui", ROOT / "imgui", "Release",f"-DBUILD_SHARED_LIBS=ON {sdl_include_arg}", generator)

    # --------------------------------------------------------------------
    # GLM (header-only)
    # --------------------------------------------------------------------
    clone_git("GLM", "v1.0.1", GLM_GIT, ROOT / "glm", revision=GLM_TAG)

    # --------------------------------------------------------------------
    # GLEW (prebuilt - headers + libs)
    # --------------------------------------------------------------------
    download_and_extract_zip("GLEW", "v2.2.0", GLEW_ZIP, ROOT / "glew", flatten=True)

    # --------------------------------------------------------------------
    # Assimp
    # --------------------------------------------------------------------
    clone_git("Assimp", "v6.0.2", ASSIMP_GIT, ROOT / "assimp", revision=ASSIMP_TAG)

    if modelformat == "1":
        model_flags = "-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=ON"
    else:
        model_flags = (
            "-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF "
            "-DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF "
            "-DASSIMP_BUILD_GLTF_IMPORTER=ON "
            "-DASSIMP_BUILD_GLTF_EXPORTER=OFF "
            "-DASSIMP_NO_EXPORT=ON "
            "-DASSIMP_BUILD_ASSIMP_TOOLS=OFF "
            "-DASSIMP_BUILD_TESTS=OFF "
            "-DASSIMP_BUILD_SAMPLES=OFF "
            "-DASSIMP_BUILD_FBX_IMPORTER=OFF "
            "-DASSIMP_BUILD_OBJ_IMPORTER=OFF "
            "-DASSIMP_BUILD_COLLADA_IMPORTER=OFF"
        )

    if compiler == "1":
        if linking == "1":
            flags = f"-DASSIMP_BUILD_ZLIB=ON -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF {model_flags}"
            build_with_cmake("Assimp", ROOT / "assimp", "Debug",  flags, generator)
            build_with_cmake("Assimp", ROOT / "assimp", "Release",flags, generator)
        else:
            flags = f"-DASSIMP_BUILD_ZLIB=ON -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF {model_flags}"
            build_with_cmake("Assimp", ROOT / "assimp", "Debug",  flags, generator)
            build_with_cmake("Assimp", ROOT / "assimp", "Release",flags, generator)
    else:
        cc = f'-DCMAKE_C_COMPILER="{GCC_PATH}" -DCMAKE_CXX_COMPILER="{GPP_PATH}"'
        if linking == "1":
            flags = f'{cc} -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF {model_flags}'
            build_with_cmake("Assimp", ROOT / "assimp", "Debug",  flags, generator)
            flags_rel = (f'{cc} -DCMAKE_CXX_FLAGS_RELEASE="-Wno-array-bounds -Wno-alloc-size-larger-than" '
                         f'-Wno-error=array-compare -Wno-error=class-memaccess '
                         f'-DASSIMP_BUILD_ZLIB=ON -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF {model_flags}')
            build_with_cmake("Assimp", ROOT / "assimp", "Release", flags_rel, generator)
        else:
            flags = f'{cc} -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF {model_flags}'
            build_with_cmake("Assimp", ROOT / "assimp", "Debug",  flags, generator)
            flags_rel = (f'{cc} -DCMAKE_CXX_FLAGS_RELEASE="-Wno-array-bounds -Wno-alloc-size-larger-than" '
                         f'-Wno-error=array-compare -Wno-error=class-memaccess '
                         f'-DASSIMP_BUILD_ZLIB=ON -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF {model_flags}')
            build_with_cmake("Assimp", ROOT / "assimp", "Release", flags_rel, generator)

    # --------------------------------------------------------------------
    # STB (header-only)
    # --------------------------------------------------------------------
    clone_git("STB", "Rev. f58f558", STB_GIT, ROOT / "stb", revision=STB_REV)

    # --------------------------------------------------------------------
    # nlohmann/json (header-only library with CMake package)
    # --------------------------------------------------------------------
    clone_git("nlohmann JSON", "v3.12.0", NLOHMANN_GIT, ROOT / "nlohmann", revision=NLOHMANN_TAG)
    
    # --------------------------------------------------------------------
    # Portable File Dialogs (header-only)
    # --------------------------------------------------------------------
    clone_git("Portable File Dialogs", "main", PFD_GIT, ROOT / "portable-file-dialogs", revision=PFD_TAG)
    
    # --------------------------------------------------------------------
    # ImGuizmo (header-only)
    # --------------------------------------------------------------------
    clone_git("imGuizmo", "master", IMGUIZMO_GIT, ROOT / "imguizmo", revision=IMGUIZMO_TAG)
    write_imguizmo_cmakelists(ROOT / "imguizmo", ROOT / "imgui", linking)
    if compiler == "1":
        if linking == "1":
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Debug",  f"-DBUILD_SHARED_LIBS=OFF", generator)
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Release",f"-DBUILD_SHARED_LIBS=OFF", generator)
        else:
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Debug",  f"-DBUILD_SHARED_LIBS=ON", generator)
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Release",f"-DBUILD_SHARED_LIBS=ON", generator)
    else:
        if linking == "1":
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Debug",  f"-DBUILD_SHARED_LIBS=OFF", generator)
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Release",f"-DBUILD_SHARED_LIBS=OFF", generator)
        else:
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Debug",  f"-DBUILD_SHARED_LIBS=ON", generator)
            build_with_cmake("imGuizmo", ROOT / "imguizmo", "Release",f"-DBUILD_SHARED_LIBS=ON", generator)

    # --------------------------------------------------------------------
    # Optional: Recast Navigation / JoltPhysics (kept commented like batch)
    # --------------------------------------------------------------------
    # download_and_extract_zip("Recast Navigation", "v1.6.0", RECAST_ZIP, ROOT / "recast", flatten=True)
    # download_and_extract_zip("JoltPhysics", "v5.3.0", JOLT_ZIP, ROOT / "jolt", flatten=True)
    # (Add their build steps here if you enable them)

    print("\n------------------------------------------------------------------------")
    print("All dependencies have been downloaded and compiled successfully.")
    print("------------------------------------------------------------------------")

if __name__ == "__main__":
    main()
