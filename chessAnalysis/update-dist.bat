@echo off
echo Updating dist files with customized versions...

REM Create directories if they don't exist
if not exist "dist\win-unpacked\resources\app.asar\freechess\src\public" mkdir "dist\win-unpacked\resources\app.asar\freechess\src\public"
if not exist "dist\win-unpacked\resources\app.asar\freechess\src\public\pages\report\styles" mkdir "dist\win-unpacked\resources\app.asar\freechess\src\public\pages\report\styles"

REM Copy customized files
echo Copying global.css...
xcopy /Y "freechess\src\public\global.css" "dist\win-unpacked\resources\app.asar\freechess\src\public\"

echo Copying index.html...
xcopy /Y "freechess\src\public\pages\report\index.html" "dist\win-unpacked\resources\app.asar\freechess\src\public\pages\report\"

echo Copying reviewpanel.css...
xcopy /Y "freechess\src\public\pages\report\styles\reviewpanel.css" "dist\win-unpacked\resources\app.asar\freechess\src\public\pages\report\styles\"

echo Update complete!
pause
