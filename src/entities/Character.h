#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include <SDL.h>
#include "Entity.h"

class Character : public Entity {
private:
    enum {
        MOVEMENT_UP,
        MOVEMENT_RIGHT,
        MOVEMENT_DOWN,
        MOVEMENT_LEFT,
        NUM_MOVEMENTS
    };
    bool m_Movement[NUM_MOVEMENTS];

public:
    Character(SDL_Renderer* Renderer, double start_x, double start_y);

    // void Draw();
    void Tick();
    void Event(const SDL_Event& CurrentEvent);
};


#endif //TRIALANDERROR_CHARACTER_H
