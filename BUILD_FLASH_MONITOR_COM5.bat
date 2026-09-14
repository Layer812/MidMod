@echo off
setlocal
cd /d "%~dp0"
call tools\find_pio.cmd
if errorlevel 1 exit /b %errorlevel%
set "PORT=%~1"
if "%PORT%"=="" set "PORT=COM5"
echo ============================================================
echo MidMod 0.1.1 - Modifiable MIDI Module
echo BUILD + FLASH + MONITOR %PORT%
echo ============================================================
python tools\validate_profiles.py
if errorlevel 1 exit /b %errorlevel%
python tools\source_qa.py
if errorlevel 1 exit /b %errorlevel%
"%PIO_EXE%" run -e cardputer -t upload --upload-port "%PORT%"
if errorlevel 1 exit /b %errorlevel%
echo.
echo [MONITOR] Ctrl+C to exit.
"%PIO_EXE%" device monitor -p "%PORT%" -b 115200
exit /b %errorlevel%
