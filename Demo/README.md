# Demo List

1. Hello World

## Building the Demo

Ensure you have CMake and a compatible compiler installed.  
Navigate to the folder of the demo you wish to build, then run the following commands:

<details>
<summary><strong>MinGW (Windows)</strong></summary>
cmake . -G "MinGW Makefiles"

cmake --build . --config Release
</details>

<details>
<summary><strong>Visual Studio (Windows)</strong></summary>
cmake . -G "Visual Studio 17 2022"

cmake --build . --config Release
</details>

<details>
<summary><strong>macOS/Linux (Makefiles)</strong></summary>
cmake .

cmake --build . --config Release
</details>

After building, the executable will be located in the bin/ folder.