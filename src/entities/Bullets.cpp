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

void Bullets::TickImpact() {
    if (m_x < 0) delete this;
    else if (m_x > m_World->Width()) delete this;
    else if (m_y < 0) delete this;
    else if (m_y > m_World->Height()) delete this;
}

void Bullets::Tick() {
    TickVelocity();
    TickImpact();
}

void Bullets::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    double XDirection = 1.0;
    double YDirection = 0.0;
    double Speed = std::sqrt(std::pow(m_xvel, 2) + std::pow(m_yvel, 2));
    if (Speed != 0.0) {
        XDirection = m_xvel / Speed;
        YDirection = m_yvel / Speed;
    }

    XDirection *= m_w;
    YDirection *= m_w;

    Render->SetColor(rand()%255, rand()%255, rand()%255, 255);
    Render->LineWorld(int(m_x - XDirection), int(m_y - YDirection),
                      int(m_x + XDirection), int(m_y + YDirection));
    // we can only rotate textures so have lines for now
}
