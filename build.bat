@echo off
REM ============================================================================
REM Qt Splash Screen Demo - CMake Build Script
REM ============================================================================
REM This script builds and runs the Qt Splash Screen demo application
REM Uses Qt 6.10.1 with MinGW 13.1.0 compiler and CMake + Ninja
REM ============================================================================

setlocal enabledelayedexpansion

REM Configuration - Modify these paths if your Qt installation is different
set QT_ROOT=C:\eng_apps\Qt
set QT_VERSION=6.10.1
set QT_COMPILER=mingw_64
set MINGW_VERSION=mingw1310_64

REM Derived paths
set QT_PATH=%QT_ROOT%\%QT_VERSION%\%QT_COMPILER%
set MINGW_PATH=%QT_ROOT%\Tools\%MINGW_VERSION%
set CMAKE_PATH=%QT_ROOT%\Tools\CMake_64\bin
set NINJA_PATH=%QT_ROOT%\Tools\Ninja

REM Project paths
set PROJECT_DIR=%~dp0
set BUILD_DIR=%PROJECT_DIR%build
set BIN_DIR=%PROJECT_DIR%bin

echo.
echo ============================================================================
echo   Qt Splash Screen Demo - CMake Build Script
echo ============================================================================
echo   Qt Path:    %QT_PATH%
echo   MinGW Path: %MINGW_PATH%
echo   CMake Path: %CMAKE_PATH%
echo   Project:    %PROJECT_DIR%
echo ============================================================================
echo.

REM Check if CMake exists
if not exist "%CMAKE_PATH%\cmake.exe" (
    echo [ERROR] CMake not found at: %CMAKE_PATH%
    echo Please verify your Qt/CMake installation path.
    pause
    exit /b 1
)

REM Check if MinGW exists
if not exist "%MINGW_PATH%\bin\g++.exe" (
    echo [ERROR] MinGW not found at: %MINGW_PATH%
    echo Please verify your MinGW installation path.
    pause
    exit /b 1
)

REM Check if Ninja exists
if not exist "%NINJA_PATH%\ninja.exe" (
    echo [ERROR] Ninja not found at: %NINJA_PATH%
    echo Please verify your Ninja installation path.
    pause
    exit /b 1
)

REM Setup environment
echo [INFO] Setting up environment...
set PATH=%CMAKE_PATH%;%NINJA_PATH%;%QT_PATH%\bin;%MINGW_PATH%\bin;%PATH%
set Qt6_DIR=%QT_PATH%\lib\cmake\Qt6

REM Clean previous build (optional)
if "%1"=="clean" (
    echo [INFO] Cleaning previous build...
    if exist "%BUILD_DIR%" rmdir /s /q "%BUILD_DIR%"
    if exist "%BIN_DIR%" rmdir /s /q "%BIN_DIR%"
)

REM Create build directory
echo [INFO] Creating build directory...
if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
if not exist "%BIN_DIR%" mkdir "%BIN_DIR%"

REM Change to build directory
cd /d "%BUILD_DIR%"

REM Run CMake configure
echo.
echo [INFO] Configuring with CMake...
"%CMAKE_PATH%\cmake.exe" ^
    -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QT_PATH%" ^
    -DCMAKE_C_COMPILER="%MINGW_PATH%\bin\gcc.exe" ^
    -DCMAKE_CXX_COMPILER="%MINGW_PATH%\bin\g++.exe" ^
    -DCMAKE_MAKE_PROGRAM="%NINJA_PATH%\ninja.exe" ^
    "%PROJECT_DIR%"

if errorlevel 1 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

REM Build with Ninja
echo.
echo [INFO] Building project with Ninja...
"%NINJA_PATH%\ninja.exe" -j%NUMBER_OF_PROCESSORS%

if errorlevel 1 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

REM Copy Qt DLLs for standalone execution
echo.
echo [INFO] Copying Qt runtime libraries...
copy /y "%QT_PATH%\bin\Qt6Core.dll" "%BIN_DIR%\" >nul 2>&1
copy /y "%QT_PATH%\bin\Qt6Gui.dll" "%BIN_DIR%\" >nul 2>&1
copy /y "%QT_PATH%\bin\Qt6Widgets.dll" "%BIN_DIR%\" >nul 2>&1
copy /y "%QT_PATH%\bin\Qt6Svg.dll" "%BIN_DIR%\" >nul 2>&1
copy /y "%QT_PATH%\bin\Qt6SvgWidgets.dll" "%BIN_DIR%\" >nul 2>&1
copy /y "%QT_PATH%\bin\Qt6Concurrent.dll" "%BIN_DIR%\" >nul 2>&1

REM Copy MinGW runtime
echo [INFO] Copying MinGW runtime libraries...
copy /y "%MINGW_PATH%\bin\libgcc_s_seh-1.dll" "%BIN_DIR%\" >nul 2>&1
copy /y "%MINGW_PATH%\bin\libstdc++-6.dll" "%BIN_DIR%\" >nul 2>&1
copy /y "%MINGW_PATH%\bin\libwinpthread-1.dll" "%BIN_DIR%\" >nul 2>&1

REM Copy Qt plugins
echo [INFO] Copying Qt plugins...
if not exist "%BIN_DIR%\platforms" mkdir "%BIN_DIR%\platforms"
copy /y "%QT_PATH%\plugins\platforms\qwindows.dll" "%BIN_DIR%\platforms\" >nul 2>&1

if not exist "%BIN_DIR%\imageformats" mkdir "%BIN_DIR%\imageformats"
copy /y "%QT_PATH%\plugins\imageformats\qsvg.dll" "%BIN_DIR%\imageformats\" >nul 2>&1

if not exist "%BIN_DIR%\iconengines" mkdir "%BIN_DIR%\iconengines"
copy /y "%QT_PATH%\plugins\iconengines\qsvgicon.dll" "%BIN_DIR%\iconengines\" >nul 2>&1

if not exist "%BIN_DIR%\styles" mkdir "%BIN_DIR%\styles"
copy /y "%QT_PATH%\plugins\styles\qwindowsvistastyle.dll" "%BIN_DIR%\styles\" >nul 2>&1

echo.
echo ============================================================================
echo   BUILD SUCCESSFUL!
echo ============================================================================
echo.

REM Run the application
if not "%1"=="norun" (
    echo [INFO] Launching application...
    cd /d "%BIN_DIR%"
    start "" "%BIN_DIR%\qt-splash-screen.exe"
)

echo.
echo [INFO] Build completed. Executable: %BIN_DIR%\qt-splash-screen.exe
echo.

cd /d "%PROJECT_DIR%"
exit /b 0
