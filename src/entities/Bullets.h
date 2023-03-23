//
// Created by janis on 3/22/2023.
//

#ifndef TRIALANDERROR_BULLETS_H
#define TRIALANDERROR_BULLETS_H

#include "Entity.h"
#include <SDL.h>

class Bullets : public Entity{
public:
    Bullets(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel);
    void Tick();

};

#endif //TRIALANDERROR_BULLETS_H
