@echo off
setlocal
cd /d "%~dp0"
call tools\find_pio.cmd
if errorlevel 1 exit /b %errorlevel%
python tools\validate_profiles.py
if errorlevel 1 exit /b %errorlevel%
python tools\source_qa.py
if errorlevel 1 exit /b %errorlevel%
"%PIO_EXE%" run -e cardputer
exit /b %errorlevel%
