//
// Created by Matq on 19/03/2023.
//

#include "TextManager.h"
#include <iostream>
#include <locale>
#include <codecvt>

std::string FString(const char* format, ...) {
    va_list args;
    va_start(args, format);

    // Determine the required buffer size
    va_list argsCopy;
    va_copy(argsCopy, args);
    int bufferSize = std::vsnprintf(nullptr, 0, format, argsCopy) + 1;  // +1 for null terminator
    va_end(argsCopy);

    // Create the buffer with the required size
    char* buffer = new char[bufferSize];

    // Format the string
    std::vsnprintf(buffer, bufferSize, format, args);
    va_end(args);
    std::string message(buffer);
    delete[] buffer;
    return message;
}

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

Texture* TextManager::Render(TTF_Font* font, const char* text, SDL_Color color, bool auto_cleanup) {
    return nullptr; // todo: fix text rendering
    // SDL_Surface* TempSurface = TTF_RenderText_Blended(font, text, color);
    // Texture* NewTexture = m_ImageHandler->TextureFromSurface(TempSurface, auto_cleanup);
    // SDL_FreeSurface(TempSurface);
    // return NewTexture;
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
