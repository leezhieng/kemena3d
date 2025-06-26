# How to Build the Dependencies

You can find the tutorial at: https://kemena3d.com/building-kemena3d-sdk

# Why Do This Separately?

Since each library may have different licensing terms, we do **not** include their source files directly in our repository. Instead, we provide a convenient batch script to **automatically download and build all required dependencies** into the project.

# Download Dependencies on Different Development OS

To avoid conflicts, it's best **not to run the batch script with different compilers** in the same project directory without a clean setup.

> For example:  
> If you build using **Visual Studio 2022**, avoid re-running the script with **MinGW**. This may result in **mixed configuration files** and broken builds.

### Recommended:
If you need to build Kemena3D using multiple compilers:
- Clone the entire Kemena3D repository into separate folders for each build environment.
- Alternatively, if switching compilers, **delete all folders inside the `Dependencies` directory** (e.g., `sdl`, `stb`, `glew`, etc.) before re-running the script.

## Windows

### 1. Visual Studio 2022 (Community Edition)
- Run: `download_dep_windows.bat`
- Choose **option 1** for compiler

### 2. MinGW (Minimalist GNU for Windows)
- Run: `download_dep_windows.bat`
- Choose **option 2** for compiler

## Linux

Coming soon...

## MacOS

Coming soon...

# Dependency Information

The automated batch script performs its tasks using the configurations below.  If the script fails, you may manually perform the steps as described. Whenever possible, we prioritize using stable official releases. If an official release isn't available, we retrieve the code directly from the GitHub repository. We always targeting a specific version tag or revision instead of pulling the latest code to ensure consistency.

### SDL

- **Website**: [SDL GitHub](https://github.com/libsdl-org/SDL)  
- **Version**: `3.2.16`  
- **Download (VS2022)**: [SDL3-devel-VC.zip](https://github.com/libsdl-org/SDL/releases/download/release-3.2.16/SDL3-devel-3.2.16-VC.zip)  
- **Download (MinGW)**: [SDL3-devel-mingw.zip](https://github.com/libsdl-org/SDL/releases/download/release-3.2.16/SDL3-devel-3.2.16-mingw.zip)  
- **Extract to**: `sdl`  
- **Build Required**: ✅ Yes

### GLM

- **Website**: [GLM GitHub](https://github.com/g-truc/glm)  
- **Version**: `1.0.1`  
- **Git Clone**: `https://github.com/g-truc/glm.git`  
- **Extract to**: `glm`  
- **Build Required**: ❌ No

### GLEW

- **Website**: [GLEW GitHub](https://github.com/nigels-com/glew)  
- **Version**: `2.2.0`  
- **Download**: [glew-2.2.0-win32.zip](https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip)  
- **Extract to**: `glew`  
- **Build Required**: ❌ No

### Recast Navigation

- **Website**: [Recast GitHub](https://github.com/recastnavigation/recastnavigation)  
- **Version**: `1.6.0`  
- **Download**: [v1.6.0.zip](https://github.com/recastnavigation/recastnavigation/archive/refs/tags/v1.6.0.zip)  
- **Extract to**: `recast`  
- **Build Required**: ✅ Yes

### Assimp

- **Website**: [Assimp GitHub](https://github.com/assimp/assimp)  
- **Version**: `6.0.2`  
- **Git Clone**: `https://github.com/assimp/assimp.git`  
- **Extract to**: `assimp`  
- **Build Required**: ✅ Yes

### STB

- **Website**: [STB GitHub](https://github.com/nothings/stb)  
- **Version**: `Rev. f58f558`  
- **Git Clone**: `https://github.com/nothings/stb.git`  
- **Extract to**: `stb`  
- **Build Required**: ❌ No

### JSON for Modern C++ (nlohmann)

- **Website**: [nlohmann/json GitHub](https://github.com/nlohmann/json)  
- **Version**: `3.12.0`  
- **Git Clone**: `https://github.com/nlohmann/json.git`  
- **Extract to**: `nlohmann`  
- **Build Required**: ❌ No

### 🔹 Jolt Physics

- **Website**: [JoltPhysics GitHub](https://github.com/jrouwe/JoltPhysics)  
- **Version**: `5.3.0`  
- **Download**: [v5.3.0.zip](https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.3.0.zip)  
- **Extract to**: `jolt`  
- **Build Required**: ✅ Yes
