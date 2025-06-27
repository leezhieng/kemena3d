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
echo Automatically download and compile dependencies for Kemena3D...
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
:: Choose Assimp setting
:: ------------------------------------------------------------------------
echo.
echo Please choose Assimp setting:
echo 1: All formats (read and write)
echo 2: GLTF only (read only)
echo.
set /p modelformat=Enter your choice (1 or 2): 

:: Validate input
if not "%modelformat%"=="1" if not "%modelformat%"=="2" (
    echo Invalid choice: %modelformat%
    goto :failed
)

:: ------------------------------------------------------------------------
:: BELOW ARE THE DEPENDENCIES
:: ------------------------------------------------------------------------

:: ------------------------------------------------------------------------
:: SDL
:: ------------------------------------------------------------------------
echo.

call :cloneGit "SDL" "v3.2.16" "https://github.com/libsdl-org/SDL.git" "release-3.2.16" "sdl"

if "%compiler%"=="1" (
	:: VS 2022
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeVs2022 "SDL" "sdl" "Debug" "-DSDL_STATIC=ON -DSDL_SHARED=OFF"
		call :buildWithCMakeVs2022 "SDL" "sdl" "Release" "-DSDL_STATIC=ON -DSDL_SHARED=OFF"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeVs2022 "SDL" "sdl" "Debug" "-DSDL_STATIC=OFF -DSDL_SHARED=ON"
		call :buildWithCMakeVs2022 "SDL" "sdl" "Release" "-DSDL_STATIC=OFF -DSDL_SHARED=ON"
	)
) else if "%compiler%"=="2" (
	:: MinGW
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeMinGW "SDL" "sdl" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DSDL_STATIC=ON -DSDL_SHARED=OFF"
		call :buildWithCMakeMinGW "SDL" "sdl" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DSDL_STATIC=ON -DSDL_SHARED=OFF"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeMinGW "SDL" "sdl" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DSDL_STATIC=OFF -DSDL_SHARED=ON"
		call :buildWithCMakeMinGW "SDL" "sdl" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DSDL_STATIC=OFF -DSDL_SHARED=ON"
	)
)

:: ------------------------------------------------------------------------
:: GLM
:: ------------------------------------------------------------------------
echo.
call :cloneGit "GLM" "v1.0.1" "https://github.com/g-truc/glm.git" "1.0.1" "glm"

:: ------------------------------------------------------------------------
:: GLEW
:: ------------------------------------------------------------------------
echo.
call :downloadAndExtractZip "GLEW" "v2.2.0" "https://github.com/nigels-com/glew/releases/download/glew-2.2.0/glew-2.2.0-win32.zip" "glew" "true"

:: ------------------------------------------------------------------------
:: Recast Navigation
:: ------------------------------------------------------------------------
echo.
call :downloadAndExtractZip "Recast Navigation" "v1.6.0" "https://github.com/recastnavigation/recastnavigation/archive/refs/tags/v1.6.0.zip" "recast" "true"
echo.

