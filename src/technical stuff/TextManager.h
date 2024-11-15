//
// Created by Matq on 19/03/2023.
//

#pragma once

#include <SDL_ttf.h>
#include "ImageManager.h"
#include <vector>
#include <string>

std::string FString(const char* format, ...);

class TextManager {
private:
    ImageManager* m_ImageHandler;
    TTF_Font* m_MainFont;

    std::vector<TTF_Font*> m_Fonts;

public:
    explicit TextManager(ImageManager* image_handler);
    ~TextManager();

    // Getting
    [[nodiscard]] ImageManager* ImageHandler() const { return m_ImageHandler; }
    [[nodiscard]] TTF_Font* GetFont(int font_index) const { return m_Fonts[font_index]; }
    [[nodiscard]] TTF_Font* GetMainFont() const { return m_MainFont; }

    // Generating
    [[nodiscard]] Texture* Render(TTF_Font* font, const char* text, SDL_Color color, bool auto_cleanup);
    [[nodiscard]] TTF_Font* LoadFont(const char* filepath, int ptsize);
};
