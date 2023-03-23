//
// Created by Matq on 19/03/2023.
//

#include "TextManager.h"
#include <iostream>

TextManager::TextManager(SDL_Renderer* renderer) {
    m_Renderer = renderer;
}

TextManager::~TextManager() {
    for (TTF_Font* Font : m_Fonts)
        TTF_CloseFont(Font);

    for (SDL_Texture* Texture : m_RenderedTexts)
        SDL_DestroyTexture(Texture);
}

SDL_Texture* TextManager::Render(TTF_Font* font, const char* text, SDL_Color color) {
    SDL_Surface* TempSurface = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* NewTexture = SDL_CreateTextureFromSurface(m_Renderer, TempSurface);
    SDL_FreeSurface(TempSurface);
    m_RenderedTexts.push_back(NewTexture);
    return NewTexture;
}

TTF_Font* TextManager::LoadFont(const char* filepath, int ptsize) {
    TTF_Font* NewFont = TTF_OpenFont(filepath, ptsize);
    if (!NewFont)
        std::printf("TTF: There was an error while loading font %s", TTF_GetError());

    m_Fonts.push_back(NewFont);
    return NewFont;
}

void TextManager::UnloadTexture(SDL_Texture* texture) {
    for (auto Iterator = m_RenderedTexts.begin(); Iterator != m_RenderedTexts.end(); Iterator++) {
        SDL_Texture* CurrentTexture = *Iterator;
        if (CurrentTexture != texture)
            continue;

        m_RenderedTexts.erase(Iterator);
        SDL_DestroyTexture(CurrentTexture);
        return;
    }
}