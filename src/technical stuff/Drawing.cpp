//
// Created by Matq on 26/03/2023.
//

#include "Drawing.h"
#include "../GameWorld.h"

Drawing::Drawing(GameReference* game_window) {
    m_GameWindow = game_window;
    m_World = nullptr;
    m_Renderer = m_GameWindow->Renderer();
}

Drawing::~Drawing() {

}

double Drawing::TranslateX(double x) {
    return x - m_World->CameraX() + m_World->GameWindow()->Width() / 2.0;
}

double Drawing::TranslateY(double y) {
    return y - m_World->CameraY() + m_World->GameWindow()->Height() / 2.0;
}


SDL_Rect Drawing::TranslateRect(const SDL_Rect& rect) {
    return { int(TranslateX(rect.x)),
             int(TranslateY(rect.y)),
             rect.w, rect.h };
}

SDL_FRect Drawing::TranslateFRect(const SDL_FRect& rect) {
    return { float(TranslateX(rect.x)),
             float(TranslateY(rect.y)),
             rect.w, rect.h };
}

void Drawing::SetWorld(GameWorld* world) {
    m_World = world;
}

void Drawing::SetRenderTarget(Texture* target) {
    if (target) SDL_SetRenderTarget(m_Renderer, target->SDLTexture());
    else SDL_SetRenderTarget(m_Renderer, nullptr);
}

void Drawing::SetBlendingMode(SDL_BlendMode blend_mode) {
    SDL_SetRenderDrawBlendMode(m_Renderer, blend_mode);
}

void Drawing::LineWorld(double x1, double y1, double x2, double y2) {
    SDL_RenderDrawLine(m_Renderer, int(TranslateX(x1)), int(TranslateY(y1)),
                       int(TranslateX(x2)), int(TranslateY(y2)));
}

void Drawing::DrawRectWorld(const SDL_Rect &rect) {
    SDL_Rect MovedRect = TranslateRect(rect);
    SDL_RenderDrawRect(m_Renderer, &MovedRect);
}

void Drawing::FillRect(const SDL_Rect& rect) {
    SDL_RenderFillRect(m_Renderer, &rect);
}

void Drawing::FillRectWorld(const SDL_Rect& rect) {
    SDL_Rect MovedRect = TranslateRect(rect);
    SDL_RenderFillRect(m_Renderer, &MovedRect);
}

void Drawing::FillRectFWorld(const SDL_FRect& rect) {
    SDL_FRect MovedRect = TranslateFRect(rect);
    SDL_RenderFillRectF(m_Renderer, &MovedRect);
}

void Drawing::RenderTexture(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_Rect& dstrect) {
    SDL_RenderCopy(m_Renderer, texture, srcrect, &dstrect);
}

void Drawing::RenderTextureWorld(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_Rect& dstrect) {
    SDL_Rect MovedRect = TranslateRect(dstrect);
    SDL_RenderCopy(m_Renderer, texture, srcrect, &MovedRect);
}

void Drawing::RenderTextureExWorld(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_Rect& dstrect, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect MovedRect = TranslateRect(dstrect);
    SDL_RenderCopyEx(m_Renderer, texture, srcrect, &MovedRect, angle, center, flip);
}

void Drawing::RenderTextureF(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_FRect& dstrect) {
    SDL_RenderCopyF(m_Renderer, texture, srcrect, &dstrect);
}

void Drawing::RenderTextureFWorld(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_FRect& dstrect) {
    SDL_FRect MovedRect = TranslateFRect(dstrect);
    SDL_RenderCopyF(m_Renderer, texture, srcrect, &MovedRect);
}

void Drawing::RenderTextureExFWorld(SDL_Texture* texture, SDL_Rect* srcrect, const SDL_FRect& dstrect, double angle, SDL_FPoint* center, SDL_RendererFlip flip) {
    SDL_FRect MovedRect = TranslateFRect(dstrect);
    SDL_RenderCopyExF(m_Renderer, texture, srcrect, &MovedRect, angle, center, flip);
}


void Drawing::RenderTextureFullscreen(SDL_Texture* texture, SDL_Rect* srcrect) {
    SDL_RenderCopy(m_Renderer, texture, srcrect, nullptr);
}

void Drawing::SetColor(SDL_Color color) {
    SetColor(color.r, color.g, color.b, color.a);
}

void Drawing::SetColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(m_Renderer, r, g, b, a);
}

void Drawing::Clear() {
    SDL_RenderClear(m_Renderer);
}

void Drawing::FillAll() {
    SDL_RenderFillRect(m_Renderer, nullptr);
}

void Drawing::UpdateWindow() {
    SDL_RenderPresent(m_Renderer);
}