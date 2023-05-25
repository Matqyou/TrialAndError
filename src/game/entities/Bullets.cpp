//
// Created by janis on 3/22/2023.
//

#include "Bullets.h"
#include <cmath>
#include <iostream>
#include "Crates.h"
#include "character/Character.h"
Texture* Bullets::ms_Texture = nullptr;

Bullets::Bullets(GameWorld* world, Entity* shooter, double damage, double start_x, double start_y, double start_xvel, double start_yvel)
 : Entity(world, ENTITY_NORMAL, GameWorld::ENTTYPE_BULLET, start_x, start_y, 8, 8, start_xvel, start_yvel, 1.0){
    m_Shooter = shooter;
    m_Damage = damage;
    m_StillCollidesShooter = true;
}

bool Bullets::TickVelocity() {
    m_Core->m_xvel *= m_Core->m_BaseDamping;
    m_Core->m_yvel *= m_Core->m_BaseDamping;

    m_Core->m_x += m_Core->m_xvel;
    m_Core->m_y += m_Core->m_yvel;

    return TickHitPoint(m_Core->m_x, m_Core->m_y);
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
    auto Char = m_World->FirstPlayer();
    for (; Char; Char = (Character*)(Char->NextType())) {
        bool Shooter = m_Shooter == Char;
        EntityCore* CharCore = Char->GetCore();
        bool CollidesPlayer = (CharCore->m_x - 25 < x) &&
                              (CharCore->m_x + 25 > x) &&
                              (CharCore->m_y - 25 < y) &&
                              (CharCore->m_y + 25 > y);

        if (Shooter && !CollidesPlayer) { m_StillCollidesShooter = false; }
        else if (CollidesPlayer && !Shooter || (Shooter && !m_StillCollidesShooter)) {
            Char->Damage(m_Damage, true);
            Char->Accelerate(m_Core->m_xvel * 0.05, m_Core->m_yvel * 0.05);
            delete this;
            return true;
        }
    }
    auto Crate = m_World->FirstCrate();
    for(; Crate; Crate = (Crates*)(Crate->NextType())){
        EntityCore* CrateCore = Crate->GetCore();
        bool CollidesCrate = (CrateCore->m_x - CrateCore->m_w /2 < x) &&
                             (CrateCore->m_x + CrateCore->m_w /2 > x) &&
                             (CrateCore->m_y - CrateCore->m_h /2 < y) &&
                             (CrateCore->m_y + CrateCore->m_h /2 > y);
        if(CollidesCrate){
            Crate->DamageCrate(m_Damage);
            delete this;
            return true;
        }
    }
    return false;
}

void Bullets::TickImpact() {
    // Deletes the bullet if it hits the border
    if (m_Core->m_x < 0 || m_Core->m_x > m_World->Width() ||
        m_Core->m_y < 0 || m_Core->m_y > m_World->Height())
        delete this;
}

void Bullets::Tick() {
    if (!TickVelocity())
        TickImpact();
}

void Bullets::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_Rect BulletRect = { int(m_Core->m_x - m_Core->m_w / 2.0), int(m_Core->m_y - m_Core->m_h / 2.0), int(m_Core->m_w), int(m_Core->m_h) };
    Render->RenderTextureWorld(ms_Texture->SDLTexture(), nullptr, BulletRect);
}
