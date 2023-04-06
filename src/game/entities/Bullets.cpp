//
// Created by janis on 3/22/2023.
//

#include "Bullets.h"
#include <cmath>
#include <iostream>
#include "Character.h"
Texture* Bullets::ms_Texture = nullptr;

Bullets::Bullets(GameWorld* world, Entity* shooter, double start_x, double start_y, double start_xvel, double start_yvel)
 : Entity(world, GameWorld::ENTTYPE_BULLET, start_x, start_y, 8, 8, 1.0){
    m_Shooter = shooter;
    m_StillCollidesShooter = true;
    m_xvel = start_xvel;
    m_yvel = start_yvel;
}

void Bullets::TickVelocity() {
    m_xvel *= m_BaseDamping;
    m_yvel *= m_BaseDamping;

    m_x += m_xvel;
    m_y += m_yvel;

    TickHitPoint(m_x, m_y);
    // Problems with code below :::::::)

//    double CurrentX = m_x;
//    double CurrentY = m_x;
//    double Distance = std::sqrt(std::pow(m_xvel, 2) + std::pow(m_yvel, 2));
//    double SliceX = m_xvel / Distance;
//    double SliceY = m_yvel / Distance;
//    int Iterations = (int)(Distance);
//    double RemainingDistance = Distance - (double)(Iterations);
//
//    // Slowly move toward the end point
//    bool Hit;
//    for (int i = 0; i < Iterations; i++) {
//        CurrentX += SliceX;
//        CurrentY += SliceY;
//
//        Hit = TickHitPoint(CurrentX, CurrentY);
//        if (Hit) break;
//    }
//
//    if (!Hit && RemainingDistance > 0.0) {
//        CurrentX += SliceX * RemainingDistance;
//        CurrentY += SliceY * RemainingDistance;
//
//        if (TickHitPoint(CurrentX, CurrentY))
//            return;
//    }
//
//    m_x += m_xvel;
//    m_y += m_yvel;

    // m_Destroy = Hit; //
}

bool Bullets::TickHitPoint(double x, double y) {
    // Check if position collides any of the players
    auto Player = m_World->FirstPlayer();
    for (; Player; Player = (Character*)(Player->NextType())) {
        bool Shooter = m_Shooter == Player;
        bool Collides = (Player->m_x - 25 < x) &&
                        (Player->m_x + 25 > x) &&
                        (Player->m_y - 25 < y) &&
                        (Player->m_y + 25 > y);

        if (Shooter && !Collides) { m_StillCollidesShooter = false; }
        else if (Collides && !Shooter || (Shooter && !m_StillCollidesShooter)) {
            Player->m_Health -= 10; // Primitive dmg function
            Player->is_hit = 7; //Play sounds of getting hit
            delete this;
            return true;
        }
    }
    return false;
}

void Bullets::TickImpact() {
    // Deletes the bullet if it hits the border
    if (m_x < 0 || m_x > m_World->Width() ||
        m_y < 0 || m_y > m_World->Height())
        delete this;
}

void Bullets::Tick() {
    TickVelocity();
    TickImpact();
}

void Bullets::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_Rect BulletRect = { int(m_x - m_w / 2.0), int(m_y - m_h / 2.0), int(m_w), int(m_h) };
    Render->RenderTextureWorld(ms_Texture->SDLTexture(), nullptr, BulletRect);
}
