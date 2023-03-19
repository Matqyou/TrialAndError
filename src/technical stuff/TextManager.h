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
    std::vector<std::pair<std::string, TTF_Font*>> sFonts;

public:
    TextManager();
    ~TextManager();

    TTF_Font* LoadFont(const std::string& key, const char *filepath, int ptsize);
};

extern TextManager TextHandler;

#endif //TRIALANDERROR_SRC_TECHNICAL_STUFF_TEXTMANAGER_H_
