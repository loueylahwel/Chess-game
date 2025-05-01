@echo off
echo Building Chess Analyzer executable...

cd %~dp0
npx electron-builder --win --dir

echo Build process completed.
pause
