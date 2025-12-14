#include "chatbot.h"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <ctime>
#include <random>

// Platform-specific includes
#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <direct.h>
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <sys/stat.h>
    #include <unistd.h>
#endif

// Simple HTTP client using curl (cross-platform)
#ifdef PLATFORM_WINDOWS
    #pragma comment(lib, "ws2_32.lib")
#endif

#include <curl/curl.h>

// Callback for curl to write response
static size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t total_size = size * nmemb;
    userp->append((char*)contents, total_size);
    return total_size;
}

ClaudeChatbot::ClaudeChatbot(const std::string& api_key, const std::string& model, int max_tokens)
    : api_key(api_key), model(model), max_tokens(max_tokens) {
    data_dir = get_data_directory();
    create_directory(data_dir);
    load_conversations();
    
    if (conversations.empty()) {
        start_new_conversation("New Chat");
    } else {
        current_conversation_id = conversations[0].id;
    }
}

std::string ClaudeChatbot::get_data_directory() {
#ifdef PLATFORM_WINDOWS
    char* appdata = getenv("APPDATA");
    if (appdata) {
        return std::string(appdata) + "\\ClaudeChatbot";
    }
    return ".\\chatbot_data";
#elif defined(PLATFORM_ANDROID)
    return "/data/data/com.claudechatbot/files";
#elif defined(PLATFORM_IOS)
    return "~/Library/Application Support/ClaudeChatbot";
#else
    char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/.claude_chatbot";
    }
    return "./chatbot_data";
#endif
}

bool ClaudeChatbot::create_directory(const std::string& path) {
#ifdef PLATFORM_WINDOWS
    return _mkdir(path.c_str()) == 0 || errno == EEXIST;
#else
    return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
#endif
}

std::string ClaudeChatbot::generate_id() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 15);
    
    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 16; i++) {
        ss << dis(gen);
    }
    return ss.str();
}

std::string ClaudeChatbot::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ClaudeChatbot::escape_json(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string ClaudeChatbot::build_messages_json(const std::vector<Message>& messages) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < messages.size(); i++) {
        ss << "{\"role\":\"" << messages[i].role << "\","
           << "\"content\":\"" << escape_json(messages[i].content) << "\"}";
        if (i < messages.size() - 1) ss << ",";
    }
    ss << "]";
    return ss.str();
}

std::string ClaudeChatbot::http_post(const std::string& url, const std::string& json_data) {
    CURL* curl;
    CURLcode res;
    std::string response;
    
    curl = curl_easy_init();
    if (!curl) {
        return "{\"error\":\"Failed to initialize curl\"}";
    }
    
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("x-api-key: " + api_key).c_str());
    headers = curl_slist_append(headers, "anthropic-version: 2023-06-01");
    
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    
    res = curl_easy_perform(curl);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return "{\"error\":\"" + std::string(curl_easy_strerror(res)) + "\"}";
    }
    
    return response;
}

std::string ClaudeChatbot::send_message(const std::string& user_message) {
    Conversation* conv = get_current_conversation();
    if (!conv) {
        start_new_conversation("New Chat");
        conv = get_current_conversation();
    }
    
    // Add user message
    Message user_msg;
    user_msg.role = "user";
    user_msg.content = user_message;
    user_msg.timestamp = get_timestamp();
    conv->messages.push_back(user_msg);
    
    // Build API request
    std::stringstream request_body;
    request_body << "{"
                 << "\"model\":\"" << model << "\","
                 << "\"max_tokens\":" << max_tokens << ","
                 << "\"messages\":" << build_messages_json(conv->messages)
                 << "}";
    
    // Send to API
    std::string response = http_post("https://api.anthropic.com/v1/messages", request_body.str());
    
    // Parse response (simple parsing - in production use a JSON library)
    std::string assistant_response = "Error: Could not parse response";
    size_t content_pos = response.find("\"text\":\"");
    if (content_pos != std::string::npos) {
        content_pos += 8;
        size_t end_pos = response.find("\"", content_pos);
        if (end_pos != std::string::npos) {
            assistant_response = response.substr(content_pos, end_pos - content_pos);
            
            // Unescape JSON
            size_t pos = 0;
            while ((pos = assistant_response.find("\\n", pos)) != std::string::npos) {
                assistant_response.replace(pos, 2, "\n");
                pos += 1;
            }
            pos = 0;
            while ((pos = assistant_response.find("\\\"", pos)) != std::string::npos) {
                assistant_response.replace(pos, 2, "\"");
                pos += 1;
            }
        }
    }
    
    // Add assistant message
    Message assistant_msg;
    assistant_msg.role = "assistant";
    assistant_msg.content = assistant_response;
    assistant_msg.timestamp = get_timestamp();
    conv->messages.push_back(assistant_msg);
    
    conv->last_modified = get_timestamp();
    save_conversations();
    
    return assistant_response;
}

