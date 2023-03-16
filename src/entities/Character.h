#ifndef TRIALANDERROR_CHARACTER_H
#define TRIALANDERROR_CHARACTER_H

#include <SDL.h>
#include "Entity.h"

class Character : public Entity {
private:


public:
    Character(SDL_Renderer* Renderer, double start_x, double start_y);

    // void Draw();
};


#endif //TRIALANDERROR_CHARACTER_H
