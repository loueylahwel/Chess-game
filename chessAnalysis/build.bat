@echo off
echo Building Chess Analyzer executable...

echo Installing electron-builder if not already installed...
npm install --save-dev electron-builder

echo Building application...
npx electron-builder --win

echo Build complete! Check the dist folder for the executable.
pause
