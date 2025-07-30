@echo off
echo Building KBD Trainer - Jets Brain Wire Helper
echo ============================================

REM Setup Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    echo Visual Studio 2022 not found, trying alternative paths...
    call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
)

REM Create build directory
if not exist "build" mkdir build
cd build

REM Set include and library paths (using parent SDL)
set INCLUDE_PATHS=/I"..\include" /I"..\..\SDL\include" /I"..\..\SDL_ttf\include" /I"..\..\SDL_image\include"
set LIB_PATHS=/LIBPATH:"..\..\build\SDL\Release" /LIBPATH:"..\..\build\SDL_ttf\Release" /LIBPATH:"..\..\build\SDL_image\Release"
set LIBS=SDL3.lib SDL3_ttf.lib SDL3_image.lib rpcrt4.lib

echo Compiling core modules...
cl /c /O2 %INCLUDE_PATHS% ..\src\core\*.c
if errorlevel 1 goto error

echo Compiling input modules...
cl /c /O2 %INCLUDE_PATHS% ..\src\input\*.c
if errorlevel 1 goto error

echo Compiling game modules...
cl /c /O2 %INCLUDE_PATHS% ..\src\game\*.c
if errorlevel 1 goto error

echo Linking executable...
link *.obj %LIB_PATHS% %LIBS% /OUT:KBDTrainer.exe /SUBSYSTEM:CONSOLE
if errorlevel 1 goto error

echo Copying required DLLs...
copy ..\..\bin\SDL3.dll . >nul
copy ..\..\bin\SDL3_ttf.dll . >nul
copy ..\..\bin\SDL3_image.dll . >nul

echo.
echo ================================================
echo Build successful! KBDTrainer.exe created in build/
echo ================================================
echo.
echo To run: cd build && KBDTrainer.exe
goto end

:error
echo.
echo ================================================
echo Build failed! Check error messages above.
echo ================================================
pause

:end
cd ..
