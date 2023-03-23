//
// Created by Matq on 19/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_

#include <SDL_ttf.h>
#include <vector>
#include <string>

class TextManager {
private:
    SDL_Renderer* m_Renderer;
    std::vector<TTF_Font*> m_Fonts;
    std::vector<SDL_Texture*> m_RenderedTexts;

public:
    TextManager(SDL_Renderer* renderer);
    ~TextManager();

    SDL_Texture* Render(TTF_Font* font, const char* text, SDL_Color color);

    TTF_Font* LoadFont(const char *filepath, int ptsize);
    TTF_Font* FirstFont() const { return m_Fonts[0]; }
    void UnloadTexture(SDL_Texture* texture);
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_
