@echo off
REM ============================================================================
REM Qt Splash Screen - Project Manager
REM Build and deploy using CMakePresets.json
REM ============================================================================

setlocal EnableExtensions EnableDelayedExpansion

set "ROOT=%~dp0"
if "%ROOT:~-1%"=="\" set "ROOT=%ROOT:~0,-1%"

set "PROJECT_NAME=qt-splash-screen"
set "BUILD_DIR=%ROOT%\build"
set "DEPLOY_DIR=%ROOT%\deploy"
set "CMAKE_SCRIPT=%ROOT%\cmake\run_windeployqt.cmake"

cd /d "%ROOT%"

:main_menu
cls
echo.
echo ================================================
echo   Qt Splash Screen - Project Manager
echo ================================================
echo.
echo   [1] Build Library
echo   [2] Build Example
echo   [3] Deploy
echo   [4] Clean build folder
echo   [0] Exit
echo.
set /p "choice=Select option [0-4]: "

if "%choice%"=="1" goto build_menu
if "%choice%"=="2" goto build_example_menu
if "%choice%"=="3" goto deploy_menu
if "%choice%"=="4" goto clean_build
if "%choice%"=="0" goto exit_script

echo Invalid choice.
timeout /t 2 >nul
goto main_menu

:build_menu
cls
echo.
echo ================================================
echo   Build Library (CMake Presets)
echo ================================================
echo.
echo   Windows MinGW64:
echo   [1] Debug
echo   [2] Release
echo.
echo   Windows Clang:
echo   [3] Debug
echo   [4] Release
echo.
echo   Linux GCC (WSL):
echo   [5] Debug
echo   [6] Release
echo.
echo   Linux Clang (WSL):
echo   [7] Debug
echo   [8] Release
echo.
echo   [0] Back
echo.
set /p "build_choice=Select option [0-8]: "

if "%build_choice%"=="0" goto main_menu
if "%build_choice%"=="1" (set "PRESET=windows-mingw64-debug" & set "TARGET=%PROJECT_NAME%" & goto do_build_run)
if "%build_choice%"=="2" (set "PRESET=windows-mingw64-release" & set "TARGET=%PROJECT_NAME%" & goto do_build_run)
if "%build_choice%"=="3" (set "PRESET=windows-clang-debug" & set "TARGET=%PROJECT_NAME%" & goto do_build_run)
if "%build_choice%"=="4" (set "PRESET=windows-clang-release" & set "TARGET=%PROJECT_NAME%" & goto do_build_run)
if "%build_choice%"=="5" (set "PRESET=linux-gcc-debug" & set "TARGET=%PROJECT_NAME%" & goto do_build_wsl_run)
if "%build_choice%"=="6" (set "PRESET=linux-gcc-release" & set "TARGET=%PROJECT_NAME%" & goto do_build_wsl_run)
if "%build_choice%"=="7" (set "PRESET=linux-clang-debug" & set "TARGET=%PROJECT_NAME%" & goto do_build_wsl_run)
if "%build_choice%"=="8" (set "PRESET=linux-clang-release" & set "TARGET=%PROJECT_NAME%" & goto do_build_wsl_run)

if not "%build_choice%"=="1" if not "%build_choice%"=="2" if not "%build_choice%"=="3" if not "%build_choice%"=="4" if not "%build_choice%"=="5" if not "%build_choice%"=="6" if not "%build_choice%"=="7" if not "%build_choice%"=="8" (
    echo Invalid choice.
    timeout /t 2 >nul
    goto build_menu
)
goto main_menu

:build_example_menu
cls
echo.
echo ================================================
echo   Build Example (CMake Presets)
echo ================================================
echo.
echo   Windows MinGW64:
echo   [1] Debug
echo   [2] Release
echo.
echo   Windows Clang:
echo   [3] Debug
echo   [4] Release
echo.
echo   Linux GCC (WSL):
echo   [5] Debug
echo   [6] Release
echo.
echo   Linux Clang (WSL):
echo   [7] Debug
echo   [8] Release
echo.
echo   [0] Back
echo.
set /p "example_choice=Select option [0-8]: "

