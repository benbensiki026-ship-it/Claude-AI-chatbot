#include "chatbot.h"
#include <iostream>
#include <string>
#include <limits>

void clear_screen() {
#ifdef PLATFORM_WINDOWS
    system("cls");
#else
    system("clear");
#endif
}

void print_menu() {
    std::cout << "\n========== Claude Chatbot Menu ==========\n";
    std::cout << "1. Chat with Claude\n";
    std::cout << "2. Start New Conversation\n";
    std::cout << "3. View All Conversations\n";
    std::cout << "4. Load Conversation\n";
    std::cout << "5. Delete Conversation\n";
    std::cout << "6. Clear Current Conversation\n";
    std::cout << "7. Search Messages\n";
    std::cout << "8. Export Conversation\n";
    std::cout << "9. Settings\n";
    std::cout << "0. Exit\n";
    std::cout << "=========================================\n";
    std::cout << "Choice: ";
}

void chat_mode(ClaudeChatbot& bot) {
    std::cout << "\n========== Chat Mode ==========\n";
    std::cout << "Type 'exit' to return to menu\n";
    std::cout << "Type 'history' to view conversation history\n\n";
    
    std::string input;
    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, input);
        
        if (input == "exit") {
            break;
        }
        
        if (input == "history") {
            Conversation* conv = bot.get_current_conversation();
            if (conv) {
                std::cout << "\n--- Conversation History ---\n";
                for (const auto& msg : conv->messages) {
                    std::cout << "[" << msg.timestamp << "] " 
                              << msg.role << ": " << msg.content << "\n\n";
                }
            }
            continue;
        }
        
        if (input.empty()) continue;
        
        std::cout << "\nClaude: ";
        std::cout.flush();
        
        std::string response = bot.send_message(input);
        std::cout << response << "\n\n";
    }
}

void view_conversations(ClaudeChatbot& bot) {
    auto conversations = bot.get_all_conversations();
    
    if (conversations.empty()) {
        std::cout << "\nNo conversations found.\n";
        return;
    }
    
    std::cout << "\n========== All Conversations ==========\n";
    for (size_t i = 0; i < conversations.size(); i++) {
        std::cout << i + 1 << ". " << conversations[i].title << "\n";
        std::cout << "   ID: " << conversations[i].id << "\n";
        std::cout << "   Created: " << conversations[i].created_at << "\n";
        std::cout << "   Messages: " << conversations[i].messages.size() << "\n";
        std::cout << "   Last Modified: " << conversations[i].last_modified << "\n\n";
    }
}

void load_conversation_menu(ClaudeChatbot& bot) {
    auto conversations = bot.get_all_conversations();
    
    if (conversations.empty()) {
        std::cout << "\nNo conversations to load.\n";
        return;
    }
    
    view_conversations(bot);
    
    std::cout << "Enter conversation number to load: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (choice > 0 && choice <= static_cast<int>(conversations.size())) {
        bot.load_conversation(conversations[choice - 1].id);
        std::cout << "Loaded conversation: " << conversations[choice - 1].title << "\n";
    } else {
        std::cout << "Invalid choice.\n";
    }
}

void delete_conversation_menu(ClaudeChatbot& bot) {
    auto conversations = bot.get_all_conversations();
    
    if (conversations.empty()) {
        std::cout << "\nNo conversations to delete.\n";
        return;
    }
    
    view_conversations(bot);
    
    std::cout << "Enter conversation number to delete (0 to cancel): ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (choice > 0 && choice <= static_cast<int>(conversations.size())) {
        std::cout << "Are you sure you want to delete \"" 
                  << conversations[choice - 1].title << "\"? (y/n): ";
        char confirm;
        std::cin >> confirm;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (confirm == 'y' || confirm == 'Y') {
            bot.delete_conversation(conversations[choice - 1].id);
            std::cout << "Conversation deleted.\n";
        }
    }
}

void search_messages_menu(ClaudeChatbot& bot) {
    std::cout << "\nEnter search query: ";
    std::string query;
    std::getline(std::cin, query);
    
    auto results = bot.search_messages(query);
    
    if (results.empty()) {
        std::cout << "No messages found matching \"" << query << "\".\n";
        return;
    }
    
    std::cout << "\n========== Search Results ==========\n";
    std::cout << "Found " << results.size() << " messages:\n\n";
    
    for (const auto& msg : results) {
        std::cout << "[" << msg.timestamp << "] " << msg.role << ":\n";
        std::cout << msg.content << "\n";
        std::cout << "---\n\n";
    }
}

