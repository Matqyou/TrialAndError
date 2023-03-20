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
    std::vector<TTF_Font*> m_Fonts;

public:
    TextManager();
    ~TextManager();

    TTF_Font* LoadFont(const char *filepath, int ptsize);
};

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_
