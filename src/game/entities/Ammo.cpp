//
// Created by Mim on 4/19/2023.
//
#include "Ammo.h"
#include "character/Character.h"
#include <cmath>
#include <iostream>
Texture* Ammo::ms_TextureGlock = nullptr;
Texture* Ammo::ms_TextureShotgun = nullptr;
Texture* Ammo::ms_TextureBurst = nullptr;
Texture* Ammo::ms_TextureMinigun = nullptr;
// Sound* Ammo::ms_PickupSounds[7] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

Ammo::Ammo(GameWorld* world, AmmoType type, double start_x, double start_y, double AmmoCount)
 : Entity(world, ENTITY_NORMAL, GameWorld::ENTTYPE_AMMO, start_x, start_y, 40, 28, 0.0, 0.0, 0.95){
    m_AmmoCount = AmmoCount;
    m_Type = type;

    if (type == AMMO_GLOCK) m_Texture = &ms_TextureGlock;
    else if (type == AMMO_SHOTGUN) m_Texture = &ms_TextureShotgun;
    else if (type == AMMO_BURST) m_Texture = &ms_TextureBurst;
    else if (type == AMMO_MINIGUN) m_Texture = &ms_TextureMinigun;
}
void Ammo::TickPickup() {
    // Check if position collides any of the players
    auto Char = m_World->FirstPlayer();
    for (; Char; Char = (Character*)(Char->NextType())) {
        EntityCore* CharCore = Char->GetCore();
        double XDistance = m_Core->m_x - CharCore->m_x;
        double YDistance = m_Core->m_y - CharCore->m_y;
        double Distance = std::sqrt(std::pow(XDistance, 2) + std::pow(YDistance, 2));

        if (Distance > 45) continue;


        Char->AmmoPickup(this);

        // TODO Increase ammo amount, also check which type of ammo has been picked up aswell
        // something like if(m_Type == AMMO_GLOCK){
        // m_WeaponType->m_Ammo == m_Amm oCapacity
        // }
    }
}

unsigned int Ammo::TakeAmmo(unsigned int request) {
    if (request > m_AmmoCount)
        request = m_AmmoCount;

    m_AmmoCount -= request;
    // TODO fix whatever the fuck this is, or remove it, since i added a pickup sound that is actually playing for me
    // if (request)  {
    //    Sound* PickupSound = ms_PickupSounds[rand()%7];
    //    m_World->GameWindow()->Assets()->SoundHandler()->PlaySound(PickupSound);
    //}

    return request;
}

void Ammo::Tick() {
    TickPickup();

    TickVelocity();
    TickWalls();

    if (m_AmmoCount <= 0) m_Alive = false;
}

void Ammo::Draw() {
    Drawing* Render = m_World->GameWindow()->RenderClass();

    SDL_FRect DrawRect = {float(m_Core->m_x) - float(m_Core->m_w / 2.0),
                          float(m_Core->m_y) - float(m_Core->m_h / 2.0),
                          float(m_Core->m_w),
                          float(m_Core->m_h)};

    Render->RenderTextureFWorld((*m_Texture)->SDLTexture(), nullptr, DrawRect);
}