if "%example_choice%"=="0" goto main_menu
if "%example_choice%"=="1" (set "PRESET=windows-mingw64-debug" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_run)
if "%example_choice%"=="2" (set "PRESET=windows-mingw64-release" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_run)
if "%example_choice%"=="3" (set "PRESET=windows-clang-debug" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_run)
if "%example_choice%"=="4" (set "PRESET=windows-clang-release" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_run)
if "%example_choice%"=="5" (set "PRESET=linux-gcc-debug" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_wsl_run)
if "%example_choice%"=="6" (set "PRESET=linux-gcc-release" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_wsl_run)
if "%example_choice%"=="7" (set "PRESET=linux-clang-debug" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_wsl_run)
if "%example_choice%"=="8" (set "PRESET=linux-clang-release" & set "TARGET=%PROJECT_NAME%-example" & goto do_build_wsl_run)

if not "%example_choice%"=="1" if not "%example_choice%"=="2" if not "%example_choice%"=="3" if not "%example_choice%"=="4" if not "%example_choice%"=="5" if not "%example_choice%"=="6" if not "%example_choice%"=="7" if not "%example_choice%"=="8" (
    echo Invalid choice.
    timeout /t 2 >nul
    goto build_example_menu
)
goto main_menu

:deploy_menu
cls
echo.
echo ================================================
echo   Deploy (Windows / Linux)
echo ================================================
echo.
echo   Windows MinGW64:
echo   [1] Debug
echo   [2] Release
echo.
echo   Windows Clang:
echo   [3] Debug
echo   [4] Release
echo.
echo   Linux GCC (WSL):
echo   [5] Debug
echo   [6] Release
echo.
echo   Linux Clang (WSL):
echo   [7] Debug
echo   [8] Release
echo.
echo   [9] Deploy ALL Windows
echo   [10] Deploy ALL (Windows + Linux)
echo   [0] Back
echo.
set /p "deploy_choice=Select option [0-10]: "

if "%deploy_choice%"=="0" goto main_menu
if "%deploy_choice%"=="1" call :do_deploy_windows windows-mingw64-debug windows\debug
if "%deploy_choice%"=="2" call :do_deploy_windows windows-mingw64-release windows\release
if "%deploy_choice%"=="3" call :do_deploy_windows windows-clang-debug windows\debug
if "%deploy_choice%"=="4" call :do_deploy_windows windows-clang-release windows\release
if "%deploy_choice%"=="5" call :do_deploy_linux_wsl linux-gcc-debug linux/gcc/debug
if "%deploy_choice%"=="6" call :do_deploy_linux_wsl linux-gcc-release linux/gcc/release
if "%deploy_choice%"=="7" call :do_deploy_linux_wsl linux-clang-debug linux/clang/debug
if "%deploy_choice%"=="8" call :do_deploy_linux_wsl linux-clang-release linux/clang/release
if "%deploy_choice%"=="9" (
    call :do_deploy_windows windows-mingw64-debug windows\debug
    call :do_deploy_windows windows-mingw64-release windows\release
    call :do_deploy_windows windows-clang-debug windows\debug
    call :do_deploy_windows windows-clang-release windows\release
)
if "%deploy_choice%"=="10" (
    call :do_deploy_windows windows-mingw64-debug windows\debug
    call :do_deploy_windows windows-mingw64-release windows\release
    call :do_deploy_windows windows-clang-debug windows\debug
    call :do_deploy_windows windows-clang-release windows\release
    call :do_deploy_linux_wsl linux-gcc-debug linux/gcc/debug
    call :do_deploy_linux_wsl linux-gcc-release linux/gcc/release
    call :do_deploy_linux_wsl linux-clang-debug linux/clang/debug
    call :do_deploy_linux_wsl linux-clang-release linux/clang/release
)

if not "%deploy_choice%"=="1" if not "%deploy_choice%"=="2" if not "%deploy_choice%"=="3" if not "%deploy_choice%"=="4" if not "%deploy_choice%"=="5" if not "%deploy_choice%"=="6" if not "%deploy_choice%"=="7" if not "%deploy_choice%"=="8" if not "%deploy_choice%"=="9" if not "%deploy_choice%"=="10" (
    echo Invalid choice.
    timeout /t 2 >nul
    goto deploy_menu
)
goto main_menu

