@echo off
echo ===================================
echo Running AnimeArenaFighter
echo ===================================

REM Check if executable exists
if not exist "build\bin\Release\AnimeArenaFighter.exe" (
    echo ERROR: AnimeArenaFighter.exe not found!
    echo Please run build.bat first.
    pause
    exit /b 1
)

REM Change to executable directory
cd build\bin\Release

REM Run the game
echo Starting game...
AnimeArenaFighter.exe

REM Return to root directory
cd ..\..\..

pause