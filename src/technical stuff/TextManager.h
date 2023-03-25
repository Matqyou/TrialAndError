//
// Created by Matq on 19/03/2023.
//

#ifndef TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_
#define TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_

#include <SDL_ttf.h>
#include "ImageManager.h"
#include <vector>
#include <string>

class TextManager {
private:
    ImageManager* m_ImageHandler;
    std::vector<TTF_Font*> m_Fonts;

public:
    TextManager(ImageManager* image_handler);
    ~TextManager();

    Texture* Render(TTF_Font* font, const char* text, SDL_Color color);

    TTF_Font* LoadFont(const char *filepath, int ptsize);
    TTF_Font* FirstFont() const { return m_Fonts[0]; }
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_
