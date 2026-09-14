@echo off
rem Helper is CALLed by the top-level scripts. It sets PIO_EXE.
set "PIO_EXE="
if exist "%USERPROFILE%\.platformio\penv\Scripts\pio.exe" set "PIO_EXE=%USERPROFILE%\.platformio\penv\Scripts\pio.exe"
if defined PIO_EXE exit /b 0
where pio.exe >nul 2>&1
if not errorlevel 1 set "PIO_EXE=pio.exe"
if defined PIO_EXE exit /b 0
where platformio.exe >nul 2>&1
if not errorlevel 1 set "PIO_EXE=platformio.exe"
if defined PIO_EXE exit /b 0
echo ERROR: PlatformIO CLI was not found.
echo Install PlatformIO Core or VSCode PlatformIO, then run this command again.
exit /b 2
