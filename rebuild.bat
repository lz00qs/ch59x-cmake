@echo off
setlocal

call "%~dp0clean.bat"
if errorlevel 1 exit /b %errorlevel%

call "%~dp0build.bat"
if errorlevel 1 exit /b %errorlevel%

endlocal