:clean_build
cls
echo.
echo Cleaning build directory: %BUILD_DIR%
echo.
if exist "%BUILD_DIR%" (
    rmdir /s /q "%BUILD_DIR%"
    echo Build folder removed.
) else (
    echo Build folder does not exist.
)
echo.
pause
goto main_menu

REM ============================================================
REM Build helpers
REM ============================================================
:do_build_run
if "%PRESET%"=="" goto main_menu
call :require_cmake
if errorlevel 1 (
    echo.
    pause
    goto main_menu
)
echo.
echo [INFO] Configuring preset: %PRESET%
cmake --preset %PRESET%
if errorlevel 1 (
    echo [ERROR] Configure failed for %PRESET%.
    echo.
    pause
    goto main_menu
)
echo.
echo [INFO] Building preset: %PRESET%
if "%TARGET%"=="" (
    cmake --build --preset %PRESET%
) else (
    cmake --build --preset %PRESET% --target %TARGET%
)
if errorlevel 1 (
    echo [ERROR] Build failed for %PRESET%.
    echo.
    pause
    goto main_menu
)
echo.
echo [OK] Build completed: %PRESET%
echo.
pause
goto main_menu

:do_build_wsl_run
if "%PRESET%"=="" goto main_menu
call :check_wsl
if errorlevel 1 (
    echo.
    pause
    goto main_menu
)
call :resolve_wsl_root
if errorlevel 1 (
    echo.
    pause
    goto main_menu
)
echo.
echo [INFO] WSL build preset: %PRESET%
echo.
if "%TARGET%"=="" (
    wsl bash -lc "cd '%WSL_ROOT%' && cmake --preset %PRESET% && cmake --build --preset %PRESET%"
) else (
    wsl bash -lc "cd '%WSL_ROOT%' && cmake --preset %PRESET% && cmake --build --preset %PRESET% --target %TARGET%"
)
if errorlevel 1 (
    echo [ERROR] WSL build failed for %PRESET%.
    echo.
    pause
    goto main_menu
)
echo.
echo [OK] WSL build completed: %PRESET%
echo.
pause
goto main_menu

:require_cmake
where cmake >nul 2>&1
if errorlevel 1 (
    echo [ERROR] cmake not found in PATH.
    echo         Install CMake or ensure it is available in PATH.
    exit /b 1
)
exit /b 0

REM ============================================================
REM Windows deploy helper
REM ============================================================
:do_deploy_windows
set "PRESET=%~1"
set "DEPLOY_SUBDIR=%~2"
if "%PRESET%"=="" goto :eof
if "%DEPLOY_SUBDIR%"=="" goto :eof

call :require_cmake
if errorlevel 1 goto :eof

set "BUILD_PRESET_DIR=%BUILD_DIR%\%PRESET%"
set "DEPLOY_TARGET=%DEPLOY_DIR%\%DEPLOY_SUBDIR%"
set "CACHE_FILE=%BUILD_PRESET_DIR%\CMakeCache.txt"

if not exist "%BUILD_PRESET_DIR%" (
    echo [ERROR] Build directory not found: %BUILD_PRESET_DIR%
    echo         Build the preset first.
    goto :eof
)

echo.
echo [INFO] Deploying preset: %PRESET%
echo [INFO] Deploy target: %DEPLOY_TARGET%

if exist "%DEPLOY_TARGET%" (
    rmdir /s /q "%DEPLOY_TARGET%"
)
mkdir "%DEPLOY_TARGET%"

set "BUILD_TYPE="
if exist "%CACHE_FILE%" (
    for /f "tokens=1,* delims==" %%A in ('findstr /b /c:"CMAKE_BUILD_TYPE:STRING=" "%CACHE_FILE%"') do set "BUILD_TYPE=%%B"
)

set "BUILD_TYPE_DIR="
if /i "%BUILD_TYPE%"=="Debug" set "BUILD_TYPE_DIR=debug"
if /i "%BUILD_TYPE%"=="Release" set "BUILD_TYPE_DIR=release"
if /i "%BUILD_TYPE%"=="RelWithDebInfo" set "BUILD_TYPE_DIR=relwithdebinfo"
if /i "%BUILD_TYPE%"=="MinSizeRel" set "BUILD_TYPE_DIR=minsizerel"
if "%BUILD_TYPE_DIR%"=="" (
    echo %PRESET% | findstr /i "debug" >nul && set "BUILD_TYPE_DIR=debug"
    if "%BUILD_TYPE_DIR%"=="" echo %PRESET% | findstr /i "release" >nul && set "BUILD_TYPE_DIR=release"
)
if "%BUILD_TYPE_DIR%"=="" set "BUILD_TYPE_DIR=release"

