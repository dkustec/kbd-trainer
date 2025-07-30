@echo off
title KBD Trainer - Setup Assistant
echo.
echo ============================================
echo     KBD Trainer - Setup Assistant
echo ============================================
echo.

:menu
echo Select an option:
echo [1] Run KBD Trainer
echo [2] Create Desktop Shortcut
echo [3] Show System Info
echo [4] Open Installation Guide
echo [5] Exit
echo.
set /p choice="Enter your choice (1-5): "

if "%choice%"=="1" goto run_game
if "%choice%"=="2" goto create_shortcut
if "%choice%"=="3" goto system_info
if "%choice%"=="4" goto open_guide
if "%choice%"=="5" goto exit
echo Invalid choice. Please try again.
echo.
goto menu

:run_game
echo.
echo Starting KBD Trainer...
if exist "KBDTrainer.exe" (
    start "" "KBDTrainer.exe"
    echo Game started successfully!
) else (
    echo ERROR: KBDTrainer.exe not found!
    echo Make sure this batch file is in the same folder as the game.
)
echo.
pause
goto menu

:create_shortcut
echo.
echo Creating desktop shortcut...
set "TARGET=%~dp0KBDTrainer.exe"
set "SHORTCUT=%USERPROFILE%\Desktop\KBD Trainer.lnk"

if exist "%TARGET%" (
    powershell "$s=(New-Object -COM WScript.Shell).CreateShortcut('%SHORTCUT%'); $s.TargetPath='%TARGET%'; $s.WorkingDirectory='%~dp0'; $s.Description='Korean Backdash Training Tool for Tekken'; $s.Save()"
    echo Desktop shortcut created successfully!
) else (
    echo ERROR: KBDTrainer.exe not found!
)
echo.
pause
goto menu

:system_info
echo.
echo ============================================
echo              System Information
echo ============================================
echo OS Version: 
systeminfo | findstr /B /C:"OS Name" /C:"OS Version"
echo.
echo Architecture:
systeminfo | findstr /B /C:"System Type"
echo.
echo Memory:
systeminfo | findstr /B /C:"Total Physical Memory"
echo.
echo Graphics:
wmic path win32_VideoController get name
echo.
echo Controllers:
powershell "Get-WmiObject Win32_PointingDevice | Select-Object Name"
echo.
pause
goto menu

:open_guide
echo.
echo Opening installation guide...
if exist "INSTALL.md" (
    start "" "INSTALL.md"
) else if exist "README.md" (
    start "" "README.md"
) else (
    echo No documentation found. Please check the GitHub repository:
    echo https://github.com/dkustec/kbd-trainer
)
echo.
pause
goto menu

:exit
echo.
echo Thanks for using KBD Trainer!
echo Train hard, play harder! 🥊
echo.
pause
exit