void ClaudeChatbot::start_new_conversation(const std::string& title) {
    Conversation new_conv;
    new_conv.id = generate_id();
    new_conv.title = title.empty() ? "New Chat" : title;
    new_conv.created_at = get_timestamp();
    new_conv.last_modified = get_timestamp();
    
    conversations.insert(conversations.begin(), new_conv);
    current_conversation_id = new_conv.id;
    save_conversations();
}

void ClaudeChatbot::load_conversation(const std::string& conversation_id) {
    for (const auto& conv : conversations) {
        if (conv.id == conversation_id) {
            current_conversation_id = conversation_id;
            return;
        }
    }
}

Conversation* ClaudeChatbot::get_current_conversation() {
    for (auto& conv : conversations) {
        if (conv.id == current_conversation_id) {
            return &conv;
        }
    }
    return nullptr;
}

std::vector<Conversation> ClaudeChatbot::get_all_conversations() {
    return conversations;
}

void ClaudeChatbot::delete_conversation(const std::string& conversation_id) {
    conversations.erase(
        std::remove_if(conversations.begin(), conversations.end(),
            [&conversation_id](const Conversation& c) { return c.id == conversation_id; }),
        conversations.end()
    );
    
    if (current_conversation_id == conversation_id && !conversations.empty()) {
        current_conversation_id = conversations[0].id;
    }
    
    save_conversations();
}

void ClaudeChatbot::clear_current_conversation() {
    Conversation* conv = get_current_conversation();
    if (conv) {
        conv->messages.clear();
        conv->last_modified = get_timestamp();
        save_conversations();
    }
}

std::vector<Message> ClaudeChatbot::search_messages(const std::string& query) {
    std::vector<Message> results;
    std::string lower_query = query;
    std::transform(lower_query.begin(), lower_query.end(), lower_query.begin(), ::tolower);
    
    for (const auto& conv : conversations) {
        for (const auto& msg : conv.messages) {
            std::string lower_content = msg.content;
            std::transform(lower_content.begin(), lower_content.end(), lower_content.begin(), ::tolower);
            
            if (lower_content.find(lower_query) != std::string::npos) {
                results.push_back(msg);
            }
        }
    }
    
    return results;
}

bool ClaudeChatbot::export_conversation(const std::string& conversation_id, const std::string& filepath) {
    for (const auto& conv : conversations) {
        if (conv.id == conversation_id) {
            std::ofstream file(filepath);
            if (!file.is_open()) return false;
            
            file << "Conversation: " << conv.title << "\n";
            file << "Created: " << conv.created_at << "\n";
            file << "Last Modified: " << conv.last_modified << "\n";
            file << "=" << std::string(60, '=') << "\n\n";
            
            for (const auto& msg : conv.messages) {
                file << "[" << msg.timestamp << "] " << msg.role << ":\n";
                file << msg.content << "\n\n";
            }
            
            file.close();
            return true;
        }
    }
    return false;
}

