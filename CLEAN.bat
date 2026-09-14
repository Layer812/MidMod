@echo off
setlocal
cd /d "%~dp0"
call tools\find_pio.cmd
if errorlevel 1 exit /b %errorlevel%
"%PIO_EXE%" run -e cardputer -t clean
exit /b %errorlevel%
