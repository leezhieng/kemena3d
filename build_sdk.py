import os
import platform
import subprocess
from pathlib import Path

def run_cmd(cmd, cwd=None):
    print(f"[RUN] {cmd}")
    result = subprocess.run(cmd, shell=True, cwd=cwd)
    if result.returncode != 0:
        raise RuntimeError(f"[ERROR] Command failed: {cmd}")

def banner():
    print(r"""
  _  __   ___   __  __    ___    _  _     ___     ____    ___
 | |/ /  | __| |  \/  |  | __|  | \| |   /   \   |__ /   |   \
 | ' <   | _|  | |\/| |  | _|   | .` |   | - |    |_ \   | |) |
 |_|\_\  |___| |_|  |_|  |___|  |_|\_|   |_|_|   |___/   |___/
                        www.kemena3d.com
 ------------------------------------------------------------------------
 Automatically compile Kemena3D SDK...
 ------------------------------------------------------------------------
""")

def choose(prompt, options: dict):
    print(prompt)
    for k, v in options.items():
        print(f"{k}: {v}")
    choice = input("Enter your choice: ").strip()
    if choice not in options:
        raise ValueError(f"Invalid choice: {choice}")
    return choice

def build_with_cmake(generator, build_mode, args):
    build_dir = f"build_{build_mode}"
    install_prefix = os.path.join(os.getcwd(), f"Output/{build_mode}")

    # Configure
    run_cmd(
        f'cmake -S . -B {build_dir} -G "{generator}" '
        f'-DCMAKE_INSTALL_PREFIX={install_prefix} '
        f'-DCMAKE_BUILD_TYPE={build_mode} {args}'
    )

    # Build
    run_cmd(f'cmake --build {build_dir} --config {build_mode}')

    # Install
    run_cmd(f'cmake --install {build_dir} --config {build_mode}')

    print("[SUCCESS] Kemena3D SDK built and installed successfully.")

def main():
    banner()
    system = platform.system()

    # Compiler selection
    if system == "Windows":
        compiler = choose(
            "\nPlease choose a compiler:",
            {
                "1": "Build with Visual Studio 2022 (Community Edition)",
                "2": "Build with MinGW (GCC 14 or above)"
            }
        )
    elif system == "Linux":
        print("\nLinux detected → using GCC (default).")
        compiler = "1"
    elif system == "FreeBSD":
        print("\nFreeBSD detected → using GCC (default).")
        compiler = "1"
    elif system == "Darwin":  # macOS
        compiler = choose(
            "\nPlease choose a compiler:",
            {
                "1": "Xcode (Clang/LLVM from Command Line Tools)",
                "2": "GCC (via Homebrew or custom install)"
            }
        )
    else:
        print(f"Unsupported platform: {system}")
        exit(1)

    # Linking selection
    linking = choose(
        "\nPlease choose static linking or dynamic linking:",
        {"1": "Static linking (library built into executable)",
         "2": "Dynamic linking (DLL / shared library)"}
    )

    # CMake generator setup
    if system == "Windows":
        if compiler == "1":
            generator = "Visual Studio 17 2022"
        elif compiler == "2":
            generator = "MinGW Makefiles"
    elif system in ("Linux", "FreeBSD"):
        generator = "Unix Makefiles"
    elif system == "Darwin":
        generator = "Xcode" if compiler == "1" else "Unix Makefiles"
    else:
        raise RuntimeError(f"No CMake generator for {system}/{compiler}")

    # Build args
    if system == "Windows":
        if compiler == "1":  # Visual Studio
            if linking == "1":
                args = "-DBUILD_SHARED_LIBS=OFF -DUSE_MINGW=OFF"
            else:
                args = "-DBUILD_SHARED_LIBS=ON -DUSE_MINGW=OFF -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON -DKEMENA_SHARED"
        else:  # MinGW
            if linking == "1":
                args = "-DBUILD_SHARED_LIBS=OFF -DUSE_MINGW=ON"
            else:
                args = "-DBUILD_SHARED_LIBS=ON -DUSE_MINGW=ON -DCMAKE_WINDOWS_EXPORT_ALL_SYMBOLS=ON -DKEMENA_SHARED"

    elif system in ("Linux", "FreeBSD", "Darwin"):
        if linking == "1":
            args = "-DBUILD_SHARED_LIBS=OFF"
        else:
            args = "-DBUILD_SHARED_LIBS=ON -DKEMENA_SHARED"

    # Debug + Release builds
    build_with_cmake(generator, "Debug", args)
    build_with_cmake(generator, "Release", args)

    print("\n------------------------------------------------------------------------")
    print("Kemena3D SDK has been compiled successfully.")
    print("------------------------------------------------------------------------")

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print("\n------------------------------------------------------------------------")
        print(f"Failed to download or compile Kemena3D SDK: {e}")
        print("------------------------------------------------------------------------")
        exit(1)
