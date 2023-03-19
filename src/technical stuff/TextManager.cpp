//
// Created by Matq on 19/03/2023.
//

#include "TextManager.h"

TextManager TextHandler;

TextManager::TextManager() {
    std::printf("Constructor called\n");
}

TextManager::~TextManager() {
    std::printf("Deconstructor called\n");
    for (const auto& Pair : sFonts)
        TTF_CloseFont(Pair.second);
}

TTF_Font* TextManager::LoadFont(const std::string& key, const char* filepath, int ptsize) {
    TTF_Font* NewFont = TTF_OpenFont(filepath, ptsize);
    std::pair<std::string, TTF_Font*> NewPair = { key, NewFont };
    sFonts.push_back(NewPair);
    return NewFont;
}
