#!/bin/bash

# Build script for Claude Chatbot
# Automatically detects platform and builds appropriately

echo "========================================"
echo "  Claude Chatbot Build Script"
echo "========================================"
echo ""

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    PLATFORM="Linux"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    PLATFORM="macOS"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    PLATFORM="Windows"
else
    PLATFORM="Unknown"
fi

echo "Detected platform: $PLATFORM"
echo ""

# Check for dependencies
echo "Checking dependencies..."

# Check for compiler
if command -v g++ &> /dev/null; then
    echo "✓ g++ found: $(g++ --version | head -n 1)"
elif command -v clang++ &> /dev/null; then
    echo "✓ clang++ found: $(clang++ --version | head -n 1)"
else
    echo "✗ No C++ compiler found! Please install g++ or clang++"
    exit 1
fi

# Check for libcurl
if command -v curl-config &> /dev/null; then
    echo "✓ libcurl found: $(curl-config --version)"
else
    echo "⚠ libcurl-dev may not be installed"
    echo "  Install with:"
    if [[ "$PLATFORM" == "Linux" ]]; then
        echo "  sudo apt-get install libcurl4-openssl-dev  # Debian/Ubuntu"
        echo "  sudo dnf install libcurl-devel            # Fedora/RHEL"
    elif [[ "$PLATFORM" == "macOS" ]]; then
        echo "  brew install curl"
    fi
fi

echo ""
echo "========================================"
echo "  Building..."
echo "========================================"
echo ""

# Choose build method
if command -v cmake &> /dev/null && [ -f "CMakeLists.txt" ]; then
    echo "Using CMake build system..."
    mkdir -p build
    cd build
    cmake ..
    make
    cd ..
    echo ""
    echo "✓ Build complete! Executable: build/claude_chatbot"
elif [ -f "Makefile" ]; then
    echo "Using Make build system..."
    make
    echo ""
    echo "✓ Build complete! Executable: claude_chatbot"
else
    echo "Using direct compilation..."
    if [[ "$PLATFORM" == "Windows" ]]; then
        g++ -std=c++17 -Iinclude src/main.cpp src/chatbot.cpp -lcurl -lws2_32 -o claude_chatbot.exe
        echo ""
        echo "✓ Build complete! Executable: claude_chatbot.exe"
    else
        g++ -std=c++17 -Iinclude src/main.cpp src/chatbot.cpp -lcurl -o claude_chatbot
        chmod +x claude_chatbot
        echo ""
        echo "✓ Build complete! Executable: claude_chatbot"
    fi
fi

echo ""
echo "========================================"
echo "  Ready to run!"
echo "========================================"
echo ""
echo "To start the chatbot:"
if [[ "$PLATFORM" == "Windows" ]]; then
    echo "  .\\claude_chatbot.exe"
else
    echo "  ./claude_chatbot"
fi
echo ""
echo "Don't forget to have your Anthropic API key ready!"
echo "Get one at: https://console.anthropic.com"