set "SOURCE_DIR=%BUILD_PRESET_DIR%"
if exist "%SOURCE_DIR%\%BUILD_TYPE_DIR%" set "SOURCE_DIR=%SOURCE_DIR%\%BUILD_TYPE_DIR%"
set "SOURCE_DLL=%SOURCE_DIR%\%PROJECT_NAME%.dll"
set "SOURCE_DLL_ALT=%SOURCE_DIR%\lib%PROJECT_NAME%.dll"
set "SOURCE_DLL_A=%SOURCE_DIR%\lib%PROJECT_NAME%.dll.a"

if exist "%SOURCE_DLL%" (
    copy /y "%SOURCE_DLL%" "%DEPLOY_TARGET%\" >nul 2>&1
) else if exist "%SOURCE_DLL_ALT%" (
    copy /y "%SOURCE_DLL_ALT%" "%DEPLOY_TARGET%\" >nul 2>&1
) else (
    echo [ERROR] DLL not found in %SOURCE_DIR%.
    echo         Build the library first.
    goto :eof
)

if exist "%SOURCE_DLL_A%" (
    copy /y "%SOURCE_DLL_A%" "%DEPLOY_TARGET%\" >nul 2>&1
)

echo.
echo [OK] Deploy completed: %DEPLOY_TARGET%
echo.
pause
goto :eof

:copy_mingw_runtime
set "CACHE_DIR=%~1"
set "TARGET_DIR=%~2"
set "CACHE_FILE=%CACHE_DIR%\CMakeCache.txt"
if not exist "%CACHE_FILE%" exit /b 0

set "CXX_COMPILER="
for /f "tokens=1,* delims==" %%A in ('findstr /b /c:"CMAKE_CXX_COMPILER:FILEPATH=" "%CACHE_FILE%"') do set "CXX_COMPILER=%%B"
if not defined CXX_COMPILER exit /b 0

for %%I in ("%CXX_COMPILER%") do set "COMPILER_BIN=%%~dpI"
if not defined COMPILER_BIN exit /b 0

set "RUNTIME_DLLS=libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll libssp-0.dll"
for %%D in (%RUNTIME_DLLS%) do (
    if exist "!COMPILER_BIN!%%D" (
        copy /y "!COMPILER_BIN!%%D" "%TARGET_DIR%\" >nul 2>&1
    )
)
exit /b 0

REM ============================================================
REM Linux (WSL) helpers
REM ============================================================
:check_wsl
where wsl >nul 2>&1
if errorlevel 1 (
    echo [ERROR] WSL not found. Install WSL to build/deploy Linux presets.
    exit /b 1
)
exit /b 0

:resolve_wsl_root
for /f "tokens=*" %%P in ('wsl wslpath "%ROOT%"') do set "WSL_ROOT=%%P"
if not defined WSL_ROOT (
    echo [ERROR] Failed to resolve WSL path for %ROOT%
    exit /b 1
)
exit /b 0

:do_deploy_linux_wsl
set "PRESET=%~1"
set "DEPLOY_SUBDIR=%~2"
if "%PRESET%"=="" goto :eof
if "%DEPLOY_SUBDIR%"=="" goto :eof
call :check_wsl
if errorlevel 1 goto :eof
call :resolve_wsl_root
if errorlevel 1 goto :eof

echo.
echo [INFO] WSL deploy preset: %PRESET%
echo [INFO] Deploy target: %DEPLOY_DIR%\%DEPLOY_SUBDIR%
echo.
wsl bash -lc "cd '%WSL_ROOT%' && cmake --preset %PRESET% && cmake --build --preset %PRESET% && cmake --install build/%PRESET% --prefix deploy/%DEPLOY_SUBDIR%"
if errorlevel 1 (
    echo [ERROR] WSL deploy failed for %PRESET%.
    goto :eof
)
echo.
echo [OK] WSL deploy completed: %PRESET%
echo.
pause
goto :eof

:exit_script
echo.
echo Bye.
echo.
exit /b 0
