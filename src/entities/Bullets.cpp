//
// Created by janis on 3/22/2023.
//

#include "Bullets.h"
#include <cmath>
#include <iostream>

Bullets::Bullets(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel)
 : Entity(world, GameWorld::ENTTYPE_BULLET, start_x, start_y, 5, 12, 1.0){
    m_xvel = start_xvel;
    m_yvel = start_yvel;
}

void Bullets::Tick() {
    TickVelocity();
    TickBouncyWalls();
}