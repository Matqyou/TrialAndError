//
// Created by Matq on 19/03/2023.
//

#pragma once

#include <SDL_ttf.h>
#include <vector>
#include <string>
#include "../client/Decals.h"

std::string FString(const char* format, ...);
std::string ErasePrefix(std::string string, const std::string& prefix);
std::string EraseSuffix(std::string string, const std::string& suffix);

class TextManager {
private:
    TTF_Font* m_MainFont;

    std::vector<TTF_Font*> m_Fonts;

public:
    explicit TextManager();
    ~TextManager();

    // Getting
    [[nodiscard]] TTF_Font* GetFont(int font_index) const { return m_Fonts[font_index]; }
    [[nodiscard]] TTF_Font* GetMainFont() const { return m_MainFont; }

    // Generating
    [[nodiscard]] Texture* Render(TTF_Font* font, const char* text, SDL_Color color, bool auto_cleanup);
    [[nodiscard]] TTF_Font* LoadFont(const char* filepath, int ptsize);
};
