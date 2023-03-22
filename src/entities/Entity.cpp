//
// Created by 11dpmbozko on 16.03.2023.
//

#include "Entity.h"

Entity::Entity(GameReference* gameWindow, double start_x, double start_y, double start_w, double start_h) {
    m_GameWindow = gameWindow;
    m_x = start_x;
    m_y = start_y;
    m_w = start_w;
    m_h = start_h;
}

void Entity::Draw() {
    SDL_Renderer* Renderer = m_GameWindow->Renderer();

    SDL_SetRenderDrawColor(Renderer, rand()%255, rand()%255, rand()%255, 255);
    SDL_FRect DrawRect = {float(m_x) - float(m_w/2),
                          float(m_y) - float(m_h/2),
                          float(m_w),
                          float(m_h)};
    SDL_RenderFillRectF(Renderer, &DrawRect);
}
