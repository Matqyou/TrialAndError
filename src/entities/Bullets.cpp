//
// Created by janis on 3/22/2023.
//

#include "Bullets.h"
#include <cmath>
#include <iostream>

Bullets::Bullets(GameWorld* world, double start_x, double start_y, double start_xvel, double start_yvel)
 : Entity(world, GameWorld::ENTTYPE_BULLET, start_x, start_y, 5, 12){
    m_xvel = start_xvel;
    m_yvel = start_yvel;
}

void Bullets::Tick() {
    if((m_x <= m_World->Width()-25) and (m_x >= 25)) m_x += m_xvel; // if on screen
    else if (m_x > m_World->Width()-25) m_xvel = -m_xvel, m_x += m_xvel;  // if going to the right
    else if (m_x < 25)m_xvel = -m_xvel, m_x += m_xvel;  // if going to the left
    if((m_y <= m_World->Height()-25) and (m_y >= 25)) m_y += m_yvel; // if on screen
    else if(m_y > m_World->Height()-25) m_yvel = -m_yvel, m_y += m_yvel; // if going below screen
    else if (m_y < 25)m_yvel = -m_yvel, m_y += m_yvel;  // if going above screen
}