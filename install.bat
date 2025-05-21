@echo off
setlocal enabledelayedexpansion

set "script_path=%~dp0"
if "%script_path:~-1%"=="\" set "script_path=%script_path:~0,-1%"
echo Script path: %script_path%
echo Started project configurator for Source Engine 2013 SDK

:: Step 1: Ask if SDK is already installed
set /p sdk_exists="Do you already have Source Engine 2013 SDK installed? (y/n): "

if /i "%sdk_exists%"=="y" (
    set /p sdk_path="Enter full path to Source SDK: "
    if not exist "%sdk_path%" (
        echo ERROR: Provided path does not exist.
        goto end
    )
    goto setup_envvar
)

:: Step 2: Check if Git is installed
where git >nul 2>nul
if errorlevel 1 (
    echo Git is not installed or not in PATH.
    echo Please install Git Bash from: https://git-scm.com/downloads
    start https://git-scm.com/downloads
    echo After installation, run this script again.
    goto end
)

:: Step 3: Git is available, clone SDK
echo Git found. Cloning SDK to current folder...
git clone --recursive "https://github.com/ValveSoftware/source-sdk-2013"
if errorlevel 1 (
    echo ERROR: Failed to clone repository.
    goto end
)

:: Set the path where we just cloned
set "sdk_path=%script_path%\source-sdk-2013\src"

:: Step 4: Set environment variable
:setup_envvar
echo Setting environment variable SE2013_SDK = %sdk_path%
setx SE2013_SDK "%sdk_path%"
if errorlevel 1 (
    echo ERROR: Failed to set environment variable.
    goto end
)

:: building projects
pushd
echo Building projects...
cd /d "%sdk_path%"
call "createallprojects.bat"
popd

:: Step 5: Ask to open Environment Variables UI
set /p show_env="Would you like to open the environment variables window now? (y/n): "
if /i "%show_env%"=="y" (
    rundll32.exe sysdm.cpl,EditEnvironmentVariables
)

:end
pause
