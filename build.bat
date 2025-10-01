@echo off
echo ===================================
echo Building DFR Game
echo ===================================

REM Check if build directory exists
if not exist "build" (
    echo ERROR: Build directory not found. Please run setup.bat first.
    pause
    exit /b 1
)

cd build

REM Build the project
echo Building Release configuration...
cmake --build . --config Release -- /m

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo ===================================
echo Build successful!
echo.
echo Executable location:
echo   build\bin\Release\AnimeArenaFighter.exe
echo.
echo To run the game:
echo   cd build\bin\Release
echo   AnimeArenaFighter.exe
echo ===================================
cd ..
pause