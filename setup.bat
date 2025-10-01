@echo off
echo ===================================
echo DFR Game Setup Script
echo ===================================

REM 1. Clone the Lost Saga reference
echo.
echo [1/5] Cloning LSFDC reference repository...
if not exist "reference\LSFDC" (
    git clone https://github.com/LSFDC/lostsaga-reference.git reference/LSFDC
    if errorlevel 1 (
        echo ERROR: Failed to clone LSFDC reference repository
        echo Please check your internet connection and git installation
        pause
        exit /b 1
    )
) else (
    echo LSFDC reference already exists, skipping...
)

REM 2. Check for Visual Studio 2022
echo.
echo [2/5] Checking for Visual Studio 2022...
where /q devenv.exe
if errorlevel 1 (
    echo WARNING: Visual Studio not found in PATH
    echo Please install Visual Studio 2022 with C++ workload from:
    echo https://visualstudio.microsoft.com/downloads/
) else (
    echo Visual Studio found
)

REM 3. Check for DirectX SDK
echo.
echo [3/5] Checking for DirectX SDK June 2010...
if not exist "%DXSDK_DIR%" (
    echo WARNING: DirectX SDK June 2010 not found
    echo Please download and install from:
    echo https://www.microsoft.com/en-us/download/details.aspx?id=6812
) else (
    echo DirectX SDK found at: %DXSDK_DIR%
)

REM 4. Setup Boost 1.50
echo.
echo [4/5] Setting up Boost 1.50...
if not exist "third_party\boost_1_50_0" (
    echo Downloading Boost 1.50...
    powershell -Command "& {Invoke-WebRequest -Uri 'https://sourceforge.net/projects/boost/files/boost/1.50.0/boost_1_50_0.zip/download' -OutFile 'boost_1_50_0.zip'}"
    
    echo Extracting Boost...
    powershell -Command "& {Expand-Archive -Path 'boost_1_50_0.zip' -DestinationPath 'third_party' -Force}"
    del boost_1_50_0.zip
    
    echo Building Boost...
    cd third_party\boost_1_50_0
    call bootstrap.bat
    b2.exe --build-type=complete --with-system --with-thread --with-date_time --with-regex --with-serialization stage
    cd ..\..
) else (
    echo Boost 1.50 already exists, skipping...
)

REM 5. Generate project files
echo.
echo [5/5] Generating project files...
if not exist "build" mkdir build
cd build

cmake -G "Visual Studio 17 2022" -A x64 ..
if errorlevel 1 (
    echo ERROR: CMake generation failed
    echo Please ensure CMake is installed: https://cmake.org/download/
    cd ..
    pause
    exit /b 1
)

echo.
echo ===================================
echo Setup complete!
echo.
echo To build the project:
echo   cd build
echo   cmake --build . --config Release
echo.
echo To run the game:
echo   build\Release\AnimeArenaFighter.exe
echo ===================================
cd ..
pause