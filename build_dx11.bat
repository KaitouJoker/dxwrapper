@echo off
setlocal

echo ======================================================================
echo   DirectX 11 (DXGI) Low-Latency Proxy DLL - One-Click Build Script
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
echo [2/4] Building dxwrapper_dxgi (x64 Release)...
msbuild "%~dp0dxwrapper_dxgi.vcxproj" /p:Configuration=Release /p:Platform=x64 /m /nologo
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed! Check compiler errors above.
    if /i not "%~1"=="nopause" pause
    exit /b 1
)

echo.
echo [3/4] Packaging Release output...
if not exist "%~dp0Release" mkdir "%~dp0Release"

if exist "%~dp0bin\Release\x64\dxgi.dll" (
    copy /y "%~dp0bin\Release\x64\dxgi.dll" "%~dp0Release\dxgi.dll" >nul
) else (
    echo [ERROR] bin\Release\x64\dxgi.dll was not generated.
    if /i not "%~1"=="nopause" pause
    exit /b 1
)

if exist "%~dp0Settings\dxgi.ini" (
    copy /y "%~dp0Settings\dxgi.ini" "%~dp0Release\dxgi.ini" >nul
)

echo.
echo [4/4] Verifying Release output...
if exist "%~dp0Release\dxgi.dll" (
    echo ======================================================================
    echo   [SUCCESS] dxgi.dll successfully built and packaged!
    echo   Output Directory: %~dp0Release\
    echo ======================================================================
    dir "%~dp0Release\dxgi.dll" "%~dp0Release\dxgi.ini"
    echo.
) else (
    echo [ERROR] Release\dxgi.dll missing.
    if /i not "%~1"=="nopause" pause
    exit /b 1
)

if /i not "%~1"=="nopause" pause
