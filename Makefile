# Makefile for Claude Chatbot
# Cross-platform C++ chatbot with Claude AI integration

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
LDFLAGS = -lcurl

# Platform detection
ifeq ($(OS),Windows_NT)
    LDFLAGS += -lws2_32
    RM = del /Q
    TARGET = claude_chatbot.exe
    MKDIR = mkdir
else
    RM = rm -f
    TARGET = claude_chatbot
    MKDIR = mkdir -p
endif

# Source files
SOURCES = src/main.cpp src/chatbot.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	$(RM) $(OBJECTS) $(TARGET)

# Install (Unix-like systems)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean install
