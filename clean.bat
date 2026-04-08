@echo off
setlocal

if exist "build\" (
    echo Found build/, removing...
    rmdir /s /q "build"
    echo Done.
) else (
    echo Not Found build/, it seems the build files are already deleted!
)

endlocal
