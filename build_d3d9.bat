@echo off
setlocal

echo ======================================================================
echo   KartRider D3D9 Low-Latency Proxy DLL - One-Click Build Script
echo ======================================================================
echo.

set VSPATH=D:\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars32.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvars32.bat
if exist "%VSPATH%" goto found_vs

set VSPATH=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars32.bat
if exist "%VSPATH%" goto found_vs

echo [ERROR] Visual Studio C++ x86 compiler (vcvars32.bat) not found!
pause
exit /b 1

:found_vs
echo [1/3] Initializing MSVC x86 environment...
echo Using: %VSPATH%
call "%VSPATH%"
if errorlevel 1 (
    echo [ERROR] Failed to initialize MSVC environment.
    pause
    exit /b 1
)

echo [2/3] Building dxwrapper_d3d9 (Win32 Release)...
msbuild dxwrapper_d3d9.vcxproj /p:Configuration=Release /p:Platform=Win32 /m /nologo
if errorlevel 1 (
    echo.
    echo [ERROR] Build failed! Check compiler errors above.
    pause
    exit /b 1
)

echo [3/3] Verifying Release output...
if exist "Release\d3d9.dll" (
    echo.
    echo ======================================================================
    echo   [SUCCESS] d3d9.dll successfully built!
    echo   Output Directory: %~dp0Release\
    echo ======================================================================
    dir "Release\d3d9.dll" "Release\dxwrapper.ini"
    echo.
) else (
    echo [ERROR] Release\d3d9.dll was not generated.
    pause
    exit /b 1
)

if "%~1"=="" pause
