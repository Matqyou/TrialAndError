//
// Created by Matq on 26/03/2023.
//

#include "Drawing.h"

Drawing::Drawing(SDL_Renderer* renderer) {
    m_Renderer = renderer;
    m_x = 0.0;
    m_y = 0.0;
}

Drawing::~Drawing() {

}

void Drawing::SetCameraPos(double x, double y) {
    m_x = x;
    m_y = y;
}

void Drawing::SetBlendingMode(SDL_BlendMode blend_mode) {
    SDL_SetRenderDrawBlendMode(m_Renderer, blend_mode);
}

void Drawing::FillRect(SDL_Rect* rect) {
    SDL_RenderFillRect(m_Renderer, rect);
}

void Drawing::RenderTexture(SDL_Texture* texture, SDL_Rect* srcrect, SDL_Rect* dstrect) {
    SDL_RenderCopy(m_Renderer, texture, srcrect, dstrect);
}

void Drawing::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
}

void Drawing::Clear() {
    SDL_RenderClear(m_Renderer);
}

void Drawing::UpdateWindow() {
    SDL_RenderPresent(m_Renderer);
}