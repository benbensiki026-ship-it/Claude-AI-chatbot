cmake_minimum_required(VERSION 3.10)
project(ClaudeChatbot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)

# Find libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/chatbot.cpp
)

# Create executable
add_executable(claude_chatbot ${SOURCES})

# Link libraries
target_link_libraries(claude_chatbot ${CURL_LIBRARIES})

# Platform-specific settings
if(WIN32)
    target_link_libraries(claude_chatbot ws2_32)
endif()

if(ANDROID)
    find_library(log-lib log)
    target_link_libraries(claude_chatbot ${log-lib})
endif()

# Installation
install(TARGETS claude_chatbot DESTINATION bin)
