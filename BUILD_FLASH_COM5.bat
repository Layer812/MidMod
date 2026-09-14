@echo off
setlocal
cd /d "%~dp0"
call tools\find_pio.cmd
if errorlevel 1 exit /b %errorlevel%
set "PORT=%~1"
if "%PORT%"=="" set "PORT=COM5"
python tools\validate_profiles.py
if errorlevel 1 exit /b %errorlevel%
python tools\source_qa.py
if errorlevel 1 exit /b %errorlevel%
"%PIO_EXE%" run -e cardputer -t upload --upload-port "%PORT%"
exit /b %errorlevel%
