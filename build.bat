@echo off
REM Build script for Claude Chatbot on Windows

echo ========================================
echo   Claude Chatbot Build Script
echo ========================================
echo.

echo Detected platform: Windows
echo.

echo Checking dependencies...

where g++ >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [OK] g++ found
    g++ --version | findstr /R "g++"
) else (
    where cl >nul 2>&1
    if %ERRORLEVEL% EQU 0 (
        echo [OK] MSVC compiler found
    ) else (
        echo [ERROR] No C++ compiler found!
        echo Please install MinGW or Visual Studio
        pause
        exit /b 1
    )
)

echo.
echo ========================================
echo   Building...
echo ========================================
echo.

if exist CMakeLists.txt (
    echo Using CMake build system...
    if not exist build mkdir build
    cd build
    cmake ..
    cmake --build .
    cd ..
    echo.
    echo [OK] Build complete! Executable: build\Debug\claude_chatbot.exe
) else if exist Makefile (
    echo Using Make build system...
    mingw32-make
    echo.
    echo [OK] Build complete! Executable: claude_chatbot.exe
) else (
    echo Using direct compilation...
    g++ -std=c++17 -Iinclude src\main.cpp src\chatbot.cpp -lcurl -lws2_32 -o claude_chatbot.exe
    echo.
    echo [OK] Build complete! Executable: claude_chatbot.exe
)

echo.
echo ========================================
echo   Ready to run!
echo ========================================
echo.
echo To start the chatbot:
echo   claude_chatbot.exe
echo.
echo Don't forget to have your Anthropic API key ready!
echo Get one at: https://console.anthropic.com
echo.
pause
