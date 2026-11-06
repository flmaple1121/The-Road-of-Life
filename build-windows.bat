@echo off
setlocal
cd /d "%~dp0"

where g++ >nul 2>nul
if errorlevel 1 (
    echo A MinGW-w64 C++ compiler is required. Add g++ to PATH and try again.
    exit /b 1
)
if not exist build mkdir build
g++ -std=c++11 -Wall -Wextra -pedantic -DROAD_OF_LIFE_GUI -municode -mwindows -pthread ^
    -static -static-libgcc -static-libstdc++ "src/The Road of Life.cpp" src/chat_win32.cpp ^
    -o "build/The Road of Life - Chat.exe" -lgdi32 -luser32
if errorlevel 1 exit /b 1
echo Built: build\The Road of Life - Chat.exe
