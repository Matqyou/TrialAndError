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
    std::string m_Text, m_RenderedText;
    TTF_Font* m_Font, *m_RenderedFont;
    SDL_Color m_Color, m_RenderedColor;

    void UpdateTexture();
public:
    TextSurface(AssetsManager *assets_handler, TTF_Font *font, const std::string& text, SDL_Color color);
    ~TextSurface();

    Texture* GetTexture() const { return m_Texture; }
    std::string GetText() const { return m_Text; }
    TTF_Font* GetFont() const { return m_Font; }
    SDL_Color GetColor() const { return m_Color; }

    void SetText(const std::string& text);
    void SetFont(TTF_Font* font);
    void SetColor(SDL_Color color);

    Texture* Update();
};

#endif //TRIALANDERROR_SRC_GAME_INDICATORS_TEXTSURFACE_H_
