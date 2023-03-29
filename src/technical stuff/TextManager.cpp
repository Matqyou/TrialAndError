//
// Created by Matq on 19/03/2023.
//

#include "TextManager.h"
#include <iostream>

TextManager::TextManager(ImageManager* image_handler) {
    m_ImageHandler = image_handler;
}

TextManager::~TextManager() {
    for (TTF_Font* Font : m_Fonts)
        TTF_CloseFont(Font);
}

Texture* TextManager::Render(TTF_Font* font, const char* text, SDL_Color color, bool auto_cleanup) {
    SDL_Surface* TempSurface = TTF_RenderText_Blended(font, text, color);
    Texture* NewTexture = m_ImageHandler->TextureFromSurface(TempSurface, auto_cleanup);
    SDL_FreeSurface(TempSurface);
    return NewTexture;
}

TTF_Font* TextManager::LoadFont(const char* filepath, int ptsize) {
    TTF_Font* NewFont = TTF_OpenFont(filepath, ptsize);
    if (!NewFont)
        std::printf("TTF: There was an error while loading font %s", TTF_GetError());

    m_Fonts.push_back(NewFont);
    return NewFont;
}
