@echo off
setlocal

if exist "build\" (
    echo Found build/
    rmdir /s /q "build"
    mkdir "build"
    cd /d "build"
    cmake -GNinja ..
    if errorlevel 1 exit /b %errorlevel%
    ninja
    if errorlevel 1 exit /b %errorlevel%
) else (
    echo Not Found build/
    mkdir "build"
    cd /d "build"
    cmake -GNinja ..
    if errorlevel 1 exit /b %errorlevel%
    ninja
    if errorlevel 1 exit /b %errorlevel%
)

endlocal
