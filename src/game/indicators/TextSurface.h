//
// Created by Matq on 15/04/2023.
//

#ifndef TRIALANDERROR_SRC_GAME_INDICATORS_TEXTSURFACE_H_
#define TRIALANDERROR_SRC_GAME_INDICATORS_TEXTSURFACE_H_

#include "../../technical stuff/AssetsManager.h"
#include "../../technical stuff/Drawing.h"

class TextSurface {
private:
    AssetsManager* m_AssetsHandler;
    Texture* m_Texture;
    std::string m_Text;
    TTF_Font* m_Font;
    SDL_Color m_Color;
    bool m_Update;
    SDL_Texture* m_SDLTexture;

    // Generating
    Texture* UpdateTexture();

public:
    TextSurface(AssetsManager* assets_handler, TTF_Font* font, const std::string& text, SDL_Color color);
    ~TextSurface();

    // Getting
    [[nodiscard]] Texture* GetTexture() const { return m_Texture; }
    [[nodiscard]] std::string GetText() const { return m_Text; }
    [[nodiscard]] TTF_Font* GetFont() const { return m_Font; }
    [[nodiscard]] SDL_Color GetColor() const { return m_Color; }
    [[nodiscard]] SDL_Texture* SDLTexture() const { return m_SDLTexture; }
    [[nodiscard]] bool GetFlaggedForUpdate() const { return m_Update; }

    // Setting
    void SetText(const std::string& text);
    void SetFont(TTF_Font* font);
    void SetColor(SDL_Color color);
    void FlagForUpdate() { m_Update = true; }

    // Generate
    Texture* RequestUpdate();
};

#endif //TRIALANDERROR_SRC_GAME_INDICATORS_TEXTSURFACE_H_
