@echo off
REM AssetTrace Build Script for Windows GCC

echo Building AssetTrace...
gcc -Wall -Wextra -std=c99 -Isrc -o assettrace.exe src/main.c src/item.c src/storage.c src/graph.c src/hash_index.c src/ranking.c src/text_match.c src/matcher.c src/registration.c src/cli.c

if %ERRORLEVEL% equ 0 (
    echo Build successful: assettrace.exe
) else (
    echo Build failed!
    exit /b %ERRORLEVEL%
)
