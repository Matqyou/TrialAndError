//
// Created by Matq on 19/03/2023.
//

#include "TextManager.h"
#include <iostream>

TextManager::TextManager() {

}

TextManager::~TextManager() {
    for (TTF_Font* Font : m_Fonts)
        TTF_CloseFont(Font);
}

TTF_Font* TextManager::LoadFont(const char* filepath, int ptsize) {
    TTF_Font* NewFont = TTF_OpenFont(filepath, ptsize);
    if (!NewFont)
        std::printf("TTF: There was an error while loading font %s", TTF_GetError());

    m_Fonts.push_back(NewFont);
    return NewFont;
}
