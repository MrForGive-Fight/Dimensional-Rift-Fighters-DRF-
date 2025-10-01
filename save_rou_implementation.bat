@echo off
echo Creating backup of Rou implementation...

REM Create backup directory with timestamp
set BACKUP_DIR=DFR_Rou_Backup_%date:~-4,4%%date:~-10,2%%date:~-7,2%_%time:~0,2%%time:~3,2%%time:~6,2%
set BACKUP_DIR=%BACKUP_DIR: =0%
mkdir "%BACKUP_DIR%" 2>nul

echo Backup directory: %BACKUP_DIR%

REM Create directory structure
mkdir "%BACKUP_DIR%\src\Heroes\Rou\States" 2>nul
mkdir "%BACKUP_DIR%\src\Heroes\Rou\Visuals" 2>nul
mkdir "%BACKUP_DIR%\src\Heroes\Rou\Tests" 2>nul
mkdir "%BACKUP_DIR%\project_files" 2>nul
mkdir "%BACKUP_DIR%\documentation" 2>nul

REM Copy Rou implementation files
echo Copying Rou implementation files...
copy "src\Heroes\Rou\Rou.h" "%BACKUP_DIR%\src\Heroes\Rou\" >nul
copy "src\Heroes\Rou\Rou.cpp" "%BACKUP_DIR%\src\Heroes\Rou\" >nul
copy "src\Heroes\Rou\States\EvolutionStateMachine.h" "%BACKUP_DIR%\src\Heroes\Rou\States\" >nul
copy "src\Heroes\Rou\States\EvolutionStateMachine.cpp" "%BACKUP_DIR%\src\Heroes\Rou\States\" >nul
copy "src\Heroes\Rou\States\FormStates.h" "%BACKUP_DIR%\src\Heroes\Rou\States\" >nul
copy "src\Heroes\Rou\Visuals\EvolutionVFX.h" "%BACKUP_DIR%\src\Heroes\Rou\Visuals\" >nul
copy "src\Heroes\Rou\Visuals\EvolutionVFX.cpp" "%BACKUP_DIR%\src\Heroes\Rou\Visuals\" >nul
copy "src\Heroes\Rou\Tests\RouTests.cpp" "%BACKUP_DIR%\src\Heroes\Rou\Tests\" >nul

REM Copy test program
echo Copying test program...
copy "game-project\src\test_rou_main.cpp" "%BACKUP_DIR%\" >nul

REM Copy Visual Studio project files
echo Copying Visual Studio project files...
copy "DFR\DFR.vcxproj" "%BACKUP_DIR%\project_files\" >nul
copy "DFR\DFR.vcxproj.filters" "%BACKUP_DIR%\project_files\" >nul

REM Create file listing
echo Creating file listing...
dir /s /b "%BACKUP_DIR%" > "%BACKUP_DIR%\file_listing.txt"

echo.
echo Backup complete! Files saved to: %BACKUP_DIR%
echo.
echo Files backed up:
echo - Rou.h / Rou.cpp (Main character implementation)
echo - EvolutionStateMachine.h/cpp (Form management)
echo - FormStates.h (Form state definitions)
echo - EvolutionVFX.h/cpp (Visual effects)
echo - RouTests.cpp (Unit tests)
echo - test_rou_main.cpp (Test program)
echo - DFR.vcxproj (Visual Studio project)
echo - DFR.vcxproj.filters (VS folder structure)
echo.
pause