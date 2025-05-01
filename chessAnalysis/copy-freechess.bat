@echo off
echo Copying Freechess files to the project...

if not exist freechess mkdir freechess

xcopy /E /I /Y ..\freechess\* freechess\

echo Done!
pause
