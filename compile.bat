@echo off
echo Compiling...
echo Would you like to remove CMake cache and rebuild?
set /p REMOVECACHE=Yes or no (Y/N):
if /I "%REMOVECACHE%"=="Y" goto:wipe
if /I "%REMOVECACHE%"=="Yes" goto:wipe
goto:cont
:wipe
echo Wiping the Cache...
if EXIST CMakeCache.txt del CMakeCache.txt
echo Generating build files
cmake .
:cont
echo Building...
cmake --build .
echo error level %ERRORLEVEL%
if "%ERRORLEVEL%"=="0" (
    echo copy
    xcopy debug\ml.exe .\ /q /y
    echo running code
    ml
    echo exit code %ERRORLEVEL%
) else echo FAILED!!!