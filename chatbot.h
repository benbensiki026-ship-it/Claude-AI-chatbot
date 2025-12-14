#ifndef CHATBOT_H
#define CHATBOT_H

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define PLATFORM_WINDOWS
#elif defined(__ANDROID__)
    #define PLATFORM_ANDROID
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define PLATFORM_IOS
    #else
        #define PLATFORM_MACOS
    #endif
#elif defined(__linux__)
    #define PLATFORM_LINUX
#endif

struct Message {
    std::string role;
    std::string content;
    std::string timestamp;
};

struct Conversation {
    std::string id;
    std::string title;
    std::vector<Message> messages;
    std::string created_at;
    std::string last_modified;
};

class ClaudeChatbot {
private:
    std::string api_key;
    std::string model;
    int max_tokens;
    std::string data_dir;
    std::vector<Conversation> conversations;
    std::string current_conversation_id;
    
    // Helper methods
    std::string generate_id();
    std::string get_timestamp();
    std::string get_data_directory();
    bool create_directory(const std::string& path);
    std::string http_post(const std::string& url, const std::string& json_data);
    std::string escape_json(const std::string& str);
    std::string build_messages_json(const std::vector<Message>& messages);
    
public:
    ClaudeChatbot(const std::string& api_key, 
                  const std::string& model = "claude-sonnet-4-20250514",
                  int max_tokens = 1000);
    
    // Core chat functions
    std::string send_message(const std::string& user_message);
    void start_new_conversation(const std::string& title = "");
    void load_conversation(const std::string& conversation_id);
    
    // Conversation management
    void save_conversations();
    void load_conversations();
    std::vector<Conversation> get_all_conversations();
    Conversation* get_current_conversation();
    void delete_conversation(const std::string& conversation_id);
    void clear_current_conversation();
    
    // Search and export
    std::vector<Message> search_messages(const std::string& query);
    bool export_conversation(const std::string& conversation_id, const std::string& filepath);
    
    // Settings
    void set_model(const std::string& new_model);
    void set_max_tokens(int tokens);
    std::string get_model() const;
};

#endif // CHATBOT_H
