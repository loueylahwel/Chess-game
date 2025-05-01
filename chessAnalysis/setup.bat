@echo off
echo Setting up Chess Analyzer...

echo Installing Electron dependencies...
npm install

echo Creating freechess directory...
if not exist freechess mkdir freechess

echo Copying Freechess files...
xcopy /E /I /Y ..\freechess\* freechess\

echo Installing Freechess dependencies...
cd freechess
npm install
npm run build
cd ..

echo Setup complete!
echo Run "npm start" to start the application
pause
