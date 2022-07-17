@echo off
if not "X%SDK_SETUP_ENV%" == "X" (
  echo Hexagon SDK environment already setup for this command prompt
  echo.
  echo please open new command prompt to set up the environment for different sdk
  exit /b
)

echo Setting up the Hexagon SDK environment for Android Studio locally...

set __HEXAGON_CMAKE_ROOT=%~dp0
set HEXAGON_CMAKE_ROOT=%__HEXAGON_CMAKE_ROOT%
set __HEXAGON_SDK_ROOT=%__HEXAGON_CMAKE_ROOT:~0,-13%

set HEXAGON_SDK_ROOT=%__HEXAGON_SDK_ROOT%
set HEXAGON_TOOLS_ROOT=%__HEXAGON_SDK_ROOT%/tools/HEXAGON_Tools/8.4.12

set SDK_SETUP_ENV=Done
