@echo off
echo ===================================
echo Building DFR Game (Debug)
echo ===================================

REM Check if build directory exists
if not exist "build" (
    echo ERROR: Build directory not found. Please run setup.bat first.
    pause
    exit /b 1
)

cd build

REM Build the project
echo Building Debug configuration...
cmake --build . --config Debug -- /m

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
echo   build\bin\Debug\AnimeArenaFighter.exe
echo.
echo To debug the game:
echo   1. Open build\AnimeArenaFighter.sln in Visual Studio
echo   2. Set AnimeArenaFighter as startup project
echo   3. Press F5 to start debugging
echo ===================================
cd ..
pause