void export_conversation_menu(ClaudeChatbot& bot) {
    auto conversations = bot.get_all_conversations();
    
    if (conversations.empty()) {
        std::cout << "\nNo conversations to export.\n";
        return;
    }
    
    view_conversations(bot);
    
    std::cout << "Enter conversation number to export: ";
    int choice;
    std::cin >> choice;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    if (choice > 0 && choice <= static_cast<int>(conversations.size())) {
        std::cout << "Enter filename (e.g., conversation.txt): ";
        std::string filename;
        std::getline(std::cin, filename);
        
        if (bot.export_conversation(conversations[choice - 1].id, filename)) {
            std::cout << "Conversation exported to " << filename << "\n";
        } else {
            std::cout << "Failed to export conversation.\n";
        }
    }
}

void settings_menu(ClaudeChatbot& bot) {
    while (true) {
        std::cout << "\n========== Settings ==========\n";
        std::cout << "Current Model: " << bot.get_model() << "\n\n";
        std::cout << "1. Change Model\n";
        std::cout << "2. Change Max Tokens\n";
        std::cout << "0. Back to Main Menu\n";
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        if (choice == 0) break;
        
        switch (choice) {
            case 1: {
                std::cout << "\nAvailable models:\n";
                std::cout << "1. claude-sonnet-4-20250514 (Sonnet 4)\n";
                std::cout << "2. claude-opus-4-20250514 (Opus 4)\n";
                std::cout << "3. claude-haiku-4-20250514 (Haiku 4)\n";
                std::cout << "Enter model number: ";
                
                int model_choice;
                std::cin >> model_choice;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                switch (model_choice) {
                    case 1:
                        bot.set_model("claude-sonnet-4-20250514");
                        std::cout << "Model set to Sonnet 4\n";
                        break;
                    case 2:
                        bot.set_model("claude-opus-4-20250514");
                        std::cout << "Model set to Opus 4\n";
                        break;
                    case 3:
                        bot.set_model("claude-haiku-4-20250514");
                        std::cout << "Model set to Haiku 4\n";
                        break;
                    default:
                        std::cout << "Invalid choice.\n";
                }
                break;
            }
            case 2: {
                std::cout << "\nEnter max tokens (100-4096): ";
                int tokens;
                std::cin >> tokens;
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                
                if (tokens >= 100 && tokens <= 4096) {
                    bot.set_max_tokens(tokens);
                    std::cout << "Max tokens set to " << tokens << "\n";
                } else {
                    std::cout << "Invalid token count.\n";
                }
                break;
            }
            default:
                std::cout << "Invalid choice.\n";
        }
    }
}

int main() {
    std::cout << "========== Claude Chatbot ==========\n";
    std::cout << "Welcome! This chatbot uses Claude AI.\n\n";
    
    std::cout << "Enter your Anthropic API key: ";
    std::string api_key;
    std::getline(std::cin, api_key);
    
    if (api_key.empty()) {
        std::cout << "API key is required. Exiting...\n";
        return 1;
    }
    
    ClaudeChatbot bot(api_key);
    
    std::cout << "\nChatbot initialized successfully!\n";
    std::cout << "Your conversations will be saved automatically.\n";
    
    while (true) {
        print_menu();
        
        int choice;
        std::cin >> choice;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 0:
                std::cout << "\nGoodbye!\n";
                return 0;
                
            case 1:
                chat_mode(bot);
                break;
                
            case 2: {
                std::cout << "\nEnter title for new conversation: ";
                std::string title;
                std::getline(std::cin, title);
                bot.start_new_conversation(title);
                std::cout << "New conversation started: " << title << "\n";
                break;
            }
                
            case 3:
                view_conversations(bot);
                break;
                
            case 4:
                load_conversation_menu(bot);
                break;
                
            case 5:
                delete_conversation_menu(bot);
                break;
                
            case 6:
                bot.clear_current_conversation();
                std::cout << "\nCurrent conversation cleared.\n";
                break;
                
            case 7:
                search_messages_menu(bot);
                break;
                
            case 8:
                export_conversation_menu(bot);
                break;
                
            case 9:
                settings_menu(bot);
                break;
                
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
        }
    }
    
    return 0;
}
