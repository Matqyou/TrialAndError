//
// Created by 11dpjgailis on 16.03.2023.
//

#include "Character.h"
Character::Character(SDL_Renderer* Renderer, double starting_x, double starting_y){
    m_Renderer = Renderer;
    m_x = starting_x;
    m_y = starting_y;
    m_w = 50;
    m_h = 50;
}

void Character::Draw() {
    SDL_SetRenderDrawColor(m_Renderer, rand()%255, rand()%255, rand()%255, 255);
    SDL_FRect DrawRect = {float(m_x) - float(m_w/2),
                       float(m_y) - float(m_h/2),
                       float(m_w),
                       float(m_h)};
    SDL_RenderFillRectF(m_Renderer, &DrawRect);
}