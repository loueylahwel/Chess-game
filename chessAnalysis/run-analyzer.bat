@echo off
echo Starting Chess Analyzer...

REM Make sure we're in the right directory
cd %~dp0

REM Check if node.js is installed
where node >nul 2>nul
if %ERRORLEVEL% neq 0 (
  echo ERROR: Node.js is not installed or not in PATH.
  echo Please install Node.js from https://nodejs.org/
  pause
  exit /b 1
)

REM Check if electron is installed
if not exist node_modules\electron (
  echo Installing electron...
  npm install --save-dev electron
)

REM Check if a PGN file path was provided
if "%~1"=="" (
  echo No PGN file specified. Starting with empty board.
  node_modules\.bin\electron .
) else (
  echo Loading PGN file: %1
  node_modules\.bin\electron . "%~1"
)

echo Chess Analyzer closed.
pause