void ClaudeChatbot::save_conversations() {
    std::string filepath = data_dir + "/conversations.dat";
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) return;
    
    size_t conv_count = conversations.size();
    file.write(reinterpret_cast<const char*>(&conv_count), sizeof(conv_count));
    
    for (const auto& conv : conversations) {
        size_t id_len = conv.id.length();
        file.write(reinterpret_cast<const char*>(&id_len), sizeof(id_len));
        file.write(conv.id.c_str(), id_len);
        
        size_t title_len = conv.title.length();
        file.write(reinterpret_cast<const char*>(&title_len), sizeof(title_len));
        file.write(conv.title.c_str(), title_len);
        
        size_t created_len = conv.created_at.length();
        file.write(reinterpret_cast<const char*>(&created_len), sizeof(created_len));
        file.write(conv.created_at.c_str(), created_len);
        
        size_t modified_len = conv.last_modified.length();
        file.write(reinterpret_cast<const char*>(&modified_len), sizeof(modified_len));
        file.write(conv.last_modified.c_str(), modified_len);
        
        size_t msg_count = conv.messages.size();
        file.write(reinterpret_cast<const char*>(&msg_count), sizeof(msg_count));
        
        for (const auto& msg : conv.messages) {
            size_t role_len = msg.role.length();
            file.write(reinterpret_cast<const char*>(&role_len), sizeof(role_len));
            file.write(msg.role.c_str(), role_len);
            
            size_t content_len = msg.content.length();
            file.write(reinterpret_cast<const char*>(&content_len), sizeof(content_len));
            file.write(msg.content.c_str(), content_len);
            
            size_t timestamp_len = msg.timestamp.length();
            file.write(reinterpret_cast<const char*>(&timestamp_len), sizeof(timestamp_len));
            file.write(msg.timestamp.c_str(), timestamp_len);
        }
    }
    
    file.close();
}

void ClaudeChatbot::load_conversations() {
    std::string filepath = data_dir + "/conversations.dat";
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) return;
    
    conversations.clear();
    
    size_t conv_count;
    file.read(reinterpret_cast<char*>(&conv_count), sizeof(conv_count));
    
    for (size_t i = 0; i < conv_count; i++) {
        Conversation conv;
        
        size_t id_len;
        file.read(reinterpret_cast<char*>(&id_len), sizeof(id_len));
        conv.id.resize(id_len);
        file.read(&conv.id[0], id_len);
        
        size_t title_len;
        file.read(reinterpret_cast<char*>(&title_len), sizeof(title_len));
        conv.title.resize(title_len);
        file.read(&conv.title[0], title_len);
        
        size_t created_len;
        file.read(reinterpret_cast<char*>(&created_len), sizeof(created_len));
        conv.created_at.resize(created_len);
        file.read(&conv.created_at[0], created_len);
        
        size_t modified_len;
        file.read(reinterpret_cast<char*>(&modified_len), sizeof(modified_len));
        conv.last_modified.resize(modified_len);
        file.read(&conv.last_modified[0], modified_len);
        
        size_t msg_count;
        file.read(reinterpret_cast<char*>(&msg_count), sizeof(msg_count));
        
        for (size_t j = 0; j < msg_count; j++) {
            Message msg;
            
            size_t role_len;
            file.read(reinterpret_cast<char*>(&role_len), sizeof(role_len));
            msg.role.resize(role_len);
            file.read(&msg.role[0], role_len);
            
            size_t content_len;
            file.read(reinterpret_cast<char*>(&content_len), sizeof(content_len));
            msg.content.resize(content_len);
            file.read(&msg.content[0], content_len);
            
            size_t timestamp_len;
            file.read(reinterpret_cast<char*>(&timestamp_len), sizeof(timestamp_len));
            msg.timestamp.resize(timestamp_len);
            file.read(&msg.timestamp[0], timestamp_len);
            
            conv.messages.push_back(msg);
        }
        
        conversations.push_back(conv);
    }
    
    file.close();
}

void ClaudeChatbot::set_model(const std::string& new_model) {
    model = new_model;
}

void ClaudeChatbot::set_max_tokens(int tokens) {
    max_tokens = tokens;
}

std::string ClaudeChatbot::get_model() const {
    return model;
}
