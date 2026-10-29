@echo off
setlocal

echo ======================================================================
echo   DirectX 9 (D3D9) Low-Latency Proxy DLL - Automated Build Script
echo   Target Architecture: x64
echo ======================================================================
echo.

set VSPATH=D:\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars64.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat
if exist "%VSPATH%" goto found_vs

if exist "%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" (
    for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do (
        if exist "%%i\VC\Auxiliary\Build\vcvars64.bat" (
            set "VSPATH=%%i\VC\Auxiliary\Build\vcvars64.bat"
            goto found_vs
        )
    )
)

echo [ERROR] Visual Studio C++ x64 compiler (vcvars64.bat) not found!
if /i not "%~1"=="nopause" pause
exit /b 1

:found_vs
echo [1/4] Initializing MSVC x64 environment...
echo Using: %VSPATH%
call "%VSPATH%"
if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    if /i not "%~1"=="nopause" pause
    exit /b 1
)

echo.
echo [2/4] Building dxwrapper_d3d9 (x64 Release)...
msbuild "%~dp0dxwrapper_d3d9.vcxproj" /p:Configuration=Release /p:Platform=x64 /m /nologo
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed! Check compiler errors above.
    if /i not "%~1"=="nopause" pause
    exit /b 1
)

echo.
echo [3/4] Packaging Release output...
if not exist "%~dp0Release" mkdir "%~dp0Release"

if exist "%~dp0bin\Release\x64\d3d9.dll" (
    copy /y "%~dp0bin\Release\x64\d3d9.dll" "%~dp0Release\d3d9.dll" >nul
) else (
    echo [ERROR] bin\Release\x64\d3d9.dll was not generated.
    if /i not "%~1"=="nopause" pause
    exit /b 1
)

if exist "%~dp0Settings\Settings.ini" (
    copy /y "%~dp0Settings\Settings.ini" "%~dp0Release\d3d9.ini" >nul
)

echo.
echo [4/4] Build complete!
echo Output files:
echo   - %~dp0Release\d3d9.dll
echo   - %~dp0Release\d3d9.ini
echo ======================================================================

if /i not "%~1"=="nopause" pause
exit /b 0
