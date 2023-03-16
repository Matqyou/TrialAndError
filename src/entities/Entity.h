//
// Created by 11dpmbozko on 16.03.2023.
//

#ifndef TRIALANDERROR_ENTITY_H
#define TRIALANDERROR_ENTITY_H

#include <SDL.h>

class Entity {
private:
    SDL_Renderer* m_Renderer;
    double m_x, m_y, m_w, m_h;

public:
    Entity(SDL_Renderer* Renderer, double start_x, double start_y, double start_w, double start_h);

    virtual void Draw();
};

#endif //TRIALANDERROR_ENTITY_H
