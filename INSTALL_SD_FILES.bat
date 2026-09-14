@echo off
setlocal
cd /d "%~dp0"
if "%~1"=="" (
  echo Usage: INSTALL_SD_FILES.bat E:
  exit /b 2
)
set "SDROOT=%~1"
if not exist "%SDROOT%\" (
  echo ERROR: drive/path not found: %SDROOT%
  exit /b 3
)
copy /Y "sdcard\sc55mk2.csv" "%SDROOT%\" >nul || exit /b 4
copy /Y "sdcard\sc88pro.csv" "%SDROOT%\" >nul || exit /b 5
copy /Y "sdcard\SC88PRO_TONE_TEST.mid" "%SDROOT%\" >nul || exit /b 6
copy /Y "sdcard\README_SC88PRO_TONE_TEST.txt" "%SDROOT%\" >nul || exit /b 7
echo Installed MidMod SD files to %SDROOT%\
echo Default profile: sc55mk2.csv
echo P = profile, M = help
exit /b 0