if "%compiler%"=="1" (
	:: VS 2022
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeVs2022 "Detour" "recast/Detour" "Debug" "-DBUILD_SHARED_LIBS=OFF"
		call :buildWithCMakeVs2022 "Detour" "recast/Detour" "Release" "-DBUILD_SHARED_LIBS=OFF"

		call :buildWithCMakeVs2022 "Recast" "recast/Recast" "Debug" "-DBUILD_SHARED_LIBS=OFF"
		call :buildWithCMakeVs2022 "Recast" "recast/Recast" "Release" "-DBUILD_SHARED_LIBS=OFF"

		::call :buildWithCMakeVs2022 "DetourCrowd" "recast/DetourCrowd" "Debug" "-DBUILD_SHARED_LIBS=OFF"
		::call :buildWithCMakeVs2022 "DetourCrowd" "recast/DetourCrowd" "Release" "-DBUILD_SHARED_LIBS=OFF"

		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Debug" "-DBUILD_SHARED_LIBS=OFF"
		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Release" "-DBUILD_SHARED_LIBS=OFF"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeVs2022 "Detour" "recast/Detour" "Debug" "-DBUILD_SHARED_LIBS=ON"
		call :buildWithCMakeVs2022 "Detour" "recast/Detour" "Release" "-DBUILD_SHARED_LIBS=ON"

		call :buildWithCMakeVs2022 "Recast" "recast/Recast" "Debug" "-DBUILD_SHARED_LIBS=ON"
		call :buildWithCMakeVs2022 "Recast" "recast/Recast" "Release" "-DBUILD_SHARED_LIBS=ON"

		::call :buildWithCMakeVs2022 "DetourCrowd" "recast/DetourCrowd" "Debug" "-DBUILD_SHARED_LIBS=ON"
		::call :buildWithCMakeVs2022 "DetourCrowd" "recast/DetourCrowd" "Release" "-DBUILD_SHARED_LIBS=ON"

		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Debug" "-DBUILD_SHARED_LIBS=ON"
		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Release" "-DBUILD_SHARED_LIBS=ON"
	)
) else if "%compiler%"=="2" (
	:: MinGW
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeMinGW "Detour" "recast/Detour" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"
		call :buildWithCMakeMinGW "Detour" "recast/Detour" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"

		call :buildWithCMakeMinGW "Recast" "recast/Recast" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"
		call :buildWithCMakeMinGW "Recast" "recast/Recast" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"

		::call :buildWithCMakeMinGW "DetourCrowd" "recast/DetourCrowd" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"
		::call :buildWithCMakeMinGW "DetourCrowd" "recast/DetourCrowd" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"

		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"
		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeMinGW "Detour" "recast/Detour" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"
		call :buildWithCMakeMinGW "Detour" "recast/Detour" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"

		call :buildWithCMakeMinGW "Recast" "recast/Recast" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"
		call :buildWithCMakeMinGW "Recast" "recast/Recast" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"

		::call :buildWithCMakeMinGW "DetourCrowd" "recast/DetourCrowd" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"
		::call :buildWithCMakeMinGW "DetourCrowd" "recast/DetourCrowd" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"

		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"
		::call :buildWithCMakeMinGW "DetourTileCache" "recast/DetourTileCache" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON"
	)
)

:: ------------------------------------------------------------------------
:: Assimp
:: ------------------------------------------------------------------------
echo.
call :cloneGit "Assimp" "v6.0.2" "https://github.com/assimp/assimp.git" "v6.0.2" "assimp"
echo.

if "%modelformat%"=="1" (
	:: All formats (read and write)
	set "MODEL_FORMAT=-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=ON -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=ON"
) else if "%modelformat%"=="2" (
	:: GLTF only (read only)
	set "MODEL_FORMAT=-DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_GLTF_IMPORTER=ON -DASSIMP_BUILD_GLTF_EXPORTER=OFF -DASSIMP_NO_EXPORT=ON -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_SAMPLES=OFF -DASSIMP_BUILD_FBX_IMPORTER=OFF -DASSIMP_BUILD_OBJ_IMPORTER=OFF -DASSIMP_BUILD_COLLADA_IMPORTER=OFF"
)

if "%compiler%"=="1" (
	:: VS 2022
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeVs2022 "Assimp" "assimp" "Debug" "-DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
		call :buildWithCMakeVs2022 "Assimp" "assimp" "Release" "-DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeVs2022 "Assimp" "assimp" "Debug" "-DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
		call :buildWithCMakeVs2022 "Assimp" "assimp" "Release" "-DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
	)
) else if "%compiler%"=="2" (
	:: MinGW
	if "%linking%"=="1" (
		:: Static build
		call :buildWithCMakeMinGW "assimp" "assimp" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
		call :buildWithCMakeMinGW "assimp" "assimp" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DCMAKE_CXX_FLAGS_RELEASE=-Wno-array-bounds;-Wno-alloc-size-larger-than;-Wno-error=array-compare -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :buildWithCMakeMinGW "assimp" "assimp" "Debug" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
		call :buildWithCMakeMinGW "assimp" "assimp" "Release" "-DCMAKE_C_COMPILER=%GCC_PATH% -DCMAKE_CXX_COMPILER=%GPP_PATH% -DCMAKE_CXX_FLAGS_RELEASE=-Wno-array-bounds;-Wno-alloc-size-larger-than;-Wno-error=array-compare -DBUILD_SHARED_LIBS=ON -DASSIMP_BUILD_TESTS=OFF %MODEL_FORMAT%"
	)
)

