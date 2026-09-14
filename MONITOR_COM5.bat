@echo off
setlocal
cd /d "%~dp0"
call tools\find_pio.cmd
if errorlevel 1 exit /b %errorlevel%
set "PORT=%~1"
if "%PORT%"=="" set "PORT=COM5"
"%PIO_EXE%" device monitor -p "%PORT%" -b 115200
exit /b %errorlevel%
