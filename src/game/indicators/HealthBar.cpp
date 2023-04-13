//
// Created by Matq on 13/04/2023.
//

#include "HealthBar.h"

HealthBar::HealthBar(GameReference* game_window, double* value, double* max_value, int width, int height, int spacing_w, int spacing_h) {
    m_GameWindow = game_window;
    m_MaxValue = max_value;
    m_Value = value;
    m_Width = width;
    m_Height = height;
    m_SpacingW = spacing_w;
    m_SpacingH = spacing_h;
    m_Ratio = 0.0;

    m_r = 0;
    m_g = 255;
    m_b = 0;
    m_a = 0;
    m_InnerWidth = m_Width - m_SpacingW * 2;
    m_InnerHeight = m_Height - m_SpacingH * 2;
    m_Texture = m_GameWindow->ImageHandler()->CreateTexture(SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,  width, height, false);
}

HealthBar::~HealthBar() {
    delete m_Texture;
}

void HealthBar::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    m_r = r;
    m_g = g;
    m_b = b;
    m_a = a;
}

Texture* HealthBar::UpdateTexture() {
    m_Ratio = *m_Value / *m_MaxValue;
    int InnerWidth = (int)((double)(m_InnerWidth) * m_Ratio);

    Drawing *Render = m_GameWindow->RenderClass();
    Render->SetRenderTarget(m_Texture);
    Render->SetColor(255, 255, 255, 255);
    Render->Clear();
    Render->SetColor(m_r, m_g, m_b, m_a);
    SDL_Rect FillRect = { m_SpacingW, m_SpacingH, InnerWidth, m_InnerHeight };
    Render->FillRect(FillRect);
    Render->SetRenderTarget(nullptr);
    return m_Texture;
}