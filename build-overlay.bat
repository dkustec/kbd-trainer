@echo off
echo Building KBD Trainer Overlay...
echo.

cd /d "%~dp0"

if not exist "build-overlay" mkdir build-overlay
cd build-overlay

echo Configuring CMake for overlay build...
cmake -DBUILD_OVERLAY=ON -DBUILD_STANDALONE=OFF ..
if errorlevel 1 (
    echo CMake configuration failed!
    pause
    exit /b 1
)

echo.
echo Building overlay components...
cmake --build . --config Release
if errorlevel 1 (
    echo Build failed!
    pause
    exit /b 1
)

echo.
echo Build completed successfully!
echo.
echo Files created:
echo - bin\KBDTrainerOverlay.dll (Overlay library)
echo - bin\KBDTrainerInjector.exe (Injection tool)
echo.
echo Usage:
echo 1. Start Tekken 7/8 or other supported fighting game
echo 2. Run bin\KBDTrainerInjector.exe as Administrator
echo 3. Press F1 in-game to toggle overlay
echo.
pause
