@echo off
setlocal EnableDelayedExpansion
set "TEMP_FOLDER=temp"
set "VS2022_PATH=%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set "GCC_PATH=C:/MinGW/bin/gcc.exe"
set "GPP_PATH=C:/MinGW/bin/g++.exe"

:: ASCII Banner
echo  _  __   ___   __  __    ___    _  _     ___     ____    ___   
echo ^| ^|/ /  ^| __^| ^|  \/  ^|  ^| __^|  ^| \^| ^|   /   \   ^|__ /   ^|   \  
echo ^| ' ^<   ^| _^|  ^| ^|\/^| ^|  ^| _^|   ^| .` ^|   ^| - ^|    ^|_ \   ^| ^|) ^| 
echo ^|_^|\_\  ^|___^| ^|_^|  ^|_^|  ^|___^|  ^|_^|\_^|   ^|_^|_^|   ^|___/   ^|___/ 
echo                        www.kemena3d.com
echo ------------------------------------------------------------------------
echo Automatically compile Kemena3D SDK...
echo ------------------------------------------------------------------------

:: ------------------------------------------------------------------------
:: Choose compiler
:: ------------------------------------------------------------------------
echo.
echo Please choose a compiler:
echo 1: Build with Visual Studio 2022 (Community Edition)
echo 2: Build with MinGW (GCC 14 or above)
echo.
set /p compiler=Enter your choice (1 or 2): 

:: Validate input
if not "%compiler%"=="1" if not "%compiler%"=="2" (
    echo Invalid choice: %compiler%
	pause
    goto :failed
)

:: ------------------------------------------------------------------------
:: Choose static linking or dynamic linking
:: ------------------------------------------------------------------------
echo.
echo Please choose static linking or dynamic linking:
echo 1: Static linking (library code built into executable)
echo 2: Dynamic linking (library code built into DLL files)
echo.
set /p linking=Enter your choice (1 or 2): 

:: Validate input
if not "%linking%"=="1" if not "%linking%"=="2" (
    echo Invalid choice: %linking%
    goto :failed
)

:: ------------------------------------------------------------------------
:: Build Kemena3D SDK
:: ------------------------------------------------------------------------
echo.

if "%compiler%"=="1" (
	:: VS 2022
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeVs2022 "Debug" "-DBUILD_SHARED_LIBS=OFF -DUSE_MINGW=OFF"
		call :buildWithCMakeVs2022 "Release" "-DBUILD_SHARED_LIBS=OFF -DUSE_MINGW=OFF"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeVs2022 "Debug" "-DBUILD_SHARED_LIBS=ON -DUSE_MINGW=OFF"
		call :buildWithCMakeVs2022 "Release" "-DBUILD_SHARED_LIBS=ON -DUSE_MINGW=OFF"
	)
) else if "%compiler%"=="2" (
	:: MinGW
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeMinGW "Debug" "-DBUILD_SHARED_LIBS=OFF -DUSE_MINGW=ON"
		call :buildWithCMakeMinGW "Release" "-DBUILD_SHARED_LIBS=OFF -DUSE_MINGW=ON"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeMinGW "Debug" "-DBUILD_SHARED_LIBS=ON -DUSE_MINGW=ON"
		call :buildWithCMakeMinGW "Release" "-DBUILD_SHARED_LIBS=ON -DUSE_MINGW=ON"
	)
)

:: ------------------------------------------------------------------------
:: End of script
:: ------------------------------------------------------------------------
goto :build_success

:: ------------------------------------------------------------------------
:: Success
:: ------------------------------------------------------------------------
:build_success
echo.
echo ------------------------------------------------------------------------
echo Kemena3D SDK has been compiled successfully.
echo ------------------------------------------------------------------------
pause
goto :eof

:: ------------------------------------------------------------------------
:: Failed
:: ------------------------------------------------------------------------
:build_failed
echo.
echo ------------------------------------------------------------------------
echo Failed to download or compile Kemena3D SDK, please try again.
echo ------------------------------------------------------------------------
pause
goto :eof

:: ------------------------------------------------------------------------
:: BELOW ARE THE FUNCTIONS
:: ------------------------------------------------------------------------

:: ------------------------------------------------------------------------
:: Build With CMake (VS 2022)
:: ------------------------------------------------------------------------
:buildWithCMakeVs2022
set "BUILD_MODE=%~1"
set "ARGS=%~2"

echo === Building Kemena3D SDK (%BUILD_MODE%) ===

if not exist "CMakeLists.txt" (
    echo [ERROR] CMakeLists.txt not found
    goto :build_failed
)

echo [INFO] Running CMake configuration...
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_INSTALL_PREFIX=%cd% %ARGS%
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    goto :build_failed
)

echo [INFO] Building Kemena3D SDK...
cmake --build build --config %BUILD_MODE%
if errorlevel 1 (
    echo [ERROR] Build failed.
    goto :build_failed
)

echo [SUCCESS] Kemena3D SDK built successfully.

echo [INFO] Installing Kemena3D SDK...
cmake --install build --config %BUILD_MODE%
if errorlevel 1 (
    echo [ERROR] Installation failed.
    goto :build_failed
)

popd
goto :eof

:: ------------------------------------------------------------------------
:: Build With CMake (MinGW)
:: ------------------------------------------------------------------------
:buildWithCMakeMinGW
set "BUILD_MODE=%~1"
set "ARGS=%~2"

echo === Building Kemena3D SDK (%BUILD_MODE%) ===

if not exist "CMakeLists.txt" (
    echo [ERROR] CMakeLists.txt not found
    goto :build_failed
)

echo [INFO] Running CMake configuration...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_INSTALL_PREFIX=%cd% %ARGS%
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    goto :build_failed
)

echo [INFO] Building Kemena3D SDK...
cmake --build build --config %BUILD_MODE%
if errorlevel 1 (
    echo [ERROR] Build failed.
    goto :build_failed
)

echo [SUCCESS] Kemena3D SDK built successfully.

echo [INFO] Installing Kemena3D SDK...
cmake --install build --config %BUILD_MODE%
if errorlevel 1 (
    echo [ERROR] Installation failed.
    goto :build_failed
)

popd
goto :eof