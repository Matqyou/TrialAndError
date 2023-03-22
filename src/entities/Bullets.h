//
// Created by janis on 3/22/2023.
//

#ifndef TRIALANDERROR_BULLETS_H
#define TRIALANDERROR_BULLETS_H

#include "Entity.h"
#include <SDL.h>

class Bullets : public Entity{
private:
    double m_xvel, m_yvel;

public:
    Bullets(GameWorld* world, double start_x, double start_y);
    void TickBullets();

};

#endif //TRIALANDERROR_BULLETS_H
