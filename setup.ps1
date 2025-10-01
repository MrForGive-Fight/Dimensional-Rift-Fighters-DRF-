# DFR Game Setup Script (PowerShell)
Write-Host "===================================" -ForegroundColor Cyan
Write-Host "DFR Game Setup Script" -ForegroundColor Cyan
Write-Host "===================================" -ForegroundColor Cyan

# Function to check if a command exists
function Test-CommandExists {
    param($command)
    $null = Get-Command $command -ErrorAction SilentlyContinue
    return $?
}

# 1. Clone the Lost Saga reference
Write-Host "`n[1/5] Cloning LSFDC reference repository..." -ForegroundColor Yellow
if (!(Test-Path "reference\LSFDC")) {
    if (Test-CommandExists git) {
        git clone https://github.com/LSFDC/lostsaga-reference.git reference/LSFDC
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: Failed to clone LSFDC reference repository" -ForegroundColor Red
            Write-Host "Please check your internet connection and git installation" -ForegroundColor Red
            Read-Host "Press Enter to continue"
            exit 1
        }
    } else {
        Write-Host "ERROR: Git is not installed" -ForegroundColor Red
        Write-Host "Please install Git from: https://git-scm.com/download/win" -ForegroundColor Yellow
        Read-Host "Press Enter to continue"
        exit 1
    }
} else {
    Write-Host "LSFDC reference already exists, skipping..." -ForegroundColor Green
}

# 2. Check for Visual Studio 2022
Write-Host "`n[2/5] Checking for Visual Studio 2022..." -ForegroundColor Yellow
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vsWhere) {
    $vsPath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
    if ($vsPath) {
        Write-Host "Visual Studio found at: $vsPath" -ForegroundColor Green
    } else {
        Write-Host "WARNING: Visual Studio 2022 C++ tools not found" -ForegroundColor Red
        Write-Host "Please install Visual Studio 2022 with C++ workload from:" -ForegroundColor Yellow
        Write-Host "https://visualstudio.microsoft.com/downloads/" -ForegroundColor Yellow
    }
} else {
    Write-Host "WARNING: Visual Studio not found" -ForegroundColor Red
    Write-Host "Please install Visual Studio 2022 with C++ workload from:" -ForegroundColor Yellow
    Write-Host "https://visualstudio.microsoft.com/downloads/" -ForegroundColor Yellow
}

# 3. Check for DirectX SDK
Write-Host "`n[3/5] Checking for DirectX SDK June 2010..." -ForegroundColor Yellow
if ($env:DXSDK_DIR) {
    Write-Host "DirectX SDK found at: $env:DXSDK_DIR" -ForegroundColor Green
} else {
    Write-Host "WARNING: DirectX SDK June 2010 not found" -ForegroundColor Red
    Write-Host "Please download and install from:" -ForegroundColor Yellow
    Write-Host "https://www.microsoft.com/en-us/download/details.aspx?id=6812" -ForegroundColor Yellow
}

# 4. Setup Boost 1.50
Write-Host "`n[4/5] Setting up Boost 1.50..." -ForegroundColor Yellow
if (!(Test-Path "third_party\boost_1_50_0")) {
    New-Item -ItemType Directory -Path "third_party" -Force | Out-Null
    
    Write-Host "Downloading Boost 1.50..." -ForegroundColor Cyan
    $boostUrl = "https://sourceforge.net/projects/boost/files/boost/1.50.0/boost_1_50_0.zip/download"
    $boostZip = "boost_1_50_0.zip"
    
    try {
        Invoke-WebRequest -Uri $boostUrl -OutFile $boostZip -UserAgent "Mozilla/5.0"
        
        Write-Host "Extracting Boost..." -ForegroundColor Cyan
        Expand-Archive -Path $boostZip -DestinationPath "third_party" -Force
        Remove-Item $boostZip
        
        Write-Host "Building Boost..." -ForegroundColor Cyan
        Push-Location "third_party\boost_1_50_0"
        & .\bootstrap.bat
        & .\b2.exe --build-type=complete --with-system --with-thread --with-date_time --with-regex --with-serialization stage
        Pop-Location
    } catch {
        Write-Host "ERROR: Failed to download or extract Boost" -ForegroundColor Red
        Write-Host $_.Exception.Message -ForegroundColor Red
        Read-Host "Press Enter to continue"
        exit 1
    }
} else {
    Write-Host "Boost 1.50 already exists, skipping..." -ForegroundColor Green
}

# 5. Check for CMake
Write-Host "`n[5/5] Checking for CMake..." -ForegroundColor Yellow
if (Test-CommandExists cmake) {
    Write-Host "CMake found" -ForegroundColor Green
    
    # Generate project files
    Write-Host "`nGenerating project files..." -ForegroundColor Yellow
    if (!(Test-Path "build")) {
        New-Item -ItemType Directory -Path "build" | Out-Null
    }
    
    Push-Location "build"
    cmake -G "Visual Studio 17 2022" -A x64 ..
    if ($LASTEXITCODE -ne 0) {
        Write-Host "ERROR: CMake generation failed" -ForegroundColor Red
        Pop-Location
        Read-Host "Press Enter to continue"
        exit 1
    }
    Pop-Location
} else {
    Write-Host "ERROR: CMake is not installed" -ForegroundColor Red
    Write-Host "Please install CMake from: https://cmake.org/download/" -ForegroundColor Yellow
    Read-Host "Press Enter to continue"
    exit 1
}

Write-Host "`n===================================" -ForegroundColor Cyan
Write-Host "Setup complete!" -ForegroundColor Green
Write-Host "`nTo build the project:" -ForegroundColor Yellow
Write-Host "  cd build" -ForegroundColor White
Write-Host "  cmake --build . --config Release" -ForegroundColor White
Write-Host "`nTo run the game:" -ForegroundColor Yellow
Write-Host "  .\build\Release\AnimeArenaFighter.exe" -ForegroundColor White
Write-Host "===================================" -ForegroundColor Cyan
Read-Host "`nPress Enter to exit"