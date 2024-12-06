//
// Created by Matq on 19/03/2023.
//

#include "TextManager.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include "../client/Assets.h"

// ANSI color codes
const std::unordered_map<char, std::string> minecraftToAnsi = {
    {'0', "\033[30m"}, // Black
    {'1', "\033[34m"}, // Dark Blue
    {'2', "\033[32m"}, // Dark Green
    {'3', "\033[36m"}, // Dark Aqua
    {'4', "\033[31m"}, // Dark Red
    {'5', "\033[35m"}, // Dark Purple
    {'6', "\033[33m"}, // Gold
    {'7', "\033[37m"}, // Gray
    {'8', "\033[90m"}, // Dark Gray
    {'9', "\033[94m"}, // Blue
    {'a', "\033[92m"}, // Green
    {'b', "\033[96m"}, // Aqua
    {'c', "\033[91m"}, // Red
    {'d', "\033[95m"}, // Light Purple
    {'e', "\033[93m"}, // Yellow
    {'f', "\033[97m"}, // White
    {'r', "\033[0m"}   // Reset
};

std::string FString(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Determine the required buffer size
    va_list argsCopy;
    va_copy(argsCopy, args);
    int bufferSize = std::vsnprintf(nullptr, 0, format, argsCopy) + 1; // +1 for null terminator
    va_end(argsCopy);

    // Create the buffer with the required size
    char* buffer = new char[bufferSize];

    // Format the string
    std::vsnprintf(buffer, bufferSize, format, args);
    va_end(args);

    std::string message(buffer);
    delete[] buffer;

    // Replace Minecraft-style color codes with ANSI codes
    size_t pos = 0;
    while ((pos = message.find('&', pos)) != std::string::npos) {
        if (pos + 1 < message.length()) {
            char colorCode = message[pos + 1];
            auto it = minecraftToAnsi.find(colorCode);
            if (it != minecraftToAnsi.end()) {
                // Replace &<code> with the corresponding ANSI code
                message.replace(pos, 2, it->second);
                continue; // Skip past the replacement
            }
        }
        ++pos; // Move past this '&'
    }

    return message + "\033[0m"; // Ensure the string ends with a reset code
}

//std::string FString(const char* format, ...) {
//    va_list args;
//    va_start(args, format);
//
//    // Determine the required buffer size
//    va_list argsCopy;
//    va_copy(argsCopy, args);
//    int bufferSize = std::vsnprintf(nullptr, 0, format, argsCopy) + 1;  // +1 for null terminator
//    va_end(argsCopy);
//
//    // Create the buffer with the required size
//    char* buffer = new char[bufferSize];
//
//    // Format the string
//    std::vsnprintf(buffer, bufferSize, format, args);
//    va_end(args);
//    std::string message(buffer);
//    delete[] buffer;
//    return message;
//}

std::string ErasePrefix(std::string string, const std::string& prefix) {
    size_t pos = string.find(prefix);
    if (pos != std::string::npos)
        string.erase(pos, prefix.length());

    return string;
}

std::string EraseSuffix(std::string string, const std::string& suffix) {
    size_t pos = string.rfind(suffix);
    if (pos != std::string::npos && pos + suffix.length() == string.length())
        string.erase(pos, suffix.length());

    return string;
}

TextManager::TextManager() {
    m_MainFont = LoadFont("assets/fonts/Minecraft.ttf", 16);
}

TextManager::~TextManager() {
    for (TTF_Font* Font : m_Fonts)
        TTF_CloseFont(Font);
}

Texture* TextManager::Render(TTF_Font* font, const char* text, SDL_Color color) {
    SDL_Surface* TempSurface = TTF_RenderText_Blended(font, text, color);
    Texture* NewTexture = Assets::Get()->TextureFromSurface(TempSurface);
    SDL_FreeSurface(TempSurface);
    return NewTexture;
}

TTF_Font* TextManager::LoadFont(const char* filepath, int ptsize) {
    TTF_Font* NewFont = TTF_OpenFont(filepath, ptsize);
    if (!NewFont) {
        std::printf("TTF: There was an error while loading font %s", TTF_GetError());
        return nullptr;
    }

    m_Fonts.push_back(NewFont);
    return NewFont;
}
