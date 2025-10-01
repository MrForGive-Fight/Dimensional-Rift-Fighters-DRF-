# DFR Build Instructions

## Prerequisites

### Required Software
1. **Visual Studio 2022** (or later)
   - With C++ desktop development workload
   - Windows 10 SDK (10.0.19041.0 or later)
   
2. **CMake 3.20+**
   - Download from https://cmake.org/download/
   - Add to PATH during installation

3. **DirectX SDK**
   - Usually included with Windows SDK
   - DirectX 11 runtime required

## Build Steps

### 1. Generate Project Files

```bash
# Create build directory
mkdir build
cd build

# Generate Visual Studio solution
cmake .. -G "Visual Studio 17 2022" -A x64
```

### 2. Build the Project

#### Option A: Using Visual Studio
1. Open `DFR.sln` in the build directory
2. Set `DFRGame` as startup project
3. Select `Release` configuration
4. Press F5 to build and run

#### Option B: Using Command Line
```bash
# From build directory
cmake --build . --config Release
```

### 3. Run the Game

```bash
# From build directory
cd bin/Release
DFRGame.exe
```

## Project Structure

```
DFR/
├── CMakeLists.txt          # Main CMake configuration
├── src/
│   ├── CMakeLists.txt      # Source CMake file
│   └── Application/
│       └── WinMain.cpp     # Entry point
├── build/                  # Generated build files (git ignored)
└── bin/                    # Output executables
```

## CMake Configuration

### Root CMakeLists.txt
```cmake
cmake_minimum_required(VERSION 3.20)
project(DFR VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find DirectX
find_package(DirectX REQUIRED)

# Add source directory
add_subdirectory(src)
```

### src/CMakeLists.txt
```cmake
# Collect all source files
file(GLOB_RECURSE DFR_SOURCES *.cpp *.h)

# Create executable
add_executable(DFRGame WIN32 ${DFR_SOURCES})

# Include directories
target_include_directories(DFRGame PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DirectX_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(DFRGame PRIVATE
    d3d11
    dxgi
    dxguid
    d3dcompiler
)

# Set output directory
set_target_properties(DFRGame PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
```

## Troubleshooting

### Error: "Cannot find DirectX"
- Ensure Windows SDK is installed
- Add DirectX include path manually:
  ```cmake
  set(DirectX_INCLUDE_DIRS "C:/Program Files (x86)/Windows Kits/10/Include/10.0.19041.0/um")
  ```

### Error: "Unresolved external symbol"
- Check all required libraries are linked
- Verify `#pragma comment(lib, "...")` in code

### Performance Issues
- Build in Release mode, not Debug
- Enable optimizations: `/O2` flag
- Disable iterator debugging in Release

### Build Warnings
- Treat warnings as errors: `/WX`
- Warning level 4: `/W4`
- Disable specific warnings if needed:
  ```cmake
  target_compile_options(DFRGame PRIVATE /wd4996)
  ```

## Debug Configuration

For debugging:
```cmake
# Enable debug symbols in Release
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG")
```

## Running Tests

```bash
# Build tests
cmake --build . --target DFRTests --config Debug

# Run tests
cd bin/Debug
DFRTests.exe
```

## Packaging

To create a distributable package:

```bash
# Install required files
cmake --install . --prefix ../package

# Package structure
package/
├── DFRGame.exe
├── assets/           # Game assets
└── redist/          # VC++ redistributables
```

## Common Issues

1. **Missing DLLs**: Include VC++ redistributables
2. **Black screen**: Check DirectX feature level
3. **Crash on startup**: Verify working directory for assets
4. **Input lag**: Ensure Release build configuration

## Development Tips

1. Use **precompiled headers** for faster builds
2. Enable **incremental linking** during development
3. Use **parallel compilation** `/MP` flag
4. Consider **unity builds** for release

## Game Controls

Once built and running:
- **Movement**: Arrow Keys or WASD
- **Special Moves**: S + Direction (Uses MANA only)
- **Gear Skills**: AS, AD, SD, ASD (Have cooldowns)
- **Block**: Hold S for 1 second
- **Stance Switch**: S + Down

## Contact

For build issues, check:
- `docs/` folder for documentation
- `CLAUDE.md` for project conventions
- `DFR_PROJECT_SUMMARY.md` for system overview