:: ------------------------------------------------------------------------
:: STB
:: ------------------------------------------------------------------------
echo.
call :cloneGit "STB" "Rev. f58f558" "https://github.com/nothings/stb.git" "f58f558c120e9b32c217290b80bad1a0729fbb2c" "stb"

:: ------------------------------------------------------------------------
:: nlohmann JSON
:: ------------------------------------------------------------------------
echo.
call :cloneGit "nlohmann JSON" "v3.12.0" "https://github.com/nlohmann/json.git" "v3.12.0" "nlohmann"

:: ------------------------------------------------------------------------
:: JoltPhysics
:: ------------------------------------------------------------------------
echo.
call :downloadAndExtractZip "JoltPhysics" "v5.3.0" "https://github.com/jrouwe/JoltPhysics/archive/refs/tags/v5.3.0.zip" "jolt" "true"
echo.

if "%compiler%"=="1" (
	:: VS 2022
	if "%linking%"=="1" (
		:: Static build
		call :callExternalScript "jolt\Build" "cmake_vs2022_cl.bat"
		call :buildWithVs2022 "JoltPhysics" "jolt\Build\VS2022_CL" "JoltPhysics.sln" "Debug" "-DBUILD_SHARED_LIBS=OFF"
		call :buildWithVs2022 "JoltPhysics" "jolt\Build\VS2022_CL" "JoltPhysics.sln" "Release" "-DBUILD_SHARED_LIBS=OFF"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :callExternalScript "jolt\Build" "cmake_vs2022_cl.bat"
		call :buildWithVs2022 "JoltPhysics" "jolt\Build\VS2022_CL" "JoltPhysics.sln" "Debug" "-DBUILD_SHARED_LIBS=ON"
		call :buildWithVs2022 "JoltPhysics" "jolt\Build\VS2022_CL" "JoltPhysics.sln" "Release" "-DBUILD_SHARED_LIBS=ON"
	)
) else if "%compiler%"=="2" (
	:: MinGW
	if "%linking%"=="1" (
		call :callExternalScript "jolt\Build" "cmake_windows_mingw.sh" "Debug"
		call :callExternalScript "jolt\Build" "cmake_windows_mingw.sh" "Release"
		
		call :buildWithCMakeMinGW "MinGW_Debug" "jolt\Build" "Debug" "-DBUILD_SHARED_LIBS=OFF"
		call :buildWithCMakeMinGW "MinGW_Release" "jolt\Build" "Release" "-DBUILD_SHARED_LIBS=OFF"
	) else if "%linking%"=="2" (
		:: Dynamic build
		call :callExternalScript "jolt\Build" "cmake_windows_mingw.sh" "Debug"
		call :callExternalScript "jolt\Build" "cmake_windows_mingw.sh" "Release"
		
		call :buildWithCMakeMinGW "MinGW_Debug" "jolt\Build" "Debug" "-DBUILD_SHARED_LIBS=ON"
		call :buildWithCMakeMinGW "MinGW_Release" "jolt\Build" "Release" "-DBUILD_SHARED_LIBS=ON"
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
echo All dependencies have been downloaded and compiled successfully.
echo ------------------------------------------------------------------------
pause
goto :eof

:: ------------------------------------------------------------------------
:: Failed
:: ------------------------------------------------------------------------
:build_failed
echo.
echo ------------------------------------------------------------------------
echo Failed to download or compile dependencies, please try again.
echo ------------------------------------------------------------------------
pause
goto :eof

:: ------------------------------------------------------------------------
:: BELOW ARE THE FUNCTIONS
:: ------------------------------------------------------------------------

:: ------------------------------------------------------------------------
:: Download and extract zip
:: ------------------------------------------------------------------------
:downloadAndExtractZip
:: %1 = NAME, %2 = VERSION, %3 = URL, %4 = DESTFOLDER, %5 = FLATTEN (true/false)

set "NAME=%~1"
set "VERSION=%~2"
set "URL=%~3"
set "DESTFOLDER=%~4"
set "FLATTEN=%~5"
set "ZIPFILE=%TEMP_FOLDER%\%DESTFOLDER%_temp.zip"

echo === Downloading %NAME% (%VERSION%) ===

if exist "%DESTFOLDER%" (
    echo Folder "%DESTFOLDER%" already exists. Skipping.
    goto :eof
)

if exist "%TEMP_FOLDER%" rmdir /s /q "%TEMP_FOLDER%"
mkdir "%TEMP_FOLDER%"

:: Download
echo Downloading ZIP...
powershell -Command "Invoke-WebRequest -Uri '%URL%' -OutFile '%ZIPFILE%'"
if errorlevel 1 (
    echo ERROR: Failed to download %NAME%.
    goto :build_failed
)

:: Extract
echo Extracting...
powershell -Command "Expand-Archive -Path '%ZIPFILE%' -DestinationPath '%TEMP_FOLDER%' -Force"
if errorlevel 1 (
    echo ERROR: Failed to extract %NAME%.
    goto :build_failed
)

del "%ZIPFILE%"

:: Create destination folder
mkdir "%DESTFOLDER%"

:: Flatten = true → move all files/subfolders from inside extracted top-level folder(s)
if /i "%FLATTEN%"=="true" (
    echo Flattening files to %DESTFOLDER%...
    for /d %%D in ("%TEMP_FOLDER%\*") do (
        xcopy "%%D\*" "%DESTFOLDER%\" /E /I /H /Y >nul
    )
) else (
    :: Non-flatten: Move top-level extracted folder (assumes only one subfolder)
    for /d %%D in ("%TEMP_FOLDER%\*") do (
        echo Moving folder %%~nxD to %DESTFOLDER%...
        move "%%D" "%DESTFOLDER%" >nul
    )
)

:: Clean up
rmdir /s /q "%TEMP_FOLDER%"
echo %NAME% setup complete.
goto :eof

:: ------------------------------------------------------------------------
:: Clone Git
:: ------------------------------------------------------------------------
:cloneGit
:: Args: %1 = name, %2 = version (for display), %3 = repo URL, %4 = dest folder name, %5 = revision/commit/tag (optional)
set "NAME=%~1"
set "VERSION=%~2"
set "REPO_URL=%~3"
set "REVISION=%~4"
set "DESTFOLDER=%~5"
set "TEMP_CLONE=%TEMP_FOLDER%"

echo === Git Clone %NAME% (%VERSION%) ===

if exist "%DESTFOLDER%" (
    echo Folder "%DESTFOLDER%" already exists. Skipping %NAME% clone.
    goto :eof
)

:: Clean up TEMP_CLONE if it exists
if exist "%TEMP_CLONE%" rmdir /s /q "%TEMP_CLONE%"

echo Cloning %NAME% repository...
git clone "%REPO_URL%" "%TEMP_CLONE%"
if errorlevel 1 (
    echo ERROR: Failed to clone %NAME% repo.
    goto :build_failed
)

pushd "%TEMP_CLONE%"

:: Checkout specific revision if provided
if not "%REVISION%"=="" (
    echo Checking out revision: %REVISION%
    git checkout %REVISION%
    if errorlevel 1 (
        echo ERROR: Failed to checkout revision '%REVISION%'.
        popd
        goto :build_failed
    )
)

echo Initializing submodules...
git submodule update --init --recursive
popd

:: Rename/move folder to desired destination
if exist "%DESTFOLDER%" (
    echo ERROR: Destination folder "%DESTFOLDER%" already exists.
    rmdir /s /q "%TEMP_CLONE%"
    goto :build_failed
)

rename "%TEMP_CLONE%" "%DESTFOLDER%"
echo %NAME% setup complete in folder "%DESTFOLDER%".
goto :eof

:: ------------------------------------------------------------------------
:: Build With CMake (Visual Studio 2022)
:: ------------------------------------------------------------------------
:buildWithCMakeVs2022
set "NAME=%~1"
set "BUILD_DIR=%~2"
set "BUILD_MODE=%~3"
set "ARGS=%~4"

echo === Building %NAME% (%BUILD_MODE%) ===

if "%BUILD_DIR%"=="" (
    echo [ERROR] You must provide the path to the source folder.
    goto :build_failed
)

if not exist "%BUILD_DIR%\CMakeLists.txt" (
    echo [ERROR] CMakeLists.txt not found in '%BUILD_DIR%'
    goto :build_failed
)

pushd "%BUILD_DIR%"

echo [INFO] Running CMake configuration...
cmake -G "Visual Studio 17 2022" -B . %ARGS%
if errorlevel 1 (
    echo [ERROR] cmake configuration failed.
    goto :build_failed
)

echo [INFO] Building %NAME%...
cmake --build . --config %BUILD_MODE%
if errorlevel 1 (
    echo [ERROR] Build failed.
    goto :build_failed
)

echo [SUCCESS] %NAME% built successfully.
popd
goto :eof

:: ------------------------------------------------------------------------
:: Build With CMake (MinGW)
:: ------------------------------------------------------------------------
:buildWithCMakeMinGW
set "NAME=%~1"
set "BUILD_DIR=%~2"
set "BUILD_MODE=%~3"
set "ARGS=%~4"

echo === Building %NAME% (%BUILD_MODE%) ===

if "%BUILD_DIR%"=="" (
    echo [ERROR] You must provide the path to the source folder.
    goto :build_failed
)

if not exist "%BUILD_DIR%\CMakeLists.txt" (
    echo [ERROR] CMakeLists.txt not found in '%BUILD_DIR%'
    goto :build_failed
)

pushd "%BUILD_DIR%"

echo [INFO] Running CMake configuration...
cmake -G "MinGW Makefiles" -B . -DCMAKE_BUILD_TYPE=%BUILD_MODE% %ARGS%
if errorlevel 1 (
    echo [ERROR] CMake configuration failed.
    goto :build_failed
)

echo [INFO] Building %NAME%...
cmake --build .
if errorlevel 1 (
    echo [ERROR] Build failed.
    goto :build_failed
)

echo [SUCCESS] %NAME% built successfully.
popd
goto :eof

:: ------------------------------------------------------------------------
:: Build With Visual Studio 2022 (Community Edition)
:: ------------------------------------------------------------------------
:buildWithVs2022
set "NAME=%~1"
set "BUILD_DIR=%~2"
set "BUILD_FILE=%~3"
set "BUILD_MODE=%~4"
set "ARGS=%~5"

pushd "%BUILD_DIR%"
echo === Building %SCRIPT% ===

:: === Set path to Visual Studio Developer Command Prompt ===
call "%VS2022_PATH%"
if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    goto :build_failed
)

:: === Set your .sln file path ===
set "SLN_PATH=%BUILD_FILE%"

:: === Set build configuration and platform ===
set "CONFIG=%BUILD_MODE%"
set "PLATFORM=x64"

:: === Build using MSBuild ===
echo [INFO] Building Solution: %BUILD_FILE% ===
MSBuild "%SLN_PATH%" /p:Configuration=%CONFIG%;Platform=%PLATFORM%
if errorlevel 1 (
    echo [ERROR] Build failed.
    goto :build_failed
)

echo [SUCCESS] Build completed.
popd
goto :eof

:: ------------------------------------------------------------------------
:: Call external script
:: ------------------------------------------------------------------------
:callExternalScript
set "SCRIPT_PATH=%~1"
set "SCRIPT=%~2"
set "ARGS=%~3"

pushd "%SCRIPT_PATH%"
echo === Running %SCRIPT% ===

call %SCRIPT% %ARGS%
if errorlevel 1 (
    echo [ERROR] '%SCRIPT_PATH%\%SCRIPT%' failed.
    goto :build_failed
)

echo [SUCCESS] %SCRIPT% called successfully.
popd
goto :eof