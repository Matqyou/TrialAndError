//
// Created by Matq on 15/04/2023.
//

#include "TextSurface.h"
#include <utility>

void TextSurface::UpdateTexture() {
    delete m_Texture;
    m_Texture = m_AssetsHandler->TextHandler()->Render(m_Font, m_Text.c_str(), m_Color, false);

    m_RenderedText = m_Text;
}

TextSurface::TextSurface(AssetsManager* assets_handler, TTF_Font* font, const std::string& text, SDL_Color color) {
    m_AssetsHandler = assets_handler;
    m_Texture = nullptr;
    m_Text = text;
    m_RenderedText = text;
    m_Font = font;
    m_Color = color;

    UpdateTexture();
}

TextSurface::~TextSurface() {
    delete m_Texture;
}

void TextSurface::SetText(const std::string& text) {
    m_Text = text;
}

void TextSurface::SetFont(TTF_Font* font) {
    m_Font = font;
}

void TextSurface::SetColor(const SDL_Color color) {
    m_Color = color;
}

Texture* TextSurface::Update() {
    if (m_Text == m_RenderedText)
        return m_Texture;

    UpdateTexture();
    return m_Texture;
}