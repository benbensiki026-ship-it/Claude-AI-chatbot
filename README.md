# Claude Chatbot - Cross-Platform AI Assistant

A full-featured chatbot application written in C++ that uses Claude AI (via Anthropic API). Supports conversation memory, cross-platform compatibility (Windows, Linux, macOS, Android, iOS), and multiple advanced features.

## Features

✨ **Core Features:**
- 💬 Interactive chat with Claude AI
- 💾 Automatic conversation history saving
- 🔄 Multiple conversation management
- 🔍 Search through all messages
- 📤 Export conversations to text files
- 🎨 Beautiful CLI interface

✨ **Advanced Features:**
- 📱 Cross-platform support (PC and Mobile)
- 🗂️ Persistent storage across sessions
- 🔧 Customizable AI models (Sonnet, Opus, Haiku)
- ⚙️ Adjustable max tokens
- 🕐 Timestamped messages
- 🗑️ Delete/Clear conversations

## Platform Support

- ✅ Windows (7, 8, 10, 11)
- ✅ Linux (Ubuntu, Debian, Fedora, Arch, etc.)
- ✅ macOS (10.12+)
- ✅ Android (NDK required)
- ✅ iOS (Xcode required)

## Prerequisites

### All Platforms:
- C++17 compatible compiler (GCC, Clang, MSVC)
- libcurl library
- CMake 3.10+ (optional, for CMake build)
- Anthropic API key (get from https://console.anthropic.com)

### Platform-Specific:

**Windows:**
```bash
# Install via vcpkg
vcpkg install curl
```

**Linux (Debian/Ubuntu):**
```bash
sudo apt-get install build-essential libcurl4-openssl-dev cmake
```

**Linux (Fedora/RHEL):**
```bash
sudo dnf install gcc-c++ libcurl-devel cmake
```

**macOS:**
```bash
brew install curl cmake
```

**Android:**
- Android NDK
- CMake for Android

**iOS:**
- Xcode with Command Line Tools
- libcurl (usually included)

## Installation & Building

### Method 1: Using Make (Quick & Easy)

```bash
# Extract the zip file
unzip claude-chatbot.zip
cd claude-chatbot

# Build the project
make

# Run the chatbot
./claude_chatbot

# (Optional) Install system-wide on Linux/macOS
sudo make install
```

### Method 2: Using CMake (Recommended for Cross-Platform)

```bash
# Extract and navigate
unzip claude-chatbot.zip
cd claude-chatbot

# Create build directory
mkdir build
cd build

# Configure and build
cmake ..
make

# Run
./claude_chatbot

# (Optional) Install
sudo make install
```

### Method 3: Manual Compilation

```bash
# Linux/macOS
g++ -std=c++17 -Iinclude src/main.cpp src/chatbot.cpp -lcurl -o claude_chatbot

# Windows (MinGW)
g++ -std=c++17 -Iinclude src/main.cpp src/chatbot.cpp -lcurl -lws2_32 -o claude_chatbot.exe

# Windows (MSVC)
cl /std:c++17 /Iinclude src/main.cpp src/chatbot.cpp /link curl.lib ws2_32.lib
```

## Usage

### First Time Setup

1. Get your API key from https://console.anthropic.com
2. Run the chatbot:
   ```bash
   ./claude_chatbot
   ```
3. Enter your API key when prompted
4. Start chatting!

### Main Menu Options

```
1. Chat with Claude        - Start chatting with AI
2. Start New Conversation  - Begin a fresh conversation
3. View All Conversations  - See all saved conversations
4. Load Conversation       - Switch to a different conversation
5. Delete Conversation     - Remove a conversation permanently
6. Clear Current          - Clear messages in current conversation
7. Search Messages        - Find specific messages across all chats
8. Export Conversation    - Save conversation to text file
9. Settings               - Adjust model and parameters
0. Exit                   - Close the application
```

### Chat Commands

While in chat mode:
- Type normally to chat with Claude
- `history` - View current conversation history
- `exit` - Return to main menu

### Examples

**Basic Chat:**
```
You: What's the weather like in Paris?
Claude: I can search for that information for you...

You: Write me a Python script for sorting a list
Claude: Here's a Python script for sorting...
```

**Search Messages:**
```
Enter search query: machine learning
Found 5 messages matching "machine learning"...
```

**Export Conversation:**
```
Enter filename: my_chat_2024.txt
Conversation exported to my_chat_2024.txt
```

## Data Storage

Conversations are automatically saved to:

- **Windows:** `%APPDATA%\ClaudeChatbot\conversations.dat`
- **Linux:** `~/.claude_chatbot/conversations.dat`
- **macOS:** `~/Library/Application Support/ClaudeChatbot/conversations.dat`
- **Android:** `/data/data/com.claudechatbot/files/conversations.dat`
- **iOS:** `~/Library/Application Support/ClaudeChatbot/conversations.dat`

## Configuration

### Supported Models

- `claude-sonnet-4-20250514` - Balanced performance (default)
- `claude-opus-4-20250514` - Most capable model
- `claude-haiku-4-20250514` - Fastest responses

### Max Tokens

Adjustable from 100 to 4096 tokens (default: 1000)

## API Key Security

⚠️ **Important:** Never share your API key or commit it to version control!

For production use, consider:
- Storing API key in environment variable: `export ANTHROPIC_API_KEY=your_key`
- Using a configuration file outside the repository
- Implementing key encryption

## Troubleshooting

### "curl not found" error:
Install libcurl-dev package for your platform (see Prerequisites)

### "API key invalid" error:
Verify your API key at https://console.anthropic.com

### Compilation errors:
Ensure you have C++17 support: `g++ --version` (should be 7.0+)

### Permission denied on Linux:
Make executable: `chmod +x claude_chatbot`

## Building for Mobile

### Android:
```bash
# Set NDK path
export ANDROID_NDK=/path/to/ndk

# Configure with CMake
cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
      -DANDROID_ABI=arm64-v8a \
      -DANDROID_PLATFORM=android-21 ..
make
```

### iOS:
```bash
# Use Xcode or command line
xcodebuild -project ClaudeChatbot.xcodeproj -scheme ClaudeChatbot
```

## Project Structure

```
claude-chatbot/
├── include/
│   └── chatbot.h          # Header file with class definitions
├── src/
│   ├── main.cpp           # CLI interface and menu system
│   └── chatbot.cpp        # Core chatbot implementation
├── build/                 # Build directory (created during compilation)
├── CMakeLists.txt         # CMake configuration
├── Makefile              # Make build file
└── README.md             # This file
```

## Contributing

Feel free to fork and submit pull requests! Some ideas:
- GUI interface (Qt, GTK, or native)
- Web interface (WebSocket server)
- Voice input/output
- Image support
- Streaming responses
- RAG (Retrieval Augmented Generation)

## License

MIT License - feel free to use for personal or commercial projects.

## Credits

Built with:
- Claude AI by Anthropic (https://anthropic.com)
- libcurl for HTTP requests
- C++17 standard library

## Support

For issues or questions:
1. Check the troubleshooting section
2. Review Anthropic API docs: https://docs.anthropic.com
3. Open an issue on GitHub

## Version History

**v1.0.0** (2024)
- Initial release
- Cross-platform support
- Full conversation management
- Search and export features
- Multiple model support

---

Made with ❤️ for the Claude community
