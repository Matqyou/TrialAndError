//
// Created by 11dpmbozko on 16.03.2023.
//

#include "Entity.h"

Entity::Entity(SDL_Renderer* Renderer, double start_x, double start_y, double start_w, double start_h) {
    m_Renderer = Renderer;
    m_x = start_x;
    m_y = start_x;
    m_w = start_w;
    m_h = start_h;
}

void Entity::Draw() {
    SDL_SetRenderDrawColor(m_Renderer, rand()%255, rand()%255, rand()%255, 255);
    SDL_FRect DrawRect = {float(m_x) - float(m_w/2),
                          float(m_y) - float(m_h/2),
                          float(m_w),
                          float(m_h)};
    SDL_RenderFillRectF(m_Renderer, &DrawRect);
}